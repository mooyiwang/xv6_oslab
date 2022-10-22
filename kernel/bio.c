// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

struct {
  struct spinlock bucket_lock[NBUCKETS];
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf bucket[NBUCKETS];
} bcache;

void
binit(void)
{
  struct buf *b;
  int i;
  for(i=0; i<NBUCKETS; i++){
    initlock(&bcache.bucket_lock[i], "bcache.bucket");

    bcache.bucket[i].prev = &bcache.bucket[i];
    bcache.bucket[i].next = &bcache.bucket[i];
  }
  

  // // Create linked list of buffers
  // bcache.head.prev = &bcache.head;
  // bcache.head.next = &bcache.head;
  for(b = bcache.buf, i = 0; b < bcache.buf+NBUF; b++, i = (i+1) % NBUCKETS){
  // for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->next = bcache.bucket[i].next;
    b->prev = &bcache.bucket[i];
    initsleeplock(&b->lock, "buffer");
    bcache.bucket[i].next->prev = b;
    bcache.bucket[i].next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int bucket_i = blockno % NBUCKETS;

  acquire(&bcache.bucket_lock[bucket_i]);

  // Is the block already cached?
  for(b = bcache.bucket[bucket_i].next; b != &bcache.bucket[bucket_i]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucket_lock[bucket_i]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.bucket[bucket_i].prev; b != &bcache.bucket[bucket_i]; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;

      b->prev->next = b->next;
      b->next->prev = b->prev;
      b->next = bcache.bucket[bucket_i].next;
      b->prev = &bcache.bucket[bucket_i];
      bcache.bucket[bucket_i].next = b;
      bcache.bucket[bucket_i].next->prev = b;

      release(&bcache.bucket_lock[bucket_i]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  // release(&bcache.bucket_lock[bucket_i]);

  int old_bucket_i = bucket_i;
  for(int next_bucket_i = (old_bucket_i+1)%NBUCKETS; next_bucket_i != old_bucket_i; next_bucket_i = (next_bucket_i+1)%NBUCKETS){
  // for(int next_bucket_i = 0; next_bucket_i < NBUCKETS; next_bucket_i = (next_bucket_i+1)){
  //   if(next_bucket_i == old_bucket_i) continue;
    acquire(&bcache.bucket_lock[next_bucket_i]);
    for(b = bcache.bucket[next_bucket_i].prev; b != &bcache.bucket[next_bucket_i]; b = b->prev){
      if(b->refcnt == 0){
        b->prev->next = b->next;
        b->next->prev = b->prev;
        release(&bcache.bucket_lock[next_bucket_i]);


        // acquire(&bcache.bucket_lock[old_bucket_i]);
        b->next = bcache.bucket[old_bucket_i].next;
        b->prev = &bcache.bucket[old_bucket_i];
        bcache.bucket[old_bucket_i].next = b;
        bcache.bucket[old_bucket_i].next->prev = b;

        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;
        
        release(&bcache.bucket_lock[old_bucket_i]);
        acquiresleep(&b->lock);
        return b;
      }
    }
    release(&bcache.bucket_lock[next_bucket_i]);
  }

  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  int bucket_i = b->blockno % NBUCKETS;

  acquire(&bcache.bucket_lock[bucket_i]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.bucket[bucket_i].next;
    b->prev = &bcache.bucket[bucket_i];
    bcache.bucket[bucket_i].next->prev = b;
    bcache.bucket[bucket_i].next = b;
  }
  release(&bcache.bucket_lock[bucket_i]);
}

void
bpin(struct buf *b) {

  int bucket_i = b->blockno % NBUCKETS;

  acquire(&bcache.bucket_lock[bucket_i]);
  b->refcnt++;
  release(&bcache.bucket_lock[bucket_i]);
}

void
bunpin(struct buf *b) {
  int bucket_i = b->blockno % NBUCKETS;
  acquire(&bcache.bucket_lock[bucket_i]);
  b->refcnt--;
  release(&bcache.bucket_lock[bucket_i]);
}
