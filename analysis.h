#if !defined (__UMAKE_ANALYSIS_H_)
#  define __UMAKE_ANALYSIS_H_ 

# include "list.h"
# include "uchar.h"
# include "ustream.h"

#  if   SYNTAX_COMMENT

// <string> -IDEP|-IDEP_CPP|-ODEP|NULL
string := expr_string
string %= expr_string_path_auto 
string := expr_string_path_auto, <>, [ASL, ERST, PQAS], PREFIX_SET:expr_string, SUFFIX_SET?expr_string
string += expr_string
string %+ expr_string_path_auto
string += expr_string_path_auto, <>, [ASL, ERST, PQAS], PREFIX_SET:expr_string, SUFFIX_SET?expr_string
// <set> -IDEP|-IDEP_CPP|-ODEP|-VBATTER|-HBATTER 
set := set2
set @= expr_string_path_auto, <>         // spath_set->one spath, suffix_set->one set 
set := expr_string_path_auto, <>
set := expr_string_path_auto, <>, [ASL, ERST, PQAS]
set += expr_string_path_auto, <>
set += expr_string_path_auto, <>, [ASL, ERST, PQAS]
set -= expr_string_path_auto, <>
set -= expr_string_path_auto, <>, [ASL, ERST, PQAS]
set -= expr_string_path_auto
// <del_sync>  
del_sync expr_string_path_auto, <> [ASL, PSQA] 
del_sync expr_string_path_auto, <> 
// <del> 
del expr_string_path_auto, <> [ASL, PSQA] 
del expr_string_path_auto, <> 
// <echo_sync>
echo_sync expr_string
echo_sync expr_string_path_auto, <> [ASL, PSQA] 
echo_sync expr_string_path_auto, <> 
// <echo>
echo expr_string
echo expr_string_path_auto, <> [ASL, PSQA] 
echo expr_string_path_auto, <>
// <mkdir> 
mkdir expr_string_path_auto
// <sys>
sys expr_string
// <sys_sync>
sys_sync expr_string
// <stat> 
stat [expr_string_path_auto | expr_string | string | set ] -IDEP |-ODEP | -IDEP_CPP PREFIX_SET:expr_string, SUFFIX_SET?expr_string $ ANC

#  endif 

/*
  AUXILIARY_GENERATE_FLAGS
 */
# define AUXILIARY_GENERATE_FLAGS_N (1<<0) /* nocare exec result, otherwise execution failure will cause fatal error */
# define AUXILIARY_GENERATE_FLAGS_A (1<<1) /*  thrown into an asynchronous scan thread to execute  */
# define AUXILIARY_GENERATE_FLAGS_C (1<<2) /*  alloc new console  */
# define AUXILIARY_GENERATE_FLAGS_S (1<<3)/*  only output string  (sync) */
# define AUXILIARY_GENERATE_FLAGS_K (1<<4)/*  only delete file  (sync)  */ /* XXX:sync with use */
# define AUXILIARY_GENERATE_FLAGS_P (1<<5)/*  system  (sync)  */ /* XXX:sync with use */

/*  
   for match result 
 */
# define MATCH_FIND 0
# define MATCH_SFALSE 1
# define MATCH_FATAL_ERROR -1

typedef int match_result;

/* for leftVal_desc::assert_type**/
enum UOP_enum {
  UOP_STRING_ASSIGN = 0,
  UOP_STRING_PERCENT_ASSIGN,
  UOP_STRING_APPEND,
  UOP_STRING_PERCENT_APPEND,
  UOP_SET_ASSIGN,
  UOP_SET_AT,
  UOP_SET_APPEND,
  UOP_SET_SUB,
  UOP_KEYWORD_DEL,
  UOP_KEYWORD_DEL_SYNC,
  UOP_KEYWORD_ECHO,
  UOP_KEYWORD_ECHO_SYNC,
  UOP_KEYWORD_MAKEDIR,
  UOP_KEYWORD_SYS,
  UOP_KEYWORD_SYS_SYNC,
  UOP_KEYWORD_STAT,
  UOP_UNSET
};

static 
uconst us *gs_Keyword [] = {
# define KEYWORD_OP_DEL 0
  _GR ("del"), 
# define KEYWORD_OP_DEL_SYNC (KEYWORD_OP_DEL+1)
  _GR ("del_sync"), 
# define KEYWORD_OP_ECHO (KEYWORD_OP_DEL_SYNC+1)
  _GR ("echo"), 
# define KEYWORD_OP_ECHO_SYNC (KEYWORD_OP_ECHO+1)
  _GR ("echo_sync"),
# define KEYWORD_OP_MKDIR (KEYWORD_OP_ECHO_SYNC+1)
  _GR ("mkdir"),
# define KEYWORD_OP_SYS (KEYWORD_OP_MKDIR+1)
  _GR ("sys"),
# define KEYWORD_OP_SYS_SYNC (KEYWORD_OP_SYS+1)
  _GR ("sys_sync"),
# define KEYWORD_OP_STAT (KEYWORD_OP_SYS_SYNC+1)
  _GR ("stat")
};

