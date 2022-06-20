#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tonkoshtan Andrey");

char* md1_data = "MD1_DATA_MSG";

extern char* md1_proc(void) { 
   return md1_data; 
} 

static char* md1_local(void) { 
   return md1_data; 
} 

extern char* g() {
   return md1_data; 
} 

EXPORT_SYMBOL(md1_data); 
EXPORT_SYMBOL(md1_proc); 

static int __init md_init(void) { 
   printk("> MD1 loaded\n");
   return 0; 
} 

static void __exit md_exit(void) { 
   printk("> MD1 exited\n");
} 

module_init(md_init); 
module_exit(md_exit);