#if !defined (__UMAKE_PATTERN_H_)
#  define __UMAKE_PATTERN_H_ 

# include "config.h"
# include "list.h"
# include "uchar.h"
# include "analysis.h"

enum AMT_MATCH_TYPE {
  AMT_MATCH = 0,
  AMT_MATCH2,
  AMT_MATCH3,
  AMT_MATCH4,
  AMT_MATCH5,
  AMT_MATCH6
};

struct __patternfx {
  struct uchar *template_;
  kbool complete;
};

struct __pattern {
  struct set_var *set;
  struct uchar *expr_string;
  struct uchar *suffix;
  struct __patternfx pref;
  struct __patternfx suff;
  struct list_ *block_list;
  enum AMT_MATCH_TYPE amat;
};

struct __pattern2_node {
  struct set_var *set;
  struct uchar *expr_base; // only attach 
  struct vnset vnset_;
  int enum_dep; 
};

struct __pattern2 {

  int ag_flags;
  int vbat_n;

  struct __pattern2_node *pat_out;
  struct list_ *nd_list;

  kbool update_;
  kbool input_;
};


int pattern_readline ( struct __pattern *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err );
int pattern2_preprocessed ( struct __pattern2 *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err );
int pattern2_calc ( struct __pattern2 *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err );
void pattern_init (struct __pattern *__pat);
void pattern2_init (struct __pattern2 *__pat);
void pattern_uninit (struct __pattern *__pat);
void pattern2_uninit (struct __pattern2 *__pat);

#endif 