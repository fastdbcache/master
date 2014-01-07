/*
 * =====================================================================================
 *
 *       Filename:  dep_misc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月05日 02时02分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "dep_struct.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  deposit_init
 *  Description:  
 * =====================================================================================
 */
DEPO *deposit_init (  ){
    DEPO *_depo;
    _depo = calloc(1, sizeof(DEPO));
    _depo->sm = calloc(LIMIT_SLAB_BYTE, sizeof(char));
    _depo->ss = 0;
    _depo->sp = 0;
    _depo->se = 0;
    return _depo;
}		/* -----  end of function deposit_init  ----- */

 /* vim: set ts=4 sw=4: */