static 
uconst us *gs_setVar_Op [] = {
# define SETVAR_AT 0
  _GR ("@="),   
# define SETVAR_ASSIGN (SETVAR_AT+1)
  _GR (":="),  
# define SETVAR_SUB (SETVAR_ASSIGN+1)
  _GR ("-="), 
# define SETVAR_APPEND (SETVAR_SUB+1)
  _GR ("+=")
};

static 
uconst us *gs_stringVar_Op [] = {
# define STRINGVAR_ASSGIN 0
  _GR (":="),   
# define STRINGVAR_APPEND (STRINGVAR_ASSGIN+1)
  _GR ("+="),  
# define STRINGVAR_PRECENT_ASSIGN_ (STRINGVAR_APPEND+1)
  _GR ("%="),
# define STRINGVAR_PRECENT_APPEND_ (STRINGVAR_PRECENT_ASSIGN_+1)
  _GR ("%+")
};

static 
uconst us *gs_stringCCl_Op [] = {
# define AGF_VBATTER 0
# define AGF_VBATTER_MTMASK (1 << AGF_VBATTER)
  _GR ("-VBATTER"),  
# define AGF_HBATTER (AGF_VBATTER+1)
# define AGF_HBATTER_MTMASK (1 << AGF_HBATTER)
  _GR ("-HBATTER"),   
# define AGF_INPUT_DEP (AGF_HBATTER+1)
# define AGF_INPUT_DEP_MTMASK (1 << AGF_INPUT_DEP)
  _GR ("-IDEP"),     
# define AGF_INPUT_DEP_CPP (AGF_INPUT_DEP+1)
# define AGF_INPUT_DEP_CPP_MTMASK (1 << AGF_INPUT_DEP_CPP)
  _GR ("-IDEP_CPP"),    
# define AGF_OUTPUT_DEP (AGF_INPUT_DEP_CPP+1)
# define AGF_OUTPUT_DEP_MTMASK (1 << AGF_OUTPUT_DEP)
  _GR ("-ODEP"),  
};

struct shell_desc {
  struct uchar *command; /* for AUXILIARY_GENERATE_FLAGS_K|AUXILIARY_GENERATE_FLAGS_S, output command/delete file */
  int flags;
};
struct string_var {
  struct uchar *name;
  struct uchar *mapper;
};
struct set_var {
  struct uchar *name;
  LIST_TYPE2_(spath) spath_chain; 
};
struct pass_varChain {
  LIST_TYPE2_(string_var) string_chain;
  LIST_TYPE2_(set_var) set_chain;
  struct uchar *passname;
};
struct pass_node {
  struct pass_node *level_n;
  struct pass_node *level_p;
  struct pass_node *parent;
  struct pass_node *child;
  struct pass_varChain *varChain; /* shared ptr with analysis::gsym **/
  struct uchar *passname;
  LIST_TYPE2_(shell_desc) shell_chain;
  int childs;
  int sstart;
  int sslen;
};
struct cppdep_node2 {
  struct uchar *atgpath; /* abs path with filename*/
  int64_t timeStamp64;
};
struct atdep {
  struct uchar *agpath;
  kbool update;
};
struct dep_cache  {
  struct uchar *agpath;
  int64_t timeStamp64;
};
struct kcache_chain { 
  uint64_t hdmod64;
  struct uchar *attach; /* pointer ref */
  LIST_TYPE2_(uchar) dt_chain;
};
struct analysis {
  LIST_TYPE2_(pass_varChain) gsym;
  LIST_TYPE2_(userpath2) userp;
  LIST_TYPE2_(precond_) prec;
  /*   if exist in this chain, then it  must be contaminated output dependency,
        entries that are dependent as output cannot be relied upon  again as output */
  LIST_TYPE2_(atdep) outdep;
  LIST_TYPE2_(dep_cache) dpcdep;  /* current's files dep cache */
  LIST_TYPE2_(uchar) collect;  /* current's files with -IDEP or -ODEP */
  LIST_TYPE2_(kcache_chain) kcc_chain; /* kcache_chain list */
  struct uchar *bpath;      /* base path for serach */
  struct pass_node *passroot;
  struct ustream *ust;
};

typedef int howInterrupt;

