// <chunkccp>==========================================================================================
void 
chunkccp_init (struct chunkccp **cc)  {
  struct chunkccp * p = umalloc (sizeof (struct chunkccp));
  p->ect_type = CHUNK_TYPE_NOFIND;
  p->sstart = 0;
  p->len = 0;
  p->passname = null;
  * cc = p;
}

void chunkccp_uninit (struct chunkccp **cc)  {
  if (cc != null)  {
    struct chunkccp *ccp = *cc;
    * cc =null;
    if (ccp != null)  {
      uchar_uninit (& ccp->passname);
      ufree (ccp);
    }
  }
}

int serach_chunk (struct list_ **chunk_chain, struct ustream *ust, struct uchar *err)  {
  /* .alter/.pass/.etreeroot/.envir */
  int id, id2;
  us *cc[] =  { _GR ("alter"),
              _GR ("pass"), 
              _GR ("etreeroot"), _GR ("envir") };
  struct locate_desc_ ld[2];
  struct list_ *chunkcc = null;
  struct chunkccp *etreeroot = null; /* must exist, only one */
  struct list_v *p, *s;

  LOCATE_SETACC_RELAXED_CONST(& ld[0], _GR ("."));
  ld[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
  ld[1].dmtach_blk = cc;
  ld[1].sig_setnums = sizeof (cc)/ sizeof (cc[0]);

  list_init (& chunkcc);
  for (id = 0; id != ust->total; id++)  {
    struct raw_line *rl = ust->line_gp[id];
    struct locate_essen_ es;
    if (rl->invalid != kfalse)
      continue ;
    locate_attach_weak (& es, rl->content, null, id+ 1);
    if ( locate_clearrc (& es) == 0) {
      rl->invalid = ktrue;
      continue ;
    }
    if ( locate_searchp (& es, &ld[0], 2, null) == 0) {
      struct chunkccp *ccp = null;
      struct locate_desc_ ld2[2];
      chunkccp_init (& ccp);
      ccp->sstart = id + 1 + 1;
      switch (ld[1].sig_match)  {
      case 0: 
        if (locate_uselessdata(& es) != 0)  {
          uchar_assign3 (err, _GR ("analysis-error:[%d] !impurities are behind the .alter \r\n"), id + 1);
          achieve L0;
        }
        rl->invalid = ktrue;
        ccp->ect_type = CHUNK_TYPE_ALTER;
        break ;
      case 3: 
        if (locate_uselessdata(& es) != 0)  {
          uchar_assign3 (err, _GR ("analysis-error:[%d] !impurities are behind the .envir \r\n"), id + 1);
          achieve L0;
        }
        rl->invalid = ktrue;
        ccp->ect_type = CHUNK_TYPE_ENVIR;
        break ;
      case 1:      
        ld2[1].etype = LOCATE_TYPE_VARIABLE;
        ccp->ect_type = CHUNK_TYPE_PASS;
        rl->invalid = ktrue;
        LOCATE_SETACC_RELAXED_CONST (& ld2[0], _GR (":="));
        if ( locate_searchp (& es, &ld2[0], 2, null) == 0) {
          /* check passname redefine */
          LIST_FOREACH (chunkcc, id2, p)  {
            if (p->ccp_o->ect_type == CHUNK_TYPE_PASS)  {
              if ( (uchar_cmp3maxs 
                     (  p->ccp_o->passname, 
                       & es.buf->str[ld2[1].ac_pos], 
                       ld2[1].ac_lens) == 0))
              {
                uchar_assign3  (  err, 
                                  _GR ("analysis-error:[%d:%d?%ls] !same passname \r\n"), 
                                  p->ccp_o->sstart -1, 
                                  id2 + 1, 
                                  p->ccp_o->passname->str);
                achieve L0;
              }
            }
          }
          if (locate_uselessdata(& es) != 0)  {
            uchar_assign3 (err, _GR ("analysis-error:[%d] !impurities are behind the .pass \r\n"), id + 1);
            achieve L0;
          }
          uchar_init (& ccp->passname);
          uchar_assign(ccp->passname,& es.buf->str[ld2[1].ac_pos], ld2[1].ac_lens );
        } else {
          uchar_assign3 (err, _GR ("analysis-error:[%d] !pass definition error \r\n"), id + 1);
       L0:chunkccp_uninit (& ccp);
          achieve cleanup;
        }
        break ;
      case 2:
        ccp->ect_type = CHUNK_TYPE_ETREEROOT;
        if (etreeroot != null)  {
          uchar_assign3 (err, _GR ("analysis-error:[%d:%d] !extra .etreeroot \r\n"), id + 1, etreeroot->sstart -1);
          achieve L0;
        }
        etreeroot = ccp;
        break ;
      default: 
        achieve cleanup ;
      }
      list_insert (chunkcc, -1, ccp);
      continue ;
    }
  }
  if (etreeroot == null)  {
    uchar_assign3 (err, _GR ("analysis-error: !no .etreeroot \r\n"));
    achieve cleanup;
  }
  /*calc tail pos */
  LIST_FOREACH (chunkcc, id, p)  {
    if (p->ccp_o->ect_type == CHUNK_TYPE_ETREEROOT)
      s = p;
    if (id != chunkcc->nums -1)  {
      p->ccp_o->len = p->level_n->ccp_o->sstart - p->ccp_o->sstart - 1;
    } else  {
      /* final pos t:8 p:7*/
      p->ccp_o->len = ust->total + 1 + 1 - p->ccp_o->sstart - 1;
    }
  }
  /* CHUNK_TYPE_ETREEROOT push head */
  list_pop_L (chunkcc, s);
  list_insert_L (chunkcc, 0, s);
  * chunk_chain = chunkcc;
  return 0;
cleanup:
  list_uninit_ISQ (& chunkcc, chunkccp_uninit);
  return -1;
}

