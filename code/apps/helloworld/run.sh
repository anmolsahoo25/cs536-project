sudo perf kvm --host stat -e LLC-load-misses qemu-system-x86_64 -kernel build/helloworld_kvm-x86_64 -npgraphic -enable-kvm
