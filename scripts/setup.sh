cp ./tmux.conf ~/.tmux.conf

sudo yum groupinstall -y 'Development Tools'

sudo yum install -y perf.x86_64 git tmux docker

sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid'

sudo systemctl start docker

git clone --branch kernel-5.10.147-133.644.amzn2 \
    --single-branch https://github.com/amazonlinux/linux
