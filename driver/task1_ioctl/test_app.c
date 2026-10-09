#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "asgn1_ioctl.h" // Nhúng file header để lấy magic numbers

int main() {
    int fd = open("/dev/asgn1", O_RDWR);
    if (fd < 0) {
        printf("Loi: Khong the mo node thiet bi /dev/asgn1\n");
        return -1;
    }

    printf("\n=== TEST 4 LENH IOCTL ===\n");

    // Test 1: Lấy thông tin Version
    char version[64];
    if (ioctl(fd, ASGN1_GET_VERSION, version) == 0) {
        printf("[1] Version Driver: %s\n", version);
    }

    // Test 2: Đổi Mode hoạt động
    if (ioctl(fd, ASGN1_SET_MODE, 1) == 0) {
        printf("[2] Da chuyen sang Echo Mode (Mode 1).\n");
    }

    // Test 3: Lấy thống kê
    struct asgn1_stats stats;
    if (ioctl(fd, ASGN1_GET_STATS, &stats) == 0) {
        printf("[3] Thong ke: Read=%d, Write=%d, Kich thuoc Buffer=%d\n", 
               stats.read_count, stats.write_count, stats.buffer_len);
    }

    // Test 4: Reset dữ liệu
    if (ioctl(fd, ASGN1_RESET_BUFFER) == 0) {
        printf("[4] Da xoa buffer va reset bộ đếm thanh cong.\n");
    }

    close(fd);
    printf("=========================\n\n");
    return 0;}
