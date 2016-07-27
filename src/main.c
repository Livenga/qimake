#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/qimake.h"

/* src/main.c */
void
print_help(const char *app);
void
print_version(const char *app);


int
main(int argc,
     char *argv[])
{
  FILE *pml_fp;
  int  t_flag;

  int opt, optindex;
  int f_help = 0, f_version = 0, f_pkg = 0;
  
  char *workdir, pml_path[MAX_PATH_LENGTH];
  char uuid[UV_SIZE], version[UV_SIZE];

  static struct option opts[] = {
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'V'},
    {"pkg",     required_argument, 0, 'p'},
    {"install", no_argument,       0, 'i'},
    {0,         0,                 0, 0},
  };


  while((opt = getopt_long(argc, argv, "hvipa:",
          (const struct option *)opts, &optindex)) != EOF) {
    switch(opt) {
      case 'h': f_help    = 1; break;
      case 'v': f_version = 1; break;
    }
  }

  if(f_help || f_version) {
    if(f_help)    print_help(argv[0]);
    if(f_version) print_version(argv[0]);
    return 0;
  }
 
  if(argc != 3) {
    fprintf(stderr, "%s: Operand Error\n", argv[0]);
    fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);
    return EOF;
  }


  // XXX: argv[1] の長さが MAX_PATH_LENGTH を超える可能性がある.
  memset((void *)pml_path, '\0', sizeof(pml_path));


  t_flag  = is_directory(argv[1]);
  if(t_flag == IS_FILE) { // 直接 pml ファイルが指定されたばいい
    if((pml_fp = fopen(argv[1], "rb")) == NULL) {
      error_message("fopen(3)", argv[1]);
      return EOF;
    }
    workdir  = pml_exist_directory(argv[1]);
    strncpy((char *)pml_path, (const char *)argv[1], strlen(argv[1]));
  }
  else if (t_flag == IS_DIRECTORY) {
    workdir = get_work_direcotry(argv[1]);
    get_pml_path((const char *)workdir, pml_path);
    pml_fp  = NULL;
  }

  printf("[Pml file path]:\t%s\n", pml_path);

  memset((void *)uuid,    '\0', sizeof(char) * UV_SIZE);
  memset((void *)version, '\0', sizeof(char) * UV_SIZE);
  get_uuid_version(workdir, pml_fp, uuid, version);


  printf("[UUID]    %s\n", uuid);
  printf("[Version] %s\n", version);
  printf("[Package Name] %s-%s.pkg\n", uuid, version);


  // プロセスの生成と qipkg コマンドの実行
  pid_t pid;
  int   i, qipkg_argc, child_process_status = EOF;
  char  *qipkg_argv[7];

  qipkg_argc = sizeof(qipkg_argv) / sizeof(qipkg_argv[0]);

  for(i = 0; i < qipkg_argc - 1; i++)
    qipkg_argv[i] = (char *)calloc(ARG_LENGTH, sizeof(char));
  strncpy(qipkg_argv[0], "qipkg", strlen("qipkg"));
  qipkg_argv[qipkg_argc - 1] = NULL;


  // パッケージのビルド
  if((pid = fork()) == 0) {
    strncpy(qipkg_argv[1], "make-package", strlen("make-package"));
    strncpy(qipkg_argv[2], pml_path, strlen(pml_path));
    strncpy(qipkg_argv[3], "--force", strlen("--force"));
    strncpy(qipkg_argv[4], "-o", 2);
    sprintf(qipkg_argv[5], "%s/%s-%s.pkg", workdir, uuid, version);

    execvp("qipkg", qipkg_argv);
  } else {
    waitpid(pid, &child_process_status, 0);
  }

  // パッケージの転送
  if((pid = fork()) == 0) {
    strncpy(qipkg_argv[1], "deploy-package", strlen("deploy-package"));
    sprintf(qipkg_argv[2], "%s/%s-%s.pkg", workdir, uuid, version);
    strncpy(qipkg_argv[3], "--url", strlen("--url"));
    //sprintf(qipkg_argv[4], "nao@%s", ip_addr);
    sprintf(qipkg_argv[4], "nao@%s", argv[2]);
    qipkg_argv[5] = NULL;

    execvp("qipkg", qipkg_argv);
  } else {
    waitpid(pid, &child_process_status, 0);
  }


  // 解放処理
  for(i = 0; i < qipkg_argc - 1; i++){
    memset((void *)qipkg_argv[i], '\0', sizeof(char) * ARG_LENGTH);
    free((void *)qipkg_argv[i]);
    qipkg_argv[i] = NULL;
  }

  memset((void *)workdir, '\0', sizeof(char) * MAX_PATH_LENGTH);
  free((void *)workdir); workdir = NULL;


  return 0;
}


/**
 * @fn print_help
 * ヘルプページの表示
 * @param  (app) プログラム名
 * @return (void)
 */
  void
print_help(const char *app)
{
  fprintf(stderr, "Usage: %s [pml file] [Pepper IP Address]\n\n", app);
  fprintf(stderr, "[Options]\n");
  fprintf(stderr, "   --help,    -h: ヘルプの表示.\n");
  fprintf(stderr, "   --version, -v: バージョンの表示.\n\n");
}

/**
 * @fn print_version
 * バージョンの表示
 * @param  (app) プログラム名
 * @return (void)
 */
  void
print_version(const char *app)
{
  fprintf(stderr, "2016/07/22 Version %s\n", _VERSION_);
}
