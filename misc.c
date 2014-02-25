/*
 * =====================================================================================
 *
 *       Filename:  pg_misc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/06/2014 12:27:04 PM
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
#include "misc.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initdbp
 *  Description:  
 * =====================================================================================
 */
DBP *initdbp (  ){
    DBP *_dbp;
    _dbp = (DBP *)calloc(1, sizeof(DBP));
    _dbp->inBuf = NULL;
    _dbp->inBufSize = 0;
    _dbp->inCursor = 0;
    _dbp->inEnd = 0;

    return _dbp;
}		/* -----  end of function initdbp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freedbp
 *  Description:  
 * =====================================================================================
 */
void freedbp ( DBP *_dbp ){
    if(!_dbp) return;
    if(_dbp->inBuf) free(_dbp->inBuf);
    _dbp->inBuf = NULL;
    free(_dbp);
    _dbp = NULL;
}		/* -----  end of function freedbp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CheckBufSpace
 *  Description:  
 * =====================================================================================
 */
int CheckBufSpace ( ssize_t endPos, DBP *_dbp ){
    char *newbuf;

    if(!_dbp)return -1;
   
    if(_dbp->inEnd != 0){
        _dbp->inCursor = _dbp->inEnd;
    }

    _dbp->inEnd += endPos;
    if(_dbp->inBufSize > _dbp->inEnd) return 0;
    
    if(!_dbp->inBuf){
        newbuf = (char *)calloc(_dbp->inEnd, sizeof(char));
    }else{
        newbuf = (char *)realloc(_dbp->inBuf, _dbp->inEnd);
    }
    _dbp->inBufSize = _dbp->inEnd; 
    
    if(newbuf){
        _dbp->inBuf = newbuf;
    } else{
        freedbp(_dbp); 
        return -1;
    }
    return 0;
}		/* -----  end of function CheckBufSpace  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInt
 *  Description:  
 * =====================================================================================
 */
int getInt ( int *result, size_t bytes, DBP *_dbp ){
    uint32 t32;
    uint16 t16;

    switch ( bytes ) {
        case 2:	
            memcpy(&t16, _dbp->inBuf + _dbp->inCursor, sizeof(uint16));
            *result = (int)ntohs(t16);
            break;

        case 4:	
            memcpy(&t32, _dbp->inBuf + _dbp->inCursor, sizeof(uint32));
            *result = (int)ntohl(t32);
            break;

        default:	
            return -1;
    }				/* -----  end switch  ----- */
    return 0;
}		/* -----  end of function getInt  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getCont
 *  Description:  
 * =====================================================================================
 */
void getCont (  ){
    return ;
}		/* -----  end of function getCont  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mcalloc
 *  Description:  
 * =====================================================================================
 */
void *mcalloc ( size_t nmemb, size_t size, const char *pathname, int flags ){
    int fd;
    int result, len;
    size_t line;
    void *start;
    struct stat sb;
    char name[1];
    HFD *_hfd, *_hfd_next;    

    len = strlen(pathname);
    if(len >FILE_PATH_LENGTH) return NULL;

    fd = open(pathname, flags, (mode_t)0660);
    if(fd==-1){
        DEBUG("open file error");
        return NULL;
    }
    fstat(fd, &sb);
    
    if(sb.st_size==0){
        line = nmemb*size;
        result = lseek(fd, line-1, SEEK_SET);
        if (result == -1) {
            close(fd);
            DEBUG("Error calling lseek() to 'stretch' the file");
            return -1;
        }
        name[0]='\0';
        /* write just one byte at the end */
        result = write(fd, name, 1);
        if (result < 0) {
            close(fd);
            DEBUG("Error writing a byte at the end of the file");
            return -1;
        }
                
        lseek(fd,0,SEEK_SET);
        fstat(fd, &sb);
    }
    start = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(start == MAP_FAILED){
        DEBUG("init mmap error %s", pathname);
        close(fd);
        return NULL;
    }
    _hfd_next = NULL;
    if(!pools_hfd){
        pools_hfd = inithfd();
    }
    TAIL_HFD(_hfd_next);
                           
    _hfd = inithfd();
    if(_hfd){
        _hfd->fd = fd;
        _hfd->fsize = sb.st_size;
        memcpy(_hfd->name, pathname, len); 
        _hfd->name[len+1]='\0';
        _hfd->len = len;
        _hfd->ptr = start;
        _hfd_next->next = _hfd;
    }

    return start;
}		/* -----  end of function mcalloc  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  unmmap
 *  Description:  
 * =====================================================================================
 */
