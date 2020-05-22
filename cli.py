#!/usr/bin/env python3
import argparse
from pathlib import Path
import nlzss11
import sys


def main() -> None:
    parser = argparse.ArgumentParser("nlzss11", description="Compress or decompress Nintendo LZSS11 data")
    parser.add_argument("-d", "--decompress", help="Decompress", action="store_true", default=False)
    parser.add_argument("-c", "--stdout", help="Write output to stdout", action="store_true", default=False)
    parser.add_argument("-l", "--level", help="Compression level (6-9)",
                        type=int, choices=[6, 7, 8, 9], default=9)
    parser.add_argument("file")

    args = parser.parse_args()

    with open(args.file, "rb") as f:
        data = f.read()

    if args.decompress or args.file.endswith(".LZ"):
        result = nlzss11.decompress(data)
        name = args.file.replace(".LZ", "")
        if Path(name).exists():
            name += ".decomp"
    else:
        result = nlzss11.compress(data, level=args.level)
        name = args.file + ".LZ"

    if args.stdout:
        sys.stdout.buffer.write(result)
    else:
        with open(name, "wb") as f:
            f.write(result)

main()
