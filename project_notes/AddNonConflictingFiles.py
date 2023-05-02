# Author: ChatGPT 4 (Michael Neises)
# Date: 2023-05-01
# Description: This script provides a utility for merging two directories into a new directory.
# It handles file conflicts by logging the conflicting files in a separate log file.

# A note from Michael: I used this to help me rebase the introspection app. My
# attestation architecture sometimes bitrots inexplicably, and I never know 
# exactly how to fix it. I shouldn't have to do it again, but this script was
# very helpful so I wanted to log it here. In other words, sometimes my
# introspection toolchain breaks, and I don't always know why; an effective
# solution is to rebase from the vm_introspect app which is maintained within
# the seL4 main repositories.

import os
import shutil

def merge_dirs(src, dst, conflicts_log='conflicts.log'):
    with open(conflicts_log, 'w') as log:
        for root, dirs, files in os.walk(src):
            dst_dir = root.replace(src, dst)
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)

            for file in files:
                src_file = os.path.join(root, file)
                dst_file = os.path.join(dst_dir, file)

                if os.path.exists(dst_file):
                    log.write(f'Conflict: {src_file} and {dst_file}\n')
                else:
                    shutil.copy2(src_file, dst_file)

if __name__ == '__main__':
    dir1 = 'vm_introspect'
    dir2 = 'attarch'
    merged_dir = 'merged'

    # Copy the contents of dir1 to the merged directory
    shutil.copytree(dir1, merged_dir)

    # Merge the contents of dir2 into the merged directory
    merge_dirs(dir2, merged_dir)
