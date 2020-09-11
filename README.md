Memsc (Master's Thesis Project)
==========

This repository contains all source code developed for my Master's thesis project. It also contains the complete thesis document titled "_**Reducing System Call Performance Overhead on Paravirtualized L<sup>4</sup>Linux**_". Memsc is a new system call forwarding mechanism implemented in the [L<sup>4</sup>Linux](https://l4linux.org/overview.shtml) kernel, a para-virtualized Linux kernel variant running on top of the [L4](https://l4re.org/) microkernel/hypervisor. It achieves lower latency for forwarding system calls from user-dedicated CPUs to kernel-dedicated CPUs and it provides applications with an asynchronous system call execution model.

Contents:

  * [Thesis-Memsc.pdf](https://github.com/ilstam/memsc-master-thesis/blob/master/Thesis-Memsc.pdf) - The thesis document
  * [l4linux-5.6.0.patch](https://github.com/ilstam/memsc-master-thesis/blob/master/l4linux-5.6.0.patch) - The L<sup>4</sup>Linux kernel patch
  * [uclibc-ng-1.0.32.patch](https://github.com/ilstam/memsc-master-thesis/blob/master/uclibc-ng-1.0.32.patch) - The uclibc-ng patch
  * [memsclib/](https://github.com/ilstam/memsc-master-thesis/tree/master/memsclib) - The Memsc user-space library
  * [examples/](https://github.com/ilstam/memsc-master-thesis/tree/master/examples) - Example code using Memsc
  * [latex\_sources/](https://github.com/ilstam/memsc-master-thesis/tree/master/latex_sources) - Latex sources of the document
