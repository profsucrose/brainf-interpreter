#include "file.h"

char* read_file(const char* path) {
    // open file in read mode ('b' is redundant but best practice)
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    // seek to end of stream to find
    // number of bytes to allocate
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // allocate buffer and write
    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}