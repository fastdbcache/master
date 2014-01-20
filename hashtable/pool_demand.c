/*
 * =====================================================================================
 *
 *       Filename:  pool_demand.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/30/2013 08:41:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pool_demand.h"


HELP_CMD help_cmd[]={
    {"item",  "found any key in memory"},
    {"stat", "check system status"},
    {"version", "version for fdbc"},
    {NULL,NULL},
};

#define SQL_SELECT

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getItemStat
 *  Description:  
 * =====================================================================================
 */
void getItemStat ( char *key, ssize_t keyl, int frontend ){
    int deslen;
    int nfields;
    char *item_desc, *res;
    int rows;
    char *item[]={"key","utime","ahit","amiss",NULL};

    nfields = 0;
    deslen = RowDesLen( item, &nfields);
    item_desc = calloc(deslen+sizeof(char), sizeof(char));
    if(!item_desc) return;

    res = item_desc;

    setRowDescription( item, res, deslen, nfields );

    Socket_Send(frontend, item_desc, deslen+sizeof(char));
    free(item_desc);
    
    rows = RowItem( key, keyl, frontend, nfields);

    CommandComplete(0, rows , frontend);
    ReadyForQuery(frontend);

}		/* -----  end of function getItemStat  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getVer
 *  Description:  
 * =====================================================================================
 */
