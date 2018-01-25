#if !defined (__UMAKE_EVENT_H_)
#  define __UMAKE_EVENT_H_ 

# include "config.h"
# include "list.h"
# include "uchar.h"

struct event_;
/* manual set-event
 */
void event_init (struct event_ **event);
void event_init2 (struct event_ **event, kbool auto_reset, uconst  us *gsym_inos, kbool *had_create);
int event_wait (struct event_ *event);
int event_signal (struct event_ *event);
int event_cancel (struct event_ *event);
void event_uninit (struct event_ **event);

#endif 