#include <stddef.h>

char __FileSystem_test_txt[] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'p', 'l', 'a', 'i', 'n', 't', 'e', 'x', 't', ' ', 'd', 'o', 'c', 'u', 'm', 'e', 'n', 't', '.', '\n', '\n', 'T', 'h', 'e', 'r', 'e', '\'', 's', ' ', 's', 'o', 'm', 'e', ' ', 'w', 'h', 'i', 't', 'e', 's', 'p', 'a', 'c', 'e', '.', '\n', '\n', '\0'};

char __FileSystem_test_json[] = {'{', '\n', ' ', ' ', '"', 'G', 'i', 'v', 'e', 'n', 'N', 'a', 'm', 'e', '"', ' ', ':', ' ', '"', 'M', 'o', 'r', 't', 'i', 'm', 'e', 'r', '"', ',', '\n', ' ', ' ', '"', 'S', 'u', 'r', 'N', 'a', 'm', 'e', '"', ' ', ':', ' ', '"', 'S', 'm', 'i', 't', 'h', '"', ',', '\n', ' ', ' ', '"', 'N', 'a', 'm', 'e', 'S', 'u', 'f', 'f', 'i', 'x', 'T', 'e', 'x', 't', '"', ' ', ':', ' ', '"', 'S', 'r', '"', ',', '\n', ' ', ' ', '"', 'P', 'r', 'e', 'f', 'e', 'r', 'r', 'e', 'd', 'N', 'a', 'm', 'e', '"', ' ', ':', ' ', '"', 'M', 'o', 'r', 't', 'y', '"', ',', '\n', '}', '\n', '\0'};


char* read_file_as_string(const char* name) {
    if (strcmp(name, "FileSystem/test.txt") == 0) return __FileSystem_test_txt;
    if (strcmp(name, "FileSystem/test.json") == 0) return __FileSystem_test_json;
    return NULL;
}
