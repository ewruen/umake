#if !defined (__UMAKE_CPP_MMAP_H_)
#  define __UMAKE_CPP_MMAP_H_ 

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

int cppsrc_mapget_include ( LIST_TYPE2_(uchar) header_list, struct uchar *files, int64_t *mod64);

#endif 