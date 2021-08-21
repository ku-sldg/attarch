The Platform AM. Listens on the "am_dp" dataport".

## Issues

The CAmkES component compilation does not look in /usr/lib/gcc-cross/aarch64-linux-gnu-gcc/8/include, and does not allow one to add the path to the include list. Hence, the contents are copied into the `include` directory here.