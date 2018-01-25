# include "thread.h"
# include <windows.h>
# include <assert.h>
# include <process.h>
# include <stdlib.h>
# include <stdint.h>
# include <intrin.h>
# include "sem.h"
# include "cas.h"
# include "global.h"

/* threadset quit mask */
#define SUBEXE_EXIT_MASK 1

struct __pt_dynamic_chunk;
struct __pt_static_chunk;
struct __pt_atomic_stack_pn;
struct __pt_atomic_stack_hd;

struct thread_ {
  HANDLE event_join;
};

struct __pt_atomic_stack_pn {
  union {
    struct __pt_atomic_stack_pn *level_n;
    intptr_t int_blk;
  };
  union {
    struct __pt_static_chunk *__static;
    struct __pt_dynamic_chunk *__dynamic;
  };
};

struct __pt_atomic_stack_hd {
  union {
    struct __pt_atomic_stack_pn *level_n;
    intptr_t int_blk;
  };
  intptr_t aba_count;
};

struct tsdummy2 {
  uint32_t release;
  uint32_t total;
};

/** thread_ensemble_ */
struct thread_ensemble_ {
  union {
    uint32_t unused[2];
    struct tsdummy2 atomic_cc;
  };
  struct semaphore_ *sem_signal;                               
  struct __pt_atomic_stack_hd schedule;
  struct __pt_atomic_stack_hd stat;
  struct __pt_atomic_stack_hd dym_blkset; // free-list 
};

struct __pt_static_chunk {
  HANDLE handle;
  DWORD id;
};

struct __pt_dynamic_chunk {
  void (* produce)(void *param, void *stkshared, int stk_bytes);
  void *param;
  struct thread_ensemble_ *ens;
  struct __pt_static_chunk *static_link;
  HANDLE join;
};

/*
  threadset entry 
*/
static 
int __stdcall _threadstartex0 (struct thread_ensemble_ *ens) {

  do 
  {
    struct __pt_atomic_stack_hd au;
    struct __pt_atomic_stack_hd av;
    struct __pt_atomic_stack_hd *ap = & ens->schedule;
    struct __pt_dynamic_chunk *__ptc = null;
    uint32_t *__prelease = null;
    uint8_t __stkshared[THREAD_SHARED_CAPACITY_KBSET];
    int sigp;
    int sigj;

    sigp = semaphore_passeren (ens->sem_signal);
    assert (sigp == 0);

    do 
    {
   L0:sigj = 0;
      au.level_n = ap->level_n;
      au.aba_count = ap->aba_count;
      av.level_n = au.level_n;
      av.aba_count = au.aba_count + 1;
      if (au.level_n == null)
        achieve L0;
      else if (au.int_blk & SUBEXE_EXIT_MASK)
        sigj = -1;
      if ( (au.int_blk & ~(intptr_t)SUBEXE_EXIT_MASK) != 0) {
        av.level_n = ((struct __pt_atomic_stack_pn *)(au.int_blk & ~(intptr_t)SUBEXE_EXIT_MASK))->level_n;
        av.int_blk |= (au.int_blk & SUBEXE_EXIT_MASK);
      } 
    } while ( casdp3 ( (void **) ap,  
                       (void **) & av,  
                       (void **) & au ) != ktrue );

    au.int_blk &= ~SUBEXE_EXIT_MASK;
    if (sigj == 0) {
      if (au.level_n != null) {
        achieve L3;
      } else {
        assert (0);
      }
    } else {
      /* quit signal */
      if (au.level_n != null) {
        /* run routine  */
      L3:__ptc = au.level_n->__dynamic;
        __prelease  =& __ptc->ens->atomic_cc.release;
        if (__ptc->produce != null)
          __ptc->produce (__ptc->param, & __stkshared[0], THREAD_SHARED_CAPACITY_KBSET);
        if (__ptc->join != null) {
          SetEvent (__ptc->join);
          __ptc->join = null;
        }
        /* insert free list  */
        do {
          av.level_n = ens->dym_blkset.level_n;
          av.aba_count = ens->dym_blkset.aba_count;
          au.level_n->level_n = av.level_n;
          au.aba_count = av.aba_count + 1;
        } while ( casdp3 ( (void **) & ens->dym_blkset,  
                           (void **) & au,  
                           (void **) & av ) != ktrue );
        add32 (__prelease, 1);
      } else {
        return 0;
      }
    }
  } while (TRUE);

  return 0;
}

