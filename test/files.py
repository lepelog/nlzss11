from pathlib import Path
import pytest
# reference implementation
from lzss3 import decompress_bytes

TEST_FILES = []
TEST_FILE_DATA = dict()
TEST_FILE_DATA_UNCOMP = dict()
for path in (Path(__file__).parent / "files").glob("*.LZ"):
    with path.open("rb") as f:
        TEST_FILES.append(pytest.param(path.name, id='/'))
        TEST_FILE_DATA[path.name] = f.read()
        TEST_FILE_DATA_UNCOMP[path.name] = decompress_bytes(TEST_FILE_DATA[path.name])
