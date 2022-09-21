
set -e
set -u

cd ../../lv_port_pc_eclipse

if [ ! -d "build" ];then
  mkdir build
else
  echo "文件夹已经存在"
fi

cd build
cmake ..
make -j8
./bin/main