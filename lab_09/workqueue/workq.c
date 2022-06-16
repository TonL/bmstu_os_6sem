#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tonkoshtan Andrey");

// Последовательность действий - создать очередь работ, инициализовать ворки, добавить ворки в очередь


#define KEYB_IRQ 1
#define LEN 84
#define KBD_DATA_REG 0x60
#define KBD_SCANCODE_MASK 0x7f
#define KBD_STATUS_MASK 0x80

static char *ASCII[] = {
	"[ESC]", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "backSpace", "[Tab]", "Q",
    "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "[Enter]", "[CTRL]", "A", "S", "D",
    "F", "G", "H", "J", "K", "L", ";", "\'", "`", "[LShift]", "\\", "Z", "X", "C", "V", "B", "N", "M",
    ",", ".", "/", "[RShift]", "[PrtSc]", "[Alt]", " ", "[Caps]", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "[Num]", "[Scroll]", "[Home(7)]", "[Up(8)]", "[PgUp(9)]", "-",
    "[Left(4)]", "[Center(5)]", "[Right(6)]", "+", "[End(1)]", "[Down(2)]", "[PgDn(3)]", "[Ins]", "[Del]"};

struct workqueue_struct *workqueue;
struct work_struct firstWork;
struct work_struct secondWork;

int tmp;
char scancode;
int status;
char *key;

void firstWorkHandler(struct work_struct *work)
{
	
    status = scancode & KBD_STATUS_MASK;
	if (!(status))
	{
		key = ASCII[(scancode & KBD_SCANCODE_MASK) - 1];
		printk(KERN_INFO "+ First Worker. Pressed key = %s", key);
	}
}

void secondWorkHandler(struct work_struct *work)
{
	printk(KERN_INFO "+ Second worker (before sleep)");
	msleep(100);
	printk(KERN_INFO "+ Second worker (after sleep):");
}

irqreturn_t irq_handler(int irq, void *dev_id) 
{
	if (irq == KEYB_IRQ)
	{
		scancode = inb(KBD_DATA_REG);
		queue_work(workqueue, &firstWork);
		queue_work(workqueue, &secondWork);
		return IRQ_HANDLED;
	}
	else
		return IRQ_NONE;
}

static int __init work_init(void) 
{
	int ret = request_irq(KEYB_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED, "interrupt", &tmp);
	if (ret) 
	{
		printk (KERN_DEBUG "request irq failed\n");	
		return -1;
	}
	
	workqueue = create_workqueue("WorkQueue");

	if (!workqueue)
	{
		free_irq(KEYB_IRQ, &tmp);
        printk(KERN_INFO "+ Error: workqueue wasn't created");
        return -ENOMEM;
	}
	INIT_WORK(&firstWork, firstWorkHandler);
	INIT_WORK(&secondWork, secondWorkHandler);

	printk(KERN_DEBUG "+ KEYBOARD IRQ handler was registered successfully \n");
	return 0;
}

static void __exit work_exit(void) 
{
	flush_workqueue(workqueue);
	destroy_workqueue(workqueue);
 	free_irq(KEYB_IRQ, &tmp);
	printk (KERN_DEBUG "module with works unloaded!\n");
}

module_init(work_init);
module_exit(work_exit);
