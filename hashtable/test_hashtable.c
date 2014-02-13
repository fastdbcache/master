/*
 * =====================================================================================
 *
 *       Filename:  test_hashtable.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/17/2013 02:31:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../config_global.h"
#include "../time_lib.h"

#include "pool_worker.h"
#include "pool_proc.h"
#include "pool_init.h"

#include	<stdlib.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] ) {
    char sql[]="SELECT option_value FROM wp_options WHERE option_name = 'widget_pages' LIMIT 1";
    char sql1[]="SELECT * FROM wp_posts WHERE ID = 2 LIMIT 1";
    char data[]="test data for hashtable";
    int len;
    HDR *_hdr;
    ULIST *_ulist;
    SLABPACK *packs;

    conn_init_global();
    conn_global->cache_method = D_MMAP;
    conn_global->deptype = D_MMAP;
    conn_global->mmap_path = "../cache";
    hcreate(2);

    /* worker thread 
    _hdr = (HDR *)calloc(1, sizeof(HDR));
    _hdr->key = sql;
    _hdr->keyl = strlen(sql);
    _hdr->stime = get_sec(); 
    _hdr->flag = H_TRUE;
    _hdr->dr = data;
    _hdr->drl = strlen(data);
   
    addHdr(_hdr); 

    _ulist = (ULIST *)calloc(1, sizeof(ULIST));
    memcpy(_ulist->key , sql, strlen(sql));
    _ulist->keyl = strlen(sql);
    _ulist->utime = get_sec();
    _ulist->flag = H_TRUE;
    
    addUlist(_ulist);*/

    /* worker thread */
    
    /* proc thread 

    hproc();*/

    /* proc thread */

    packs = (SLABPACK *)calloc(1, sizeof(SLABPACK));
    /* worker thread */
    hkey(sql, strlen(sql), packs);
    
    DEBUG("sql res:%s", packs->pack);

    
    packs->pack = NULL;
    /* worker thread */


    /* worker thread */

    hkey(sql1, strlen(sql1), packs);
    if(packs->pack){
        DEBUG("sql 1 ok: res2:%s", packs->pack);

        free(packs->pack);
    }else{
        DEBUG("error res2 is: NULL\n");
    }

    free(packs);
    /* worker thread */
    return 0;
}				/* ----------  end of function main  ---------- */
 /* vim: set ts=4 sw=4: */

