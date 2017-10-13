#include <stdio.h>
#include <stdlib.h>

int unlzma(unsigned char *buf, long in_len,
           long (*fill)(void*, unsigned long),
           long (*flush)(void*, unsigned long),
           unsigned char *output,
           long *posp,
           void(*error)(char *x)
          );

#define BUFSIZE (16 * (1<<20))

static void error(char *x) { fputs(x, stderr); };

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc > 1)
        fp = fopen(argv[1], "r");
    else
        fp = stdin;

    if (!fp)
        return 1;

    unsigned char *inbuf = malloc(BUFSIZE);
    unsigned char *outbuf = malloc(BUFSIZE);

    if (!inbuf || !outbuf)
        return 2;

    size_t rd;
    size_t sz = 0;
    while ((rd = fread(inbuf, 1, BUFSIZE, fp)) > 0)
        sz += rd;

    long dec_size;
    int ret = unlzma(inbuf, sz, NULL, NULL, outbuf, &dec_size, error);

    printf("Read %zu compressed bytes (ret=%d):\n%s\n", dec_size, ret, outbuf);
    return 0;
}
