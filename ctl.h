#if !defined (__UMAKE_CTL_H_)
#  define __UMAKE_CTL_H_ 

/* simple multi-threaded command line parameter tree generation  */

# include "config.h"
# include "list.h"
# include "uchar.h"
# include "ustream.h"
# include "event.h"
# include "sem.h"
# include "thread.h"
# include "process.h"
# include "analysis.h"
# include "file.h"

/* for async/host atomic stack */
# define CTL_LEVEL_1_SIGNAL 1
# define CTL_LEVEL_2_SIGNAL 2

/* simple shared memory set  */
# define PASS_CAPACITY_STATUS_BUFFER (28*_DUMMY_KB)
# define PASS_CAPACITY_PTRSET_BLOCK (4*_DUMMY_KB)

struct ctl_;
struct ctl_sweep;
struct ctl_base;
struct ctl_atomic_stack_pn;

struct ctl_atomic_stack_hd {
  union {
    struct ctl_atomic_stack_pn *level_n;
    intptr_t int_blk;
  };
  intptr_t aba_count;
};

struct ctl_atomic_stack_pn {
  union {
    struct ctl_atomic_stack_pn *level_n;
    intptr_t int_blk;
  };
  struct ctl_base *cdb;
};

struct ctl_base {
  struct process_ *handle;          /* process handle */
  struct semaphore_ **sem_nofity;    /* attach sem object */
  struct shell_desc * sh;   /* for exec */
  struct ctl_ *ctl;       /* main ctl */
  struct ctl_pass *cps;
  struct ctl_atomic_stack_hd *pas_wait; /* link object waiting stack for async scan */
                  /*   for asynchronous ctl_base, 
                           process handle needs to be synchronized */
  int status;     
};
struct ctl_pass {
  struct ctl_pass *parent;
  struct ctl_pass *child;
  struct ctl_pass *level_n;
  struct ctl_pass *level_p;
  struct ctl_ *ctl;
  struct semaphore_ *sem_nofity; /*  nofity cur wait-queue have remain process handle **/
  
  struct event_ *evt_gate; /*  in addition to the original root node, 
                             this object is required by\
                                the next sub nodes to wake up  */
  struct event_ **evt_attachParnet; /* attahch parent, if this node's work completes
                                  signal parent node, for the root node,\
                     this parameter is NULL */
  struct thread_ *thread_h; /* current thread handle */
  struct ctl_atomic_stack_hd as_wait; /* <atomic-stack> the process completes the stack of signals */
  struct ctl_atomic_stack_hd cmd_stack;
  int32_t _shell_cc;  /*shell nums total count */
  int32_t *_ptr_parentpass_total; /* for the parent node not root */
  int32_t _childpass_total; /* for the parent node not root */
  int32_t _childpass_total_dtor; /* for the parent node not root */
  struct ctl_atomic_stack_pn **cs_cmdgarbage; /* zombie, waiting for the recycled atom node */
  int32_t shp_ccmdg;
  unsigned char *shp_baseptr;      /*  shared state output-utf8 buffer  */
  int shp_basecount;
  int shp_basecount_re; /*remain count   */
};

/* asynchronous scan thread, \
   this feature may degrade performance */
struct ctl_sweep {
  struct ctl_ *ctl;
  struct ctl_atomic_stack_hd schedu_stack;
  struct semaphore_ *sem_gate; /* signal gate */
};

/* backend control */
struct ctl_ {
  struct ctl_pass *root;      /*  root node of the tree, typically, root is detach thread singal node */
  struct ctl_sweep *sweep;  /*  async scan  thread_set*/
  volatile int exit_code;         /* exit code for main [for host call] */
  volatile int as_total;       /* for async scan thread set */
  volatile int32_t ib_except;   /* assert fisrt except 0:true 1:false */
  struct thread_ **as_handle;         /*  for async scan thread set */
  struct thread_ensemble_ *th_factory;         /* async scan thread set, pass thread 's phandle */
  int32_t mspin_lock;              /* min spin */
  int32_t mp_count;        /* pass total */
};

//TODO: peephole optimization, (all is async, ...) 
//TODO: chain to array (maybe...)
//TODO: multiple queues (maybe...)
//TODO: asynchronous inside every thread node
void analysis_to_ctl (struct ctl_ **ctl, struct ctl_pass *ct, struct pass_node *pn);
int ctl_do (struct ctl_ **ctl0_);

#endif 