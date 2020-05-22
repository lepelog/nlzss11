import pytest
import nlzss11
# reference implementation
from lzss3 import decompress_bytes
from pathlib import Path
from files import TEST_FILES, TEST_FILE_DATA, TEST_FILE_DATA_UNCOMP


@pytest.mark.parametrize("file", TEST_FILES)
def test_decompression(file):
    assert TEST_FILE_DATA_UNCOMP[file] == nlzss11.decompress(TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_roundtrip(file):
    assert TEST_FILE_DATA_UNCOMP[file] == nlzss11.decompress(nlzss11.compress(TEST_FILE_DATA_UNCOMP[file]))


@pytest.mark.parametrize("file", TEST_FILES)
def test_compress(file):
    assert TEST_FILE_DATA_UNCOMP[file] == decompress_bytes(bytes(nlzss11.compress(TEST_FILE_DATA_UNCOMP[file])))
    