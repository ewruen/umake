#if !defined (__UMAKE_CONFIG_H_)
#  define __UMAKE_CONFIG_H_ 

/*
 * The initial allocation of a wide number of characters 
 */
#define DEFAULT_UCHAR_INIT_SIZE 32 

/*
 * If the hardware supports, open this macro
 *
 * XXX:
 * If CPU supports the CRC assembly instruction of SSE, 
 * it will use iSCSI (Castagnoli) polynomial, 
 * it's not the same as the standard CRC32 table[such as Ethernet/ZIP], please note
 *
 * Thanks stackoverflow's Mark Adler
 * Reference:http://stackoverflow.com/questions/29174349/mm-crc32-u8-gives-different-result-than-reference-code
 */
// #define _SIMD_CRC32_SUPPORT

/* #define __X86_64_ */
#define __X86_32_ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

/* Under windows, you can only use the CL/ICL compiler due to 
   the possibility of using the compiler's SEH processing routines */
#ifdef _WIN32 
# define UC_S__(sTrInG) L##sTrInG
# define UC_T__(sTrInG) L##sTrInG
# define _GR(sTrInG_) UC_S__(sTrInG_)
# define _GC(sTrInG_) UC_T__(sTrInG_)
# define _QR(sTrInG_) (sTrInG_)
# define _QC(sTrInG_) (sTrInG_)
# define SLASHCHAR "\\"
  static  wchar_t SLASH_CHAR_ = (wchar_t ) UC_T__('\\');
  static  wchar_t *SLASH_STRING_ = (wchar_t *) UC_S__("\\");
  static  wchar_t *WRAP_STRING_ = (wchar_t *) UC_S__("\r\n");
# define CRTDBG_MAP_ALLOC
#  if defined (_DEBUG)
#   define leak_infos_get _CrtDumpMemoryLeaks
#  else 
#   define leak_infos_get (void)0
#  endif
#
# include <crtdbg.h>
# include <windows.h>
# include <tlhelp32.h>
# include <psapi.h>
#endif 

#if defined (__X86_64_)
#  error "NO SUPPORT X64"
# elif ! defined (__X86_32_)
#  error "ERROR NO SELECT PLATFORM"
#endif 

#if defined (_MSC_VER)
# define finline static __forceinline 
# define fastcall __fastcall 
# define utls __declspec(thread)
#elif defined (__GNUC__)
# define finline static inline __attribute__((always_inline)) 
# define fastcall __attribute__((fastcall))
# define utls __thread
#endif 

#if !defined (uconst)
# define uconst
#endif 

#define U_SIGN(_xXyYzZ_)
#define U_LIST_TYPE_(_xXyYzZ_)
#define SIGN(_xXyYzZ_)
#define LIST_TYPE_(_xXyYzZ_)
#define LIST_TYPE2_(_xXyYzZ_) struct list_ *

  /* achieve */
# define achieve goto 

  /* for debug out **/
# if defined (_DEBUG)
#  define __DEBUGOUT(...) kprint(__VA_ARGS__)
# else 
#  define USELESS___
#  define __DEBUGOUT(...) USELESS___
# endif 

/* null **/
#if !defined (null)
# define null ((void *) 0)
#endif 

/* unicode string */
#if !defined (us)
# define us wchar_t
#endif 

/* boolean type */
#define kfalse 0
#define ktrue 1 

typedef int kbool;

/* memio **/
#if !defined (_DEBUG)
# define umalloc malloc 
# define ufree free 
# define urealloc realloc 
# define ucalloc calloc 
#else
static 
  void *umalloc (int size) 
    {
      void *p = malloc (size);
      assert (p != null);
      return p;
    }
static
  void ufree (void *p) 
    {
      assert (p != null);
      free (p);
    }
static
  void *urealloc (void *p, int s) 
    {
      void *u = realloc (p, s);
      assert (u != null);
      return u;
    }
static
  void *ucalloc (int count, int size) 
    {
      void *u = calloc (count, size);
      assert (u != null);
      return u;
    }
#endif 
 

#endif 