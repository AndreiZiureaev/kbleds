#include <linux/module.h>
#include <linux/vt_kern.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_AUTHOR("Andrei Ziureaev");
MODULE_LICENSE("GPL");

#define PREFIX "kbleds: "

#define BLINK_DELAY HZ/2
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

#define BUFLEN 100

static char buf[BUFLEN];
static struct timer_list my_timer;
static struct tty_driver *my_driver;
static char kbledstatus = 0;

/* Print a line to the console from which the function was called. */
static void print_string(char *str)
{
    struct tty_struct *my_tty = get_current_tty();

    if (my_tty == NULL)
        return;

    my_tty->driver->ops->write(my_tty, str, strlen(str));
    // print CRLF
    my_tty->driver->ops->write(my_tty, "\015\012", 2);
}

static void my_timer_func(struct timer_list *unused)
{
    if (kbledstatus == ALL_LEDS_ON)
        kbledstatus = RESTORE_LEDS;
    else
        kbledstatus = ALL_LEDS_ON;

    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, kbledstatus);

    if (mod_timer(&my_timer, jiffies + BLINK_DELAY))
    {
        printk(KERN_ALERT PREFIX "existing timer modified. Shouldn't happen.");
    }
}

static int __init kbleds_init(void)
{
    print_string(PREFIX "loading...");
    scnprintf(buf, BUFLEN, PREFIX "fg_console is %x.", fg_console);
    print_string(buf);

    if (!vc_cons[fg_console].d)
        return -1;

    scnprintf(buf, BUFLEN, PREFIX "console[%i/%i] #%i, tty %lx.",
              fg_console,
              MAX_NR_CONSOLES,
              vc_cons[fg_console].d->vc_num,
              (unsigned long)vc_cons[fg_console].d->port.tty);
    print_string(buf);

    my_driver = vc_cons[fg_console].d->port.tty->driver;
    scnprintf(buf, BUFLEN, PREFIX "tty driver magic %x.", my_driver->magic);
    print_string(buf);

    timer_setup(&my_timer, my_timer_func, 0);
    mod_timer(&my_timer, jiffies + BLINK_DELAY);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    print_string(PREFIX "unloading...");
    if (del_timer(&my_timer))
    {
        print_string(PREFIX "deleted active timer.");
    }
    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
