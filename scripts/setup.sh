cp ./tmux.conf ~/.tmux.conf

sudo yum groupinstall -y 'Development Tools'

sudo yum install -y perf.x86_64 git tmux docker openssl-devel dwarves ninja-build pixman-devel glib2-devel

sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid'

sudo systemctl start docker

git clone --branch kernel-5.10.147-133.644.amzn2 \
    --single-branch https://github.com/amazonlinux/linux

cp /boot/config-5.10.147-133.644.amzn2.x86_64 ./.config

(cd linux; yes | make oldconfig; make -j 47)

(cd linux; make M=arch/x86/kvm)

sudo depmod

(cd linux; sudo cp arch/x86/kvm/kvm.ko /lib/modules/5.10.147-133.644.amzn2.x86_64/kernel/arch/x86/kvm/)

(cd linux; sudo cp arch/x86/kvm/kvm-intel.ko /lib/modules/5.10.147-133.644.amzn2.x86_64/kernel/arch/x86/kvm/)

wget https://download.qemu.org/qemu-6.1.0.tar.xz

tar -xvf qemu-6.1.0.tar.xz

(cd qemu-6.1.0; ./configure --target-list=x86_64-softmmu; make -j47; sudo make install; sudo update-alternatives --install /usr/bin/qemu-system-x86_64 qemu-system-x86_64 /usr/local/bin/qemu-system-x86_64 100)
