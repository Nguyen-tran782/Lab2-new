#!/bin/sh

echo "=== BAT DAU KIEM THU TASK 2 (MISC DEVICE) ==="

echo "[1] Nạp module counter_driver.ko..."
insmod counter_driver.ko

echo ""
echo "[2] Kiem tra su ton tai cua /dev/counter"
ls -l /dev/counter

echo ""
echo "[3] Test Read (Moi lan doc tang len 1)"
echo "Doc lan 1: " && cat /dev/counter
echo "Doc lan 2: " && cat /dev/counter
echo "Doc lan 3: " && cat /dev/counter

echo ""
echo "[4] Test Write (Dat gia tri ve 100)"
echo "100" > /dev/counter
echo "Doc sau khi Write 100: " && cat /dev/counter

echo ""
echo "[5] Test Write (Reset ve 0)"
echo "0" > /dev/counter
echo "Doc sau khi Reset: " && cat /dev/counter

echo ""
echo "[6] Hien thi thong ke tu /proc/counter_info"
cat /proc/counter_info

echo ""
echo "[7] Go bo module"
rmmod counter_driver
echo "=== KIEM THU HOAN TAT ==="
