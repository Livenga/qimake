#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/qimake.h"


/**
 * @fn get_pml_path
 * pml ファイルパスの取得
 *
 * @param  (workdir) ワークディレクトリパスを格納する先頭アドレス
 * @param  (pml)     pml パスを格納する先頭アドレス
 * @return (int) 失敗(*.pml が存在しない)は -1 で.
 */
int
get_pml_path(const char *workdir,
             char       *pml)
{
  int status = EOF;
  DIR *workdir_obj;
  struct dirent *workdir_inner;

  if((workdir_obj = opendir(workdir)) == NULL) return EOF;

  while((workdir_inner = readdir(workdir_obj)) != NULL) {
    if(strstr(workdir_inner->d_name, ".pml") != NULL) {
      status = 0;

      strncat(pml, workdir, strlen(workdir));
      strncat(pml, "/", 1);
      strncat(pml, workdir_inner->d_name, strlen(workdir_inner->d_name));
      break;
    }
  }

  closedir(workdir_obj);
  return status;
}

/**
 * @fn get_work_direcotry
 * 作業ディレクトリパスの取得
 *
 * @param  (path) 指定ディレクトリ名 ptr
 * @return () パス文字列のポインタ
 */
#define CWD_LENGTH      (128)
char *
get_work_direcotry(const char *path)
{
  char *workdir, cwd[CWD_LENGTH];

  workdir = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));

  if(path[0] != '/') {
    memset((void *)cwd, '\0', sizeof(cwd));
    getcwd((char *)cwd, sizeof(cwd));


    strncpy(workdir, (const char *)cwd, sizeof(char) * strlen(cwd));
    strncat(workdir, (const char *)"/", 1);
  }
  strncat(workdir, (const char *)path, strlen(path));

  return workdir;
}


/**
 * @fn pml_exist_directory
 * 直接 pml ファイルが指定された場合,
 * pml ファイルが存在するディレクトリのパス取得
 *
 * @param  (path)   pml パス
 * @return (char *) ディレクトリパス
 */
char *
pml_exist_directory(const char *path)
{
  char *workdir, *pre_ptr, *nx_ptr;
  workdir = (char *)calloc(MAX_PATH_LENGTH, sizeof(char));

  if((pre_ptr = strchr(path, '/')) != NULL) {
    while(1) {
      nx_ptr  = strchr((const char *)pre_ptr + 1, '/');
      if(nx_ptr == NULL) break;

      pre_ptr = nx_ptr;
    }
  }
  
  else workdir[0] = '.';
  memmove((void *)workdir, (const void *)path, sizeof(char) * (pre_ptr - path));

  return workdir;
}


/**
 * @fn is_directory
 * 指定パスがディレクトリかどうか
 *
 * @param  (path) パスしてい
 * @return (int) define IS_*
 */
int
is_directory(const char *path)
{
  struct stat buf;
  mode_t mode;

  memset((void *)&buf, '\0', sizeof(buf));
  if(stat(path, &buf) < 0) {
    error_message("stat(2)", path);
    return EOF;
  }

  mode = buf.st_mode;
  if(S_ISDIR(mode))      return IS_DIRECTORY;
  else if(S_ISREG(mode)) return IS_FILE;

  return -1;
}


void
error_message(const char *fx,
              const char *ptr)
{
  if(ptr == NULL)
    fprintf(stderr, "%s: %s\n", fx, strerror(errno)); 
  else
    fprintf(stderr, "%s(%s): %s\n", fx, ptr, strerror(errno));
}
