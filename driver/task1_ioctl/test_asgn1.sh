#!/bin/sh

set -e
echo "=== BAT DAU KIEM THU TASK 1 ==="

# 1. Nạp driver
insmod asgn1_driver.ko
mknod /dev/asgn1 c 241 0
chmod 666 /dev/asgn1

# 2. Test các hàm Read/Write cơ bản
echo "Hello Embedded Linux" > /dev/asgn1
cat /dev/asgn1

# 3. Chạy file C userspace để gọi ioctl
./test_app

# 4. Xem log kernel
echo "=== LOG KERNEL (dmesg) ==="
dmesg | tail -n 15

# 5. Dọn dẹp
rm /dev/asgn1
rmmod asgn1_driver
echo "=== KIEM THU HOAN TAT ==="
