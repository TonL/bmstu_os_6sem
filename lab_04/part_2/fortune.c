#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kuzmin Kirill");

#define DIRNAME "fortune_dir"
#define FILENAME "fortune"
#define SYMLINK "fortune_ln"
#define FILEPATH DIRNAME "/" FILENAME

static struct proc_dir_entry *fortune_dir = NULL;
static struct proc_dir_entry *fortune = NULL;
static struct proc_dir_entry *fortune_ln = NULL;

static char *cookie_pot;
static int next_fortune;
static int cookie_idx;

static char tmp[PAGE_SIZE];

ssize_t fortune_read(struct file *file, char __user *buf, size_t count,
                     loff_t *offp) {
  int length;

  printk("+fortune: read called\n");
  if (*offp > 0 || !cookie_idx) {
    printk(KERN_INFO "+fortune: offp case");
    return 0;
  }

  if (next_fortune >= cookie_idx)
    next_fortune = 0;

  length = snprintf(tmp, PAGE_SIZE, "%s\n", &cookie_pot[next_fortune]);

  if (copy_to_user(buf, tmp, length)) {
    printk(KERN_ERR "+fortune: copy_to_user error\n");
    return -EFAULT;
  }

  next_fortune += length;
  *offp += length;

  printk(KERN_INFO "+fortune: read successfully\n");
  return length;
}

ssize_t fortune_write(struct file *file, const char __user *buf, size_t length,
                      loff_t *offp) {
  if (length > PAGE_SIZE - cookie_idx + 1) {
    printk(KERN_ERR "+fortune: cookie_pot overflow error\n");
    return -ENOSPC;
  }

  if (copy_from_user(&cookie_pot[cookie_idx], buf, length)) {
    printk(KERN_ERR "+fortune: copy_to_user error\n");
    return -EFAULT;
  }

  cookie_idx += length;
  cookie_pot[cookie_idx - 1] = '\0';

  printk(KERN_INFO "+fortune: write successfully\n");
  return length;
}

int fortune_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "+fortune: called open\n");
  return 0;
}

int fortune_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "+fortune: called release\n");
  return 0;
}

static struct proc_ops fops = {.proc_read = fortune_read,
                               .proc_write = fortune_write,
                               .proc_open = fortune_open,
                               .proc_release = fortune_release};

static void freemem(void) {
  if (fortune_ln)
    remove_proc_entry(SYMLINK, NULL);

  if (fortune)
    remove_proc_entry(FILENAME, fortune_dir);

  if (fortune_dir)
    remove_proc_entry(DIRNAME, NULL);

  if (cookie_pot)
    vfree(cookie_pot);
}

static int __init fortune_init(void) {
  if (!(cookie_pot = vmalloc(PAGE_SIZE))) {
    freemem();
    printk(KERN_ERR "+fortune: error during vmalloc executing\n");
    return -ENOMEM;
  }

  memset(cookie_pot, 0, PAGE_SIZE);

  if (!(fortune_dir = proc_mkdir(DIRNAME, NULL))) {
    freemem();
    printk(KERN_ERR "+fortune: error during directory creation\n");
    return -ENOMEM;
  }

  if (!(fortune = proc_create(FILENAME, 0666, fortune_dir, &fops))) {
    freemem();
    printk(KERN_ERR "+fortune: error during file creation\n");
    return -ENOMEM;
  }

  if (!(fortune_ln = proc_symlink(SYMLINK, NULL, FILEPATH))) {
    freemem();
    printk(KERN_ERR "+fortune: error during symlink creation\n");
    return -ENOMEM;
  }

  cookie_idx = 0;
  next_fortune = 0;

  printk(KERN_INFO "+fortune: module loaded successfully\n");

  return 0;
}

static void __exit fortune_exit(void) {
  freemem();
  printk(KERN_INFO "+fortune: module unloaded successfully\n");
}

module_init(fortune_init); 
module_exit(fortune_exit);