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
docker run -it sahoo9/cs536-dev:0.1
kraft up -p kvm -m x86_64 -t helloworld helloworld
```

This should print hello world on the screen.