# define INTERRUPT_terminal_ch 0
# define INTERRUPT_byEOF 1

struct interrupt_desc {
  int sig_setnums;
  int sig_retnums;
  kbool interrupt_bysymbols;
  uconst us **terminal_symset;
  howInterrupt inter_reason;
};

/* left value desc */
struct leftVal_desc {
  enum UOP_enum uop_enum;
  struct pass_varChain *pvc;
  struct set_var *set;
  struct string_var *string;
  LIST_TYPE2_(set_var) set_chain;
  LIST_TYPE2_(string_var) string_chain;
};
struct stringv_desc {
  struct pass_varChain *pvarc;// only attach 
  struct string_var *strvar;// only attach 
};
struct setv_desc {
  struct pass_varChain *pvarc;// only attach 
  struct set_var *setvar;// only attach 
};
struct vnset {
  int bc_total;
  int bc_count;
  int s_count;
  int s_goal;
  int x_count;
  int x_goal;
  int k_count;
  int k_goal;
  struct list_v *sgp;
  struct list_v *xgp;
  struct list_v *kgp;
  kbool compelte;
};
enum  VECTOR_NODE_TYPE  {
  VECTOR_STRING,
  VECTOR_SET_VERT, // -IDEP/-IDEPCPP/-ODEP/none
  VECTOR_SET_HORI, // -IDEP/-IDEPCPP/none
  VECTOR_SET_PURE  // -IDEP/-IDEPCPP/-ODEP/none
};
enum  VECTOR_NODE_ATTR  {
  VECTOR_OUTPUT,
  VECTOR_INPUT, 
  VECTOR_NONE,
};
struct vector_node  {
  int vpflags;
# define CB_TAIL 1
# define CB_ALL 2
# define CG_HEAD 4
# define CG_ALL 8
  int hplink_flags;
  enum  VECTOR_NODE_TYPE type;
  enum  VECTOR_NODE_ATTR attr;
  struct uchar *clink;
  struct uchar *hlink;
  struct uchar *strbase;
  struct vnset vnset_;
};
struct vector_bset {
  int vnums; /* if noexist equal -1*/
  int ppflags;
  kbool upbase; 
  kbool input;
  //kbool outflags;
  struct vector_node *output;
  LIST_TYPE2_(vector_node) pnt; /* vector node*/
};
enum chunk_type {
  CHUNK_TYPE_ALTER,
  CHUNK_TYPE_PASS,
  CHUNK_TYPE_ETREEROOT,
  CHUNK_TYPE_ENVIR,
  CHUNK_TYPE_NOFIND
};
struct chunkccp {
  enum chunk_type ect_type;
  struct uchar *passname;
  int sstart;
  int len;
};
struct alter_ {
  struct uchar *name;
  struct uchar *ext;
};

void pass_tree_uninit (struct pass_node *pp_);

void sheel_desc_init (struct shell_desc **sh_desc);
void shell_desc_uninit (struct shell_desc **sh_desc);

void pass_varChain_init (struct pass_varChain **pv);
void pass_varChain_uninit (struct pass_varChain **pv);

void  atdep_init (struct atdep **at);
void  atdep_uninit (struct atdep **at);
void  dep_cache_init (struct dep_cache **at);
void  dep_cache_uninit (struct dep_cache **at);

void chunkccp_init (struct chunkccp **cc);
void chunkccp_uninit (struct chunkccp **cc);
int serach_chunk (struct list_ **chunk_chain, struct ustream *ust, struct uchar *err);
int do_etreeroot ( struct pass_node **pn, struct analysis *lysis, struct list_ *chunk_chain, struct ustream *ust, struct uchar *err);
int ae_replace ( struct list_ *chunk_chain, struct list_ *env2_chain, struct ustream *ust, struct uchar *bpath, struct uchar *err);
void do_pass ( struct pass_node *pn, struct analysis *lysis, struct uchar *err, int *ret);
void do_pass_shirk ( struct pass_node *pn);
void kcache_chain_uninit (struct kcache_chain **kcc_);

struct pass_varChain *
  varChain_findg (LIST_TYPE2_(pass_varChain) gsym, uconst us *pass_name);
struct pass_varChain *
  varChain_findg2 (LIST_TYPE2_(pass_varChain) gsym, uconst us *pass_name, int len);
struct string_var *
  stringVar_find (struct pass_varChain *varChain, uconst us *var_name); 
struct string_var *
  stringVar_find2 (struct pass_varChain *varChain, us *var_name, int len);
struct set_var *
  setVar_find2 (struct pass_varChain *varChain, us *var_name, int len);
struct set_var *
  setVar_find (struct pass_varChain *varChain, uconst us *var_name);
#endif 