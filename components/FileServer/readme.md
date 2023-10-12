# File System Component

The File System component allows for easy embedding of plain text files into a C project, storing them as statically declared character arrays. This ensures that these files are always available at runtime without the need for file system operations or external dependencies.

## Usage

To use the File System component:

1. Place any files you wish to embed into the `FileSystem` directory.
2. Run the Python script `embed_files.py` (or if integrated into your build system, simply build your project).
3. In your C code, use the function `read_file_as_string` to get a pointer to the embedded file's content by providing the file's path as an argument. Example: 

    ```c
    char* content = read_file_as_string("FileSystem/test.txt");
    ```

4. To query the size of the embedded file, use the function `query_size`. Example:

    ```c
    int size = query_size("FileSystem/test.txt");
    ```

## Python Script: `embed_files.py`

The provided Python script processes the files inside the `FileSystem` directory and generates the corresponding C source and header files. 

### Features:

- Automatically escapes special characters like newlines (`\n`), single quotes (`'`), and backslashes (`\`).
- Produces a `FileSystem.c` file containing all embedded files as static char arrays.
- Generates a `FileSystem.h` header file that contains the declarations for accessing these files in other parts of your C project.

### Integration with CMake:

The script can be integrated with the CMake build system to ensure that any changes to files in the `FileSystem` directory are automatically embedded in the build process.

## Potential Applications:

- Embedding configuration files or scripts that your program might need at runtime.
- Storing small data files for testing or demonstration purposes without relying on external file operations.

## Notes:

- This component is designed for embedding small to medium-sized text files. Embedding very large files may increase the compile time and the binary size considerably.
- Always ensure the character encoding of your files is compatible with the requirements of your project.

