# Author: Michael Neises & ChatGPT 4
# Date 9 May 2023
# Purpose: Collect from System.map virtual addresses for several kernel symbols.

# Example output:

# #define INTRO___START_RODATA_VADDR ffff0000087c0000
# #define INTRO___START_DATA_RO_AFTER_INIT_VADDR ffff00000899d7dc
# #define INTRO___END_DATA_RO_AFTER_INIT_VADDR ffff00000899e160
# #define INTRO___END_RODATA_VADDR ffff000008bbf000
# #define INTRO_INIT_TASK_VADDR ffff000008f92280
# #define INTRO_MODULES_VADDR ffff000008fb61e0
# #define INTRO_PAGE_SIZE 0x1000
# #define INTRO_MODULE_NAME_LEN 56

# These are some of the specific definitions required for introspection.

import os
import re

def process_map_file(filename):
    with open(filename, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) == 3:
                symbol = parts[2]
                if symbol in ["__start_rodata", "__end_rodata", "modules", "init_task", "__start_ro_after_init", "__end_ro_after_init", "swapper_pg_dir"]:
                    print(f"#define INTRO_{symbol.upper()}_VADDR 0x{parts[0]}")
                # the following two conditions account for linux 4.9
                if symbol in ["__start_data_ro_after_init"]:
                    print(f"#define INTRO___START_RO_AFTER_INIT_VADDR 0x{parts[0]}")
                if symbol in ["__end_data_ro_after_init"]:
                    print(f"#define INTRO___END_RO_AFTER_INIT_VADDR 0x{parts[0]}")

def process_config_file(filename):
    with open(filename, 'r') as file:
        for line in file:
            if "PAGE_SHIFT" in line:
                parts = line.split('=')
                shift_val = int(parts[1])
                page_size = hex(1 << shift_val)
                print(f"#define INTRO_PAGE_SIZE {page_size}")
            if "CONFIG_ARM64_VA_BITS=" in line:
                parts = line.split('=')
                va_bits = int(parts[1])
                print(f"#define INTRO_VA_BITS {va_bits}")

def process_module_files(module_h, moduleparam_h, config):
    unsigned_long_size = 4  # Default to 32-bit
    with open(config, 'r') as file:
        for line in file:
            if "CONFIG_ARM64=y" in line:
                unsigned_long_size = 8  # Update to 64-bit if appropriate
                break

    with open(moduleparam_h, 'r') as file:
        for line in file:
            if "#define MAX_PARAM_PREFIX_LEN" in line:
                # Use regex to extract the expression within the parentheses
                prefix_len_expr = re.search(r'\((.*)\)', line).group(1)
                # Replace sizeof(unsigned long) with the actual size
                prefix_len_expr = prefix_len_expr.replace('sizeof(unsigned long)', str(unsigned_long_size))
                # Now we can safely use eval() as the expression is fully formed
                prefix_len = eval(prefix_len_expr)
                break

    with open(module_h, 'r') as file:
        for line in file:
            if "#define MODULE_NAME_LEN MAX_PARAM_PREFIX_LEN" in line:
                print(f"#define INTRO_MODULE_NAME_LEN {prefix_len}")



def main():
    map_file = './linux-stable/System.map'
    if os.path.exists(map_file):
        process_map_file(map_file)
    else:
        print(f"File {map_file} does not exist.")
        
    config_file = './linux-stable/.config'
    if os.path.exists(config_file):
        process_config_file(config_file)
    else:
        print(f"File {config_file} does not exist.")

    module_h_file = './linux-stable/include/linux/module.h'
    moduleparam_h_file = './linux-stable/include/linux/moduleparam.h'

    if os.path.exists(module_h_file) and os.path.exists(moduleparam_h_file) and os.path.exists(config_file):
        process_module_files(module_h_file, moduleparam_h_file, config_file)
    else:
        print("One or more files do not exist.")

if __name__ == "__main__":
    main()
