#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 
#include <linux/mutex.h>   
#include <linux/string.h>  // Để dùng hàm xử lý chuỗi
#include "asgn1_ioctl.h"   

#define DEVICE_NAME "asgn1"
#define MAJOR_NUM 241 

static char device_buffer[1024];
static struct asgn1_stats drv_stats;
static int current_mode = 1; 
static DEFINE_MUTEX(asgn1_mutex); 

// --- CÁC HÀM READ / WRITE CƠ BẢN ---
static int asgn1_open(struct inode *inode, struct file *file) {
    mutex_lock(&asgn1_mutex);
    drv_stats.open_count++;
    mutex_unlock(&asgn1_mutex);
    return 0;
}

static int asgn1_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t asgn1_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    int len;
    mutex_lock(&asgn1_mutex);
    
    len = strlen(device_buffer);
    if (*f_pos >= len) {
        mutex_unlock(&asgn1_mutex);
        return 0; // Đã đọc hết (EOF)
    }
    
    if (count > len - *f_pos) {
        count = len - *f_pos;
    }
    
    if (copy_to_user(buf, device_buffer + *f_pos, count)) {
        mutex_unlock(&asgn1_mutex);
        return -EFAULT;
    }
    
    *f_pos += count;
    drv_stats.read_count++;
    mutex_unlock(&asgn1_mutex);
    return count;
}

static ssize_t asgn1_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    char temp_buf[512];
    
    // Giới hạn kích thước để không tràn buffer
    if (count > 512) count = 512; 
    
    if (copy_from_user(temp_buf, buf, count)) {
        return -EFAULT;
    }
    temp_buf[count] = '\0'; // Đảm bảo kết thúc chuỗi

    mutex_lock(&asgn1_mutex);
    
    // Logic xử lý Mode 0 và Mode 1
    if (current_mode == 0) {
        // Mode 0: Kernel mode - Thêm tiền tố
        snprintf(device_buffer, sizeof(device_buffer), "[KERNEL]%s", temp_buf);
    } else {
        // Mode 1: Echo mode - Lưu nguyên bản
        snprintf(device_buffer, sizeof(device_buffer), "%s", temp_buf);
    }

    drv_stats.write_count++;
    drv_stats.last_write_size = count;
    drv_stats.buffer_len = strlen(device_buffer);
    
    mutex_unlock(&asgn1_mutex);
    return count;
}

// --- HÀM XỬ LÝ IOCTL ---
static long asgn1_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    long ret = 0;
    mutex_lock(&asgn1_mutex);

    switch(cmd) {
        case ASGN1_RESET_BUFFER:
            device_buffer[0] = '\0';
            drv_stats.write_count = 0;
            drv_stats.read_count = 0;
            drv_stats.buffer_len = 0;
            drv_stats.last_write_size = 0;
            break;

        case ASGN1_GET_STATS:
            if (copy_to_user((struct asgn1_stats *)arg, &drv_stats, sizeof(struct asgn1_stats))) {
                ret = -EFAULT;
            }
            break;

        case ASGN1_SET_MODE:
            if (arg == 0 || arg == 1) {
                current_mode = arg;
            } else {
                ret = -EINVAL; 
            }
            break;

        case ASGN1_GET_VERSION:
            {
                char version_str[64] = "asgn1_driver v1.0 - Tran Dinh Khoi Nguyen - SE200558";
                if (copy_to_user((char *)arg, version_str, sizeof(version_str))) {
                    ret = -EFAULT;
                }
            }
            break;

        default:
            ret = -ENOTTY;
            break;
    }

    mutex_unlock(&asgn1_mutex);
    return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = asgn1_open,
    .release = asgn1_release,
    .read = asgn1_read,
    .write = asgn1_write,
    .unlocked_ioctl = asgn1_ioctl, 
};

static int __init asgn1_init(void) {
    return register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
}

static void __exit asgn1_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init(asgn1_init);
module_exit(asgn1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Dinh Khoi Nguyen - SE200558");
MODULE_DESCRIPTION("Assignment 1 - ioctl driver");