void getVer ( int frontend ){
    int deslen;
    int nfields;
    char *item_desc, *res;
    char *item[]={"version",NULL};
    uint32 tlen, nf, _ulen;
    char *crd, *newbuf;
    ssize_t total, kley;   

    nfields = 0;
    deslen = RowDesLen( item, &nfields);
    item_desc = calloc(deslen+sizeof(char), sizeof(char));
    if(!item_desc) return;

    res = item_desc;

    setRowDescription( item, res, deslen, nfields );

    Socket_Send(frontend, item_desc, deslen+sizeof(char));
    free(item_desc);
    
    total = sizeof(uint32) + sizeof(uint16) + sizeof(uint32) + strlen(conn_global->fdbc);
    newbuf = calloc(total+sizeof(char), sizeof(char));

    if(!newbuf) return ;

    crd = newbuf;
    memcpy(crd, "D", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(total);
    memcpy(crd, &tlen, sizeof(uint32));
    crd += sizeof(uint32);
    nf = htons((nfields));
    memcpy(crd, &nf, sizeof(uint16));
    crd += sizeof(uint16);

    kley = strlen(conn_global->fdbc);
    _ulen = htonl(kley);
    memcpy(crd, &_ulen, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, conn_global->fdbc, kley);

    Socket_Send(frontend, newbuf, total+sizeof(char));   
    if(newbuf)
        free(newbuf);

    CommandComplete(0, 1 , frontend);
    ReadyForQuery(frontend);

}		/* -----  end of function getVer  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  RowDesLen
 *  Description:  
 * =====================================================================================
 */
int RowDesLen ( char **parastat, ssize_t *count ){
    ub4 total;
    ssize_t i;

    total = sizeof(uint32) + sizeof(uint16);
    for(i=0; parastat[i]; i++){    
        total += strlen(parastat[i]) + 1 + (sizeof(uint32) + sizeof(uint16)) * 3;
    }
    *count = i;

    return total;
}		/* -----  end of function RowDesLen  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setRowDescription
 *  Description:  
 * =====================================================================================
 */
void setRowDescription ( char **parastat, char *crd, ssize_t count, ssize_t nfields ){
    uint32  tlen, plen;
    int         i;
    int         tableid;
    int         columnid;
    int         typid;
    int         typlen;
    int         atttypmod;
    int         format;

    memcpy(crd, "T", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(count);
    memcpy(crd, &tlen, sizeof(uint32));
    crd+=sizeof(uint32);

    plen = htons(nfields);    
    memcpy(crd, &plen, sizeof(uint16));
    crd+=sizeof(uint16);
    
    for(i=0; parastat[i]; i++){        
        memcpy(crd,parastat[i],strlen(parastat[i]));
        crd += strlen(parastat[i])+1;
        tableid = htonl(0); 
        memcpy(crd, &tableid, sizeof(uint32));
        crd+=sizeof(uint32);
        columnid = htons(i+1);
        memcpy(crd,&columnid , sizeof(uint16));
        crd+=sizeof(uint16);
        typid = htonl(25);
        memcpy(crd,&typid , sizeof(uint32));
        crd+=sizeof(uint32);
        typlen = htons(65535);
        memcpy(crd, &typlen, sizeof(uint16));
        crd+=sizeof(uint16);
        atttypmod = htonl(-1);
        memcpy(crd,&atttypmod , sizeof(uint32));
        crd+=sizeof(uint32);
        format = htons(0);
        memcpy(crd,&format , sizeof(uint16));
        if(i!=(nfields-1))
            crd+=sizeof(uint16);
    } 
}		/* -----  end of function setRowDescription  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gethtabstat
 *  Description:  
 * =====================================================================================
 */
void gethtabstat ( int frontend ){
    int deslen;
    int nfields;
    char *item_desc, *res;
    char *htabstat[]={"count", "bcount","lcount","hit","miss","set","get","bytes M", "max_bytes M",NULL};

    nfields = 0;
    deslen = RowDesLen( htabstat, &nfields);
    item_desc = calloc(deslen+sizeof(char), sizeof(char));
    if(!item_desc) return;

    res = item_desc;

    setRowDescription( htabstat, res, deslen, nfields );

    Socket_Send(frontend, item_desc, deslen+sizeof(char));
    free(item_desc);
    
    RowHtab(frontend, nfields); 
    CommandComplete(0, 1 , frontend);
    ReadyForQuery(frontend);

}		/* -----  end of function gethtabstat  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fdbcHelp
 *  Description:  
 * =====================================================================================
 */
void fdbcHelp ( int frontend ){
    int deslen;
    int nfields;
    char *item_desc, *res;
    char *helps[]={"command","desc",NULL};
    HELP_CMD *_helps;
    int i;

    nfields = 0;
    deslen = RowDesLen( helps, &nfields);
    item_desc = calloc(deslen+sizeof(char), sizeof(char));
    if(!item_desc) return;

    res = item_desc;

    setRowDescription( helps, res, deslen, nfields );

    Socket_Send(frontend, item_desc, deslen+sizeof(char));
    free(item_desc);
    
    i=0;
    for(_helps=help_cmd; _helps->cmd; _helps++, i++){
        RowHelp ( _helps, frontend , nfields );
    }

    CommandComplete(0, i , frontend);
    ReadyForQuery(frontend);
}		/* -----  end of function fdbcHelp  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fdbcSet
 *  Description:  
 * =====================================================================================
 */
void fdbcSet ( int frontend ){
    CommandComplete(1, 1 , frontend);
    ReadyForQuery(frontend);
}		/* -----  end of function fdbcSet  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  RowItem
 *  Description:  
 * =====================================================================================
 */
int RowItem ( char *key, ssize_t keyl, int frontend , ssize_t nfields ){
    uint64_t hval, hjval;
    char *crd, *newbuf, nlen[256];
    HITEM *ph, *_h;
    int  utime_len, ahit_len, amiss_len, y;
    ssize_t total;
    uint32 tlen, nf, _ulen;
    HITEM **pools_hitem;
    ub1 md5[MD5_LENG];
    MD5_CTX *ctx;

    if(!key) return 0;
    if(keyl<1) return 0;
    /*
    bzero(md5, MD5_LENG);

    ctx = calloc(1, sizeof(MD5_CTX));
    MD5_Init(ctx);
    MD5_Update(ctx, key, keyl);
    MD5_Final(md5, ctx);
    free(ctx);
  */
    hval = lookup(key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash(key, keyl);


    HITEM_SWITCH((y=(hval&pools_htab->mask)));
    ph = pools_hitem[y]->next;

    _h = NULL;
    while ( ph ) {
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl > 0)
            ){
            _h = ph;            
        }
        ph = ph->next;
    }
    if(!_h) return 0;
    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->utime);
    utime_len = strlen(nlen);

    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->ahit);
    ahit_len = strlen(nlen);

    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->amiss);
    amiss_len = strlen(nlen);

    total = sizeof(uint32) + sizeof(uint16) + _h->keyl
            + utime_len + ahit_len + amiss_len + sizeof(uint32)*nfields;
    newbuf = calloc(total+sizeof(char), sizeof(char));

    if(!newbuf) return 0;

    crd = newbuf;
    memcpy(crd, "D", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(total);
    memcpy(crd, &tlen, sizeof(uint32));
    crd += sizeof(uint32);
    nf = htons((nfields));
    memcpy(crd, &nf, sizeof(uint16));
    crd += sizeof(uint16);

    _ulen = htonl(_h->keyl);
    memcpy(crd, &_ulen, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, _h->key, _h->keyl);
    crd += _h->keyl;

    _ulen = htonl(utime_len);
    memcpy(crd, &_ulen, sizeof(uint32));
    crd+=sizeof(uint32);
    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->utime);
    memcpy(crd, nlen, utime_len);
    crd += utime_len;

    _ulen = htonl(ahit_len);
    memcpy(crd, &_ulen, sizeof(uint32));
    crd+=sizeof(uint32);
    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->ahit);
    memcpy(crd, nlen, ahit_len);
    crd += ahit_len;

    _ulen = htonl(amiss_len);
    memcpy(crd, &_ulen, sizeof(uint32));
    crd+=sizeof(uint32);
    bzero(nlen, strlen(nlen));
    snprintf(nlen, 255, "%lu", _h->amiss);
    memcpy(crd, nlen, amiss_len);
    
    Socket_Send(frontend, newbuf, total+sizeof(char));
    if(newbuf)
        free(newbuf);

    return 1;
}		/* -----  end of function RowItem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  RowHtab
 *  Description:  
 * =====================================================================================
 */
