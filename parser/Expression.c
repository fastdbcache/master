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

int yyparse(_ly **myly, yyscan_t scanner);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parser_do
 *  Description:  
 * =====================================================================================
 */
_ly *parser_do (char *str, int len){
    char *p = str;
    _ly *lys, *tail;
    yyscan_t scanner; 
    YY_BUFFER_STATE state;

    tail = _init_ly();
    if (yylex_init(&scanner)) {
        // couldn't initialize
        DEBUG("ly 0");
        return NULL;
    }

    state = yy_scan_bytes(p, len, scanner);

    yy_switch_to_buffer(state, scanner);
    yyparse(&tail, scanner);
    /*     if(yyparse(&tail, scanner)){
        DEBUG("1 tab:%s", tail->tab);
        return NULL;
    } */

    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);

    if(tail != NULL && tail->tab != NULL){
        return tail;
    }else{
        free(tail);
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

void _lysave(_ly *myly, _ly *_src){
    _ly  *_des;
   
    if(!_src)return; 
    _des = _src;

    if( !_des->tab) {
    
        return ;
    }

    if(myly->tab == NULL){
        
        myly->tab = calloc(1, _des->len*sizeof(char));
        memcpy(myly->tab, _des->tab, _des->len);
        myly->len = _des->len;
    }

    /*   because only parser change sql
    else{
        _l = _init_ly(); 
        _l->tab = calloc(1, len*sizeof(char));
        memcpy(_l->tab, s, len);
        _l->len = len;
        myly->next = _l;
        myly = _l;
    }*/
}
/* 
void
main(int ac, char **av)
{
    extern int yydebug ;
    yydebug = 1;
    _ly *tly;
    char sql[]="insert into a(c)values(111);";

    tly = NULL;
    if(ac < 1){
        return;
    }
   
      
	//if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
	//	perror(av[1]);
	//	exit(1);
	//}
    printf("sql: %s, len:%d\n", sql, strlen(sql));
    tly = parser_do(sql, strlen(sql));
    for(;tly; tly=tly->next)
        printf("tab: %s\n", tly->tab);
} main */


 /* vim: set ts=4 sw=4: */

