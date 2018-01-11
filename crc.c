#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>

#include "arg.h"
#include "crc.h"
#include "error.h"

uint8_t is_file(char *name)
{
  struct stat st;
  if ( !(stat(name, &st) != -1 && !S_ISDIR(st.st_mode) ))
      DIE("%s: Not a valid file.\n", name);
  return 0;
}

uint8_t* hex_decode(char *in, unsigned int len, uint8_t *out)
{
    /*if ( strlen(in) % 2 != 0 ) {*/
        /*DIE("%s\n", "Your input string must be even!");*/
    /*}*/
    if (in[strspn(in, "0123456789abcdefABCDEF")] != 0) {
       DONT_DIE("%s: Not a valid hex string.\n", in);
       if (infile != NULL) {
           free(filecontents);
           fclose(filetoencode);
        }
       free(out);
       exit(EXIT_FAILURE);
    }
    unsigned int i, t, hn, ln;

    for (t = 0, i = 0; i < len; i+=2, ++t) {

        hn = in[i] > '9' ? (in[i]|32) - 'a' + 10 : in[i] - '0';
        ln = in[i+1] > '9' ? (in[i+1]|32) - 'a' + 10 : in[i+1] - '0';

        out[t] = (hn << 4 ) | ln;
    }

    return out;
}

/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];
/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;
/* Make the table for a fast CRC. */
void make_crc_table(void)
{
    unsigned long c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (unsigned long)n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
      should be initialized to all 1's, and the transmitted value
      is the 1's complement of the final running CRC (see the
      crc() routine below)). */

unsigned long update_crc(unsigned long crc, uint8_t* buf, int len)
{
    unsigned long c = crc;
    int n;

    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc(uint8_t* buf, int len)
{
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

/*void dummy (void) {*/
    /*return;*/
/*}*/

void run(void) {

    uint32_t arglen, outlen = 0;
    uint8_t* output;
    char* buf_string;
    struct stat st;

    if (infile != NULL) {
        if ( stat(infile, &st) != -1 && st.st_size > 0) {
            filetoencode = fopen(infile, "rb");
            arglen = st.st_size;
            printf("filesize: %d\n", arglen);
            arglen--; // Decrement the lenght to omit the line end character
            outlen = (arglen % 2 == 0 ? arglen / 2 : (arglen / 2 + 1));
        } else {
            DIE("%s: This file is empty.\n", infile);
        }
    }
    if (string != NULL) {
        arglen = strlen(string);
    /*outlen = (arglen % 2 == 0 ? arglen / 2 : (arglen / 2 + 2));*/
        if (arglen % 2 == 0) {
            buf_string = malloc(arglen+1);
            memset(buf_string, 0, arglen);
            strncpy(buf_string, string, arglen+1);
            outlen = arglen / 2;
        } else {
            buf_string = malloc(arglen+2);
            memset(buf_string, 0, arglen+2);
            strncpy(buf_string, string, arglen+2);
            strncat(buf_string, "0", arglen+2);
            printf("%s\n", buf_string);
            outlen = arglen / 2 + 1;
        }

    }
    output = malloc(outlen);
    memset(output, 0, outlen);
    if (string != NULL) {
        hex_decode(buf_string, arglen, output);
    } else if (infile != NULL) {
        /*outlen--;*/
        if  (arglen % 2 == 0) {
            filecontents = malloc(arglen+1);
            memset(filecontents, 0, arglen+1);
        } else {
            filecontents = malloc(arglen+2);
            memset(filecontents, 0, arglen+2);
        }
        /*filecontents = malloc(arglen);*/
        /*memset(filecontents, 0, arglen);*/
        /*if  (arglen % 2 == 0) {*/
        /*}*/
        arglen = fread(filecontents, 1, arglen, filetoencode);
        if  (arglen % 2 != 0)
            strcat(filecontents, "0");
        /*arglen--;*/
        /*printf("filestuff: %s\n", filecontents);*/
        hex_decode(filecontents, arglen, output);
        free(filecontents);
    }
    printf("arglen: %d\n", arglen);
    printf("outlen: %d\n", outlen);
    printf("Chunk length: 0x%x\n", outlen-4);
    printf("CRC 4 byte sum: %lx\n", crc(output, outlen));
    free(output);
    if (string != NULL)
        free(buf_string);
    if (infile != NULL)
        fclose(filetoencode);
}

char *argv0;

int main(int argc, char *argv[])
{
    ARGBEGIN {
        case 's':
        case 'S':
            string = ARGF();
            break;
        case 'f':
        case 'F':
            infile = ARGF();
            is_file(infile);
            break;
        case 'v':
        case 'V':
        default:
            DIE("crc32 " VERSION " %s\n", argv0);
    } ARGEND;
    if (string == NULL && infile == NULL)
        DIE("crc32 " VERSION " %s\n", argv0);
    /*printf("argc: %d\n", argc);*/
    run();
    /* compile with gcc -std=c11 -Wall -pedantic -Werror=old-style-declaration -g crc.c -o crc */
    return EXIT_SUCCESS;
}