void RowHtab (int frontend , ssize_t nfields){
    char *crd, *newbuf, nlen[256];
    uint32 tlen, nf, _ulen;
    ssize_t count_len, bcount_len, lcount_len;
    ssize_t hit_len, miss_len, set_len, get_len;
    ssize_t byte_len,maxbyte_len, total;

    #define CALC(res, val, len) do{\
        uint32 _clen;                \
        char carr[256];          \
        _clen = htonl((len));       \
        memcpy(crd, &_clen, sizeof(uint32));    \
        (res)+=sizeof(uint32);                \
        bzero(carr, 256);          \
        snprintf(carr, 255, "%lu", (val));  \
        memcpy((res), carr, (len));   \
        (res) += (len);   \
    }while(0)

    COUNT( pools_htab->count, count_len );
    COUNT( pools_htab->bcount, bcount_len );
    COUNT( pools_htab->lcount, lcount_len );
    COUNT( pools_htab->hit, hit_len );
    COUNT( pools_htab->miss, miss_len );
    COUNT( pools_htab->set, set_len );
    COUNT( pools_htab->get, get_len );
    COUNT( (((pools_htab->bytes)/1024)/1024), byte_len);
    COUNT( (((conn_global->maxbytes)/1024)/1024), maxbyte_len);

    total = sizeof(uint32) + sizeof(uint16) + count_len + bcount_len
            + lcount_len + hit_len + miss_len + set_len + get_len
            + byte_len + maxbyte_len + sizeof(uint32)*nfields;
    newbuf = calloc(total+sizeof(char), sizeof(char));

    if(!newbuf) return ;

    crd = newbuf;
    memcpy(crd, "D", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(total);
    memcpy(crd, &tlen, sizeof(uint32));
    crd += sizeof(uint32);
    nf = htons((nfields));
    memcpy(crd, &nf, sizeof(uint16));
    crd += sizeof(uint16);
     
    CALC(crd, pools_htab->count, count_len);
    CALC(crd, pools_htab->bcount, bcount_len);
    CALC(crd, pools_htab->lcount, lcount_len);
    CALC(crd, pools_htab->hit, hit_len);
    CALC(crd, pools_htab->miss, miss_len);
    CALC(crd, pools_htab->set, set_len);
    CALC(crd, pools_htab->get, get_len);
    CALC(crd, ((pools_htab->bytes/1024)/1024), byte_len);
    CALC(crd, ((conn_global->maxbytes/1024)/1024), maxbyte_len);

    Socket_Send(frontend, newbuf, total+sizeof(char));
    if(newbuf)
        free(newbuf);

}		/* -----  end of function RowHtab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  RowHelp
 *  Description:  
 * =====================================================================================
 */
