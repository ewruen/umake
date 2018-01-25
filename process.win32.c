#include "process.h"
#include "global.h"
#include <windows.h>

struct process_ {
  HANDLE process;
  HANDLE pipe_r; 
  kbool rd_pipe;
};

int process_rd_pipe (  struct process_ *process, struct uchar **infos) {
  if (process != null && process->rd_pipe) {
    CHAR buf[1024];
    DWORD reads;
    BOOL ret;
    CHAR *tb_stream1 = NULL;
    DWORD tb_nums1 = 0;
    CHAR *tb_stream2 = NULL;
    DWORD tb_nums2 = 0;
    int rts = -1;
    uchar_init (infos);
    uchar_clearbuf (*infos);

    while (TRUE) {
      ret = ReadFile
              ( process->pipe_r,      // handle of the read end of our pipe
                buf,            // address of buffer that receives data
                sizeof (buf),                  // number of bytes to read
                & reads, // address of number of bytes read
                NULL   );
      if (ret == FALSE) {
        if ( GetLastError () == ERROR_BROKEN_PIPE)
          break;
L0:     if (tb_stream1 != NULL)
          ufree (tb_stream1);
        if (tb_stream2 != NULL);
          ufree (tb_stream2);
        if (rts != 0)
          uchar_uninit (infos);
        return rts;
      } else if (reads == 0) {
        break;  //FIXME: maybe err
      } else {
        if (tb_stream1 == NULL)
          tb_stream1 = umalloc (reads);
        else
          tb_stream1 = urealloc (tb_stream1, tb_nums1 + reads);
        memcpy (& tb_stream1[tb_nums1], buf, reads);
        tb_nums1 += reads;
      }
    }
    rts = 0;
    if (! (tb_nums1 == 0 && (tb_stream1 == NULL))) {
      if ( IsTextUnicode (tb_stream1, tb_nums1, null) == FALSE) {
        tb_stream2 = umalloc (tb_nums1 * 4);
        tb_nums2 = tb_nums1 * 4;
        tb_nums2 = MultiByteToWideChar (CP_ACP, 0, tb_stream1, tb_nums1, (LPWSTR) tb_stream2, tb_nums2); 
        uchar_insert (*infos, -1, (us *) tb_stream2, tb_nums2);
      } else {
        uchar_insert (*infos, -1, (us *) tb_stream1, tb_nums1/ 2); // XXX:tb_nums1/ 2
      }
    }
    achieve L0;
  }
  return -1;
}

int process_rd_pipe_utf8 (  struct process_ *process, unsigned char *bufuc, int len, int *reads) {
  if (len <= 0) {
    if (reads != null)
      * reads = 0;
    else ;
    return 0;
  }
  else ;

  if (process != null && process->rd_pipe) {
    DWORD reads_;
    BOOL ret;
    WCHAR *bufcc = (void *)bufuc;
    WCHAR *buqs = null;
    int rts = -1;

    while (TRUE) {
      ret = ReadFile
        ( process->pipe_r,      // handle of the read end of our pipe
        bufuc,            // address of buffer that receives data
        len,                  // number of bytes to read
        & reads_, // address of number of bytes read
        NULL   );
      if (ret == FALSE) {
        if ( GetLastError () == ERROR_BROKEN_PIPE)
          return 0;
        else ;
        return rts;
      } else if (reads_ == 0) {
        if (reads != null)
          *reads = reads_;
        else ;
        return 0;  //FIXME: maybe err
      } else {
        if (reads_ > len)
          reads_ = len;
        if (reads != null)
          *reads = reads_;
        else ;
        return 0;
      }
    }
  }
  return -1;
}

/* pipe code from MSDN's demo */

