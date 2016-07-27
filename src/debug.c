#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/qimake.h"


#ifdef _QIDEBUG
void
d_print_fp(FILE *fp)
{
  char *buffer;
  long fp_length;

  fseek(fp, 0L, SEEK_END);
  fp_length = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  buffer = (char *)calloc(fp_length, sizeof(char));
  fread((void *)buffer, sizeof(char), fp_length, fp);

  printf("%s\n", buffer);

  memset((void *)buffer, '\0', sizeof(char) * fp_length);
  free((void *)buffer); buffer = NULL;
  fseek(fp, 0L, SEEK_SET);
}

#endif
