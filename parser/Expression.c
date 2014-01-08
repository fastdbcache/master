/*
 * =====================================================================================
 *
 *       Filename:  Expression.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/2014 06:23:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "Expression.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parser_do
 *  Description:  
 * =====================================================================================
 */
_ly *parser_do (char *str, int len){
    char *p = str;
    _ly *lys, *tail;
    tail = _init_ly();
    yyscan_t scanner;

    if (yylex_init(&scanner)) {
        // couldn't initialize
        return NULL;
    }
    YY_BUFFER_STATE state = yy_scan_bytes(p, len);
    /*YY_BUFFER_STATE state = yy_scan_string(p, scanner);*/
      yy_switch_to_buffer(state);
    if(yyparse(&tail, scanner)){
        return NULL;
    }
    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);

    if(tail != NULL && tail->tab != NULL){
        return tail;
    }else{
        return NULL;
    }
}		/* -----  end of function parser_do  ----- */

_ly *_init_ly(){
    _ly *_l;
    _l = calloc(1, sizeof(_ly));
    _l->tab = NULL;
    _l->next = NULL;
    _l->len = 0;
    return _l;
}
/*
void _len(int l){
    if(ly != NULL){
       ly->len = l;
    }
}
*/
void _save(_ly *sly, char *s, int len){
    _ly *_l;

    if( s == NULL || len < 0) return;

    if(sly->tab == NULL){
        sly->tab = calloc(1, len*sizeof(char));
        memcpy(sly->tab, s, len);
        sly->len = len;
    }
    /* because only parser change sql
    else{
        _l = _init_ly(); 
        _l->tab = calloc(1, len*sizeof(char));
        memcpy(_l->tab, s, len);
        _l->len = len;
        tail->next = _l;
        tail = _l;
    }*/
}

_ly *_get(){
    if(ly != NULL && ly->tab != NULL){
        return ly;
    }else{
        return NULL;
    }
}
 /* vim: set ts=4 sw=4: */

