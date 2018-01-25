# include "analysis.h"
# include "ctl.h"
# include "env.h"
# include "file.h"
# include "locate.h"
# include "precond.h"
# include "udcache.h"
# include "msvc_env.h"
# include "path.h"
# include <Shlwapi.h>

// TODO://./
int main (void) {
    
  us *ap = _GR ("umake ~ simple make util for windows");
  us *cmda = GetCommandLineW();
  us *cmdw;
  us cmdbuff[MAX_PATH];
  us xfb[MAX_PATH];

  int rets= -1;
  int sig =0, s = -1;
  int s2;
  int q;
  int b, p, d;
  int id;
  
  kbool oc = kfalse;

  BOOL bSuccess;

  struct uchar *err_ = null;
  struct uchar *bpath = null;
  struct uchar *ap_bin = null;
  struct uchar *abp_bin = null;
  struct uchar *qa = null;
  struct uchar *qa2 = null;
  struct uchar *qa3 = null;
  struct analysis *lysis = null;
  struct ctl_ *ctl = null;
  struct list_ *chunkq = null;

  if (cmda == null || ( (q = wcslen (cmda)) <= 0))
    achieve LLM;
  else; 
  bSuccess = SetPriorityClass (GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
  assert (bSuccess != FALSE);
  bSuccess = SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
  assert (bSuccess != FALSE);
  bSuccess = SetProcessPriorityBoost (GetCurrentProcess(), TRUE);
  assert (bSuccess != FALSE);
  bSuccess = SetThreadPriorityBoost (GetCurrentThread(), TRUE);
  assert (bSuccess != FALSE);

  ZeroMemory (cmdbuff, sizeof (cmdbuff));
  memcpy (cmdbuff, cmda, wcslen (cmda) *sizeof (us));
  PathRemoveBlanksW (cmdbuff);
  q = wcslen (cmdbuff);
  /* skip self command  */
  cmda = PathGetArgsW (cmdbuff);
  PathRemoveBlanksW (cmda);
  if ( ((uintptr_t)cmda) > (uintptr_t)& cmdbuff[q]) {
LLM: kprint2 (ap);
      system ("pause");
      return 0;
  }
  if ((q = wcslen (cmda)) <= 0)
    achieve LLM;
  else if ((d = ((cmda[0] == _GC ('"')) ? 0 : 1)) == 0) {
    cmda = & cmda[1];
    q --;
  }
  cmdw = cmda;
  // set console use UTF-8
  SetConsoleCP (CP_UTF8);
  SetConsoleOutputCP (CP_UTF8);

  for (id = 0; id < q; id++) {
    us cc = cmda[id];
    if ( (cc == _GC(' ')
      || cc == _GC('\t') 
      ||  cc == _GC('\r')
      ||cc == _GC('\n')) && (d == 1))
    {
      if (id == (q - 1))
        oc = ktrue;
      s = id;
      q = q - id - 1;
      cmda = & cmda[id+1];
      achieve Q3 ;
    }
    if (cc == _GC ('"') && (d == 0))
    {
      if (id == (q - 1))
        oc = ktrue;
      s = id - 1;
      q = q - id - 1;
      cmda = & cmda[id+1];
      achieve Q3 ;
    }
  }
  // over ?? 
  if (s == -1 && (d == 0)) {
    kprint2 (_GR ("command erros, mismatch right \""));
    return -1;
  } else {
    // backup one ch, calc 
    cmda = & cmda[q];
    s = id - 1;
    q = q - s;
    oc = ktrue;
  }
Q3:
  uchar_ginit ();
  /* init analysis obj*/
  lysis = umalloc (sizeof (struct analysis));
  list_init (& lysis->collect);
  list_init (& lysis->dpcdep);
  list_init (& lysis->gsym);
  list_init (& lysis->outdep);
  list_init (& lysis->prec);
  list_init (& lysis->userp);
  list_init (& lysis->kcc_chain);
  lysis->passroot = null;
  lysis->ust = null;

  GetCurrentDirectoryW (sizeof (xfb), xfb);
  uchar_init (& bpath);
  uchar_assign (bpath, xfb, wcslen (xfb));
  env2_insert (lysis->userp, _GR ("UMAKE_HOST"), bpath->str, wcslen ( _GR ("UMAKE_HOST")), bpath->length);
  uchar_init (& ap_bin);
  uchar_assign (ap_bin, cmdw, s + 1);
  if ( path_inheritgetbin (& abp_bin, bpath, ap_bin) != 0
    || (file_lastmod9 (abp_bin->str, null) != 0)) 
  {
    // errors 
    kprint2 (_GR ("errors source path/file"));
    achieve cleanup;
  }
  uchar_init (& lysis->bpath);
  slashclr_getpath (abp_bin->str, abp_bin->length, & s);
  uchar_assign (lysis->bpath, abp_bin->str, s);
  env2_insert (lysis->userp, _GR ("UMAKE_FILE"), abp_bin->str, wcslen ( _GR ("UMAKE_FILE")), s);
  GetModuleFileNameW (NULL, xfb, sizeof (xfb));
  slashclr_getpath (xfb, wcslen (xfb), & s);
  env2_insert (lysis->userp, _GR ("UMAKE"), xfb, wcslen ( _GR ("UMAKE")), s);


__claenup:;

  if (oc == ktrue) {
    // nocontent 
    achieve LLG;
  }
  // get -setenv|-setdef 
  {
    struct locate_desc_ ld[6];
    struct locate_desc_ lq[5];
    struct locate_desc_ lp[6];

    struct locate_essen_ ess;

    us xfb2[512];
    us  *ccommand[] =  {
      _GR ("setenv"),
      _GR ("setdef"),
      _GR ("openlog"),
      _GR ("ttyclose"),
      _GR ("about")
    };
    us  *ccm[] = {
      _GR ("]"),
      _GR (",")
    };

    ld[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7;
    ld[1].dmtach_blk = & ccommand[0];
    ld[1].sig_setnums = sizeof (ccommand)/ sizeof (ccommand[0]);

    LOCATE_SETACC_RELAXED_CONST (& lq[0], _GR ("["));
    lq[1].etype = LOCATE_TYPE_VARIABLE;
    LOCATE_SETACC_TIGHTLY_CONST (& lq[2], _GR (":="));
    lq[3].etype = LOCATE_TYPE_STRING;
    lq[4].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
    lq[4].dmtach_blk = & ccm[0];
    lq[4].sig_setnums = sizeof (ccm)/ sizeof (ccm[0]);

    LOCATE_SETACC_RELAXED_CONST (& lp[0], _GR ("["));
    lp[1].etype = LOCATE_TYPE_VARIABLE;
    LOCATE_SETACC_TIGHTLY_CONST (& lp[2], _GR (":="));
    lp[3].etype = LOCATE_TYPE_NUMBER;
    lp[4].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
    lp[4].dmtach_blk = & ccm[0];
    lp[4].sig_setnums = sizeof (ccm)/ sizeof (ccm[0]);

    LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR ("-"));

    uchar_init (& qa);
    uchar_assign (qa, & cmda[0], q);

    locate_attach (& ess, qa, -1 );

    while  (ktrue)   {
      LB:if (locate_uselessdata(& ess) == 0)
          break  ;
        if (locate_searchp (& ess, & ld[0], 2, null) == 0)   {
          struct uchar *ua= null, *ub;
          switch  (ld[1].sig_match)  {  
          case  0: // -setenv  
            {
                if (locate_searchp (& ess, & lq[0], 1, null) == 0)  {
                    ua= null;
                    while (ktrue)  {
                      if (locate_searchp (& ess, & lq[1], 4, null) == 0)  {
                        //check sname  
                        if ( env2_exist_weak (lysis->userp, lq[1].acceptORacc, lq[1].ac_lens) == 0)   {
                            // define redefine 

                        }
                        uchar_init (& ub);
                        uchar_assign (ub, lq[3].acceptORacc, lq[3].ac_lens);
                        path_inheritgetbin (& ua, bpath, ub);
                        env2_insert (lysis->userp, lq[1].acceptORacc, ua->str, lq[1].ac_lens, s);
                        uchar_uninit (& ua);
                        uchar_uninit (& ub);

                        if (lq[4].sig_match == 0)
                          achieve LB;
                        else ;
                      }
                    }

                    // nomatch 
                }
                // no match [ 
            }
          case  1: // -setdef  
            // match [ 
            // match def, def ] 
            // def | def := number 
            {
              if (locate_searchp (& ess, & lp[0], 1, null) == 0)  {
                ua= null;
                while (ktrue)  {
                  kbool numb = kfalse;
                  if (locate_searchp (& ess, & lq[1], 3, null) == 0)  {
                    numb = ktrue;
                    achieve PP;
                  } else if (locate_searchp (& ess, & lq[1], 1, null) == 0)  {
                    struct precond_ *prec;
                PP: if (precond_exist (lysis->prec, lp[1].acceptORacc, lp[1].ac_lens) != null)  {

                    }
                    precond_init(& prec);
                    prec->type_numb = numb;
                    if   (numb != kfalse)  {
                      us  c = lp[1].acceptORacc [lp[1].ac_lens];
                      prec->numb = _wtoi (& lp[1].acceptORacc [lp[1].ac_lens]);
                      lp[1].acceptORacc [lp[1].ac_lens] = c;
                    }
                    list_insert_tail (lysis->prec, prec);
                  } else  {
                    // nomatch 
                  }
                  if (locate_searchp (& ess, & lq[4], 1, null) == 0)  {
                    if (lp[4].sig_match == 0)
                      achieve LB;
                    else ;
                  }
                  // nomatch 
                } 
              }
            }
          case  2:
          case  3:
          case  4:
          default  :
            ;

          }
        }
        // errors  
    }
    uchar_uninit (& qa);
LLG: uchar_init (& err_);
    /* try open files   */
    open_usfile (abp_bin->str, & lysis->ust);
    assert (lysis->ust != null);

    /* precond processed  */
    //outinfos_usfile (lysis->ust);
    msvc_envdo (lysis->userp);
    sig = precond_set (lysis->userp, lysis->prec, lysis->ust, err_);
    if (sig != 0) achieve cleanup;
    // outinfos_usfile (lysis->ust);
    {
      
      struct udcache *udc = null;
      udcache_load (lysis->userp, abp_bin->str, & udc);
      if (udc != null)  {
        for (s =0; s != udc->count; s++)  {
          struct dep_cache *dep_c; 
          dep_cache_init (& dep_c); 
          uchar_assign0 (dep_c->agpath, udc->ucache_asr[s].fullpath );
          dep_c->timeStamp64 = udc->ucache_asr[s].timestamp64;
          list_insert_tail(lysis->dpcdep, dep_c);
        }
        udcache_uninit (& udc);
      }
      // return 0;
      sig = serach_chunk (& chunkq, lysis->ust, err_);
      if (sig != 0) achieve cleanup;
      sig = ae_replace (chunkq, lysis->userp, lysis->ust, lysis->bpath, err_);
      if (sig != 0) achieve cleanup;
      sig = do_etreeroot ( & lysis->passroot, lysis, chunkq, lysis->ust, err_);
      if (sig != 0) achieve cleanup;
      // outinfos_usfile (lysis->ust);
      do_pass (lysis->passroot, lysis, err_, & sig);
      if (sig != 0) achieve cleanup;
      do_pass_shirk (lysis->passroot);
      if (lysis->passroot->childs <= 0)
        achieve cleanup;
      else ;
      analysis_to_ctl (& ctl, null, lysis->passroot);
      ctl_do (& ctl);


      /* write into cache files  */
      {
        struct list_v *p;
        int id;
        udcache_t udt;
        udcache_open (lysis->userp, abp_bin->str, & udt);
        LIST_FOREACH (lysis->collect, id, p)  {
          uint64_t mod64;
          if (file_lastmod9 (p->uchar_o->str, & mod64) == 0)  {
            udcache_appenditem (udt, p->uchar_o->str, mod64);
          }
        }
                      
        udcache_close (& udt);
      }
      // dtor .. 
cleanup:
      if (sig != 0)
        kprint2 (err_->str);
      else ;
      pass_tree_uninit (lysis->passroot);
      list_uninit_ISQ (& lysis->prec, precond_uninit);
      list_uninit_ISQ (& lysis->userp, env2_uninit);
      list_uninit_ISQ (& lysis->outdep, atdep_uninit);
      list_uninit_ISQ (& lysis->kcc_chain, kcache_chain_uninit);
      list_uninit_ISQ (& lysis->dpcdep, dep_cache_uninit);
      list_uninit_ISQ (& lysis->collect, uchar_uninit);
      list_uninit_ISQ (& lysis->gsym, null);
      list_uninit_ISQ (& chunkq, chunkccp_uninit);

      uchar_uninit (& err_);
      uchar_uninit (& bpath);
      uchar_uninit (& ap_bin);
      uchar_uninit (& abp_bin);
      uchar_uninit (& qa );
      uchar_uninit (& qa2 );
      uchar_uninit (& qa3 );
      uchar_uninit (& lysis->bpath);

      close_usfile (& lysis->ust);

      ufree (lysis);
      uchar_guninit ();
      leak_infos_get ();
      return sig;
    }


  }
       

  ; 
}