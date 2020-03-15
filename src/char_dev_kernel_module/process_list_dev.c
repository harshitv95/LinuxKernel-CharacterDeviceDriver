/**
 * Character Device driver (Kernel Module)
 * 
 * @author Harshit Vadodaria (harshitv95@gmail.com)
 * @version 1.0
 * */

#include <linux/kernel.h>
// #include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

int init_module(void);
void cleanup_module(void);
static void init_proc_list(void);
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static char* get_process_state_str(int);


#define DEVICENAME "processlist"

static int device_major;
static dev_t dev_num;
struct class *dev_class;
struct device *dev;

static int device_open = 0;

static int processses_read = 0;
static int bytes_to_write = 0;
static char output[10000];

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .release = dev_release
};

static char *devnode(struct device *dev, umode_t *mode) {
    if (!mode) return NULL;
    *mode = 0666;
    return NULL;
}

int init_module() {
    if ((device_major = register_chrdev(0, DEVICENAME, &fops)) <= 0) {
        printk(KERN_ALERT "Reigstering device %s failed with %d\n", DEVICENAME, device_major);
        return device_major;
    }
    else {
        printk(KERN_INFO "Device %s registered with major %d\n", DEVICENAME, device_major);
        
        // Creating Class (sys/class/{DEVICENAME})
        dev_num = MKDEV(device_major, 0);
        dev_class = class_create(THIS_MODULE, DEVICENAME);
        if (IS_ERR(dev_class)) {
            printk(KERN_WARNING "Failed to create class for device %s with major %d, unregistering", DEVICENAME, device_major);
            unregister_chrdev_region(dev_num, 1);
            return -1;
        }
        dev_class->devnode = devnode;

        // Class created successfully, now creating device node (/dev/{DEVICENAME})
        // that lets user space interact with hardware through kernel
        dev = device_create(dev_class, NULL, dev_num, NULL, DEVICENAME);
        if (IS_ERR(dev)) {
            printk(KERN_WARNING "Failed to create node for device %s with major %d, unregistering", DEVICENAME, device_major);
            // revert
            class_destroy(dev_class);
            unregister_chrdev_region(dev_num, 1);
            return -1;
        }

        return 0;
    }
}

void cleanup_module() {
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev(device_major, DEVICENAME);
    printk(KERN_INFO "Device %s with major %d un-registered\n", DEVICENAME, device_major);
}

static void init_proc_list() {
    struct task_struct *task;
    char line[1000];
    int count = 0;

    for_each_process(task) {
        line[0] = '\0';
        sprintf(line, "PID=%d PPID=%d CPU=%d STATE=%s\n", task->pid, task->parent->pid, task->cpu, get_process_state_str(task->state));
        bytes_to_write += strlen(line);
        strcat(output, line);
        count++;
    }
    processses_read = 1;
    printk(KERN_INFO "Read %d processes into memory, total size: %d bytes", count, bytes_to_write);
}

static int dev_open(struct inode *inode, struct file *file_ptr) {
    if (device_open)
        return -EBUSY;
    processses_read = 0;
    bytes_to_write = 0;
    device_open = 1;
    printk(KERN_INFO "Device [%s] opened\n", DEVICENAME);
    return 0;
}

static ssize_t dev_read(struct file * file_ptr, char * buf, size_t len, loff_t * off) {
    int bytes_read = 0;
    char *out_read = output;
    if (!processses_read)
        init_proc_list();

    if (*out_read == '\0') return 0;
    
    printk(KERN_INFO "Writing %d bytes into buffer of length %zu bytes", bytes_to_write, len);
    while ((bytes_read < len) && *out_read && bytes_to_write > 0) {
        put_user(*(out_read++), buf++);
        bytes_read++;
        bytes_to_write--;
    }
    return bytes_read;
}



static int dev_release(struct inode * node, struct file * file_ptr) {
    device_open = 0;
    output[0] = '\0';
    processses_read = 0;
    printk(KERN_INFO "Device [%s] closed\n", DEVICENAME);
    return 0;
}

static char* get_process_state_str(int state_num) {
    switch (state_num)
    {
        case TASK_RUNNING:
            return "TASK_RUNNING";
        case TASK_INTERRUPTIBLE:
            return "TASK_INTERRUPTIBLE";
        case TASK_UNINTERRUPTIBLE:
            return "TASK_UNINTERRUPTIBLE";
        case __TASK_STOPPED:
            return "__TASK_STOPPED";
        case __TASK_TRACED:
            return "__TASK_TRACED";
        case EXIT_DEAD:
            return "EXIT_DEAD";
        case EXIT_ZOMBIE:
            return "EXIT_ZOMBIE";
        case EXIT_TRACE:
            return "EXIT_TRACE";
        case TASK_PARKED:
            return "TASK_PARKED";
        case TASK_DEAD:
            return "TASK_DEAD";
        case TASK_WAKEKILL:
            return "TASK_WAKEKILL";
        case TASK_WAKING:
            return "TASK_WAKING";
        case TASK_NOLOAD:
            return "TASK_NOLOAD";
        case TASK_NEW:
            return "TASK_NEW";
        case TASK_STATE_MAX:
            return "TASK_STATE_MAX";
        case TASK_KILLABLE:
            return "TASK_KILLABLE";
        case TASK_STOPPED:
            return "TASK_STOPPED";
        case TASK_TRACED:
            return "TASK_TRACED";
        case TASK_IDLE:
            return "TASK_IDLE";
        case TASK_NORMAL:
            return "TASK_NORMAL";
        case TASK_REPORT:
            return "TASK_REPORT";
    }
    return "N/A";
}