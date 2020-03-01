#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h> /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>  /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h> /* For vc_cons */
#include <linux/vt_kern.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_AUTHOR("Andrei Ziureaev");
MODULE_LICENSE("GPL");

#define BLINK_DELAY HZ / 5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

#define BUFLEN 100

static char buf[BUFLEN];
struct timer_list my_timer;
struct tty_driver *my_driver;
char kbledstatus = 0;

extern int fg_console;

static void print_string(char *str)
{
    struct tty_struct *my_tty = get_current_tty();

    if (my_tty == NULL)
        return;

    my_tty->driver->ops->write(my_tty, str, strlen(str));
    my_tty->driver->ops->write(my_tty, "\015\012", 2);
}

/*
 * Function my_timer_func blinks the keyboard LEDs periodically by invoking
 * command KDSETLED of ioctl() on the keyboard driver. To learn more on virtual
 * terminal ioctl operations, please see file:
 *     /usr/src/linux/drivers/char/vt_ioctl.c, function vt_ioctl().
 *
 * The argument to KDSETLED is alternatively set to 7 (thus causing the led
 * mode to be set to LED_SHOW_IOCTL, and all the leds are lit) and to 0xFF
 * (any value above 7 switches back the led mode to LED_SHOW_FLAGS, thus
 * the LEDs reflect the actual keyboard status).  To learn more on this,
 * please see file:
 *     /usr/src/linux/drivers/char/keyboard.c, function setledstate().
 *
 */

static void my_timer_func(unsigned long ptr)
{
    int *pstatus = (int *)ptr;

    if (*pstatus == ALL_LEDS_ON)
        *pstatus = RESTORE_LEDS;
    else
        *pstatus = ALL_LEDS_ON;

    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, *pstatus);

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

    init_timer(&my_timer);
    my_timer.function = my_timer_func;
    my_timer.data = (unsigned long)&kbledstatus;
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    print_string("kbleds: unloading...\n");
    del_timer(&my_timer);
    my_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
