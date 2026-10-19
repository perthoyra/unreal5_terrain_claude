"""Generate a tileable 512x512 ground detail texture (soil grain + pebbles).

The texture is close to neutral; AVoxelTerrain tints it per voxel through vertex colours
(grass, dirt, rock, sand, river bed). Only the Python standard library is used.
"""

import argparse
import math
import random
import struct
import zlib
from pathlib import Path

SIZE = 512


def tileable_value_noise(size, period, seed):
    rng = random.Random(seed)
    lattice = [[rng.random() for _ in range(period)] for _ in range(period)]
    out = [[0.0] * size for _ in range(size)]
    scale = period / size
    for y in range(size):
        fy = y * scale
        y0 = int(fy)
        ty = fy - y0
        ty = ty * ty * (3 - 2 * ty)
        r0, r1 = lattice[y0 % period], lattice[(y0 + 1) % period]
        row = out[y]
        for x in range(size):
            fx = x * scale
            x0 = int(fx)
            tx = fx - x0
            tx = tx * tx * (3 - 2 * tx)
            a = r0[x0 % period] + (r0[(x0 + 1) % period] - r0[x0 % period]) * tx
            b = r1[x0 % period] + (r1[(x0 + 1) % period] - r1[x0 % period]) * tx
            row[x] = a + (b - a) * ty
    return out


def main():
    project_dir = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, default=project_dir / "Tools" / "generated" / "T_Ground.png")
    args = parser.parse_args()

    octaves = [(4, 0.45), (8, 0.25), (16, 0.15), (64, 0.10), (128, 0.05)]
    noise = [[0.0] * SIZE for _ in range(SIZE)]
    for i, (period, amp) in enumerate(octaves):
        layer = tileable_value_noise(SIZE, period, seed=1234 + i)
        for y in range(SIZE):
            for x in range(SIZE):
                noise[y][x] += layer[y][x] * amp

    # Pebbles: small darker/lighter discs with a soft rim, wrapped for tiling.
    rng = random.Random(42)
    pebble = [[0.0] * SIZE for _ in range(SIZE)]
    for _ in range(420):
        cx, cy = rng.uniform(0, SIZE), rng.uniform(0, SIZE)
        rad = rng.uniform(2.0, 7.0)
        tone = rng.uniform(-0.25, 0.2)
        for dy in range(-int(rad) - 1, int(rad) + 2):
            for dx in range(-int(rad) - 1, int(rad) + 2):
                d = math.hypot(dx, dy) / rad
                if d < 1.0:
                    px, py = int(cx + dx) % SIZE, int(cy + dy) % SIZE
                    pebble[py][px] = tone * (1.0 - d * d) + (0.08 if d < 0.5 else 0.0) * (tone > 0)

    raw = bytearray()
    for y in range(SIZE):
        raw.append(0)
        for x in range(SIZE):
            v = 0.70 + (noise[y][x] - 0.5) * 0.45 + pebble[y][x]
            v = min(max(v, 0.0), 1.0)
            raw.extend((int(v * 255), int(v * 0.96 * 255), int(v * 0.90 * 255)))

    def chunk(tag, body):
        return struct.pack(">I", len(body)) + tag + body + struct.pack(">I", zlib.crc32(tag + body))

    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", SIZE, SIZE, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9)) + chunk(b"IEND", b"")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_bytes(png)
    print(f"Ground texture written to {args.out}")


if __name__ == "__main__":
    main()
