# CS536 - Project Proposal

## Improving performance of networked services using code specialization

### Motivation
Applications contend heavily for scarce CPU resources such as
caches, memory bandwidth and compute units. This situation is even worse when
applications are networked, since there is a new degree of contention with the
network IO data. Thus, carefully managing and designing applications is
necessary to avoid such contention and extract good network performance.

But, given that applications are typically deployed on an operating system and
pull-in large amounts of code, it is very difficult to predictably partition
resources among these. Thus, we propose to use code specialization to ensure
that only the necessary code is running on the cores and this leads to better
sharing and utilization of resources.

Our project approach relies on running unikernels instead of full Operating
Systems to execute applications. Since unikernels only bring in the code
necessary for the application, it is much easier to partition these resources
thus allowing more predictable performance.

### Project Deliverable

The ideal deliverable of a project would be a setup where we have the following
3 components -

1. A unikernel running ML inference on network packets
2. A unikernel running a networked application (Raft)
3. A unikernel running the network stack

Thus, we can predictably partition resources betweeen these unikernels to ensure
that each hits the necessary performance and latency requirements.

The major components of the project would be -

1. Machine learning application unikernel (1 person)
2. Raft unikernel (1 person)
3. Networking unikernel + resource partitioning runtime (2 people)
4. Monitoring and evaluation (2 people)
