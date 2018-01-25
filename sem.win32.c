#include "sem.h"
#include "global.h"

struct semaphore_ {
  HANDLE semaphore;
};

void semaphore_init (struct semaphore_ **sem, int s) {
  struct semaphore_ *cc = umalloc (sizeof (struct semaphore_));
  HANDLE sem0_ = CreateSemaphore (null, (LONG) s, LONG_MAX, null);
  assert (sem0_ != NULL);
  cc->semaphore = sem0_;
  * sem = cc; 
}

int semaphore_passeren (struct semaphore_ *sem) {
  int s = -1;
  if (sem != null) {
    if (WaitForSingleObject (sem->semaphore, INFINITE) == WAIT_FAILED)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

int semaphore_vrijgeven (struct semaphore_ *sem) {
  int s = -1;
  if (sem != null) {
    LONG a;
    if (ReleaseSemaphore (sem->semaphore, 1, & a) == FALSE)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

int semaphore_vrijgeven_n (struct semaphore_ *sem, int n) {
  int s = -1;
  if (sem != null) {
    LONG a;
    if (ReleaseSemaphore (sem->semaphore, n, & a) == FALSE)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

int semaphore_vrijgevenall (struct semaphore_ *sem) {
  if (sem != null) 
  {
    LONG tmp;
    LONG write = 1;
    /* try release one times operate */
LLC:if (ReleaseSemaphore (sem->semaphore, 1, & tmp) == FALSE)
      if (GetLastError () == ERROR_TOO_MANY_POSTS)
        return 0;
      else 
        return -1;
    else ;
    /* calc new write */
    write = LONG_MAX - tmp - 1;
    if (ReleaseSemaphore (sem->semaphore, write, & tmp) == FALSE)
      if (GetLastError () == ERROR_TOO_MANY_POSTS)
        achieve LLC;
      else 
        return -1;
    else ;
    return 0;
  }
  return 0;
}

void semaphore_uninit (struct semaphore_ **sem) {
  if (sem != null) {
    struct semaphore_ *e = *sem;
    *sem = null;
    if (e != null) {
      if (e->semaphore)
        CloseHandle (e->semaphore);
      ufree (e);
    }
  }
}