int process_create (   struct process_ **process,  struct uchar *cmd,  kbool new_cui, kbool rd_pipe
                /* , 
  kbool wr_pipe */ ) {

  BOOL bSuccess;
  PROCESS_INFORMATION pi;
  STARTUPINFOW  si;
  SECURITY_ATTRIBUTES sa;
  HANDLE hPipeOutputRead = NULL;
  HANDLE hPipeOutputWrite;
  HANDLE hPipeInputRead;
  HANDLE hPipeInputWrite;
  WCHAR sgbuf_t[2048];
  WCHAR *sgbuf;
  struct process_ *pce = null;

# if defined (_MSC_VER)
  __try {
    * process = null;
  } __except ( EXCEPTION_EXECUTE_HANDLER) {
      return -1;
  }
# else 

# endif 
  ZeroMemory (& si, sizeof(si));
  ZeroMemory (& pi, sizeof(pi));
  
  si.cb = sizeof(si);
  if (rd_pipe != kfalse) {
    ZeroMemory (& sa, sizeof(SECURITY_ATTRIBUTES));

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    /* create pipe for standard output redirection. */
    CreatePipe ( & hPipeOutputRead,  
                 & hPipeOutputWrite,
                 & sa,      
                 0  );

    /* create pipe for standard input redirection. */
    CreatePipe ( & hPipeInputRead,  
                 & hPipeInputWrite,
                 & sa,      
                 0  );

    si.dwFlags     = STARTF_USESTDHANDLES;
    si.hStdInput   = hPipeInputRead;
    si.hStdOutput  = hPipeOutputWrite;
    si.hStdError   = hPipeOutputWrite;
  }
  if (cmd->length >= sizeof (sgbuf_t)/ sizeof (sgbuf_t[0]))
    sgbuf = _wcsdup (cmd->str);
  else {
    sgbuf = sgbuf_t;
    memcpy (sgbuf, cmd->str, sizeof (us) * cmd->length);
    sgbuf[cmd->length] = 0;
  }
  bSuccess = CreateProcessW (
                    null, 
                    sgbuf,
                    null, 
                    null, 
                    rd_pipe ? ktrue : kfalse,
                    new_cui ? CREATE_NEW_CONSOLE : 0,
                    null,
                    null,
                  & si,	
                  & pi);

  if (cmd->length >= sizeof (sgbuf_t)/ sizeof (sgbuf_t[0]))
    free (sgbuf);
  else ;

  if (bSuccess != TRUE) {
    win32_thread_local_err = GetLastError ();
    return -1;
  }
  if (rd_pipe != kfalse) {
    CloseHandle (hPipeOutputWrite);
    CloseHandle (hPipeInputRead);
    CloseHandle (hPipeInputWrite);
  }

  if (bSuccess) {

    pce = umalloc (sizeof (struct process_));
    pce->pipe_r = hPipeOutputRead;
    pce->rd_pipe = !! rd_pipe;
    pce->process = pi.hProcess;
    * process = pce;
    // funny test 
    // ----------------------------------------------------------------------------------
    bSuccess = SetPriorityClass (pi.hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
    assert (bSuccess != FALSE);
    bSuccess = SetThreadPriority (pi.hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    assert (bSuccess != FALSE);
    bSuccess = SetProcessPriorityBoost (pi.hProcess, TRUE);
    assert (bSuccess != FALSE);
    bSuccess = SetThreadPriorityBoost (pi.hThread, TRUE);
    assert (bSuccess != FALSE);
    CloseHandle (pi.hThread);
    return 0;
  } else {
    win32_thread_local_err =GetLastError();
    if (hPipeOutputRead != null)
      CloseHandle (hPipeOutputRead);
    * process = null;
    // __debugbreak ();
    return -1;
  }
}

int process_create_detach (us *commandline) {

  BOOL bSuccess;
  PROCESS_INFORMATION pi;
  STARTUPINFOW  si;
  WCHAR *sgbuf;

  ZeroMemory (& si, sizeof(si));
  ZeroMemory (& pi, sizeof(pi));
  
  si.cb = sizeof(si);

  sgbuf = _wcsdup (commandline);
  bSuccess = CreateProcessW (
                    null, 
                    sgbuf,
                    null, 
                    null, 
                    kfalse,
                    CREATE_NEW_CONSOLE,
                    null,
                    null,
                  & si,	
                  & pi);
  free (sgbuf);
  if (bSuccess != TRUE) {
    win32_thread_local_err = GetLastError ();
    return -1;
  }

  if (bSuccess) {
    CloseHandle (pi.hThread);
    CloseHandle (pi.hProcess);
    return 0;
  } else {
    win32_thread_local_err =GetLastError();
    return -1;
  }
}
void process_colsehandle (         struct process_ **process) {
  struct process_ *p = null;
# if defined (_MSC_VER)
  __try {
# else 

# endif 
  if (process != null) {
    p = * process;
    * process = null;
    if (p != null) {
      if (p->pipe_r != null && p->rd_pipe)
        CloseHandle (p->pipe_r);
      if (p->process != null)
        CloseHandle (p->process);
      ufree (p);
    }
  }
# if defined (_MSC_VER)
  } __except ( EXCEPTION_EXECUTE_HANDLER) {
# else 

# endif 
    if (p != null)
      ufree (p);
    return ;
# if defined (_MSC_VER)
  }
# else 

# endif 
}

int process_join (         struct process_ *process) {
  int s = -1;
  if (process != null) {
    if (WaitForSingleObject (process->process, INFINITE) == WAIT_FAILED) 
      win32_thread_local_err = GetLastError ();
    else s = 0;
  }
  return s;
}

int process_exitcode (         struct process_ *process, int *exitcode) {
  int s = -1;
  if (process != null) {
    if (GetExitCodeProcess (process->process, exitcode) == FALSE) 
      win32_thread_local_err = GetLastError ();
    else s = 0;
  }
  return s;
}

#if 0
#include "locate.h"
int main (void)
{
  struct process_ *ppc;
  struct uchar *s;
  int cc;
  uchar_ginit ();
  process_create (& ppc, _GR ("D:\\MinGW\\bin\\gcc.exe -c D:\\MinGW\\bin\\ac0.c"), TRUE, TRUE );
  process_join (ppc);
  process_exitcode (ppc, & cc);
  process_rd_pipe (ppc, & s);
  process_colsehandle (& ppc);
  kprint (s->str);
  uchar_uninit (& s);
  uchar_guninit ();
  leak_infos_get ();
  return 0;
}

#endif 