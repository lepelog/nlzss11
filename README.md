## nlzss11
### Library for data compression using Nintendo's variant of the lzss algorithm

Reference implementation: [nlzss](https://github.com/magical/nlzss)

## Usage

*nlzss11* can be installed with `pip3 install nlzss11`.

### `nlzss11.get_uncompressed_size(data)`

Returns the size in bytes of the file if uncompressed or None if the bytes are not a valid Nintendo LZSS 11 compressed file

### `nlzss11.decompress(data)`

Decompresses Yaz0-compressed data from a bytes-like object `data`. Returns a bytes object containing the uncompressed data.

### `nlzss11.decompress_unsafe(data)`

Decompresses Yaz0-compressed data from a bytes (*not* bytes-like) object `data`. Returns a bytes object containing the uncompressed data.

Unlike nlzss11.decompress, this function assumes that the input data is well-formed. In exchange for slightly improved performance, no sanity checks are performed. **Warning**: Do not use on untrusted data.

### `nlzss11.compress(data, level=7)`

Compresses a bytes-like object `data`. Returns a bytes-like object containing the compressed data.

`level` is the compression level (6-9). 6 is fastest and 9 is slowest. Higher compression levels result in better compression. 7 is a good compromise between compression ratio and performance.

## Project information

This project is a fork from the amazing [syaz0 Project](github.com/zeldamods/syaz0), just modified to match the algorithm found in The Legend of Zelda: Skyward Sword.

### Building from source

Building *nlzss11* from source requires:

* CMake 3.10+
* A compiler that supports C++17
* Everything needed to build [zlib-ng](https://github.com/zlib-ng/zlib-ng)
* pybind11 2.4+ (including CMake config files)
* setuptools

When no binary build is available, pip will automatically build from source during the install process.

To build and install from source run `pip3 install .`.

### Building for manylinux
- Use the manylinux docker container in this directory:  
  `docker run -v $PWD:/home/build/nlzss11 -ti quay.io/pypa/manylinux2010_x86_64 bash`
- Run `bash build-on-manylinux.sh`
- run `setup.py bdist_wheel` with every cpython version you want from `/opt/python`

## Changelog
### 1.3
- decompress support for files > 0xFFFFFF (found in SSHD)

## License

This software is licensed under the terms of the GNU General Public License, version 2 or later.
