sudo yum groupinstall -y 'Development Tools'
sudo yum install -y perf.x86_64 git
sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid'
