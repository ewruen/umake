#include "ustream.h"
#include "file.h"
#include "locate.h"

int open_usfile (uconst us *path_name, struct ustream **uss) {
  return open_usfile2 (path_name, ktrue, uss, null);
}

int open_usfile2 (uconst us *ptname, kbool clearsome, struct ustream **uss, uint64_t *mod64) {

  int err_ =-1;

  if (    uss != null 
       && ptname != null
       && file_exist (ptname) == 0 )
  {
    struct ustream *p = umalloc ( sizeof (struct ustream));
    us *xfb = null;

    int as = 0;
    int istart = -1;
    int linec = 0;
    int id;
    int fsize;
    int fsize2;
    int udsize;
    int ap_cnt;
    int w32f_uni = IS_TEXT_UNICODE_REVERSE_MASK | IS_TEXT_UNICODE_UNICODE_MASK;
    wchar_t CNA = 0, CNB = 0;  /* for line wrap type */
    unsigned char *knb = null;
    unsigned char *knb2 = null;

    struct file_ *file;

    kbool isUtf8 = kfalse;
    kbool isUcs2 = kfalse ;
    kbool isRevse = kfalse;

    //udcc = udr;
    id = file_open (& file, ptname, FILE2_EXIST, FILE2_READ);
    assert (id == 0);
    id = file_size (file, & fsize);
    assert (id == 0);

    p->total = -1;
    p->line_gp = null;
    knb = ucalloc (fsize * 3 + 36, 1); //XXX:size 
    assert (knb != null);
    id = file_read (file, 0, knb, fsize, & linec /*temp use*/);
    assert (id == 0);
    assert (fsize == linec);
    
    /*check is exist utf8 bom?*/
    if (fsize >= 3
       && ( (knb[0] == 0xEF)
         && (knb[1] == 0xBB)
         && (knb[2] == 0xBF)) ) 
    {
      knb2 = & knb[3];
      fsize2 = fsize - 3;
      isUtf8 = ktrue;
    }
    /*check is exist ucs-2 bom?*/
    else if (fsize >= 2
      && ( (knb[0] == 0xFF)
      && (knb[1] == 0xFE)) ) 
    {
      knb2 = & knb[2];
      fsize2 = fsize - 2;
      isUcs2 = ktrue;
      isRevse = kfalse;
    }
    else if (fsize >= 2
      && ( (knb[0] == 0xFE)
      && (knb[1] == 0xFF)) ) 
    {
      knb2 = & knb[2];
      fsize2 = fsize - 2;
      isUcs2 = ktrue;
      isRevse = ktrue;
    }
    else 
    {
      knb2 = knb;
      fsize2 = fsize;
    }

    /* TODO: utf-8 test 
    1byte 0xxxxxxx 
    2byte 110xxxxx 10xxxxxx 
    3byte 1110xxxx 10xxxxxx 10xxxxxx 
    4byte 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
    5byte 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
    6byte 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (isUcs2 == kfalse
      &&  (isUtf8 == kfalse)) 
    {
      IsTextUnicode (knb2, fsize2, & w32f_uni);
      if (w32f_uni & IS_TEXT_UNICODE_UNICODE_MASK) 
        isUcs2 = ktrue;
      if (w32f_uni & IS_TEXT_UNICODE_REVERSE_MASK)
        isRevse = ktrue;
    }

    if (isUcs2 != kfalse) {
      xfb = (us * )knb2;
      udsize = fsize2/ 2;
    } else {
      udsize = MultiByteToWideChar (CP_UTF8, 0, & knb2[0], fsize2, (LPWSTR) & knb2[fsize2], fsize2 *2); 
      xfb = (LPWSTR) & knb2[fsize2];
    }

    if (isRevse) {
      /* swap it  */
      int id;
      for (id = 0; id != udsize; id++) {
        xfb[0] = (((uint16_t)(xfb[0])) >> 8) | ((((uint16_t)(xfb[0])) & 0xFF) << 8);
      }
    }

    /* get line count */
    p->total = 1;
    id = 0;
    
    for (;;)  {
      /** check wrapline type */
      if (id >= udsize) {
        break;
      }
      if (xfb[id] == 0x000D) {
        if (xfb[id+1] == 0x000A)  {
          ap_cnt = 2;
          CNA = 0x000D;
          CNB = 0x000A;
        } else {
          ap_cnt = 1;
          CNA = 0x000D;
        }
      } else if (xfb[id] == 0x000A) {
        if (xfb[id+1] == 0x000D) 
          assert (0);
        else {
          ap_cnt = 1;
          CNA = 0x000A;
        }
      } else {
        ++ id;
        continue;
      }
      break;
    }

    if ((!CNA) && !CNB) /* onel ine */
      achieve  L0;
    else ;

    for (id = 0;;)  {
      /** 0D0A */
      if (id >= udsize) {
        break;
      }
      if (xfb[id] == CNA) {
        if (ap_cnt == 1 || (xfb[id+1] == CNB) ) {
          p->total++;
          id +=ap_cnt;
          continue;
        } 
      }
      id += 1;
    }
 L0:if (id == 2 && (p->total)) {
      achieve cleanup;
    }

    /* alloc new buffer **/
    for (id = 0, 
      p->line_gp = umalloc (sizeof (struct raw_line *) * p->total)
      ; id != p->total; id++) {
        p->line_gp[id] = umalloc (sizeof (struct raw_line));
      uchar_init (& p->line_gp[id]->content);
      p->line_gp[id]->invalid = kfalse;
    }

    /* fill line **/

    id = 0;
    as = 0;
    linec = 0;

    for (;;)  {
      /** 0D0A */
      if (id >= udsize) {
        uchar_insert (p->line_gp[linec]->content, 0, & xfb[as], id - as);
        break;
      }       
      if (xfb[id] == CNA) {
        if (ap_cnt == 1 || (xfb[id+1] == CNB) ) {
          /* new line, collect previous **/
          uchar_insert (p->line_gp[linec]->content, 0, & xfb[as], id - as);
          linec++;
          as = id +ap_cnt;
          id +=ap_cnt;
          continue;
        } 
      }
      id += 1;
    }
    /* serach '\' */
    {
      int id;
      int ap_start = -1;
      int ap_lens = -1;
      kbool ap_line = kfalse;
      
      for (id = 0; id != p->total; id++) {
        struct raw_line *pt = p->line_gp[id];
        if (pt->content->length <= 0) {
          pt->invalid = ktrue;
          ap_line = kfalse;
        } else if (pt->content->str[pt->content->length -1] == _GC('\\')) {
          if (ap_line == kfalse) {
            ap_line = ktrue;
            ap_lens = 1;
            ap_start = id + 1;
          } else {
            ap_lens ++;
          }
        } else {
          ap_line = kfalse;
        }
        if (ap_line == kfalse && ap_lens != -1) {
          int ia;
          struct raw_line *cm = p->line_gp[ap_start -1];
          for (ia = ap_start; ap_lens; ap_lens--) {
            struct raw_line *tm = p->line_gp[ia];
            uchar_remove (cm->content, cm->content->length -1, 1);
            uchar_insert3(cm->content, -1, tm->content->str);
            tm->invalid = ktrue;
            ia ++;
          }
          ap_lens = -1;
        }
      }
    }
    /* comment check */
    {
      if (clearsome != kfalse)  {
        for (id = 0; id != p->total; id++) {
          struct raw_line *pt = p->line_gp[id];
          if (pt->invalid == kfalse) {
            struct locate_essen_ es;
            locate_attach_weak(& es, pt->content, null, id+1);
            if (locate_clearrc (& es) == 0)
              pt->invalid = ktrue;
            else { uchar_setlens (pt->content, es.doing.length); }
          }
        }
      }
    }
    if (mod64 != null) {
      int id = file_lastmod (file, mod64);
      assert (id == 0);
    }
    err_ = 0;

cleanup:
    if (knb != null) {
      ufree (knb);
    }
    if (file != null) {
      file_close (& file);
    }
    * uss = p;
  }
  return err_;
}


