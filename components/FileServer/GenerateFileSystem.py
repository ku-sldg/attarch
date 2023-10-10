import os
import shutil

def convert_to_array(data):
    content = []
    for ch in data:
        if ch == '\n':
            content.append("'\\n'")
        elif ch == '\'':  # apostrophe
            content.append("'\\''")
        else:
            content.append(f"'{ch}'")
    content.append("'\\0'")  # null terminator
    return ", ".join(content)

def sanitize_file_name(name):
    """Converts forward slashes to underscores to make a valid C variable."""
    return name.replace('/', '_').replace('.', '_')

def process_file(file_path, c_file):
    """Processes a single file and adds its array to FileSystem.c."""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content_raw = f.read()

    char_list = convert_to_array(content_raw)
    array_name = sanitize_file_name(file_path)

    # Add array definition to the .c file content
    c_file_content = f'char {array_name}[] = {{{char_list}}};\n\n'
    c_file.write(c_file_content)
    
    return file_path, array_name

def main():
    # Process files in FileSystem
    file_mappings = []

    with open('./FileSystem.c', 'w') as c_file:
        for root, _, files in os.walk('./FileSystem'):
            for file in files:
                file_path = os.path.join(root, file)
                original_path, array_name = process_file(file_path, c_file)
                file_mappings.append((original_path[2:], array_name))

    # Create FileSystem.h
    master_header_content = f"""
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

char* read_file_as_string(const char* name);

#endif  // FILESYSTEM_H
    """
    with open("./FileSystem.h", 'w') as f:
        f.write(master_header_content)

    # Append the `read_file_as_string` function to the FileSystem.c file
    c_function_content = "\n\nchar* read_file_as_string(const char* name) {\n"
    for original, array_name in file_mappings:
        c_function_content += f'    if (strcmp(name, "{original}") == 0) return {array_name};\n'
    c_function_content += "    return NULL;\n}"
    with open("./FileSystem.c", 'a') as f:
        f.write(c_function_content)

if __name__ == "__main__":
    main()

