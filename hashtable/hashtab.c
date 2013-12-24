/*
 * =====================================================================================
 *
 *       Filename:  hashtab.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2013 06:00:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef HASHTAB
#include "hashtab.h"
#endif

/*
 * hgrow - Double the size of a hash table.
 * Allocate a new, 2x bigger array,
 * move everything from the old array to the new array,
 * then free the old array.
 */
void hgrow()
{
    ub4     newsize = (ub4)1<<(++pools_htab->logsize);
    ub4     newmask = newsize-1;
    ub4     i;
    HITEM   **old_hitem, **new_hitem;

    DEBUG("hgrow %d", pools_htab->logsize);
    free(pools_hitem_row);
    pools_hitem_row = calloc(newsize, sizeof(ub4)) ;

    hitem_group->move = inithitem( newsize );
    
    old_hitem = hitem_group->usable;
    new_hitem = hitem_group->move;
   
    for(i=pools_htab->logsize; i--;){
        HITEM *this, *that, *new;

        if(!old_hitem[i]) continue;
        this = old_hitem[i]->next;
        
        while( this ){
            that = this;
            this = that->next;

            if(that->drl == 0) continue;  /* clear the empty drl  */

            new = new_hitem[(that->hval & newmask)];                
            that->next = new->next;
            new->next = that;
            pools_htab->count++;
        }
        hitem_group->bucket--;
    }

    freehitem(hitem_group->usable, pools_htab->logsize);
    
    hitem_group->usable = hitem_group->move;
    hitem_group->move = NULL;
    hitem_group->bucket = newsize;
    
    pools_htab->logsize = newsize; 
    pools_htab->mask = newmask;
}



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findhslab
 *  Description:  
 * =====================================================================================
 */
HSLAB *findhslab ( ssize_t i, sb2 _sid){
    HSLAB *slab, *p;

    slab = pools_hslab[i];

    for(p=slab; p; p=p->next){
        if(p->id == _sid){             
            return p;
        }
    }

    return NULL;
}		/* -----  end of function findhslab  ----- */





/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hsms
 *  Description:  
 * =====================================================================================
 */
int hsms ( ub4 bytes ){
    int i;
    /*float l;*/

    if(bytes > MAX_SLAB_BYTE){
        printf("big bytes\n");
        return -1;
    }

    i = 0;
    /*  l = (bytes / SLAB_BEGIN);
    while( l >= conn_global->factor ){
        l = l / conn_global->factor;
        i++;
    }
    if( i>0 ) --i;
    */
    for ( ; slabclass[i].chunk > 0; i++ ) {
        if(slabclass[i].size >= bytes) return i;
    }
    printf("not found size\n");
    return -1;
}		/* -----  end of function hsms  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addfslab
 *  Description:  
 * =====================================================================================
 */
void addfslab ( HITEM *_ph){
    FSLAB  *f, *fslab;
        
   /* pthread_mutex_lock(&work_lock_fslab);  */
    f = pools_fslab;

    while ( f && f->next ) {
        f = f->next;
    }

    fslab = (FSLAB *)calloc(1, sizeof(FSLAB));
    fslab->psize = _ph->psize;
    fslab->sid = _ph->sid;
    fslab->sa = _ph->sa;
    fslab->next = NULL;
    f->next = fslab;

    _ph->drl = 0;
   /*  pthread_mutex_unlock(&work_lock_fslab);  */
    return;
}		/* -----  end of static function addfslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findfslab
 *  Description:  
 * =====================================================================================
 */
FSLAB *findfslab ( sb2 _psize ){
    FSLAB *fslab, *res;

    fslab = pools_fslab;

    do {
        res = fslab;
        fslab = fslab->next;
        if(fslab == NULL) return NULL;
        if(fslab->psize == _psize){
            res->next = fslab->next;
            return fslab;
        }
    } while ( 1 );				/* -----  end do-while  ----- */
    
    return NULL;
}		/* -----  end of function findfslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findslab
 *  Description:  
 * =====================================================================================
 */
FSLAB *findslab ( sb2 _psize ,int i){
    FSLAB *fslab, *fs_tmp;
    HSLAB *hslab, *hs_tmp;

    fslab = findfslab(_psize);
    
    if(fslab != NULL){
        /* 1. find a slab from freeslab */
        return fslab;
    }else{
        /* 2. find a slab from pools_hslab */
        hslab = pools_hslab[i];
        if(!hslab){
            printf("hslab error\n");
            return NULL;
        } 
        loop:
        
            if(hslab->sm == NULL){
                hslab->sm = (ub1 *)calloc(MAX_SLAB_BYTE, sizeof(ub1));
                if(!hslab->sm) return NULL;
                hslab->ss = 0;
                hslab->sf = slabclass[i].chunk;
                
            }

            if(hslab->sf > 0){
                fs_tmp = (FSLAB *)calloc(1, sizeof(FSLAB));
                if(!fs_tmp) return NULL;
                /*memcpy(hslab->sm + hslab->ss*_psize, hdr->dr, hdr->drl);*/
                fs_tmp->sid = hslab->id;
                fs_tmp->sa =  hslab->ss;
                fs_tmp->psize = _psize;
                hslab->ss++;
                hslab->sf--;

                return fs_tmp;
            }else{
                if(pools_htab->bytes <= conn_global->maxbytes){
                    hs_tmp = hslabcreate(i);
                    if(!hs_tmp) return NULL;
                    hs_tmp->id = hslab->id + 1;
                    hslab->next = hs_tmp;
                    hslab = hslab->next;

                    goto loop;
                }else {
                    perror("bytes is eq conn_global->bytes\n");
                    return NULL;
                }
            }
    }
    return NULL;
}		/* -----  end of function findslab  ----- */


 /* vim: set ts=4 sw=4: */