int thread_init (struct thread_ensemble_ **ensemble, int nums) {

  int id;
  struct thread_ensemble_ ensemble0s_;
  struct thread_ensemble_ *ensemble0p_ = null;

  if (nums < 0) {
    * ensemble = null;
    return -1;
  }

  ensemble0s_.atomic_cc.release = nums;
  ensemble0s_.atomic_cc.total = nums;
  ensemble0s_.schedule.aba_count = 0;
  ensemble0s_.schedule.level_n = null;
  ensemble0s_.stat.aba_count = 0;
  ensemble0s_.stat.level_n = null;
  ensemble0s_.dym_blkset.int_blk = 0;
  ensemble0s_.dym_blkset.aba_count = 0;

  ensemble0p_ = umalloc (sizeof (struct thread_ensemble_));
  semaphore_init (& ensemble0s_.sem_signal, 0);

  /* copy instance **/
  (void ) memcpy (ensemble0p_, & ensemble0s_, sizeof (struct thread_ensemble_));

  for (id = 0; id != nums; id++) 
  {
    struct __pt_static_chunk *sc = umalloc (sizeof (struct __pt_static_chunk));
    struct __pt_atomic_stack_pn *pn = umalloc (sizeof (struct __pt_atomic_stack_pn));
    pn->__static = sc;
    pn->level_n = ensemble0p_->stat.level_n;
    ensemble0p_->stat.level_n = pn;
    
    sc->handle = (HANDLE) _beginthreadex (null, STACK_SIZE_PARAM_IS_A_RESERVATION, _threadstartex0, ensemble0p_, 0, & sc->id);
    assert (sc->handle != null);
  }

  * ensemble = ensemble0p_;
  return 0;
}

int thread_setnums_nosync_bad ( struct thread_ensemble_ *ensemble__, int total )
{
  __try 
  {
    if (ensemble__ != null) {
      if (ensemble__->atomic_cc.total < total)  {
        int s = total - ensemble__->atomic_cc.total;
        int id;
        for (id = 0; id != s; id++) 
        {
          struct __pt_static_chunk *sc = umalloc (sizeof (struct __pt_static_chunk));
          struct __pt_atomic_stack_pn *pn = umalloc (sizeof (struct __pt_atomic_stack_pn));
          pn->__static = sc;
          pn->level_n = ensemble__->stat.level_n;
          ensemble__->stat.level_n = pn;

          sc->handle = (HANDLE) _beginthreadex (null, STACK_SIZE_PARAM_IS_A_RESERVATION, _threadstartex0, ensemble__, 0, & sc->id);
          assert (sc->handle != null);
        } 
        ensemble__->atomic_cc.total += s;
        ensemble__->atomic_cc.release += s;
      }
      return 0;
    }
    return -1;
  }
  __except (EXCEPTION_CONTINUE_EXECUTION) {
    return -1;
  }
  return -1;
}

