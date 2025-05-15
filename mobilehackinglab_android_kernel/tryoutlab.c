#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/cred.h>
#include <linux/slab.h>

MODULE_LICENSE("MobileHackingLab"); 

#define MAX 1000

/* IOCTL cmd's */
#define CREATE_MSG _IO(1,0)
#define CREATE_BUF _IO(1,1)
#define READ_MSG _IO(1,2)
#define LOG_MSG _IO(1,3)
#define DELETE_MSG _IO(1,4)
#define DELETE_BUF _IO(1,5)

/* Global Arrays */

char *buffers[MAX];
struct msg *msgs[MAX];

struct user_req {
	unsigned int buf_id;
	unsigned int msg_id;
	char buffer[128];
};


struct msg {
	void (*log_func)(unsigned int);
	void (*secret_func)(void);
	char *buffer;
	unsigned int id;
	char pad[96];
};

void priv_esc(void){
	commit_creds(prepare_kernel_cred(NULL));
}

void kernel_log(unsigned int id){
	printk(KERN_ALERT "Message is logged: %u",id);
}

static long driver_ioctl(struct file *filp, unsigned int cmd, unsigned long __user arg)
{	
	struct user_req req;
	struct msg *obj = NULL;
	char *buf = NULL;
	int ret;
	unsigned int buf_id;
	unsigned int msg_id;

	ret = copy_from_user(&req,(struct user __user*)arg,sizeof(struct user_req));
	if (req.msg_id > MAX || req.buf_id > MAX)
	{
		printk(KERN_INFO "Incorrect msg_id or buf_id");
	}
	
	buf_id = req.buf_id;
	msg_id = req.msg_id;

	switch (cmd)
	{
	case CREATE_MSG:

		if (msgs[msg_id]){
			printk(KERN_INFO "Msg with id already exist");
			return 0;
		}

		obj = kmalloc(sizeof(struct msg),GFP_KERNEL);
		obj->id = msg_id;
		obj->secret_func = priv_esc;
		obj->log_func = kernel_log;
		msgs[msg_id] = obj;
		printk(KERN_INFO "New message created");
		break;
	case CREATE_BUF:

		if (buffers[buf_id] || !msgs[msg_id]){
			printk(KERN_INFO "Buffer already exist or msg doesn't exist");
			return 0;
		}
			
		buf = kmalloc(sizeof(req.buffer),GFP_KERNEL);
		memcpy(buf,req.buffer,sizeof(req.buffer));
		buffers[buf_id] = buf;
		msgs[msg_id]->buffer = buf;
		
		printk(KERN_INFO "Buffer created and linked to msg");

		break;
	case READ_MSG:

		if (!msgs[msg_id]){
			printk(KERN_INFO "Msg with msg_id doesn't exist");
			return 0;
		}	
		obj = msgs[msg_id];
		memcpy(req.buffer,obj->buffer,sizeof(req.buffer));
		ret = copy_to_user((struct user_req __user*)arg,&req,sizeof(struct user_req));
		printk(KERN_INFO "Msg buffer read");

		break;
	case LOG_MSG:

		if (!msgs[msg_id]){
			printk(KERN_INFO "Msg with msg_id doesn't exist");
			return 0;
		}

		obj = msgs[msg_id];
		obj->log_func(obj->id);

		break;
	case DELETE_MSG:

		if (!msgs[msg_id]){
			printk(KERN_INFO "Msg with msg_id doesn't exist");
			return 0;
		}
		obj = msgs[msg_id];
		kfree(obj);

		break;
	case DELETE_BUF:

		if (!buffers[buf_id])
		{	
			printk(KERN_INFO "Buffer with buf_id doesn't exist");
			return 0;
		}
		buf = buffers[buf_id];
		kfree(buf);
		buffers[buf_id] = NULL;
		printk(KERN_INFO "Buffer removed");
		break;

	default:
		printk(KERN_INFO "Incorrect ioctl");
		break;
	}

	printk(KERN_INFO "");
	return 0;
}

static struct proc_ops pops = {
    .proc_ioctl = driver_ioctl
};

struct proc_dir_entry *proc_interface = NULL;

int init_module(void)
{	
	printk(KERN_INFO "Welcome to MobileHackingLab - Android Kernel Tryoutlab");
	printk(KERN_INFO "Interact with driver --> /proc/tryout");
	
    proc_interface = proc_create("tryout", 0666, NULL, &pops);
	int i = 0;
  	for (i = 0; i < MAX; i++)
	{
		buffers[i] = NULL;
		msgs[i] = NULL;
	}
	printk(KERN_INFO "");
	return 0;
}

void cleanup_module(void)
{
	if (proc_interface) {
		proc_remove(proc_interface);
	}
}