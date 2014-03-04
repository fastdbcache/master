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
    _ly  *tail;
    yyscan_t scanner; 
    YY_BUFFER_STATE state;

    tail = _init_ly();
    if (yylex_init(&scanner)) {
        // couldn't initialize
        FLOG_WARN("ly 0");
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
    _l = malloc(sizeof(_ly));
    if(!_l) return NULL;

    _l->tab = NULL;
    _l->next = NULL;
    _l->len = 0;
    return _l;
}

void _lysave(_ly *myly, char *_src){
    char *des = _src;
    int len;

    if(!_src)return; 
    len = 0;
    
    while(des && *(des) != ' ' && *(des) != '(' && *(des) != '\t' && *(des) != '\r' && *(des) != '\n' ){
        len++;
        des++;
    }

    if(myly->tab == NULL){
        myly->len = len;
        myly->tab = calloc(1, myly->len*sizeof(char));
        if(!myly->tab){
            freely(myly);
            myly = NULL;
            return ;
        }
        memcpy(myly->tab, _src, myly->len);

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
 * ===  FUNCTION  ======================================================================
 *         Name:  freely
 *  Description:  
 * =====================================================================================
 */
void freely ( _ly *_fly){
    _ly *tmp;

    while(_fly){
        tmp = _fly;
        _fly = _fly->next;
        if(tmp->tab) free(tmp->tab);
        free(tmp);
    }
}		/* -----  end of function freely  ----- */

/*
void
main(int ac, char **av)
{
    extern yydebug;
    yydebug = 1; 
    _ly *tly;
    //char sql[]="insert into a(c)values(111);";
    //char sql[]="DELETE FROM wp_posts WHERE wp_posts.id = (select * from (select id from wp_posts order by RAND() limit 1) as x);";
    char sql[]="DELETE FROM pgwp_options WHERE option_name = '_transient_doing_cron'";
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
    printf("tab:%s\n", tly->tab);
    freely(tly); 

} main */


 /* vim: set ts=4 sw=4: */