void RowHelp ( HELP_CMD *_helps, int frontend , ssize_t nfields ){
    char *crd, *newbuf;
    uint32 tlen, nf, _ulen;     
    ssize_t total, cmd_len, desc_len;

    #define CALCH(val, len) do{\
        _ulen = htonl((len));       \
        memcpy(crd, &_ulen, sizeof(uint32));    \
        crd+=sizeof(uint32);                \
        memcpy(crd, (val), (len));   \
        crd += (len);   \
    }while(0)
         
    if(!_helps->cmd) return;
   
    cmd_len = strlen(_helps->cmd);
    desc_len = strlen(_helps->desc);

    total = sizeof(uint32) + sizeof(uint16) + cmd_len + desc_len
             + sizeof(uint32)*nfields;
    newbuf = calloc(total+sizeof(char), sizeof(char));

    if(!newbuf) return ;

    crd = newbuf;
    memcpy(crd, "D", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(total);
    memcpy(crd, &tlen, sizeof(uint32));
    crd += sizeof(uint32);
    nf = htons((nfields));
    memcpy(crd, &nf, sizeof(uint16));
    crd += sizeof(uint16);
     
    CALCH(_helps->cmd, cmd_len);
    CALCH(_helps->desc, desc_len);
   
    Socket_Send(frontend, newbuf, total+sizeof(char));
    if(newbuf)
        free(newbuf);

}		/* -----  end of function RowHelp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CommandComplete
 *  Description:  
 * =====================================================================================
 */
void CommandComplete (int sqlcmd, ssize_t rows, int frontend ){
    char res[32];
    char *SQLS[]={"SELECT", "UPDATE FDBC","INSERT FDBC 0", "DELETE FDBC"};
    char *newbuf, *crd;
    ssize_t total, len;

    snprintf(res, 31, "%s %d", SQLS[sqlcmd], rows);
    total = sizeof(uint32) + strlen(res) + 1;
    crd = calloc(total+sizeof(char), sizeof(char));
    newbuf = crd;
    memcpy(crd, "C", sizeof(char));
    crd+=sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, res, strlen(res)+1);

    Socket_Send(frontend, newbuf, len+sizeof(char));
    free(newbuf);
}		/* -----  end of function CommandComplete  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ReadyForQuery
 *  Description:  
 * =====================================================================================
 */
void ReadyForQuery ( int frontend ){
    char *newbuf, *crd;
    ssize_t total, len;
    total = sizeof(uint32)+sizeof(char);
    crd = calloc(sizeof(char)+total, sizeof(char));
    newbuf = crd;
    memcpy(crd, "Z", sizeof(char));
    crd += sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, "I", sizeof(char)); 
    Socket_Send(frontend, newbuf, len+sizeof(char));
    free(newbuf);
}		/* -----  end of function ReadyForQuery  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setCacheRowDescriptions
 *  Description:  
 * =====================================================================================
 */
