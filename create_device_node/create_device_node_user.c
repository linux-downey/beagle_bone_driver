#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


static char buf[256] = {1};

int main(int argc,char *argv[])
{
    int fd = open("/dev/basic_demo",O_RDWR);
    if(fd < 0)
    {
        perror("Open file failed!!!\r\n");
    }
    int ret = write(fd,"huangdao!",strlen("huangdao!"));
    if(ret < 0){
         perror("Failed to write!!");
    }
    ret = read(fd,buf,7);
    if(ret < 0)
    {
        perror("Read failed!!");
    }
    else
    {
        printf("recv data = %s ",buf);
    }

    close(fd);

    return 0;
}




