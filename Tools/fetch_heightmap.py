"""Download a free DEM and convert it to the terrain heightmap used by AVoxelTerrain.

Source: AWS Terrain Tiles (Terrarium encoding), public dataset, no account or key needed.
        https://registry.opendata.aws/terrain-tiles/
Default location: SP Crater, Arizona - a single, isolated cinder cone (~250 m high).

Output (written to Content/Terrain):
    heightmap.r16   - little-endian uint16, Resolution x Resolution, row 0 = north edge,
                      column 0 = west edge (maps to Unreal +X = east, +Y = south).
    heightmap.json  - metadata (resolution, spacing, elevation range, source location).

Only the Python standard library is used.
"""

import argparse
import json
import math
import struct
import urllib.request
import zlib
from pathlib import Path

TILE_URL = "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png"
TILE_SIZE = 256
EARTH_M_PER_PX_Z0 = 156543.03392


def decode_png(data: bytes):
    """Minimal PNG decoder for 8-bit, non-interlaced RGB/RGBA images."""
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("not a PNG")
    pos, idat = 8, bytearray()
    width = height = color_type = None
    while pos < len(data):
        length = struct.unpack(">I", data[pos:pos + 4])[0]
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if ctype == b"IHDR":
            width, height, depth, color_type, _, _, interlace = struct.unpack(">IIBBBBB", body)
            if depth != 8 or interlace != 0 or color_type not in (2, 6):
                raise ValueError("unsupported PNG format")
        elif ctype == b"IDAT":
            idat += body
        elif ctype == b"IEND":
            break
    bpp = 3 if color_type == 2 else 4
    raw = zlib.decompress(bytes(idat))
    stride = width * bpp
    prev = bytearray(stride)
    rows = []
    i = 0
    for _ in range(height):
        ftype = raw[i]
        line = bytearray(raw[i + 1:i + 1 + stride])
        i += 1 + stride
        if ftype == 1:
            for x in range(bpp, stride):
                line[x] = (line[x] + line[x - bpp]) & 0xFF
        elif ftype == 2:
            for x in range(stride):
                line[x] = (line[x] + prev[x]) & 0xFF
        elif ftype == 3:
            for x in range(stride):
                left = line[x - bpp] if x >= bpp else 0
                line[x] = (line[x] + ((left + prev[x]) >> 1)) & 0xFF
        elif ftype == 4:
            for x in range(stride):
                a = line[x - bpp] if x >= bpp else 0
                b = prev[x]
                c = prev[x - bpp] if x >= bpp else 0
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                pred = a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)
                line[x] = (line[x] + pred) & 0xFF
        rows.append(line)
        prev = line
    return width, height, bpp, rows


