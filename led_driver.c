#include <linux/module.h>       // Required for all kernel modules (MODULE_LICENSE etc)
#include <linux/kernel.h>       // printk() - kernel's version of printf
#include <linux/init.h>         // __init and __exit macros
#include <linux/fs.h>           // file_operations struct - for open/read/write/close
#include <linux/uaccess.h>      // copy_from_user() - safely copy data from userspace
#include <linux/gpio.h>         // gpio_request, gpio_set_value etc
#include <linux/device.h>       // class_create, device_create - to auto-create /dev entry
#include <linux/cdev.h>         // cdev struct - character device registration

#define DEVICE_NAME "myled"     // Name that appears in /dev/myled
#define CLASS_NAME  "led"       // Name of device class in /sys/class/
#define GPIO_LED    17          // GPIO pin number (BCM numbering)

// Module metadata - mandatory for every kernel module
MODULE_LICENSE("GPL");          // Must be GPL to use kernel functions freely
MODULE_AUTHOR("Vinod_Gaddam");
MODULE_DESCRIPTION("Simple LED GPIO Device Driver");
MODULE_VERSION("1.0");

// Global variables for the driver
static int    major_number;             // Kernel assigns this - identifies our device
static struct class*  led_class = NULL; // Device class pointer
static struct device* led_device = NULL;// Device pointer

// ─────────────────────────────────────────────
// FILE OPERATION FUNCTIONS
// These are called when userspace does open/write/read/close on /dev/myled
// ─────────────────────────────────────────────

// Called when userspace opens /dev/myled
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "myled: Device opened\n");
    // printk is the kernel's logger - view with: dmesg | tail
    return 0;
}

// Called when userspace writes to /dev/myled
// e.g: echo "1" > /dev/myled  (LED ON)
// e.g: echo "0" > /dev/myled  (LED OFF)
static ssize_t dev_write(struct file *filep, const char __user *buffer,
                         size_t len, loff_t *offset)
{
    char cmd;

    // copy_from_user() safely copies data from userspace to kernel space
    // Direct pointer access is NOT allowed in kernel - userspace memory is virtual
    if (copy_from_user(&cmd, buffer, 1)) {
        return -EFAULT;  // Return error if copy fails
    }

    if (cmd == '1') {
        gpio_set_value(GPIO_LED, 1);   // Set GPIO HIGH → LED ON
        printk(KERN_INFO "myled: LED ON\n");
    } else if (cmd == '0') {
        gpio_set_value(GPIO_LED, 0);   // Set GPIO LOW → LED OFF
        printk(KERN_INFO "myled: LED OFF\n");
    } else {
        printk(KERN_WARNING "myled: Invalid command. Use '1' or '0'\n");
    }

    return len;  // Return number of bytes written (required by kernel)
}

// Called when userspace closes /dev/myled
static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "myled: Device closed\n");
    return 0;
}

// ─────────────────────────────────────────────
// FILE OPERATIONS STRUCT
// This tells the kernel which functions handle which system calls
// It's like a vtable / function pointer table
// ─────────────────────────────────────────────
static struct file_operations fops = {
    .owner   = THIS_MODULE,   // Prevents module from being unloaded while in use
    .open    = dev_open,
    .write   = dev_write,
    .release = dev_release,
};

// ─────────────────────────────────────────────
// INIT FUNCTION - runs when you do: sudo insmod led_driver.ko
// ─────────────────────────────────────────────
static int __init led_driver_init(void)
{
    printk(KERN_INFO "myled: Initializing driver\n");

    // Step 1: Allocate a major number dynamically
    // Major number identifies which driver handles /dev/myled
    // Minor number identifies which instance (we only have one LED)
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "myled: Failed to register major number\n");
        return major_number;
    }
    printk(KERN_INFO "myled: Registered with major number %d\n", major_number);

    // Step 2: Create a device class - appears in /sys/class/led/
    // This allows udev to auto-create /dev/myled
    led_class = class_create(CLASS_NAME);
    if (IS_ERR(led_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "myled: Failed to create class\n");
        return PTR_ERR(led_class);
    }

    // Step 3: Create the actual device node /dev/myled
    led_device = device_create(led_class, NULL, MKDEV(major_number, 0),
                               NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "myled: Failed to create device\n");
        return PTR_ERR(led_device);
    }

    // Step 4: Request GPIO pin from the kernel
    // gpio_request() tells the kernel "I am using GPIO 17, don't let others use it"
    if (!gpio_is_valid(GPIO_LED)) {
        printk(KERN_ALERT "myled: Invalid GPIO pin\n");
        return -ENODEV;
    }

    gpio_request(GPIO_LED, "sysfs");         // Reserve GPIO 17
    gpio_direction_output(GPIO_LED, 0);      // Set as OUTPUT, initial value LOW

    printk(KERN_INFO "myled: Driver initialized successfully\n");
    return 0;
}

// ─────────────────────────────────────────────
// EXIT FUNCTION - runs when you do: sudo rmmod led_driver
// Always clean up everything you allocated in init
// ─────────────────────────────────────────────
static void __exit led_driver_exit(void)
{
    gpio_set_value(GPIO_LED, 0);         // Turn LED off before unloading
    gpio_free(GPIO_LED);                 // Release GPIO pin back to kernel

    device_destroy(led_class, MKDEV(major_number, 0));  // Remove /dev/myled
    class_destroy(led_class);                            // Remove /sys/class/led
    unregister_chrdev(major_number, DEVICE_NAME);        // Free major number

    printk(KERN_INFO "myled: Driver unloaded\n");
}

// Register the init and exit functions with the kernel
module_init(led_driver_init);
module_exit(led_driver_exit);
