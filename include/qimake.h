#ifndef _QIMAKE_H
#define _QIMAKE_H
#define _VERSION_ "0.0.1(R:2)"

#define _QIDEBUG
#ifdef _QIDEBUG
#include "qi_debug.h"
#endif


#define UV_SIZE         (128)
#define MAX_PATH_LENGTH (1024)


#define ARG_LENGTH (1024)


#define IS_DIRECTORY (1)
#define IS_FILE      (2)


/* src/io.c */
extern FILE *
ropen(const char *directory,
      const char *file);


/* src/utility.c */
extern int
get_pml_path(const char *workdir,
             char       *pml);

extern char *
get_work_direcotry(const char *path);

extern char *
pml_exist_directory(const char *path);

extern int
is_directory(const char *path);

extern void
error_message(const char *fx,
              const char *ptr);


/* src/qi.c */
extern int
get_uuid_version(const char *workdir,
                 FILE       *_pml_fp,
                 char       *uuid,
                 char       *version);
#endif
