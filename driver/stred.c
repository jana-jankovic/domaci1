#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#define BUFF_SIZE 200
MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

DECLARE_WAIT_QUEUE_HEAD(appendQ);

char str[100];
int pos = 0;
int endRead = 0;
int r=0;
int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened lifo\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed lifo\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len;
		
	if (endRead){
		endRead = 0;
		r = 0;
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}

	len = scnprintf(buff,BUFF_SIZE , "%c", str[r]);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	r ++;
	if (r == 100) {
		endRead = 1;
	}
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	char komanda[BUFF_SIZE]; 
	char ostatak[20];
	int isti = 1;
        char string[BUFF_SIZE];
	char str2[BUFF_SIZE];
	char *shrink_s;
	int value;
	int ret;
	int i=0,j=0;
	int s=0,k=0;
        int duzina;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

	
         sscanf(buff,"%9s%d%4s",komanda,&value,ostatak);
	while(buff[i] != '=' && buff[i] != '\0' && buff[i] != '\n')
	{
		komanda[k]=buff[i];
		k++;
		i++;
	}
	i++;
	komanda[k] = 0;	

	while(buff[i] != '\0' && buff[i] != '\n')
	{
		string[s]=buff[i];
		s++;
		i++;
	}	
	string[s] = 0;


      	if(strcmp(komanda,"string") == 0)
        {
		for(i = 0; i < s; i++)
		{
		    str[i] = string[i];
		    pos++;
		}
		pos--;
		while(i != 100)
		{	
		str[i] = 0;
		i++;
		}
	}


	      
        if(strcmp(komanda,"clear") == 0)
        {
		for(i = 0; i < 100; i++)
		{
		    str[i] = 0;
		}	
		pos = 0;
		
		wake_up_interruptible(&appendQ);
	}

     
        if(strcmp(komanda,"shrink") == 0)
        {
	
		shrink_s = strim(str);
		i=0;
		pos = 0;	
		while(*shrink_s != '\0')
	        {
			str[i] = *shrink_s;
			shrink_s++;
			i++;
			pos++;
		} 
		pos--;
		while(i != 100)
		{	
		str[i] = 0;
		i++;
		}

		wake_up_interruptible(&appendQ);
        }


        if(strcmp(komanda,"append") == 0)
	{
	/*	if(wait_event_interruptible(appendQ,(100 - pos > s)))
			return -ERESTARTSYS;
		if(100 - pos > s)
		{	
		for(i = 0; i< s;i++)
	        {
			str[pos] = string[i];
			pos++;
		}
		}
	*/
		strcat(str,string);
		
	}

	
        if(strcmp(komanda,"truncate") == 0)
	{
		duzina = strlen(str);
		for(i = duzina-value; i < duzina;i++)
	        {
			str[i] = 0;
		
		}
		pos = pos - value;	
	
		wake_up_interruptible(&appendQ);
	}
		
        if(strcmp(komanda,"remove") == 0)
	{
		k = 0;

		for(i = 0;i < 100; i++)
		{
			if(str[i] == string[0])
			{
				isti = 0;
				for(j = 0; j< s; j++ )
				{	
				 if(str[i+j] != string[j]) isti = 1;
				}
				if (isti == 0) i = i+s-1;
			}	
			
			if(isti == 1){ str2[k] = str [i];
			k++;
			}

			if(isti == 0) isti =1;

		
		}

		
		for(i = 0;i < k; i++)
		{
		  str[i] = str2[i];
		}
		for(i = k;i<100;i++)
		{
		 str[i] = 0;
		}
		pos = k;	
	
		wake_up_interruptible(&appendQ);
	}
		
	/*if(pos<10)
	{
		ret = sscanf(buff,"%d",&value);
		if(ret==1)//one parameter parsed in sscanf
		{
			printk(KERN_INFO "Succesfully wrote value %d", value); 
			lifo[pos] = value; 
			os=pos+1;
		}
		else
		{
			printk(KERN_WARNING "Wrong command format\n");
		}
	}
	else
	{
		printk(KERN_WARNING "Lifo is full\n"); 
	}
         */
	return length;
}

static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<100; i++)
		str[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);
