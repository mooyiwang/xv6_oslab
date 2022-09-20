#include "kernel/types.h"
#include "kernel/param.h"
#include "user.h"

int main(int argc, char *argv[])
{
    char buf[512];
    char *a[MAXARG];
    char *p, *q;
    int i;

    // copy original args
    for (i = 1; i < argc; i++)
    {
        a[i - 1] = argv[i];
    }

    while (read(0, buf, 512) > 0)
    {

        p = buf;
        q = buf;
        // extract args from stdin, 从stdin读入的是“一整块”（当管道发送端发送多行时,管道读出的是全部数据）， 借助\0判断是否有多行， 空格分参数
        for (; p[0] != '\0';)
        {
            i = argc - 1;
            for (; p[0] != '\n'; p++)
            {
                if (p[0] == ' ')
                {
                    *p++ = '\0';
                    a[i++] = q;
                    q = p;
                }
            }
            *p++ = '\0';
            a[i++] = q;
            q++;
            a[i] = 0;

            if (fork() == 0)
            {
                exec(a[0], a);
            }
            wait(0);
        }
    }

    exit(0);
}