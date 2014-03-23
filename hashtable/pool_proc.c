/*
 * =====================================================================================
 *
 *       Filename:  pool_proc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 03:25:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include "pool_proc.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hproc
 *  Description:  
 * =====================================================================================
 */
void hproc ( ){

    /* update table tlist 
    htlist();*/

    /* hdr to hitem  */
    fetchdti(); 
   
}		/* -----  end of function hproc  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fetchdti
 *  Description:  fetch pools_hdr to pools_hitem , use haru 
 * =====================================================================================
 */
void fetchdti (  ){
    HDR *pd, *_pd;

    pd = pools_hdr_tail;
    
    while(pd){

        if(pd->flag == H_FALSE){ 
            goto clear;
        }
        if(!pd->key){
            goto clear;
        }
        
        if( pools_htab->bytes < conn_global->maxbytes ){        
            haddHitem(pd);
        }

        clear:
            if(pd == pools_hdr_head){
                pd->flag = H_FALSE;
                pools_hdr_tail = pd;
                break;
            }else{
                _pd = pd;
                pd = pd->next;
                freeHdr(_pd);                
            }
    }   
}		/* -----  end of function fetchdti  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  haddHitem
 *  Description:  hdr to hitem
 * =====================================================================================
 */
word haddHitem ( HDR *mhdr ){ 
    HITEM  *ph;
    HDR *hdr;
    ub2    x, y ;
    uint64_t _new_hval, _new_hjval;
    int i, m, n ;
    HG *pool_hg;
    HROW *_hrow;

    hdr = mhdr;
    if(hdr == NULL) return -1;
    if(hdr->drl > (LIMIT_SLAB_BYTE-sizeof(uint32)*2)) return -1;
    if(hdr->keyl > (KEY_LENGTH-1)){
       FLOG_NOTICE("keyl > KEY_LENGTH key:%s", hdr->key);
       return -1;
    }
    _new_hval = lookup(hdr->key, hdr->keyl, 0);
    _new_hjval = jenkins_one_at_a_time_hash(hdr->key, hdr->keyl);

    n = 0;
    i = hsms((hdr->drl + sizeof(uint32)*2));
    if(i == -1) return -1;

    do{
        pool_hg = hitem_group[n];
        if(pool_hg == NULL){
            FLOG_WARN("pool_hg is null n:%d", n);
            break;
        }
        x=(_new_hval&pool_hg->mask);
        _hrow = pool_hg->hrow + x;
        y = _new_hjval&(MAX_HITEM_LENGTH-1);
        ph = _hrow->hitem + y;

        if(!ph) {
            FLOG_NOTICE("ph is null x:%d, y:%d", x, y);
            break;
        }
        if(ph->drl == 0){
            if(saveHitem ( ph, hdr, i ) == -1){
                return -1;
            }
            ph->hval = _new_hval;
            ph->hjval = _new_hjval;
            ph->ahit = 0;
            ph->amiss = 1;
            pools_htab->count++; 
            pool_hg->count++;
            hrule ( ph, i,  x,  y, n );
            return 0;     
        }
        else if(_new_hval == ph->hval &&
            (hdr->keyl == ph->keyl) &&
            (_new_hjval == ph->hjval) 
            ){
            m = hsms(ph->psize);
            if(m == -1) return -1; 
            if(m != i){  /* old size != new size , free old slab */
                addfslab(ph);  /* free old slab */                                
            }
            if(saveHitem ( ph, hdr, i ) == -1){
                return -1;
            }
            
            /* 
            if((ph->amiss / ph->ahit) > LIMIT_PERCENT){
                ph->drl = 0;
                break;
            }*/
            ph->amiss++;
            hrule ( ph, i,  x,  y, n );
            return 0;
        }else{            
            n++;
            
            if(n == pools_htab->gcount){
                hgrow(n);
            }
        }
        
    }while(n<MAX_HG_LENGTH);

    return -1;
}		/* -----  end of function haddHitem  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  saveHitem
 *  Description:  
 * =====================================================================================
 */