def fetch_tile(z, x, y):
    url = TILE_URL.format(z=z, x=x, y=y)
    req = urllib.request.Request(url, headers={"User-Agent": "unreal_test-heightmap-tool"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        width, height, bpp, rows = decode_png(resp.read())
    tile = []
    for row in rows:
        tile.append([row[k] * 256.0 + row[k + 1] + row[k + 2] / 256.0 - 32768.0
                     for k in range(0, width * bpp, bpp)])
    return tile


def lonlat_to_global_px(lon, lat, z):
    n = 2 ** z
    gx = (lon + 180.0) / 360.0 * n * TILE_SIZE
    gy = (1.0 - math.asinh(math.tan(math.radians(lat))) / math.pi) / 2.0 * n * TILE_SIZE
    return gx, gy


class Mosaic:
    """Elevation samples covering a rectangle of global pixel space."""

    def __init__(self, z, gx0, gy0, gx1, gy1):
        tx0, ty0 = int(gx0 // TILE_SIZE), int(gy0 // TILE_SIZE)
        tx1, ty1 = int(gx1 // TILE_SIZE), int(gy1 // TILE_SIZE)
        self.ox, self.oy = tx0 * TILE_SIZE, ty0 * TILE_SIZE
        self.w = (tx1 - tx0 + 1) * TILE_SIZE
        self.h = (ty1 - ty0 + 1) * TILE_SIZE
        self.data = [[0.0] * self.w for _ in range(self.h)]
        for ty in range(ty0, ty1 + 1):
            for tx in range(tx0, tx1 + 1):
                print(f"  tile {z}/{tx}/{ty}")
                tile = fetch_tile(z, tx, ty)
                bx, by = (tx - tx0) * TILE_SIZE, (ty - ty0) * TILE_SIZE
                for r in range(TILE_SIZE):
                    self.data[by + r][bx:bx + TILE_SIZE] = tile[r]

    def at(self, gx, gy):
        x = min(max(int(gx) - self.ox, 0), self.w - 1)
        y = min(max(int(gy) - self.oy, 0), self.h - 1)
        return self.data[y][x]


def catmull_rom_weights(t):
    t2, t3 = t * t, t * t * t
    return (
        -0.5 * t3 + t2 - 0.5 * t,
        1.5 * t3 - 2.5 * t2 + 1.0,
        -1.5 * t3 + 2.0 * t2 + 0.5 * t,
        0.5 * t3 - 0.5 * t2,
    )


def resample(mosaic, gx_center, gy_center, m_per_px, resolution, spacing):
    """Separable Catmull-Rom resample onto a regular metric grid centred on (gx, gy)."""
    half = (resolution - 1) * spacing * 0.5
    xs = [gx_center + (i * spacing - half) / m_per_px - mosaic.ox for i in range(resolution)]
    ys = [gy_center + (j * spacing - half) / m_per_px - mosaic.oy for j in range(resolution)]

    x_taps = []
    for x in xs:
        x0 = math.floor(x)
        x_taps.append((x0 - 1, catmull_rom_weights(x - x0)))

    needed_rows = range(max(math.floor(min(ys)) - 1, 0), min(math.floor(max(ys)) + 3, mosaic.h))
    horiz = {}
    for r in needed_rows:
        src = mosaic.data[r]
        horiz[r] = [w[0] * src[b] + w[1] * src[b + 1] + w[2] * src[b + 2] + w[3] * src[b + 3]
                    for b, w in x_taps]

    grid = []
    for y in ys:
        y0 = math.floor(y)
        w = catmull_rom_weights(y - y0)
        r0, r1, r2, r3 = horiz[y0 - 1], horiz[y0], horiz[y0 + 1], horiz[y0 + 2]
        grid.append([w[0] * a + w[1] * b + w[2] * c + w[3] * d
                     for a, b, c, d in zip(r0, r1, r2, r3)])
    return grid


def count_mountains(grid, spacing, prominence=40.0, radius_m=600.0):
    """Count local maxima that rise more than `prominence` metres above their surroundings.

    `radius_m` must exceed a crater diameter so rim points of one cone are not counted twice.
    """
    step = max(int(10.0 / spacing), 1)
    small = [row[::step] for row in grid[::step]]
    n = len(small)
    rad = int(radius_m / (spacing * step))
    peaks = []
    for y in range(rad, n - rad, 2):
        for x in range(rad, n - rad, 2):
            h = small[y][x]
            window = [small[yy][xx] for yy in range(y - rad, y + rad + 1, 3)
                      for xx in range(x - rad, x + rad + 1, 3)]
            if h >= max(window) and h - min(window) > prominence:
                if all(math.hypot(px - x, py - y) > rad for px, py in peaks):
                    peaks.append((x, y))
    return len(peaks)


def write_preview_png(path, grid, lo, hi):
    n = len(grid)
    raw = bytearray()
    for row in grid:
        raw.append(0)
        raw.extend(int((v - lo) / (hi - lo) * 255.0) for v in row)

    def chunk(tag, body):
        return struct.pack(">I", len(body)) + tag + body + struct.pack(">I", zlib.crc32(tag + body))

    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", n, n, 8, 0, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9)) + chunk(b"IEND", b"")
    Path(path).write_bytes(png)


def main():
    project_dir = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--lat", type=float, default=35.5836, help="approximate latitude of the mountain")
    parser.add_argument("--lon", type=float, default=-111.6319, help="approximate longitude of the mountain")
    parser.add_argument("--zoom", type=int, default=15, help="tile zoom level (15 = ~3.9 m/px here)")
    parser.add_argument("--size", type=float, default=2000.0, help="terrain edge length in metres")
    parser.add_argument("--spacing", type=float, default=1.0, help="output grid spacing in metres")
    parser.add_argument("--out", type=Path, default=project_dir / "Content" / "Terrain")
    parser.add_argument("--preview", type=Path, help="optional 8-bit preview PNG path")
    args = parser.parse_args()

    z = args.zoom
    m_per_px = EARTH_M_PER_PX_Z0 * math.cos(math.radians(args.lat)) / 2 ** z
    gx, gy = lonlat_to_global_px(args.lon, args.lat, z)

    # Recentre on the highest point within 600 m of the given coordinates.
    search = 600.0 / m_per_px
    print("Locating summit...")
    probe = Mosaic(z, gx - search, gy - search, gx + search, gy + search)
    best = (-1e9, gx, gy)
    r = int(search)
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            if dx * dx + dy * dy <= r * r:
                h = probe.at(gx + dx, gy + dy)
                if h > best[0]:
                    best = (h, gx + dx, gy + dy)
    _, gx, gy = best

    margin = (args.size * 0.5 + 50.0) / m_per_px
    print("Downloading terrain area...")
    mosaic = Mosaic(z, gx - margin, gy - margin, gx + margin, gy + margin)

    resolution = int(round(args.size / args.spacing)) + 1
    print(f"Resampling to {resolution}x{resolution} @ {args.spacing} m...")
    grid = resample(mosaic, gx, gy, m_per_px, resolution, args.spacing)

    lo = min(min(row) for row in grid)
    hi = max(max(row) for row in grid)
    args.out.mkdir(parents=True, exist_ok=True)
    scale = 65535.0 / (hi - lo)
    with open(args.out / "heightmap.r16", "wb") as f:
        for row in grid:
            f.write(struct.pack(f"<{resolution}H", *(int(round((v - lo) * scale)) for v in row)))

    n = 2 ** z * TILE_SIZE
    center_lon = gx / n * 360.0 - 180.0
    center_lat = math.degrees(math.atan(math.sinh(math.pi * (1.0 - 2.0 * gy / n))))
    meta = {
        "resolution": resolution,
        "spacing_m": args.spacing,
        "min_elevation_m": round(lo, 3),
        "max_elevation_m": round(hi, 3),
        "center_lat": round(center_lat, 6),
        "center_lon": round(center_lon, 6),
        "zoom": z,
        "source": "AWS Terrain Tiles (Terrarium), https://registry.opendata.aws/terrain-tiles/",
        "layout": "uint16 little-endian, row 0 = north, column 0 = west",
    }
    (args.out / "heightmap.json").write_text(json.dumps(meta, indent=2))

    mountains = count_mountains(grid, args.spacing)
    print(f"Elevation {lo:.1f} .. {hi:.1f} m (relief {hi - lo:.1f} m), "
          f"centre {center_lat:.5f}, {center_lon:.5f}, mountains detected: {mountains}")
    if args.preview:
        write_preview_png(args.preview, grid, lo, hi)
        print(f"Preview written to {args.preview}")


if __name__ == "__main__":
    main()