void unmmap ( const char *pathname ){
    HFD *_hfd, *_hfd_next;
    int len;
    _hfd = pools_hfd;
    len = strlen(pathname);
    while(_hfd && _hfd->next){
        _hfd_next = _hfd;
        _hfd = _hfd->next;
        if(_hfd->len == len &&
            strncmp(_hfd->name, pathname, len)==0){

            if (munmap(_hfd->ptr, _hfd->fsize) == -1) {
                perror("Error un-mmapping the file");
                /* Decide here whether to close(fd) and exit() or not. Depends... */
            }            
            DEBUG("size:%llu", _hfd->fsize);
            close(_hfd->fd);
            _hfd_next->next = _hfd->next;
            free(_hfd);
            _hfd=NULL;
            return;
        }
    }
    
}		/* -----  end of function unmmap  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithfd
 *  Description:  
 * =====================================================================================
 */
HFD *inithfd ( ){
    HFD *_hfd;

    _hfd = (HFD *)calloc(1, sizeof(HFD));
    if(!_hfd) return NULL;
    _hfd->fd = 0;
    _hfd->fsize = 0; 
    _hfd->name[0] = '\0'; 
    _hfd->len = 0;
    _hfd->ptr = NULL;
    _hfd->next = NULL;
    return _hfd;
}		/* -----  end of function inithfd  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freehfd
 *  Description:  
 * =====================================================================================
 */
void freehfd ( HFD *_hfd ){
    if(_hfd->next){
        freehfd(_hfd->next);
    }
    close(_hfd->fd);
    free(_hfd);
}		/* -----  end of function freehfd  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  checkLimit
 *  Description:  
 * =====================================================================================
 */
void checkLimit ( DBP *dbp ){
    DBP *_dbp;
    char limit[]="LIMIT";
    char setlimit[20];
    int len, i;
    uint32 start_addr, total_len;

    _dbp = dbp;
    
    if(memmem(_dbp->inBuf, _dbp->inEnd, limit, 5) != NULL) return;
    bzero(setlimit, 20);
    snprintf(setlimit, 19, " %s %d;", limit, conn_global->limit_rows);

    len = strlen(setlimit)+1;
    i = 0;

    while(i++<_dbp->inEnd){
        if((*(_dbp->inBuf+_dbp->inEnd-i) >47 &&
            *(_dbp->inBuf+_dbp->inEnd-i) <58 )||
            (*(_dbp->inBuf+_dbp->inEnd-i) >64 &&
            *(_dbp->inBuf+_dbp->inEnd-i) <91 )||
            *(_dbp->inBuf+_dbp->inEnd-i) >96 &&
           (*(_dbp->inBuf+_dbp->inEnd-i) <123 )||
            *(_dbp->inBuf+_dbp->inEnd-i) ==34 ||
            *(_dbp->inBuf+_dbp->inEnd-i) ==39 ){                        
            i=1;
            break;
        }
        if(*(_dbp->inBuf+_dbp->inEnd-i) == ';'){
            
            break;
        } 
    }
    if(i==_dbp->inEnd)return;
     
    start_addr = _dbp->inEnd-i;
    
    if(CheckBufSpace((len - i), _dbp) != 0){
        DEBUG("CheckBufSpace error");
        return -1;
    }
    memcpy(_dbp->inBuf+start_addr, setlimit, len);
     
    _dbp->inCursor = sizeof(char)+sizeof(uint32);
    total_len = htonl((_dbp->inEnd-sizeof(char)));
    memcpy(_dbp->inBuf+sizeof(char), &total_len, sizeof(uint32));
     
}		/* -----  end of function checkLimit  ----- */

 /* vim: set ts=4 sw=4: */

