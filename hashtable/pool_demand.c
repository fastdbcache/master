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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setRowDescription
 *  Description:  
 * =====================================================================================
 */
size_t setRowDescription ( char *parastat, int frontend ){
    char *crd, *newbuf;
    uint32 total, tlen, plen;
    int i, nfields;
    int         tableid;
    int         columnid;
    int         typid;
    int         typlen;
    int         atttypmod;
    int         format;

    nfields = 0;

    total = sizeof(uint32) + sizeof(uint16);
    for(i=0; parastat[i]; i++){
        nfields++;
        total += strlen(parastat[i]) + 1 + (sizeof(uint32) + sizeof(uint16)) * 3;
    }

    crd = calloc(total+sizeof(char), sizeof(char));
    if(!crd){
        DEBUG("error");
        return -1;
    }

    newbuf = crd;
    memcpy(crd, "T", sizeof(char));
    crd+=sizeof(char);
    tlen = htonl(total);
    memcpy(crd, &tlen, sizeof(uint32));
    crd+=sizeof(uint32);

    plen = htons(nfields);    
    memcpy(crd, &plen, sizeof(uint16));
    crd+=sizeof(uint16);

    
    for(i=0; i<nfields; i++){
        
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
    Socket_Send(frontend, newbuf, total+sizeof(char));

    free(newbuf);

    return nfields;
}		/* -----  end of function setRowDescription  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setDataRow
 *  Description:  
 * =====================================================================================
 */
void setDataRow ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function setDataRow  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CommandComplete
 *  Description:  
 * =====================================================================================
 */
void CommandComplete ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function CommandComplete  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ReadyForQuery
 *  Description:  
 * =====================================================================================
 */
void ReadyForQuery ( <+argument_list+> ){
    return <+return_value+>;
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
    char *tmp, *_tmp, *_newtmp, msgbuf[256];
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

