#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "../include/qimake.h"


/**
 * @fn pml_open
 * 拡張子 .pml ファイルポインタの取得
 *
 * @param  (workdir) 作業ディレクトリ
 * @return 成功の場合はファイルポインタ, 失敗は NULL ポインタを返す
 */
static FILE *
pml_open(const char *workdir)
{
  FILE *fp = NULL;
  DIR           *wdir;
  struct dirent *rdir;


  if((wdir = opendir(workdir)) == NULL) {
    error_message("opendir", workdir);
    return NULL;
  }

  while((rdir = readdir(wdir)) != NULL) {
    if(strstr((const char *)rdir->d_name, (const char *)".pml") != NULL) {
      fp = ropen(workdir, rdir->d_name);
      break;
    }
  }

  closedir(wdir);
  return fp;
}


/**
 * @fn manifest_open
 * マニフェストファイルのファイルポインタの取得
 *
 * @param (workdir) 作業ディレクトリ
 * @return 成功: ファイルポインタ, 失敗: NULL ポインタ
 */
static FILE *
manifest_open(const char *workdir,
              FILE       *pml_fp)
{
  FILE *manifest_fp = NULL;
  long pml_length;
  char *pml, *manifest_ptr, *p;


  fseek(pml_fp, 0L, SEEK_END);
  pml_length = ftell(pml_fp);
  fseek(pml_fp, 0L, SEEK_SET);

  pml = (char *)calloc(pml_length, sizeof(char));

  fread((void *)pml, sizeof(char), pml_length, pml_fp);

  manifest_ptr = strstr((const char *)pml, (const char *)"src=\"");
  if(manifest_ptr != NULL) {
    manifest_ptr += strlen("src=\"");

    if((p = strchr((const char *)manifest_ptr, '"')) != NULL) {
      *p = '\0';
      manifest_fp = ropen(workdir, manifest_ptr);
    }
  }

  memset((void *)pml, '\0', sizeof(char) * pml_length);
  free((void *)pml); pml = NULL;

  return manifest_fp;
}



/**
 * @fn get_uuid_version
 * qipkg の引数に必要な uuid と version の取得
 *
 * @param (workdir) 作業ディレクトリ
 * @param (uuid)    uuid 文字列のポインタ
 * @param (version) version 文字列のポインタ
 */
int
get_uuid_version(const char *workdir,
                 FILE       *_pml_fp,
                 char       *uuid,
                 char       *version)
{
  FILE *pml_fp, *manifest_fp;

  // TODO: 正常なファイルポインタが返却されなかった場合の処理を追加する
  if(_pml_fp != NULL)
    pml_fp = _pml_fp;
  else
    pml_fp = pml_open(workdir);


  manifest_fp = manifest_open(workdir, pml_fp);

  fclose(pml_fp);

#ifdef _QIDEBUG
  printf("_pml_fp:     %p,  pml_fp: %p\n", _pml_fp, pml_fp);
  printf("manifest_fp: %p\n", manifest_fp);
  //d_print_fp(manifest_fp);
#endif


  // マニフェストファイルの読み込み
  long manifest_length;
  char *manifest;
  char *seek_ptr, *uuid_ptr, *version_ptr, *p;

  fseek(manifest_fp, 0L, SEEK_END);
  manifest_length = ftell(manifest_fp);
  fseek(manifest_fp, 0L, SEEK_SET);

  manifest = (char *)calloc(manifest_length, sizeof(char));
  fread((void *)manifest, sizeof(char), manifest_length, manifest_fp);

  // uuid, version 取得
  seek_ptr = strstr(manifest, "package");
  if(seek_ptr != NULL) {
    uuid_ptr    = strstr(seek_ptr, "uuid=\"");
    version_ptr = strstr(seek_ptr, "version=\"");

    // uuid の抽出
    if(uuid_ptr  != NULL) {
      uuid_ptr += strlen("uuid=\"");

      if((p = strchr(uuid_ptr, '"')) != NULL) {
        *p = '\0';
        strncpy(uuid, uuid_ptr, strlen(uuid_ptr));
      }
    }

    // version の抽出
    if(version_ptr != NULL) {
      version_ptr += strlen("version=\"");

      if((p = strchr(version_ptr, '"')) != NULL) {
        *p = '\0';
        strncpy(version, version_ptr, strlen(version_ptr));
      }
    }
  }

  // 解放処理
  memset((void *)manifest, '\0', sizeof(char) * manifest_length);
  free((void *)manifest);
  manifest = NULL;
  fclose(manifest_fp);
  return 0;
}
