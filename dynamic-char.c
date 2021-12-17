#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/cred.h>
#include <linux/fs.h>

#define DRIVER_NAME "chardev"
#define DEVICE_NAME "mychardev"

#define BUFFER_LEN 20

static char device_buffer[BUFFER_LEN+1];

dev_t device;
struct cdev my_char_dev;
struct class *my_char_class = NULL;

/* assinaturas das funcoes do driver */
static int     mychardev_open    (struct inode *inode, struct file *file);
static int     mychardev_release (struct inode *inode, struct file *file);
static ssize_t mychardev_read    (struct file *file,   char __user *buf,       size_t count, loff_t *offset);
static ssize_t mychardev_write   (struct file *file,   const char __user *buf, size_t count, loff_t *offset);

struct file_operations f_ops ={
    .owner   = THIS_MODULE,
    .open    = mychardev_open,
    .release = mychardev_release,
    .read    = mychardev_read,
    .write   = mychardev_write
};

static int
mydriver_uevent (struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666); //chmod automatico
    return 0;    
}

/* module initialization entry point */

static int lkm_init ( void )
{
    int err;
    device_buffer[20] = '\0';

    printk(" hello kernel\n");

    err = alloc_chrdev_region(&device, 0, 1, DRIVER_NAME);

    if(err) {
        printk(" registro do char device falhou \n");
    } else {
        printk(" char device registrado\n");
    }

    my_char_class = class_create(THIS_MODULE, DRIVER_NAME);
    my_char_class->dev_uevent = mydriver_uevent;

    cdev_init(&my_char_dev, &f_ops);
    my_char_dev.owner = THIS_MODULE;
    cdev_add(&my_char_dev, device, 1);

    device_create(my_char_class, NULL, device, NULL, DEVICE_NAME);

    printk ("device criado\n");

    return 0;
}

/* module clean-up entry point */

static void lkm_exit ( void )
{
    printk(" Goodbye Kernel\n");

    device_destroy(my_char_class, device);
    class_unregister(my_char_class);
    class_destroy(my_char_class);
    unregister_chrdev_region(device, MINORMASK);

    printk(" char device removido do kernel \n");
}

/* funcoes */

static int
mychardev_open (struct inode *inode, struct file *file)
{
    static int device_access_cnt = 0;
    printk("ID do Usuario %d", current_uid().val);
    printk("device aberto %d vezes", ++device_access_cnt);
    return 0;

}

static int
mychardev_release (struct inode *inode, struct file *file)
{
    printk("device liberado");
    return 0;
}

static ssize_t
mychardev_read (struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    device_buffer[20] = '\0';
    size_t datalen = strlen(device_buffer);
    
    if (count > datalen) {
        count = datalen;
    }

    if (copy_to_user(buf, device_buffer, count)){
        return -EFAULT;
    } else {
        printk("device lido");
    }

    return count;
}

static ssize_t 
mychardev_write (struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
  
    int err;
    int max_bytes = count;

    if(*offset + count < BUFFER_LEN){
        err = copy_from_user(device_buffer + *offset, buf, count);
        printk("escrito: %s\n", device_buffer);
        *offset += count;
    }
    else{       
        int falta = BUFFER_LEN - *offset;
        int offset_buf = 0;
        while(*offset + count >= BUFFER_LEN){
            falta = BUFFER_LEN - *offset;
            err = copy_from_user(device_buffer + *offset, buf + offset_buf, falta);
            printk("escrito(1/2): %s\n", device_buffer);
            count -= falta; 
            offset_buf += falta;
            *offset = 0;
        }
        err = copy_from_user(device_buffer + *offset, buf + offset_buf, count);
        printk("escrito(2/2): %s\n", device_buffer);
        *offset += count;
    }

    if (err) printk("Erro ao copia bytes do user space, qnt: %d", err);

    return max_bytes;

}

/* registration of the entry points */
module_init(lkm_init);
module_exit(lkm_exit);

/* information about the module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("@mfbsouza");
MODULE_DESCRIPTION("A LKM to print some messages");
