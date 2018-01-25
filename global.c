# if defined (_MSC_VER)
# include "config.h"
# include "uchar.h"
# include "file.h"
/* for debug */
__declspec (thread) DWORD win32_thread_local_err = 0;
# else 
# endif 

volatile
long uspinlock = 0;

int getsys_err_threadlocal (struct uchar **err_desc)  {
#  if defined (_WIN32)
  wchar_t *xbfp = null;
  int size = FormatMessageW 
                 ( FORMAT_MESSAGE_ALLOCATE_BUFFER 
                 | FORMAT_MESSAGE_FROM_SYSTEM 
                 | FORMAT_MESSAGE_IGNORE_INSERTS,
                   null, 
                   win32_thread_local_err,
                   MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), 
                   (LPWSTR) & xbfp, 0, null);
  assert (size != 0);
  if (* err_desc == null)
    uchar_init (err_desc);
  uchar_assign (* err_desc, xbfp, size);
  /* release buffer memblock */
  LocalFree (xbfp);

  return 0;
#  endif 
}

int getsys_err2 (struct uchar **err_desc)  {
#  if defined (_WIN32)
  wchar_t *xbfp = null;
  int size = FormatMessageW 
                 ( FORMAT_MESSAGE_ALLOCATE_BUFFER 
                 | FORMAT_MESSAGE_FROM_SYSTEM 
                 | FORMAT_MESSAGE_IGNORE_INSERTS,
                   null, 
                   GetLastError (), // @TODO: maybe getlasrerror base thread ....?
                   MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), 
                   (LPWSTR) & xbfp, 0, null);
  assert (size != 0);
  if (* err_desc == null)
    uchar_init (err_desc);
  uchar_assign (* err_desc, xbfp, size);
  /* release buffer memblock */
  LocalFree (xbfp);

  return 0;
#  endif 
}

int getself_dir (struct uchar **dir_desc) {
#  if defined (_WIN32)
  // @! int id;
  int id, iq;
  WCHAR xfb[1024];
  id = GetModuleFileNameW (NULL, xfb, sizeof (xfb)/ sizeof (xfb[0]));
  assert (id != 0);
  assert (id < sizeof (xfb)/ sizeof (xfb[0]));
  // @! slashclr_getpath (xfb, id, & id);  
  // ?? @TODO: normal, id copy to stack var, not same ref, but
  //    if __forceinline use, will cause bug?? 
  slashclr_getpath (xfb, id, & iq);
  if (*dir_desc == null)
    uchar_init (dir_desc);
  uchar_assign (* dir_desc, xfb, iq);
  return 0;
#  endif 
}