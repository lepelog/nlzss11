import pytest
import nlzss11
from lzss3 import decompress_bytes
from files import TEST_FILES, TEST_FILE_DATA


@pytest.mark.parametrize("file", TEST_FILES)
def test_lzss3(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(decompress_bytes, TEST_FILE_DATA[file])

@pytest.mark.parametrize("file", TEST_FILES)
def test_nlzss11(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(nlzss11.decompress, TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_nlzss11_fast(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(nlzss11.decompress_unsafe, TEST_FILE_DATA[file])
