import pytest
import nlzss11
from files import TEST_FILES, TEST_FILE_DATA, TEST_FILE_DATA_UNCOMP

@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_nlzss11_lv6(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(nlzss11.compress, TEST_FILE_DATA_UNCOMP[file], level=6)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_nlzss11_lv7(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(nlzss11.compress, TEST_FILE_DATA_UNCOMP[file], level=7)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_nlzss11_lv8(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(nlzss11.compress, TEST_FILE_DATA_UNCOMP[file], level=8)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_nlzss11_lv9(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(nlzss11.compress, TEST_FILE_DATA_UNCOMP[file], level=9)