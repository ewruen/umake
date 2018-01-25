#if !defined (__UMAKE_SEM_H_)
#  define __UMAKE_SEM_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"

struct semaphore_;

void semaphore_init (struct semaphore_ **sem, int s);
int semaphore_passeren (struct semaphore_ *sem);
int semaphore_vrijgeven (struct semaphore_ *sem);
int semaphore_vrijgeven_n (struct semaphore_ *sem, int n);
int semaphore_vrijgevenall (struct semaphore_ *sem);
void semaphore_uninit (struct semaphore_ **sem);

#endif 