#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shatskiy Rostislav");

static int __init md_init(void) {
   printk("> MD2 load started\n");
   printk("> Data exported from md1: %s\n", md1_data);
   printk("> String from md1_proc(): %s\n", md1_proc());
   return 0; 
} 

static void __exit md_exit(void) { 
   printk("> MD2 exited\n");
} 

module_init(md_init);
module_exit(md_exit);