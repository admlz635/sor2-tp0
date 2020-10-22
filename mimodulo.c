#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include "mimodulo.h"

// constantes
#define DEVICE_NAME "UNGS"
#define SUCCESS 0
#define BUFFER_MAX_SIZE 1024
#define CAESAR_SHIFT 4

static char kernel_buffer[BUFFER_MAX_SIZE];

static unsigned long kernel_buffer_size = 0;

static int Major;
static char *msg_print;

static struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release};

int init_module(void)
{ /* Constructor */
  Major = register_chrdev(0, DEVICE_NAME, &fops);
  printk(KERN_INFO "%s: Driver registrado\n", DEVICE_NAME);
  printk(KERN_INFO "%s: Major asignado %d\n", DEVICE_NAME, Major);

  return 0;
}

void cleanup_module(void)
{ /* Destructor */
  unregister_chrdev(Major, DEVICE_NAME);
  printk(KERN_INFO "%s: Driver deregistrado\n", DEVICE_NAME);
}

static void caesar_encrypt(char *message, const int key)
{
  int i;
  char ch;
  for (i = 0; message[i] != '\0'; ++i)
  {
    ch = message[i];

    if (ch >= 'a' && ch <= 'z')
    {
      ch = ch + key;

      if (ch > 'z')
      {
        ch = ch - 'z' + 'a' - 1;
      }

      message[i] = ch;
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
      ch = ch + key;

      if (ch > 'Z')
      {
        ch = ch - 'Z' + 'A' - 1;
      }

      message[i] = ch;
    }
  }
}

static int device_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "%s: open", DEVICE_NAME);
  msg_print = kernel_buffer;
  try_module_get(THIS_MODULE);
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "%s: release", DEVICE_NAME);
  module_put(THIS_MODULE);
  return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;

  printk(KERN_INFO "%s: read", DEVICE_NAME);

  // printk("%s : Data leida desde [kernel]: %s\n", DEVICE_NAME, kernel_buffer);

  if (*msg_print == 0)
  {
    return 0;
  }

  // Se copian los datos desde el `kernel` al `usuario`
  while (length && *msg_print)
  {
    put_user(*(msg_print++), buffer++);
    length--;
    bytes_read++;
  }

  return bytes_read;
}

static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
  printk(KERN_INFO "%s: write", DEVICE_NAME);

  kernel_buffer_size = length;
  if (kernel_buffer_size > BUFFER_MAX_SIZE)
  {
    kernel_buffer_size = BUFFER_MAX_SIZE;
  }

  // Se copian los datos desde el usuario hacia el `kernel`
  if (copy_from_user(kernel_buffer, buffer, kernel_buffer_size))
  {
    return -EFAULT;
  }

  kernel_buffer[kernel_buffer_size] = 0;

  //printk("%s : Data leida desde [user] %s\n", DEVICE_NAME, kernel_buffer);

  caesar_encrypt(kernel_buffer, CAESAR_SHIFT);

  return kernel_buffer_size;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("UNGS");
MODULE_DESCRIPTION("Un primer driver");