void remove_rawline (struct raw_line **n) {

  if (n != null) {
    if (* n != null) {
      struct raw_line *p = *n;
      * n= null;
      if (p->content != null) {
        uchar_uninit (& p->content);
      }
      ufree (p);
    }
  }
}

void close_usfile (struct ustream **us) {

  if (us != null) {
    if (* us != null) {
      struct ustream *p = *us;
      * us= null;
      if (p->line_gp != null && (p->total >0)) {
        int id;
        for ( id =0; id != p->total; id++) {
          remove_rawline (& p->line_gp[id]);      
        }
      }
      ufree (p->line_gp);
      ufree (p);
    }
  }
}

void outinfos_usfile (struct ustream *uss) {
  if (uss != null)  {
    int id;
    for (id =0; id != uss->total; id++) {
      kprint (_GR("line[%d?%ls]: %ls \r\n"), id+1, uss->line_gp[id]->invalid ?_GR("N"):_GR("E"), uss->line_gp[id]->content->str);
    }
  }
}

# if 0

void debug_output (struct ustream *s) {

  int id;
  for ( id =0; id != s->total; id++) {
    kprint (_GR("line:%d  "), id+1);
    if (! s->line_gp[id]->invalid)
      kprint (s->line_gp[id]->content->str);
    else kprint (_GR("invailed"));
    kprint (_GR("\n"));
  }

}
int main (void) {
  struct ustream *mq;
  uchar_ginit ();
  open_usfile(L"D:\\ppc.esc", & mq);
  debug_output (mq);
  close_usfile(& mq);
  uchar_guninit ();
  leak_infos_get();
  return 0;
}
# endif 