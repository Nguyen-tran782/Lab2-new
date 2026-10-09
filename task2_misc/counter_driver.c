#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/atomic.h>
#include <linux/slab.h>

// Định nghĩa các biến nguyên tử (atomic variables)
static atomic_t counter;
static atomic_t total_reads;
static atomic_t total_writes; // Bao gồm cả reset

// --- PROCFS INTERFACE ---
// Hàm hiển thị thống kê khi gọi cat /proc/counter_info
static int counter_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "=== COUNTER STATS ===\n");
    seq_printf(m, "Current Counter Value : %d\n", atomic_read(&counter));
    seq_printf(m, "Total Reads           : %d\n", atomic_read(&total_reads));
    seq_printf(m, "Total Writes/Resets   : %d\n", atomic_read(&total_writes));
    return 0;
}

static int counter_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, counter_proc_show, NULL);
}

static const struct proc_ops counter_proc_ops = {
    .proc_open    = counter_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// --- MISC DEVICE OPERATIONS ---
// Hàm đọc giá trị counter
static ssize_t counter_read(struct file *file, char __user *user_buf, size_t size, loff_t *ppos)
{
    char buf[32];
    int len;
    int current_val;

    // Xử lý offset (*ppos) để tránh vòng lặp vô tận khi dùng lệnh cat
    if (*ppos > 0)
        return 0;

    current_val = atomic_read(&counter);
    len = snprintf(buf, sizeof(buf), "%d\n", current_val);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    // Cập nhật offset và tự động tăng counter
    *ppos = len;
    atomic_inc(&counter);     // Auto-increment on read
    atomic_inc(&total_reads); // Cập nhật thống kê

    return len;
}

// Hàm ghi giá trị counter
static ssize_t counter_write(struct file *file, const char __user *user_buf, size_t size, loff_t *ppos)
{
    char buf[32];
    int val;
    size_t copy_len = min(size, sizeof(buf) - 1);

    if (copy_from_user(buf, user_buf, copy_len))
        return -EFAULT;
    
    buf[copy_len] = '\0'; // Đảm bảo chuỗi kết thúc đúng cách

    // Chuyển chuỗi ASCII thành số nguyên (String to Integer)
    if (kstrtoint(buf, 10, &val) != 0)
        return -EINVAL; // Lỗi nếu ghi không phải là số

    if (val < 0)
        return -EINVAL; // Lỗi nếu ghi số âm

    atomic_set(&counter, val);
    atomic_inc(&total_writes); // Cập nhật thống kê

    return size;
}

static const struct file_operations counter_fops = {
    .owner = THIS_MODULE,
    .read  = counter_read,
    .write = counter_write,
};

// Khai báo miscdevice struct
static struct miscdevice counter_misc_dev = {
    .minor = MISC_DYNAMIC_MINOR, // Kernel tự động cấp minor number
    .name  = "counter",          // Tên device node sẽ tạo ở /dev/counter
    .fops  = &counter_fops,
};

// --- INIT & EXIT ---
static int __init counter_init(void)
{
    int ret;

    // Khởi tạo các biến nguyên tử về 0
    atomic_set(&counter, 0);
    atomic_set(&total_reads, 0);
    atomic_set(&total_writes, 0);

    // Đăng ký misc device thay vì chrdev
    ret = misc_register(&counter_misc_dev);
    if (ret) {
        pr_err("counter_driver: Failed to register misc device\n");
        return ret;
    }

    // Tạo file procfs
    proc_create("counter_info", 0444, NULL, &counter_proc_ops);

    pr_info("counter_driver: Loaded successfully. Major=10, Minor=%d\n", counter_misc_dev.minor);
    return 0;
}

static void __exit counter_exit(void)
{
    remove_proc_entry("counter_info", NULL);
    misc_deregister(&counter_misc_dev);
    pr_info("counter_driver: Unloaded\n");
}

module_init(counter_init);
module_exit(counter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Dinh Khoi Nguyen - SE200558");
MODULE_DESCRIPTION("Misc Device Counter Driver for Task 2");
