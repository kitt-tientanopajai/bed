/* bed.c
 * A binary file editor v. 0.0.3
 * Kitt Tientanopajai <kitty@kitty.in.th>
 * Copyright (c) 2004-2009 Kitt Tientanopajai
 *
 * bed accepts three arguments: filename, offset, and value. It writes 
 * the value at the offset of the file. The offset and value should be
 * specified in hexadecimal format, i.e., begins with 0x or 0X. bed 
 * supports files larger than 2 GB.
 *
 * This is a free software. It can be distributed under the term of
 * GNU General Public License. */

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define VERSION "0.02"

/* Print usage information */
void
print_usage()
{
    printf("Usage: bed [-h | -v | -q] offset value ... infile\n");
    printf("Write the value at the offset of the file.\n\n");
    printf("  -h           display this help and exit\n");
    printf("  -q           quiet mode, nothing display\n");
    printf("  -v           output version information and exit\n\n");
    printf("Example: bed 0x4c5 0x0a filename\n");
}

/* Print version information */
void
print_version()
{
    printf("bed v.%s\n", VERSION);
    printf("Written by Kitt Tientanopajai <kitty@kitty.in.th>\n\n");
    printf("This is a free software, distributed under the term of GNU GPL. There is NO\n");
    printf("WARRANTY; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

/* Parse a hex string into an unsigned integer */
int
parse_hex(const char *hex_str, uint64_t *value)
{
    char *endptr;
    unsigned long long result;
    
    if (hex_str == NULL) {
        return 0;
    }
    
    // Handle "0x" prefix
    if (strlen(hex_str) >= 2 && (hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X'))) {
        result = strtoull(hex_str + 2, &endptr, 16);
    } else {
        result = strtoull(hex_str, &endptr, 16);
    }
    
    // Check for conversion errors
    if (*endptr != '\0' || result > UINT64_MAX) {
        return 0;
    }
    
    *value = result;
    return 1;
}

/* Main function */
int
main(int argc, char *argv[])
{
    int quiet = 0;
    FILE *file_ptr = NULL;
    uint64_t offset;
    uint64_t value;
    int arg_index;
    
    /* Check for help/version/quiet flags first */
    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }
    
    if (argv[1][0] == '-' && strlen(argv[1]) == 2) {
        switch (argv[1][1]) {
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
            case 'q':
                quiet = 1;
                arg_index = 2;
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    } else {
        arg_index = 1;
    }
    
    /* Validate that we have at least enough arguments for one offset-value pair */
    if (argc - arg_index < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }
    
    /* Open the file */
    char *filename = argv[argc - 1];
    file_ptr = fopen64(filename, "r+b");
    if (file_ptr == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename);
        exit(EXIT_FAILURE);
    }
    
    /* Process offset-value pairs from right to left */
    int i = argc - 2;  /* Start with the last offset-value pair */
    while (i > arg_index) {
        /* Parse the value */
        if (!parse_hex(argv[i], &value)) {
            fprintf(stderr, "Error: Invalid value '%s'\n", argv[i]);
            fclose(file_ptr);
            exit(EXIT_FAILURE);
        }
        
        /* Parse the offset */
        if (!parse_hex(argv[i - 1], &offset)) {
            fprintf(stderr, "Error: Invalid offset '%s'\n", argv[i - 1]);
            fclose(file_ptr);
            exit(EXIT_FAILURE);
        }
        
        /* Seek to the offset */
        if (fseeko64(file_ptr, offset, SEEK_SET) != 0) {
            perror("Error seeking in file");
            fclose(file_ptr);
            exit(EXIT_FAILURE);
        }
        
        /* Write the value */
        unsigned char byte_value = (unsigned char)value;
        if (fwrite(&byte_value, 1, 1, file_ptr) != 1) {
            perror("Error writing to file");
            fclose(file_ptr);
            exit(EXIT_FAILURE);
        }
        
        /* Print status if not in quiet mode */
        if (!quiet) {
            printf("File: %s: changed the value at 0x%" PRIx64 " to 0x%" PRIx64 "\n", 
                   filename, offset, value);
        }
        
        i -= 2; /* Move to next offset-value pair */
    }
    
    /* Close file and exit */
    fclose(file_ptr);
    exit(EXIT_SUCCESS);
}