int saveHitem ( HITEM *_ph, HDR *_hdr, int i ){
    HITEM *ph = _ph;
    HDR *hdr = _hdr;
    ssize_t total_size;
    uint32 pre_sid, pre_sa;
    sb2 slab_id;
    ub1 *slab_sm;

    if(_hdr->drl<1)return -1;

    total_size = slabclass[i].size;
    if(pools_fslab[i].sa != -1 &&
       pools_fslab[i].psize != 0 ){
        ph->sid = pools_fslab[i].sid;
        ph->sa = pools_fslab[i].sa;
        freefslab(i); 
    }else{
        slab_id = findslab(total_size);
        if(slab_id == -1){
            droprule(i, ph);             
        }else {        
            ph->sid = slab_id;
            ph->sa = pools_hslab[slab_id].ss - total_size;
        }
    }
    if(ph->sa == -1) return -1;
    if((ph->sa+total_size) > conn_global->default_bytes){
        FLOG_NOTICE("sa add total_size error");
        return -1;
    }
    if(pools_hslab[ph->sid].sm == NULL) return -1;

    slab_sm = pools_hslab[ph->sid].sm + ph->sa;
    if(!slab_sm) return -1;
    
    memcpy(ph->key, hdr->key, hdr->keyl);
    ph->keyl  = hdr->keyl;
    ph->drl = hdr->drl;
    ph->psize = slabclass[i].size;    
    ph->utime = hdr->stime;
    pre_sa = htonl(-1); 
    memcpy(slab_sm, &pre_sa, sizeof(uint32));
    pre_sid = htonl(0); 
    memcpy(slab_sm+sizeof(uint32), &pre_sid, sizeof(uint32));

    memcpy(slab_sm+sizeof(uint32)*2, hdr->dr, hdr->drl);

    pools_htab->miss++;

    return 0;
}		/* -----  end of function saveHitem  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hrule
 *  Description:  
 * =====================================================================================
 */
void hrule ( HITEM *hitem, int isize, int x, int y, int id ){
    int  _size;
    HITEM *ph;
    HARU *_haru;
    HG *pool_hg;
    HROW *_hrow;

    if(!hitem) return;
    if(isize<0)return ;
    if(id >= MAX_HG_LENGTH) return ;

    _size = isize;
    test_rule:
        if(_size >= MAX_HARU_POOL) return ; 
        _haru = pools_haru_pool+_size;
        
        if(_haru->hid == 0 &&
           _haru->x == 0 &&
           _haru->y == 0 ){
            _haru->x = x;
            _haru->y = y;
            _haru->hid = id;
            return ;
        }
        if(_haru->x == x &&
            _haru->y == y &&
            _haru->hid == id) return ;

        pool_hg = hitem_group[_haru->hid];
        if(pool_hg == NULL){
            FLOG_WARN("pool_hg is null n:%d", _haru->hid);
            return;
        }
        _hrow = pool_hg->hrow + _haru->x;
        ph = _hrow->hitem + _haru->y;

        if(ph->ahit > hitem->ahit){
            _haru->x = x;
            _haru->y = y;
            _haru->hid = id;
        }else{
            _size += MAX_SLAB_CLASS;
            goto test_rule;
        }  
}		/* -----  end of function hrule  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  droprule
 *  Description:  
 * =====================================================================================
 */
void droprule ( int isize, HITEM *_ph ){
    HITEM *ph;
    HARU *_haru;
    HG *pool_hg;
    HROW *_hrow;
        
    _haru = pools_haru_pool+isize;
    if(_haru->hid==0){
        _ph->sa = -1;
        _ph->drl = 0;
        return ;
    }

    pool_hg = hitem_group[_haru->hid];
    _hrow = pool_hg->hrow + _haru->x;
    ph = _hrow->hitem + _haru->y;
    if(ph->drl == 0){
        _ph->sa = -1;
        _ph->drl = 0;
        return;
    }

    ph->drl = 0;
    ph->sa = -1;
    ph->hval = 0;
    ph->drops++;
    _ph->sa = ph->sa;
    _ph->sid = ph->sid;
    _haru->hid = 0;
    _haru->x = 0;
    _haru->y = 0;

}		/* -----  end of function droprule  ----- */

 /* vim: set ts=4 sw=4: */

