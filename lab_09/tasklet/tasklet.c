#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey");
MODULE_DESCRIPTION("tasklet");

#define SCANCODE_MASK 0x7f
#define STATUS_MASK 0x80
#define IRQ 1
static int dev_id;
static int scancode, status;

static char *ascii_map[] = {
    "[ESC]", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "bs", "[Tab]", "Q",
    "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "[Enter]", "[CTRL]", "A", "S", "D",
    "F", "G", "H", "J", "K", "L", ";", "\'", "`", "[LShift]", "\\", "Z", "X", "C", "V", "B", "N", "M",
    ",", ".", "/", "[RShift]", "[PrtSc]", "[Alt]", " ", "[Caps]", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "[Num]", "[Scroll]", "[Home(7)]", "[Up(8)]", "[PgUp(9)]", "-",
    "[Left(4)]", "[Center(5)]", "[Right(6)]", "+", "[End(1)]", "[Down(2)]", "[PgDn(3)]", "[Ins]", "[Del]"};

char my_tasklet_data[] = "KEYBOARD INTERRUPT";

void my_tasklet_handler(struct tasklet_struct *t) // нижняя половина ()
{
    status = scancode & STATUS_MASK;
    if (!status)
    {
        char *key = ascii_map[(scancode & SCANCODE_MASK) - 1];
        printk(KERN_INFO "my_tasklet: symbol - %s, state - %ld, count - %d, data - %s\n", key, t->state, t->count, t->data);
    }
}

DECLARE_TASKLET(my_tasklet, my_tasklet_handler);

// тасклет будет выполнен
// - по завершении обработчика аппаратного прерывания,
// - на процессоре, на котором был выполнен обработчик прерывания

irqreturn_t my_interrupt_handler(int irq, void *dev) // обработчик апп прерывания (верхняя половина)
{
	if (irq == IRQ)
    {
        scancode = inb(0x60);
		tasklet_schedule(&my_tasklet);
        my_tasklet.data = (unsigned long) &my_tasklet_data; 
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static int __init my_tasklet_init(void)
{
	int ret = request_irq(IRQ, my_interrupt_handler, IRQF_SHARED, "my_interrupt_handler", &dev_id); // shared
	if (ret)
		printk(KERN_ERR "my_tasklet: my_interrupt_handler wasn't registered\n");
	else
		printk(KERN_INFO "my_tasklet: module loaded\n");
	return ret;
}

static void __exit my_tasklet_exit(void)
{
	tasklet_kill(&my_tasklet);
	free_irq(IRQ, &dev_id);
	printk(KERN_DEBUG "my_tasklet: module unloaded\n");
}

module_init(my_tasklet_init)
module_exit(my_tasklet_exit)

// аппаратное прерывание выполняется при
// - запрещенных прерываниях на данном процессоре
// - запрещенных прерываниях на других процессорах по данной линии IRQ

// Какие обязательные действия выполняют обработчики прерываний?
// - планирование тасклета
// - добавление в очередь воркера

// воркеры будут выполнены, когда добавляются в очередь

// кто отслеживает ворки? ((основной планировщик ядра линукс?))
// ворки тред, которые в цикле проверяет, может ли выполниться воркер
// как только дойдет очередь, то возьмет на выполнение