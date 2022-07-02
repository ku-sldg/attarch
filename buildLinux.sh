cd linux &&
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean &&
(rm .config || true) &&
(rm .config.old || true) &&
(rm Module.symvers || true) &&
cp  ../../../../../camkes-vm-images/qemu-arm-virt/linux_configs/config .config &&
cp .config .config.old &&
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- prepare &&
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

