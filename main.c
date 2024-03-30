// *************************************
// Cora Filesystem Traverser
// Module Initialization
// Environment: Kernel space
// 'main.c'
// Author: jcjuarez
// *************************************

#include <linux/fs.h>
#include <linux/err.h> 
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>

//
// Global reference to the filesystem metadata dispatcher.
//
static struct task_struct* g_filesystem_metadata_dispatcher_thread;

//
// Directory to be traversed by the metadata dispatcher.
//
#define DIRECTORY_PATH "/home/jcjuarez/test"

//
// Sleep time in milliseconds for the ilesystem metadata dispatcher across traversals.
//
#define FILESYSTEM_METADATA_DISPATCHER_THREAD_SLEEP_MS 1000u

//
// Prints the filesystem entry metadata to the kernel ring buffer.
//
static
bool
print_filesystem_entry(
    struct dir_context* p_context,
    const char* p_name,
    int p_name_length,
    loff_t p_offset,
    u64 p_ino,
    unsigned int p_dt_entry_type)
{
    switch (p_dt_entry_type)
    {
        case DT_REG:

            printk(KERN_INFO "Found file: %.*s.\n", p_name_length, p_name);
            break;

        case DT_DIR:

            printk(KERN_INFO "Found directory: %.*s.\n", p_name_length, p_name);
            break;

        case DT_LNK:

            printk(KERN_INFO "Found symlink: %.*s.\n", p_name_length, p_name);
            break;

        default:

            printk(KERN_INFO "Found other: %.*s, type: %d.\n", p_name_length, p_name, p_dt_entry_type);
            break;
    }

    return true;
}

//
// Filesystem metadata dispatcher function. Started by the main kernel thread
// handle at the module initialization and is stopped at module offloading time.
//
static
int
filesystem_metadata_dispatcher(
    void *p_data)
{
    printk(KERN_INFO "Filesystem metadata dispatcher thread has started.\n");

    while (!kthread_should_stop())
    {
        struct file *file;
        struct dir_context ctx = { .actor = print_filesystem_entry, };

        printk(KERN_INFO "Traversing directory: %s\n", DIRECTORY_PATH);

        file = filp_open(DIRECTORY_PATH, O_RDONLY, 0);

        if (IS_ERR(file))
        {
            printk(KERN_ERR "Could not open %s\n", DIRECTORY_PATH);
            return PTR_ERR(file);
        }

        //
        // Iterate directory through a callback context.
        //
        iterate_dir(file, &ctx);

        filp_close(file, NULL);

        msleep(FILESYSTEM_METADATA_DISPATCHER_THREAD_SLEEP_MS);
    }

    printk(KERN_INFO "Filesystem metadata dispatcher thread has stopped.\n");
    return 0;
}

//
// Main entry point for the module.
//
static
int
__init
cora_init(
    void)
{
    printk(KERN_INFO "Cora filesystem traverser module loading process started.\n");

    g_filesystem_metadata_dispatcher_thread = kthread_create(
        filesystem_metadata_dispatcher,
        NULL,
        "filesystem_metadata_dispatcher");

    if (IS_ERR(g_filesystem_metadata_dispatcher_thread))
    {
        printk(KERN_ERR "Error creating filesystem metadata dispatcher kernel thread.\n");
        return PTR_ERR(g_filesystem_metadata_dispatcher_thread);
    }

    //
    // Wake up the kernel thread. All the module logic will be
    // redirected towards it and the initialization will complete.
    //
    wake_up_process(g_filesystem_metadata_dispatcher_thread);

    printk(KERN_INFO "Cora filesystem traverser module has been loaded.\n");

    return 0;
}

//
// Exit point for the module. Handles system cleanup.
//
static
void
__exit
cora_exit(
    void)
{
    if (g_filesystem_metadata_dispatcher_thread)
    {
        printk(KERN_INFO "Stopping the filesystem metadata dispatcher thread.\n");
        kthread_stop(g_filesystem_metadata_dispatcher_thread);
    }

    printk(KERN_INFO "Cora filesystem traverser module has been unloaded.\n");
}

module_init(cora_init);
module_exit(cora_exit);

MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jcjuarez");
MODULE_DESCRIPTION("Filesystem traversal module");
