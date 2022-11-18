# Lab: mmap
https://pdos.csail.mit.edu/6.S081/2020/labs/mmap.html

mmap 和 munmap 系统调用允许 UNIX 程序对其地址空间进行详细控制。它们可用于在进程之间共享内存，将文件映射到进程地址空间，以及作为用户级页面错误方案的一部分，例如讲座中讨论的垃圾收集算法。在本实验中，您将向 xv6 添加 mmap 和 munmap，重点关注**内存映射文件**。

难度：⭐⭐⭐⭐⭐