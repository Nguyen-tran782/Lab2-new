#ifndef ASGN1_IOCTL_H
#define ASGN1_IOCTL_H

#include <linux/ioctl.h>

/* Định nghĩa cấu trúc thống kê sẽ trả về userspace */
struct asgn1_stats {
    int open_count;
    int write_count;
    int read_count;
    int buffer_len;
    int last_write_size;
};

/* 
 * Định nghĩa Magic Number. 
 * Giống như một ID riêng biệt cho driver của bạn để kernel không nhầm với driver khác.
 * Ở đây chọn ký tự 'A' (từ Asgn1)
 */
#define ASGN1_MAGIC 'A'

/* 
 * Định nghĩa 4 ioctl commands theo yêu cầu Task 1
 * _IO: Lệnh không truyền dữ liệu (chỉ ra lệnh)
 * _IOR: Lệnh đọc dữ liệu từ Kernel về Userspace
 * _IOW: Lệnh ghi dữ liệu từ Userspace xuống Kernel
 */
#define ASGN1_RESET_BUFFER _IO(ASGN1_MAGIC, 0)
#define ASGN1_GET_STATS    _IOR(ASGN1_MAGIC, 1, struct asgn1_stats)
#define ASGN1_SET_MODE     _IOW(ASGN1_MAGIC, 2, int)
#define ASGN1_GET_VERSION  _IOR(ASGN1_MAGIC, 3, char[64])

#endif
