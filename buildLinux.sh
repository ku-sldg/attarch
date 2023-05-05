cd linux &&

# ensure the working directories are clean
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean &&
(rm .config || true) &&
(rm .config.old || true) &&
(rm Module.symvers || true) &&

# copy in our specific config file
cp  ../../../../../camkes-vm-images/qemu-arm-virt/linux_configs/config .config &&
cp .config .config.old &&

# when our config file doesn't account for some of this kernel version's
# options, choose the default everywhere.
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- oldconfig &&

# prepare the source tree for compilation
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- prepare &&

# compile the kernel using all available CPU cores
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

