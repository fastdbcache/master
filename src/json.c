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
void Json_Root ( char *src_json ){
    char *ptr;
    ssize_t val_len;
    
    FJSON *_fjson;
    ptr = src_json;
     
    _fjson = calloc(1, sizeof(FJSON));
        
    while ( *ptr ) {
         
        switch ( *ptr ) {
            case ' ':	                
            case '\t':	
            case '\n':
            case '\r'
                break;

            case '[':
            case '{':
                val_len = 0;
                break;
            case ']':
                
                break;
            case '}':
                break;
            case '\\':  *ptr2++='\\';   break;
            case '\"':  *ptr2++='\"';   break;
            case '\b':  *ptr2++='b';    break;
            case '\f':  *ptr2++='f';    break;
            case '\n':  *ptr2++='n';    break;
            case '\r':  *ptr2++='r';    break;
            case '\t':  *ptr2++='t';    break;
            case ',':
                
                _fjson->
                break;

            case '"':
                break;
            default:
                val_len++;
                break;
        }				/* -----  end switch  ----- */
        ptr++;
    }
    return <+return_value+>;
}		/* -----  end of function Json_Root  ----- */
 /* vim: set ts=4 sw=4: */

