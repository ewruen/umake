/* simple thread pool, XXX:volume contraction lack 
 *.*/

#if !defined (__UMAKE_THREAD_H_)
#  define __UMAKE_THREAD_H_

#include "config.h"
#include "uchar.h"

# define _DUMMY_KB (1024)
# define THREAD_SHARED_CAPACITY_KBSET (36*_DUMMY_KB)

struct thread_;
struct thread_ensemble_;

int thread_init (struct thread_ensemble_ **ensemble__, int nums);
void thread_uninit (struct thread_ensemble_ **ensemble__);
int thread_join (struct thread_ *thread);
void thread_close (struct thread_ **thread);
int thread_setnums_nosync_bad ( struct thread_ensemble_ *ensemble__, int total );
int thread_work ( struct thread_ensemble_ *ensemble__, 
struct thread_ **thread, 
  void (* routine)(void *param, void *stkshared_mem, int membytes_count), 
  void *param );
// XXX:simple .
int cpu_core_nums (void);

#endif 