#ifndef UNLZMA_H
#define UNLZMA_H

int unlzma(unsigned char *buf, long in_len,
           long (*fill)(void*, unsigned long),
           long (*flush)(void*, unsigned long),
           unsigned char *output,
           long *posp,
           void(*error)(char *x)
          );

#endif
