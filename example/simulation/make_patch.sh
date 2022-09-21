set -e
set -u

script_dir=$(cd $(dirname $0);pwd)
cd ../../lv_port_pc_eclipse
git diff > ${script_dir}/sim.patch