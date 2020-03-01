#include <linux/module.h>
// #include <linux/kernel.h>
// #include <linux/configfs.h>
// #include <linux/init.h>
// #include <linux/tty.h>
// #include <linux/kd.h>
// #include <linux/vt.h>
// #include <linux/console_struct.h>
#include <linux/vt_kern.h>
// #include <linux/timer.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_AUTHOR("Andrei Ziureaev");
MODULE_LICENSE("GPL");

#define BLINK_DELAY HZ/2
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

#define BUFLEN 100

static char buf[BUFLEN];
static struct timer_list my_timer;
static struct tty_driver *my_driver;
static char kbledstatus = 0;

// extern int fg_console;

static void print_string(char *str)
{
    struct tty_struct *my_tty = get_current_tty();

    if (my_tty == NULL)
        return;

    my_tty->driver->ops->write(my_tty, str, strlen(str));
    my_tty->driver->ops->write(my_tty, "\015\012", 2);
}

static void my_timer_func(struct timer_list * t)
{
    if (kbledstatus == ALL_LEDS_ON)
        kbledstatus = RESTORE_LEDS;
    else
        kbledstatus = ALL_LEDS_ON;

    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, kbledstatus);

    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
    int i;

    print_string("kbleds: loading\n");
    scnprintf(buf, BUFLEN, "kbleds: fgconsole is %x\n", fg_console);
    print_string(buf);

    for (i = 0; i < MAX_NR_CONSOLES; i++)
    {
        if (!vc_cons[i].d)
            break;
        scnprintf(buf, BUFLEN, "poet_atkm: console[%i/%i] #%i, tty %lx\n", i,
                  MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                  (unsigned long)vc_cons[i].d->port.tty);
        print_string(buf);
    }
    print_string("kbleds: finished scanning consoles\n");

    my_driver = vc_cons[fg_console].d->port.tty->driver;
    scnprintf(buf, BUFLEN, "kbleds: tty driver magic %x\n", my_driver->magic);
    print_string(buf);

    timer_setup_on_stack(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    print_string("kbleds: unloading...\n");
    del_timer_sync(&my_timer);
    destroy_timer_on_stack(&my_timer);
    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
