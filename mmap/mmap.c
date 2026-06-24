#include "mmap.h"

void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc , char *argv[])
{
    int fd;
    char *p=NULL;
    fd=open("test_mmap",O_RDWR|O_CREAT|O_TRUNC,0664);
    if(fd==-1)
    {
        sys_err("open error");
    }

    ftruncate(fd,20);//给文件扩容
    int len=lseek(fd,0,SEEK_END);

    p=mmap(NULL,len,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);//内存映射区
    if(p==MAP_FAILED)
    {
        perror("mmap error");
    }

    //对文件进行读写操作
    strcpy(p,"hello mmap");
    printf("%s\n",p);

    //清除映射区
    int ret=munmap(p,20);
    if(ret==-1)
    {
        perror("mummap error");
    }
    return 0;
}