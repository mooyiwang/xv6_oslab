# Lab: Copy-on-Write Fork for xv6
https://pdos.csail.mit.edu/6.828/2020/labs/cow.html

虚拟内存提供了一定程度的间接性：内核可以通过将PTE标记为无效或只读来拦截内存引用，从而导致页面错误，并且可以通过修改PTE来更改地址的含义。在计算机系统中有一种说法，任何系统问题都可以通过间接解决。懒惰分配实验提供了一个例子。本实验室探索了另一个例子：写时复制。

难度：⭐
## Task: Implement copy-on write
