/* bed.c
 * A binary file editor v. 0.02
 * Kitt Tientanopajai <kitty@kitty.in.th>
 * Copyright (c) 2005 Kitt Tientanopajai
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

void help ()
{
	printf ("Usage: bed [-h | -v | -q] offset value ... infile\n");
	printf ("Write the value at the offset of the file.\n\n");
	printf ("  -h           display this help and exit\n");
	printf ("  -q           quiet mode, nothing display\n");
	printf ("  -v           output version information and exit\n\n");
	printf ("Example: bed 0x4c5 0x0a filename\n");
}

void versioninfo ()
{
	printf ("bed v.%s\n", VERSION);
	printf ("Written by Kitt Tientanopajai <kitty@kitty.in.th>\n\n");
	printf ("This is a free software, distributed under the term of GNU GPL. There is NO\nWARRANTY; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

int main (int argc, char *argv[]) 
{
	int quiet;
	FILE *fp;
	uint64_t offset;
	unsigned int value;

	int i;
	
	/* default value */
	quiet = 0;
		
	/* parse args */
	if (argv[1] != NULL) {
		if (argv[1][0] == '-') {
			switch (argv[1][1]) {
			case 'h':
				help ();
				exit (0);
			case 'v':
				versioninfo ();
				exit (0);
			case 'q':
				quiet = 1;
				break;
			default:
				help ();
				exit (1);
			}
		}
	} else {
		help ();
		exit (1);
	}

	i = argc - 1;
	fp = fopen64 (argv[i], "r+b");
	if (fp != NULL) {
		for (i--; i > 1; i -= 2) {
			if (argv[i] != NULL) {
				sscanf (argv[i], "%" PRIx8, &value);
			} else {
				fclose (fp);
				exit (1);
			}
						
			if (argv[i - 1] != NULL) {
				sscanf (argv[i - 1], "%" PRIx64, &offset);
			} else {
				fclose (fp);
				exit (1);
			}
							
			if (fseeko64 (fp, offset, SEEK_SET)) {
				fclose (fp);
				exit (1);
			}

			fwrite (&value, 1, 1, fp);
			if (!quiet) {
				printf ("File: %s: changed the value at 0x%" PRIx64 " to 0x%" PRIx8 "\n", argv[argc - 1], offset, value);
			}
		}
		fclose (fp);
		exit (0);
	} else {
		exit (1);
	}
}
