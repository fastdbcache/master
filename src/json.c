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
    _JSONC _jsonc;

    FJSON *_fjson, *_tjson;
    ptr = src_json;
     
    
    _tjson = _fjson;
    ptrs = NULL; 
    while ( *ptr ) {
         
        switch ( *ptr ) {
            case ' ':	                
            case '\t':	
            case '\n':
            case '\r':
                break;

            case '[':
                _fjson = calloc(1, sizeof(FJSON));
                _jsonc = J_OBJECT;
                val_len = 0;
                break;
            case '{':
                _fjson = calloc(1, sizeof(FJSON));
                _jsonc = J_ARRAY;
                val_len = 0;
                break;
            case ',':
                _fjson = calloc(1, sizeof(FJSON));
                break;
            case '}':
                
                _jsonc = J_END;
            case ']':
                if(_jsonc == J_NUMBER){
                    if(val_len < NUMBER_LENGTH){
                        memcpy(number, ptrs, val_len);
                        _tjson->number = atoi(number);
                    }
                }
                _jsonc = J_NEXT;
                val_len = 0;
                ptrs = NULL;
                break;
            case '\\':  *ptr2++='\\';   break;
            case '\"':  *ptr2++='\"';   break;
            case '\b':  *ptr2++='b';    break;
            case '\f':  *ptr2++='f';    break;
            case '\n':  *ptr2++='n';    break;
            case '\r':  *ptr2++='r';    break;
            case '\t':  *ptr2++='t';    break;

            case '"':
                if(!ptrs){
                    _jsonc = J_STRING;
                    ptrs = ptr;
                }else{
                    _tjson->string = ptrs;
                    _tjson->string_len = val_len;
                    ptrs = NULL;
                }
                break;
            default:
                if(*ptr>='0' && *ptr<='9'){
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



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Json_Number
 *  Description:  
 * =====================================================================================
 */
void Json_Number ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function Json_Number  ----- */
 /* vim: set ts=4 sw=4: */

