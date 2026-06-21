#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vinod");
MODULE_DESCRIPTION("LED driver using Device Tree + gpiod API for Pi 5");
MODULE_VERSION("2.0");

#define DEVICE_NAME "myled"
#define CLASS_NAME  "led"

static int major_number;
static struct class *led_class = NULL;
static struct device *led_device = NULL;
static struct gpio_desc *led_gpiod = NULL;

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "myled: Device opened\n");
    return 0;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer,
                          size_t len, loff_t *offset)
{
    char cmd;
    if (copy_from_user(&cmd, buffer, 1))
        return -EFAULT;

    if (cmd == '1') {
        gpiod_set_value(led_gpiod, 1);
        printk(KERN_INFO "myled: LED ON\n");
    } else if (cmd == '0') {
        gpiod_set_value(led_gpiod, 0);
        printk(KERN_INFO "myled: LED OFF\n");
    }
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "myled: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .write   = dev_write,
    .release = dev_release,
};

static int led_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "myled: probe() called - DT node matched!\n");

    led_gpiod = gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpiod)) {
        printk(KERN_ALERT "myled: Failed to get LED gpiod\n");
        return PTR_ERR(led_gpiod);
    }

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        gpiod_put(led_gpiod);
        return major_number;
    }

    led_class = class_create(CLASS_NAME);
    if (IS_ERR(led_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        gpiod_put(led_gpiod);
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, MKDEV(major_number, 0),
                                NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        gpiod_put(led_gpiod);
        return PTR_ERR(led_device);
    }

    printk(KERN_INFO "myled: Driver initialized, /dev/myled created\n");
    return 0;
}

static void led_remove(struct platform_device *pdev)
{
    gpiod_set_value(led_gpiod, 0);
    gpiod_put(led_gpiod);
    device_destroy(led_class, MKDEV(major_number, 0));
    class_destroy(led_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "myled: Driver unloaded\n");
}

static const struct of_device_id led_of_match[] = {
    { .compatible = "vinod,led-driver", },
    { }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_platform_driver = {
    .probe  = led_probe,
    .remove = led_remove,
    .driver = {
        .name           = "led_driver_v2",
        .of_match_table = led_of_match,
    },
};

module_platform_driver(led_platform_driver);
