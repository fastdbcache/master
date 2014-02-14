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
            *result = (int)htonl(t32);
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
    int result;
    size_t line;
    void *start;
    struct stat sb;
    char name[1];
    HFD *_hfd, *_hfd_next;    

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

    TAIL_HFD(_hfd_next);
    _hfd = inithfd();
    if(_hfd){
        _hfd->fd = fd;
        _hfd->fsize = sb.st_size;
        _hfd_next->next = _hfd;
    }

    return start;
}		/* -----  end of function mcalloc  ----- */

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

 /* vim: set ts=4 sw=4: */

