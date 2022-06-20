#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shatskiy Rostislav");

static int __init md_init(void) {
    printk("> MD load started");
    struct task_struct *task = &init_task;
    do {
        printk(KERN_INFO "> pid=%d, name=%s, ppid=%d, name_p=%s, state=%ld, prio=%d\n",
                task->pid, task->comm, task->parent->pid, task->parent->comm, task->state, task->prio);
    } while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO "> current pid=%d, name=%s, ppid=%d, name_p=%s, state=%ld, prio=%d\n",
           current->pid, current->comm, current->parent->pid, current->parent->comm, task->state, task->prio);
    printk(KERN_INFO "> MD loaded");
    return 0;
}

static void __exit md_exit(void) {
    printk("> MD exited\n");
}

module_init(md_init);
module_exit(md_exit);