/*
 * =====================================================================================
 *
 *       Filename:  json.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/2014 03:55:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "json.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Json_Root
 *  Description:  
 * =====================================================================================
 */
void Json_Root ( char *src_json, FJSON *_json ){
    char *ptr, *ptrs;
    char number[NUMBER_LENGTH];
    ssize_t val_len;
    _JSONC _jsonc, _sjsonc;

    FJSON *_fjson, *_tjson;
    ptr = src_json;
     
    _fjson = _json; 
    _tjson = NULL;
    ptrs = NULL; 
    while ( *ptr ) {
         
        switch ( *ptr ) {
            case ' ':	                
            case '\t':	
            case '\n':
            case '\r':
                break;

            case '[':
                _sjsonc = J_ARRAY;
                val_len = 0; 
                break;
            case '{':
                
                _sjsonc = J_OBJECT;
                val_len = 0;
                break;
            case ',':
                if(_sjsonc == J_OBJECT) break;
            case '}':
                
            case ']':
                if(_jsonc == J_NUMBER && ptrs){
                    if(val_len < NUMBER_LENGTH){
                        bzero(number, NUMBER_LENGTH);
                        memcpy(number, ptrs, val_len);
                        
                        _tjson->number = atoi(number);
                    }
                    _jsonc = J_NEXT; 
                }
                 
                if(_tjson && _jsonc==J_NEXT){
                    _fjson->next = _tjson;
                    _fjson = _tjson;
                }
                _jsonc = J_END;
                val_len = 0;
                ptrs = NULL;
                break;
            /*case '\\':  *ptr++='\\';   break;
            case '\"':  *ptr++='\"';   break;
            case '\b':  *ptr++='b';    break;
            case '\f':  *ptr++='f';    break;
            case '\n':  *ptr++='n';    break;
            case '\r':  *ptr++='r';    break;
            case '\t':  *ptr++='t';    break;
              */
            case '"':
                if(!ptrs){
                    _tjson = calloc(1, sizeof(FJSON));
                    _jsonc = J_STRING;
                    ptrs = ptr+1;                     
                }else{
                    _tjson->string = ptrs;
                    _tjson->string_len = val_len - 2;
                    ptrs = NULL;
                    _jsonc = J_NEXT;
                    val_len = 0;
                }
                break;
            default:
                if(*ptr>='0' && *ptr<='9' && _jsonc == J_NEXT && !ptrs){
                    ptrs = ptr;
                    _jsonc = J_NUMBER;
                }
                val_len++;
                break;
        }				/* -----  end switch  ----- */
        ptr++;
    }

    return ;
}		/* -----  end of function Json_Root  ----- */


 /* vim: set ts=4 sw=4: */

