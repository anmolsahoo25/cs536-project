#sudo qemu-system-x86_64 -kernel build/helloworld_kvm-x86_64 -m 512M -nographic -enable-kvm
sudo perf kvm --guest stat -e LLC-load-misses -e migrations taskset -c 40 qemu-system-x86_64 -kernel build/helloworld_kvm-x86_64 -m 256M -nographic -enable-kvm
