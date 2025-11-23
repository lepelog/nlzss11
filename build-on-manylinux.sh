# activate devtoolset to be able to use c++17
/opt/rh/devtoolset-8/enable

# install cmake
yum install -y cmake3
# override old
ln -sf /usr/bin/cmake3 /usr/bin/cmake

# install pybind11
mkdir ~/temp_pybind
cd ~/temp_pybind
curl -L https://github.com/pybind/pybind11/archive/v2.13.6.tar.gz -o pybind11.tar.gz && tar -xzf pybind11.tar.gz && cd pybind11-2.13.6/

mkdir build && cd build
cmake -DPYBIND11_TEST=false ..
make && make install

cd ~ && rm -rf ~/temp_pybind

cd /home/build/nlzss11
