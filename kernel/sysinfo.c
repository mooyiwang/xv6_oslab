//
// system calls about system's overall infomation
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stat.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "sysinfo.h"

uint64
sys_sysinfo(void)
{
    uint64 info;  //user pointer to struct sysinfo
    
    if(argaddr(0, &info) < 0)
        return -1;
    return sysinfo(info);
}

