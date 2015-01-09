#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/miscdevice.h>
#include <linux/semaphore.h>


MODULE_AUTHOR("tdonohu1, mstromb1");
MODULE_DESCRIPTION("Implementing a String Pipe Using Character-device");
MODULE_LICENSE("DUAL BSD/GPL");

int err;
int N = 0;
module_param(N, int, S_IRUGO);

struct semaphore empty;
struct semaphore full;
struct semaphore mutex;

static int r_init(void);
static void r_cleanup(void);

ssize_t my_write(struct file *filep,const char *buff,size_t count,loff_t *offp );
ssize_t my_read(struct file *filep,char *buff,size_t count,loff_t *offp );

struct queue {
    int size;
    int start;
    int count; //Number of valid data (strings) in the queue that have not been consumed
    const char **element;
};
 
typedef struct queue queue_t;
 
queue_t queue;


void init(queue_t *queue, int size) {
    queue->size = size;
    queue->start = 0;
    queue->count = 0;
    queue->element = kmalloc(sizeof(char*) * size, GFP_KERNEL);
     
}

void free_queue(queue_t *queue) {
    kfree(queue->element);
     
}

int full_q(queue_t *queue) {
    if (queue->count == queue->size) {
        return 1;//True
    } else {
        return 0;//False
    }
}
 
int empty_q(queue_t *queue) {
    if (queue->count == 0) {
        return 1;//True
    } else {
        return 0;//False
    }
}
     
void push(queue_t *queue, const char *data) 
{
    int index;
    if (full_q(queue)) 
    {
        printk("overflow\n");//Because we block if full, it can never overflow 
    } 
    else 
    {
        index = queue->start  + queue->count++;
        if (index >= queue->size) 
		{
            index-=queue->size;
        }

        queue->element[index] = data;
    }
}
 
 
const char* popqueue(queue_t *queue) {
    const char * element;
    if (empty_q(queue)) {
        printk("underflow\n");
        return "0";
    } else {
       /* FIFO implementation */
       element = queue->element[queue->start];
       queue->start++;
       queue->count--;
       if (queue->start == queue->size) {
          queue->start-=queue->size;
       }
        
       return element;
    }
}

struct file_operations my_fops={
	read: my_read,
	write: my_write,
};

static struct miscdevice my_misc_device={
	.minor=MISC_DYNAMIC_MINOR,
	.name="mypipe",
	.fops=&my_fops
};

ssize_t my_write(struct file *filep,const char *buff,size_t count,loff_t *offp )
{
	char* mybuffer;

	if(down_interruptible(&empty)){//Can be stopped by ctrl+c (interrupted)
		 /* signal received, semaphore not acquired ... */
		return -ERESTARTSYS;
	}

	if(down_interruptible(&mutex)){//Can be stopped by ctrl+c (interrupted)
		 /* signal received, semaphore not acquired ... */
		up(&empty);
		return -ERESTARTSYS;
	}

	mybuffer = kmalloc(sizeof(char) * count, GFP_KERNEL); 

	/* critical region ... */
	if ( copy_from_user(mybuffer,buff,count) != 0 ){ //count
		printk( "User -> kernel copy failed!\n" );
		up(&empty);
		up(&mutex);
		return -EFAULT;
	}


	//allocate and write
	push(&queue, mybuffer);


	/* release the given semaphore */
	up(&mutex);
	up(&full);


	return 0;
}

ssize_t my_read(struct file *filep,char *buff,size_t count,loff_t *offp )
{
	const char* pop_buffer;

	if(down_interruptible(&full)){
		 /* signal received, semaphore not acquired ...*/ 
		return -ERESTARTSYS;
	}

	if(down_interruptible(&mutex)){
		 /* signal received, semaphore not acquired ... */
		up(&full);
		return -ERESTARTSYS;
	}

	//pop queue
	pop_buffer = popqueue(&queue);

	/* critical region ... */
	if ( copy_to_user(buff,pop_buffer,count) != 0 )
	{
		printk( "Kernel -> user copy failed!\n" );
		up(&full);
		up(&mutex);
		return -EFAULT;
	}


	//kfree and read
	kfree(pop_buffer);
	
	/* release the given semaphore */
	up(&mutex);
	up(&empty);
	
	return 0;
}

module_init(r_init);
module_exit(r_cleanup);

static int r_init(void)
{
	sema_init(&empty, N);
	sema_init(&full, 0);
	sema_init(&mutex, 1);
	
	init(&queue, N);

	printk(KERN_ALERT "Registering my device!\n");//About to register
	if((err = misc_register(&my_misc_device))<0) //We register the device with its miscdevice information
	{
		printk(KERN_ALERT "My_Device failed to register!\n");//Fails if less than 0
		return err;
	}
	printk(KERN_ALERT "Device registered!\n");//Woo, we made it!
	return 0;
}

static void r_cleanup(void)
{
	free_queue(&queue);
	printk("Deregistering miscdevice\n");
	if((err = misc_deregister(&my_misc_device))<0)
	{
		printk(KERN_ALERT "My_Device failed to unregister!\n");//This shouldn't happen because if it registers, it should be able to de-register
	}
	printk("Miscdevice successfully deregistered!\n");

}

