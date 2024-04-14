#!/bin/bash
# Accept input as a command line argument
opt=$1
# Check if the argument was given
if [ -z "$opt" ]; then
    echo "No Linux kernel version provided. Note that 4.9.y Linux is no longer supported."
    exit 1
fi
# Array of options
options=("4.9.y" "4.14.y" "4.19.y" "5.4.y" "5.10.y" "5.15.y" "6.1.y")
# Check if the argument is a valid option
valid=false
for i in "${options[@]}"; do
    if [ "$i" == "$opt" ]; then
        valid=true
        break
    fi
done
if $valid; then
    echo "You chose version $opt"
else
    echo "Invalid option $opt"
    exit 1
fi
# Clone the Linux kernel with the chosen version
cd attarch || { echo "Failed to change directory to: attarch"; exit 1; }

git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable --branch linux-${opt} || { echo "Failed to clone Linux kernel"; exit 1; }

# Navigate to 'linux-stable' directory
cd linux-stable || { echo "Failed to change directory to: linux-stable"; exit 1; }

# Clean the build using make clean and remove config files if they exist
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean || { echo "Failed to clean"; exit 1; }
rm -f .config .config.old Module.symvers || { echo "Failed to remove .config, .config.old, or Module.symvers"; exit 1; }

# Copy the specific config file and check if operation was successful
cp  ../../../../../camkes-vm-images/qemu-arm-virt/linux_configs/config .config || { echo "Failed to copy config file"; exit 1; }
cp .config .config.old || { echo "Failed to backup .config file"; exit 1; }

# when our config file doesn't account for some of this kernel version's
# options, choose the default everywhere.
yes "" | make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- oldconfig || { echo "Failed to update .config file"; exit 1; }

# prepare the source tree for compilation
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- prepare || { echo "Failed to prepare source tree for compilation"; exit 1; }

# compile the kernel using all available CPU cores sans one
make -j$(( $(nproc)-1 )) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- || { echo "Failed to compile the kernel"; exit 1; }

