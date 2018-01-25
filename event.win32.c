#include "event.h"
#include "global.h"

struct event_ {
  HANDLE event0_;
};

void event_init (struct event_ **event) {
  struct event_ *cc = umalloc (sizeof (struct event_));
  HANDLE event0_ = CreateEvent (null, ktrue, kfalse, null);
  assert (event0_ != NULL);
  cc->event0_ = event0_;
  * event = cc; 
}

int event_wait (struct event_ *event) {
  int s = -1;
  if (event != null) {
    if (WaitForSingleObject (event->event0_, INFINITE) == WAIT_FAILED)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

int event_signal (struct event_ *event) {
  int s = -1;
  if (event != null) {
    if (SetEvent (event->event0_) == FALSE)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

int event_cancel (struct event_ *event) {
  int s = -1;
  if (event != null) {
    if (ResetEvent (event->event0_) == FALSE)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

void event_init2 (struct event_ **event, kbool auto_reset, uconst  us *gsym_inos, kbool *had_open) {
  struct event_ *cc = umalloc (sizeof (struct event_));
  HANDLE event0_ = CreateEventW (null, ! auto_reset, kfalse, gsym_inos);
  assert (event0_ != NULL);
  if (GetLastError () == ERROR_ALREADY_EXISTS)
    if (had_open != null)
      * had_open = ktrue;
    else ;
  else if (had_open != null)
    * had_open = kfalse;
  cc->event0_ = event0_;
  * event = cc; 
}

void event_uninit (struct event_ **event) {
  if (event != null) {
    struct event_ *e = *event;
    *event = null;
    if (e != null) {
      if (e->event0_)
        CloseHandle (e->event0_);
      ufree (e);
    }
  }
}