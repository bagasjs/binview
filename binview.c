#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void fatal(const char* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

void usage(FILE *f)
{ 
    fprintf(f, "USAGE: binview <ARGS> [FLAGS]\n");
    fprintf(f, "    -bin       Show every byte in binary format\n");
    fprintf(f, "    -dec       Show every byte in decimal format\n");
    fprintf(f, "    -plain     Do not show anything except the file data\n");
    fprintf(f, "    -line <n>  Show <n> bytes per line\n");
}

const char *shift(int *argc, char ***argv, const char *error_msg) {
    if(*argc < 1) {
        usage(stderr);
        fatal(error_msg);
    }
    const char *result = (*argv)[0];
    *argc -= 1;
    *argv += 1;
    return result;
}

#define HEX_FORMAT 0
#define BIN_FORMAT 1 // TODO: support this 
#define DEC_FORMAT 2

int main(int argc, char **argv)
{
    /** const char *program = */ shift(&argc, &argv, "Unreachable");
    const char *file_path = 0;
    uint64_t per_line = 8;
    int view_format = HEX_FORMAT;
    bool plain = false;

    while(argc > 0) {
        const char *item = shift(&argc, &argv, "Unreachable");
        if(strcmp(item, "-line") == 0) {
            const char *per_line_cstr = shift(&argc, &argv, "Please provide the value of `line` flag");
            per_line = atoi(per_line_cstr);
        } else if(strcmp(item, "-bin") == 0) {
            fatal("Apology, binary format is currently not supported");
            view_format = BIN_FORMAT;
        } else if(strcmp(item, "-dec") == 0) {
            view_format = DEC_FORMAT;
        } else if(strcmp(item, "-plain") == 0) {
            plain = true;
        } else if(!file_path) {
            file_path = item; // this might be dangerous
        }
    }

    if(!file_path) {
        usage(stderr);
        fatal("Please provide the file path argument");
    }

    FILE *f = fopen(file_path, "rb");
    if(!f) {
        fprintf(stderr, "ERROR: Could not read to file `%s`\n", file_path);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    if(ferror(f)) {
        fprintf(stderr, "ERROR: Failed to seek into the end of the file `%s`\n", file_path);
        return 1;
    }

    uint64_t file_size = ftell(f);
    if(ferror(f)) {
        fprintf(stderr, "ERROR: Failed to check file `%s` size\n", file_path);
        return 1;
    }

    fseek(f, 0, SEEK_SET);
    if(ferror(f)) {
        fprintf(stderr, "ERROR: Failed to seek into the beginning of the file `%s`\n", file_path);
        return 1;
    }

    uint8_t *file_data = malloc(file_size + 1);
    if(!file_data) {
        fprintf(stderr, "ERROR: Buy more RAM LOL `%s`\n", file_path);
        return 1;
    }


    uint64_t i = 0;
    uint64_t row = 0;
    if(!plain) {
        printf("file: %s\n", file_path);
        printf("size: %lu byte/s\n", file_size);
        if(view_format == DEC_FORMAT) {
            printf("format: decimal\n");
        } else {
            printf("format: hexadecimal\n");
        }
        printf("%lu | ", row + 1);
    }

    while(i < file_size) {
        uint8_t current_byte = (uint8_t)fgetc(f);
        if(ferror(f)) {
            fprintf(stderr, "Failed to read file data at index `%lu`\n", i);
            return 1;
        }

        if(view_format == DEC_FORMAT) {
            printf("%03d ", current_byte);
        } else {
            printf("%02X ", current_byte);
        }

        i += 1;
        if(i % per_line == 0) {
            row += 1;
            if(plain) {
                putchar('\n');
            } else {
                printf("\n%lu | ", row + 1);
            }
        }
    }

    return 0;
}
