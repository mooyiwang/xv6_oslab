// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  int ref[(PHYSTOP - KERNBASE) / PGSIZE];
} phys_page_ref;


//init ref array
void
ppgrefinit()
{
  initlock(&phys_page_ref.lock, "physical page ref");
  int cnt = sizeof(phys_page_ref.ref) / sizeof(int);
  for(int i=0; i < cnt; i++)
    phys_page_ref.ref[i] = 1;
}

//ref count increment
void
refinc(uint64 pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("refinc");

  acquire(&phys_page_ref.lock);
  phys_page_ref.ref[PA2IDX(pa)] += 1;
  release(&phys_page_ref.lock);
}

//ref count decrement
void
refdec(uint64 pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("refinc");

  acquire(&phys_page_ref.lock);
  phys_page_ref.ref[PA2IDX(pa)] -= 1;
  release(&phys_page_ref.lock);
}

//get ref count number
int
getref(uint64 pa)
{
  return phys_page_ref.ref[PA2IDX(pa)];
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  ppgrefinit();
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  refdec((uint64)pa);
  if(getref((uint64)pa) != 0)
    return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    refinc((uint64)r);
  }
  return (void*)r;
}
