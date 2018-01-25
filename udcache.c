#include "udcache.h"
#include "file.h"
#include "env.h"
#include "ustream.h"
#include "locate.h"

void udcache_line_uninit (struct udcache_line **udl) {
  if (udl != null) {
    struct udcache_line *udl_ = *udl;
    * udl = null;
    if (udl_ != null) {
      if (udl_->fullpath)
        uchar_uninit (& udl_->fullpath);
      else ;
      ufree (udl_);
    }
  }
}

void udcache_line_init (struct udcache_line **udl) {
  struct udcache_line *udl_ = umalloc (sizeof (struct udcache_line));
  udl_->timestamp64 = 0;
  uchar_init (& udl_->fullpath);
  * udl = udl_;
}

void udcache_uninit (struct udcache **udcache_) {
  if (udcache_ != null) {
    struct udcache *udc = *udcache_;
    if (udc != null) {
      if (udc->count > 0 && udc->ucache_asr) {
        int id; 
        for (id =0; id != udc->count; id++) {
          if (udc->ucache_asr[id].fullpath != null)
            uchar_uninit (& udc->ucache_asr[id].fullpath);
          else ;
        }
        if (udc->ucache_asr)
          ufree (udc->ucache_asr);
        else ;
      }
      ufree (udc);
    }
  }
}

int udcache_find (struct udcache *udcache_, struct uchar *fname, int64_t *timestamp64) {
  if (udcache_ && fname) {
    if (udcache_->count > 0) {
      int id;
      for (id = 0; id != udcache_->count; id++) {
        // if (udcache_->ucache_asr[id].disable == kfalse) {
          if (! path_cmp (udcache_->ucache_asr[id].fullpath, fname)) {
            if (timestamp64 != null)
              * timestamp64 = udcache_->ucache_asr[id].timestamp64;
            else ;
            return 0;
          }
        // }
      }
    }
  }
  return -1;
}

int udcache_load (LIST_TYPE2_(userpath2) uplist, uconst us *file_name, struct udcache **udcache_) {

  us uni_cahce[512];
  us uni_cc[96];
  struct uchar *uct =null;
  struct uchar *ucc = null;
  struct udcache *udc = null;
  struct ustream *usta= null;
  struct locate_desc_ ld[3];
  int id;
  int crc32 = crc32_get (0, file_name, wcslen (file_name) * sizeof (us));
  swprintf (uni_cc, sizeof (uni_cc)/ sizeof (uni_cc[0]), _GR("%lc0x%08X.udcache"), SLASH_CHAR_, crc32);
  ld[0].etype = LOCATE_TYPE_STRING;
  ld[1].etype = LOCATE_TYPE_NUMBER;
  env2_find_weak (uplist, _GR ("UMAKE"), wcslen (_GR ("UMAKE")), & uct );
  uchar_insert3 (uct, -1, SLASH_STRING_);
  uchar_insert3 (uct, -1, uni_cc);

  /* find file **/
  if ( 0 !=open_usfile (uct->str, & usta) ||(usta->total == 1 && (usta->line_gp[0]->content->length <= 0))) {
    uchar_uninit (& uct);
    close_usfile(& usta);
    return -1;
  }
  /* decode it **/
  udc = umalloc (sizeof (struct udcache));
  udc->count = usta->total;
  udc->ucache_asr  = umalloc (sizeof (struct udcache_line) * usta->total);
  uchar_uninit (& uct);
  for (id = 0; id != usta->total; id++) {
    struct locate_essen_ es;
    locate_attach (& es, usta->line_gp[id]->content, id+1);
    locate_searchp(& es, & ld[0], 2, null);
    uchar_init (& udc->ucache_asr[id].fullpath);
    uchar_assign (udc->ucache_asr[id].fullpath, ld[0].acceptORacc, ld[0].ac_lens);
    ld[1].acceptORacc[ld[1].ac_lens] = 0;
#  if  defined (_WIN32)
    udc->ucache_asr[id].timestamp64 = _wtoi64 (ld[1].acceptORacc);
#  endif 
  }
  * udcache_ = udc;
  close_usfile(& usta);
  return 0;
}

int udcache_open (LIST_TYPE2_(userpath2) uplist, uconst us *file_name, udcache_t *udc_t) {

  us uni_cc[96];
  struct uchar *uct =null;
  struct uchar *ucc = null;
  struct udcache *udc = null;
  struct locate_desc_ ld[3];
  int crc32 = crc32_get (0, file_name, wcslen (file_name) * sizeof (us));
  FILE *fd;
  swprintf (uni_cc, sizeof (uni_cc)/ sizeof (uni_cc[0]), _GR("%lc0x%08X.udcache"), SLASH_CHAR_, crc32);
  ld[0].etype = LOCATE_TYPE_STRING;
  ld[1].etype = LOCATE_TYPE_NUMBER;
  env2_find_weak (uplist, _GR ("UMAKE"), wcslen (_GR ("UMAKE")), & uct );
  uchar_insert3 (uct, -1, SLASH_STRING_);
  uchar_insert3 (uct, -1, uni_cc);

  /* find file **/
  if ( (fd = _wfopen (uct->str, _GR("wb"))) == null) {
    uchar_uninit (& uct);
    return -1;
  }
  uchar_uninit (& uct);
  * udc_t  = *(udcache_t *) & fd;
  fseek (fd, 0, SEEK_SET);
  return 0;
}

void udcache_close ( udcache_t *udc_t) {
  FILE *p= * udc_t;
  fclose (p);
}

int udcache_appenditem (udcache_t udc_t, uconst us *fullpath, int64_t timestamp64) {
# if defined (_WIN32)
  fwprintf (udc_t, _GR("\"%ls\" %I64u\n"), fullpath, timestamp64);
# else 
  fwprintf (udc_t, _GR("\"%ls\" %llu\n"), fullpath, timestamp64);
# endif 
  return 0;
}