void setCacheRowDescriptions ( int frontend ){
    char *crd, *field, *newbuf;
    uint32 total, len;
    int i, nfields;
    int         tableid;
    int         columnid;
    int         typid;
    int         typlen;
    int         atttypmod;
    int         format;
    char *rowdes[]={"key","keyl","drl","psize","sid","sa","hval","hjval","utime","ahit","amiss"};
    char res[32]={'\0'};
    total = 0;
    nfields = 11;

    total += sizeof(uint32)+sizeof(uint16);

    field = NULL;
    for(i=0; i<nfields; i++){
        total += strlen(rowdes[i])+1+sizeof(uint32)+sizeof(uint16)+sizeof(uint32)+sizeof(uint16)+sizeof(uint32)+sizeof(uint16);
    }

    crd = calloc(total+sizeof(char), sizeof(char));
    newbuf = crd;
    memcpy(crd, "T", sizeof(char));
    crd+=sizeof(char);
    len = total; 
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);

    nfields = htons(nfields);    
    memcpy(crd, &nfields, sizeof(uint16));
    crd+=sizeof(uint16);

    
    for(i=0; i<11; i++){
        
        memcpy(crd,rowdes[i],strlen(rowdes[i]));
        crd += strlen(rowdes[i])+1;
        tableid = htonl(0); 
        memcpy(crd, &tableid, sizeof(uint32));
        crd+=sizeof(uint32);
        columnid = htons(i+1);
        memcpy(crd,&columnid , sizeof(uint16));
        crd+=sizeof(uint16);
        typid = htonl(25);
        memcpy(crd,&typid , sizeof(uint32));
        crd+=sizeof(uint32);
        typlen = htons(65535);
        memcpy(crd, &typlen, sizeof(uint16));
        crd+=sizeof(uint16);
        atttypmod = htonl(-1);
        memcpy(crd,&atttypmod , sizeof(uint32));
        crd+=sizeof(uint32);
        format = htons(0);
        memcpy(crd,&format , sizeof(uint16));
        if(i!=(nfields-1))
            crd+=sizeof(uint16);
    }
    Socket_Send(frontend, newbuf, len+sizeof(char));

    free(newbuf);
    
    HITEM *_h;
    int  m, nf, tlen, _tlen, tl;
    uint32 _ulen;
    HITEM **pools_hitem;
    char *tmp,  *_newtmp, msgbuf[256];
    _tlen = 0, tl=0; 
    tmp = NULL, _newtmp=NULL;
    m=0;
    for(i=0; i<pools_htab->logsize; i++){
        HITEM_SWITCH(i);
        _h = pools_hitem[i];
        if(_h->next) _h = _h->next;
        else continue;
        
        for(; _h; _h=_h->next){
            if(_tlen==0){
                _tlen = _h->keyl+sizeof(_h->keyl)+sizeof(_h->drl)+sizeof(_h->psize)
                    + sizeof(_h->sid)+sizeof(_h->sa)+sizeof(_h->hval)+sizeof(_h->hjval)
                    +sizeof(_h->utime)+sizeof(_h->ahit)+sizeof(_h->amiss)+(11*sizeof(uint32));
                tlen = _tlen;
            }
            tl = 0;        
            tmp = (char *)calloc(_tlen, sizeof(char));
            _newtmp = tmp;
            m++; 
            DEBUG("i:%d", i);
            _ulen = htonl(_h->keyl);
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl += sizeof(uint32);
            memcpy(tmp, _h->key, _h->keyl);
            tmp += _h->keyl;
            tl += _h->keyl;

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->keyl);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->drl);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->psize);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf); 
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->sid);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->sa);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->hval);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->hjval);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->utime);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->ahit);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);

            bzero(msgbuf, 256);
            snprintf(msgbuf, 255, "%lu", _h->amiss);
            _ulen = htonl(strlen(msgbuf));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            tl+=sizeof(uint32);
            memcpy(tmp, msgbuf, strlen(msgbuf));
            tmp += strlen(msgbuf);
            tl+=strlen(msgbuf);
            
            total = sizeof(uint32)+sizeof(uint16) + tl;
            crd = calloc(total+sizeof(char), sizeof(char));
            newbuf = crd;
            memcpy(crd, "D", sizeof(char));
            crd+=sizeof(char);
            len = total;
            total = htonl(total);
            memcpy(crd, &total, sizeof(uint32));
            crd += sizeof(uint32);
            DEBUG("tl %d tlen:%d", tl, m);
            nf = htons((11));
            memcpy(crd, &nf, sizeof(uint16));
            crd += sizeof(uint16);
            memcpy(crd, _newtmp, tl);

            char *ds, data[1024];
            uint16 num;
            uint32 nlen;
            num = ntohs(nf);
            ds = _newtmp;
            for(;num>0;num--){
                memcpy(&nlen, ds, sizeof(uint32));
                nlen = ntohl(nlen);
                ds+=sizeof(uint32);
                bzero(data, 1024);
                memcpy(data, ds, nlen);
                DEBUG("len:%d, byte:%s",nlen, data );
                ds+=nlen;
            } 

            Socket_Send(frontend, newbuf, len+sizeof(char));
            free(newbuf);
            newbuf = NULL;
            free(_newtmp);
            _newtmp = NULL;
        }
    }
    
    snprintf(res, 31, "SELECT %d", m);
DEBUG("C---Z res: %s", res);
    total = sizeof(uint32)+strlen(res)+1;
    crd = calloc(total+sizeof(char), sizeof(char));
    newbuf = crd;
    memcpy(crd, "C", sizeof(char));
    crd+=sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, res, strlen(res)+1);
    Socket_Send(frontend, newbuf, len+sizeof(char));
    free(newbuf);

    
    total = sizeof(uint32)+sizeof(char);
    crd = calloc(sizeof(char)+total, sizeof(char));
    newbuf = crd;
    memcpy(crd, "Z", sizeof(char));
    crd += sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, "I", sizeof(char)); 
    Socket_Send(frontend, newbuf, len+sizeof(char));
}	/*	 -----  end of function setCacheRowDescriptions  ----- */



 /* vim: set ts=4 sw=4: */