int thread_work ( struct thread_ensemble_ *ensemble__, 
                  struct thread_ **thread, 
                  void (* routine)(void *param, void *stkshared_mem, int membytes_count), 
                  void *param )
{
  BOOL request_new;
  BOOL request_event = !! thread;

  __try 
  {
    struct tsdummy2 tu, tv, trs;
    struct __pt_atomic_stack_hd au, av;

    if (ensemble__->schedule.int_blk & SUBEXE_EXIT_MASK) {
      return -1;
    }

    do 
    {
      tu.release = ensemble__->atomic_cc.release;
      tu.total = ensemble__->atomic_cc.total;

      if (tu.release > 0) {
          /* remain free thread */
        tv.release = tu.release - 1;
        tv.total = tu.total;
        trs.release = 1;
        trs.total = 0;
        request_new = FALSE;
      } else {
        /*  must alloc new thread   */
        tv.release = tu.release;
        tv.total = tu.total + 1;
        trs.release = 0;
        trs.total = -1;
        request_new = TRUE;
      }
    } while ( casdp3 ( (void **) & ensemble__->atomic_cc,  
                       (void **) & tv,  
                       (void **) & tu ) != ktrue );

    if (ensemble__->schedule.int_blk & SUBEXE_EXIT_MASK) {
      /** backup */
      add64 (& ensemble__->atomic_cc, * (int64_t *)& trs);
      return -1;
    } else {

      struct __pt_dynamic_chunk *dc;// = umalloc (sizeof (struct __pt_dynamic_chunk));
      struct __pt_atomic_stack_pn *pn;// = umalloc (sizeof (struct __pt_atomic_stack_pn));
      struct thread_ *ed = null; // umalloc (sizeof (struct thread_));

      do 
      {
        au.level_n = ensemble__->dym_blkset.level_n;
        au.aba_count = ensemble__->dym_blkset.aba_count;
        av.level_n = au.level_n;
        av.aba_count = au.aba_count + 1;
        if (au.level_n == null)
          achieve Q7;
        else if (au.level_n != null)
          av.level_n = au.level_n->level_n;
      } while ( casdp3 ( (void **) & ensemble__->dym_blkset,  
                         (void **) & av,  
                         (void **) & au ) != ktrue );

      dc = au.level_n->__dynamic;
      dc->join = null;
      pn = au.level_n; 
      achieve Q9;
Q7:   dc = umalloc (sizeof (struct __pt_dynamic_chunk));
      pn = umalloc (sizeof (struct __pt_atomic_stack_pn));
Q9:   dc->ens = ensemble__;
      dc->join = request_event ? CreateEventW (null, TRUE, FALSE, null) : null;
      dc->param = param;
      dc->produce = (void *) routine;
      dc->static_link = null; // XXX:current maybe useless .

      if (dc->join != null) {
        ed = umalloc (sizeof (struct thread_));
        ed->event_join = dc->join;
      }

      pn->__dynamic = dc;
      pn->level_n = null;
       
      /* insert dispatch stack */ 
      do {
        au.level_n = ensemble__->schedule.level_n;
        au.aba_count = ensemble__->schedule.aba_count;
        av.level_n = au.level_n;
        av.aba_count = au.aba_count + 1;
        if (! (au.int_blk & SUBEXE_EXIT_MASK)) {
          pn->level_n = au.level_n;
          av.level_n = pn;
        }
      } while ( casdp3 ( (void **) & ensemble__->schedule,  
                       (void **) & av,  
                       (void **) & au ) != ktrue );
      if (au.int_blk & SUBEXE_EXIT_MASK) {
        add64 (& ensemble__->atomic_cc, * (int64_t *)& trs);
        if (dc != null)
          ufree (dc);
        if (pn != null)
          ufree (pn);
        if (ed != null)
          CloseHandle (ed);
        return -1;
      } else if (request_new != kfalse) {
        struct __pt_static_chunk *sc = umalloc (sizeof (struct __pt_static_chunk));
        struct __pt_atomic_stack_pn *pn = umalloc (sizeof (struct __pt_atomic_stack_pn));
        pn->__static = sc;
        pn->level_n = null;
        do {
          au.level_n = ensemble__->stat.level_n;
          au.aba_count = ensemble__->stat.aba_count;
          av.level_n = au.level_n;
          av.aba_count = av.aba_count + 1;
          pn->level_n = au.level_n;
          av.level_n = pn;
        } while ( casdp3 ( (void **) & ensemble__->stat,  
                         (void **) & av,  
                         (void **) & au ) != ktrue );

        sc->handle = (HANDLE) 
          _beginthreadex (null, STACK_SIZE_PARAM_IS_A_RESERVATION, _threadstartex0, ensemble__, 0, & sc->id);
        assert ( sc->handle != null);
      }
      
      { 
        // LONG tmp; // PV-op 
        semaphore_vrijgeven (ensemble__->sem_signal);
      }
      if (request_event != FALSE) {
        * thread = ed;
      }
      return 0;
    }
  }
  __except (EXCEPTION_CONTINUE_EXECUTION) {
    return -1;
  }

  return 0;
}

