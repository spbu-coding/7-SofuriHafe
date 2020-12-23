#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISALLOWED_SYMBOLS ".,;:!?"
#define MAX_STRING_LENGTH 1000
#define error(...) (fprintf(stderr, __VA_ARGS__))

int des(const void* str1, const void* str2)
{
    return -strcmp(*((char**) str1), *((char**) str2));
}

int is_allowed_char(char c)
{
    char disallowed_chars[] = DISALLOWED_SYMBOLS;

    for(size_t i = 0; i < strlen(disallowed_chars); i++)
    {
        if(disallowed_chars[i] == c) return 0;
    }

    return 1;
}

void free_strings_array(char** strings_array, size_t size)
{
    for(size_t i = 0; i < size; i++) free(strings_array[i]);

    free(strings_array);

    strings_array = NULL;
}

int read_file(FILE* file, size_t max_string_length, char*** strings_array)
{
    int realloc_size = 10;

    int size;
    char buffer[MAX_STRING_LENGTH];

    for(size = 0;; size++)
    {
        if(fgets(buffer, max_string_length, file) == NULL) break;

        if(size % realloc_size == 0)
        {
            void *memory = realloc(*strings_array, (size + realloc_size) * sizeof(char*));

            if(memory == NULL)
            {
                error("\nUnable to allocate memory\n");
                free_strings_array(*strings_array, size);
                free(*strings_array);
                return -1;
            }

            else *strings_array = memory;
        }

        (*strings_array)[size] = calloc(sizeof(char), MAX_STRING_LENGTH);

        for(size_t i = 0, j = 0; i < strlen(buffer); i++)
        {
            if(is_allowed_char(buffer[i])) (*strings_array)[size][j++] = buffer[i];
        }

        (*strings_array)[size][strlen(buffer)] = '\0';
    }

    return size;
}

int write_file(FILE* file, size_t strings_count, char** strings_array)
{
    size_t max_strings_count = 100;

    if(strings_count > 0)
    {
        for(size_t i = 0; i < strings_count && i < max_strings_count; i++)
        {
            if(fputs(strings_array[i], file) == EOF)
            {
                error("\nUnable to write the file\n");

                return -1;
            }

            if(strcspn(strings_array[i], "\n") == strlen(strings_array[i]))
            {
                if(fputs("\n", file) == EOF)
                {
                    error("\nUnable to write the file\n");

                    return -1;
                }
            }
        }
    }

    else
    {
        if(fputs("\n", file) == EOF)
        {
            error("\nUnable to write the file\n");

            return -1;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        error("\nUnsupported arguments number\n");

        return -1;
    }

    FILE *in_file = fopen(argv[1], "r");

    if(in_file == NULL)
    {
        error("\nUnable to open the file\n");

        return -1;
    }

    char** strings_array = NULL;

    int strings_count;

    if((strings_count = read_file(in_file, MAX_STRING_LENGTH, &strings_array)) < 0)
    {
        fclose(in_file);
        error("\nUnable to read the file\n");

        return -1;
    }

    qsort(strings_array, (size_t)strings_count, sizeof(char*), des);

    if(write_file(stdout, (size_t)strings_count, strings_array) != 0)
    {
        fclose(in_file);
        free_strings_array(strings_array, (size_t)strings_count);
        error("\nUnable to write the file\n");

        return -1;
    }

    fclose(in_file);
    free_strings_array(strings_array, (size_t)strings_count);

    return 0;
}
