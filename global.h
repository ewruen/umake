#if !defined (__UMAKE_GLOBAL_H_)
#  define __UMAKE_GLOBAL_H_ 

# if defined (_MSC_VER)
# include "config.h"
extern utls DWORD win32_thread_local_err;
# else 
# endif 

extern volatile
  long uspinlock;

int getsys_err_threadlocal (struct uchar **err_desc);
int getsys_err2 (struct uchar **err_desc);
// not slash in tail 
int getself_dir (struct uchar **dir_desc);

# endif