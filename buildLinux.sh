#!/bin/bash

# Navigate to 'linux-stable' directory
cd linux-stable || { echo "Failed to change directory to: linux-stable"; exit 1; }

# ensure the working directories are clean
# Clean the build using make clean and remove config files if they exist
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean || { echo "Failed to clean"; exit 1; }
rm -f .config .config.old Module.symvers || { echo "Failed to remove .config, .config.old, or Module.symvers"; exit 1; }

# copy in our specific config file
# Copy the specific config file and check if operation was successful
cp  ../../../../../camkes-vm-images/qemu-arm-virt/linux_configs/config .config || { echo "Failed to copy config file"; exit 1; }
cp .config .config.old || { echo "Failed to backup .config file"; exit 1; }

# when our config file doesn't account for some of this kernel version's
# options, choose the default everywhere.
# Update the config file with default options for new options
yes "" | make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- oldconfig || { echo "Failed to update .config file"; exit 1; }

# prepare the source tree for compilation
# Prepare the source tree for compilation and check if operation was successful
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- prepare || { echo "Failed to prepare source tree for compilation"; exit 1; }

# compile the kernel using all available CPU cores
# Compile the kernel and check if operation was successful
make -j$(( $(nproc)-1 )) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- || { echo "Failed to compile the kernel"; exit 1; }
#echo "Enter the linux-stable directory and issue this command to build the Linux kernel."
#echo "X is the number of threads to use."
#echo "make -jX ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-"
