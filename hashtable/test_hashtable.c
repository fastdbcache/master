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
    hcreate(8);

    /* worker thread */
    _hdr = (HDR *)calloc(1, sizeof(HDR));
    _hdr->key = sql;
    _hdr->keyl = strlen(sql);
    _hdr->stime = get_sec(); 
    _hdr->flag = H_TRUE;
    _hdr->dr = data;
    _hdr->drl = strlen(data);
   
    addHdr(_hdr, 0); 

    _ulist = (ULIST *)calloc(1, sizeof(ULIST));
    _ulist->key = sql;
    _ulist->keyl = strlen(sql);
    _ulist->utime = get_sec();
    _ulist->flag = H_TRUE;
    
    addUlist(_ulist, 0);

    /* worker thread */
    
    /* proc thread */

    hproc();

    /* proc thread */

    packs = (SLABPACK *)calloc(1, sizeof(SLABPACK));
    /* worker thread */
    hkey(sql, strlen(sql), packs);
    
    printf("res:%s\n", packs->pack);

    
    free(packs->pack);
    packs->pack = NULL;
    /* worker thread */


    /* worker thread */

    hkey(sql1, strlen(sql1), packs);
    if(packs->pack){
        printf("res2:%s\n", packs->pack);

        free(packs->pack);
    }else{
        printf("res2 is: NULL\n");
    }

    free(packs);
    /* worker thread */
    return 0;
}				/* ----------  end of function main  ---------- */
 /* vim: set ts=4 sw=4: */

