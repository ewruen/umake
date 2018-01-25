void isLeafnode (struct pass_node *pn, us *passanme, int len, kbool *interrrupt, kbool *isleaf, struct pass_node **attc)  {

  if (pn == null)
    return ;
  if (* interrrupt)
    return ;
  if (pn->child == null)  {
    /* self, and leaf node */
    if (uchar_cmp3 (pn->passname, passanme, len) == 0)  {
      * attc = pn;
      * interrrupt = ktrue;
      * isleaf = ktrue;
    }
  } else  {
    /* foreach it */
    struct pass_node *p = pn->child;
    int id;
    for (id = 0; id != pn->childs; p = p->level_n, id++)  {
      isLeafnode (p, passanme, len, interrrupt, isleaf, attc);
      if (* interrrupt)
        return ;
    }
  }
}

void isExistnode (struct pass_node *pn, us *passanme, int len, kbool *interrrupt, kbool *isexist)  {

  if (pn == null)
    return ;
  if (* interrrupt)
    return ;
  {
    /* foreach it */
    struct pass_node *p = pn->child;
    int id;
    if (uchar_cmp3maxs (pn->passname, passanme, len) == 0)  {
      * interrrupt = ktrue;
      * isexist = ktrue;
      return ;
    }
    for (id = 0; id != pn->childs; p = p->level_n, id++)  {
      isExistnode (p, passanme, len, interrrupt, isexist);
      if (* interrrupt)
        return ;
    }
  }
}

void pass_node_init (struct pass_node **pnm)  {

  struct pass_node *p = umalloc (sizeof (struct pass_node));
  p->child = null;
  p->childs = 0;
  p->level_n = p;
  p->level_p = p;
  p->parent = null;
  uchar_init (& p->passname);
  list_init (& p->shell_chain);
  p->sstart = 0;
  p->sslen = 0;
  pass_varChain_init (& p->varChain);
  * pnm  = p;
}

void pass_node_uninit (struct pass_node **pnm) {
  if (pnm != null)  {
    struct pass_node *pn = *pnm;
    *pnm = null;
    if (pn != null)  {
      uchar_uninit (& pn->passname);
      list_uninit_ISQ (& pn->shell_chain, shell_desc_uninit);
      pass_varChain_uninit (& pn->varChain);
      ufree (pn);
    }
  }
}

void pass_tree_uninit (struct pass_node *pp_) {
  int id;
  int iq;
  struct pass_node *pn = null;
  struct pass_node *pg;

  if (pp_ == null)  {
    return  ;
  }

  pg = pp_->child;
  iq = pp_->childs;

  pass_node_uninit (& pp_);
  /* foreach child node */
  for (id = 0; id != iq; id++, pg = pn)  {
    pn = pg->level_n  ;
    pass_tree_uninit (pg);
  }
}