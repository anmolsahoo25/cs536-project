# CS586 Project

## Overview

This repo tracks the project progress for the group - 

1. Jiangqiong
2. Joshua
3. Vedant
4. Sowmya
5. Keerthana
6. Anmol

Project Title - Improving resource allocation for networked applications using
unikernels and Intel RDT

## Developing

We can use Docker to setup a development environment. Please install Docker for
your platform and follow the steps below, to check if everything is working -

```
docker pull sahoo9/cs536-dev:0.1
docker run -v /apps:`pwd`/code/apps -it sahoo9/cs536-dev:0.1
cd helloworld
kraft configure
kraft build
qemu-system-x86_64 -kernel build/helloworld_kvm-x86_64 -nographic
```

This should print hello world on the screen.
