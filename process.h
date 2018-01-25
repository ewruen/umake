#if !defined (__UMAKE_PROCESS_H_)
#  define __UMAKE_PROCESS_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"

struct process_;

void process_colsehandle ( struct process_ **process);            
int process_join (      struct process_ *process);
int process_exitcode (  struct process_ *process, int *exitcode);
/* void process_wr_pipe (  struct process_ *process, struct uchar *infos); */
int process_rd_pipe (  struct process_ *process, struct uchar **infos);
/* XXX: must set up the console UTF8 code page in init time */
int process_rd_pipe_utf8 (  struct process_ *process, unsigned char *buf, int len, int *reads);
int process_create (   struct process_ **process,  struct uchar *cmd,  kbool new_cui, kbool rd_pipe
                /* , 
  kbool wr_pipe */ );
int process_create_detach ( us *commandline);

#endif 