int thread_join (struct thread_ *thread) {
  int s = -1;
  if (thread != null) {
    if (WaitForSingleObject (thread->event_join, INFINITE) == WAIT_FAILED)
      win32_thread_local_err = GetLastError();
    else s = 0;
  }
  return s;
}

void thread_close (struct thread_ **thread) {
  if (thread != null) {
    struct thread_ *ed = *thread; 
    *thread = null;
    if (ed != null) {
      if (ed->event_join != null)
        CloseHandle (ed->event_join);
      ufree (ed);
    }
  }
}

void thread_uninit (struct thread_ensemble_ **ensemble__) {

  if (ensemble__ != null) {
    struct thread_ensemble_ *ens = *ensemble__;
    *ensemble__ = null;
    if (ens != null) {
      /* block schedule ..*/
      or32 (& ens->schedule.int_blk, SUBEXE_EXIT_MASK);
      semaphore_vrijgevenall (ens->sem_signal);
      do {
        /* pop it */
        struct __pt_atomic_stack_hd au;
        struct __pt_atomic_stack_hd av;
        struct __pt_atomic_stack_pn *ap;

        do 
        {
          au.level_n = ens->stat.level_n;
          au.aba_count = ens->stat.aba_count;
          av.level_n = au.level_n;
          av.aba_count = au.aba_count + 1;
          if (au.level_n == null)
            continue;
          if ( au.level_n != null) 
            av.level_n = au.level_n->level_n;
        } while ( casdp3 ( (void **) & ens->stat,  
                            (void **) & av,  
                            (void **) & au ) != ktrue );

        if (au.level_n != null) {
          WaitForSingleObject (au.level_n->__static->handle, INFINITE);
          CloseHandle (au.level_n->__static->handle);
          ufree (au.level_n->__static);
          ufree (au.level_n);
        } else {
           // no find 
           // XXX:  spin competitive consumption
           if (     (ens->atomic_cc.release == ens->atomic_cc.total) //XXX:
               &&   (ens->stat.level_n == null)
               &&   ((ens->schedule.int_blk & (~(intptr_t)SUBEXE_EXIT_MASK))  == 0) )
           {
             // release all dym-block  
             for (ap = ens->dym_blkset.level_n; ap != null; ) {
               struct __pt_atomic_stack_pn *cc = ap->level_n;
               ufree (ap->__dynamic);
               ufree (ap);
               ap = cc;
             }
             break;
           }
        }
      } while (ktrue);

      semaphore_uninit (& ens->sem_signal);
      ufree (ens);
    }
  }
}

int cpu_core_nums (void) {

  SYSTEM_INFO info;

  GetSystemInfo (& info);
  return info.dwNumberOfProcessors;
}

# if 0

int cc (void *pc, void *s88, void *s){
  wprintf (L"Hello WOrld\n");
  //printf ("Hello World");
  //malloc (55);
}

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
int main (void) {
#define NUMBS 32
  int id;
  struct thread_ensemble_ *SM;
  struct thread_ *kk;
  thread_init (& SM, 0);
  thread_setnums_nosync_bad (SM, NUMBS);
  for (id = 0; id != NUMBS; id++)
    thread_work (SM, null, cc, null);

  _CrtDumpMemoryLeaks();

  thread_uninit (& SM);
  // thread_close (& kk);
  // while (TRUE) {}
  putchar ('S');

  _CrtDumpMemoryLeaks();
  return 0;
}

# endif 