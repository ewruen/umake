#if !defined (MSVC_ENV_241)
# define MSVC_ENV_241

# if defined (_WIN32)
#  include "config.h"
#  include "list.h"
int 
msvc_envdo (LIST_TYPE2_(userpath2) list);
# endif 

#endif 