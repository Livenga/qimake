#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/**
 * @fn ropen
 * ディレクトリとファイル名を指定してファイルを開く
 *
 * @param (directory) ディレクトリ名
 * @param (file)      ファイル名
 * @return            ファイル記述子
 */
FILE *
ropen(const char *directory,
      const char *file)
{
  extern int errno;
  
  FILE *fp;
  long pathlen;
  char *pathname;


  pathlen  = strlen(directory) + strlen(file) + 2;
  pathname = (char *)calloc(pathlen, sizeof(char));

  sprintf(pathname, "%s/%s", directory, file);

  fp = fopen(pathname, "rb");
  if(fp == NULL)
    fprintf(stderr, "fopen(3): %s, %s\n", pathname, strerror(errno));

  memset((void *)pathname, '\0', pathlen * sizeof(char));
  free((void *)pathname); pathname = NULL;

  return fp;
}
