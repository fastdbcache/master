/*
 * =====================================================================================
 *
 *       Filename:  config_global.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2013 10:34:09 AM
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
#include "config_global.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  conn_init_global
 *  Description:  
 * =====================================================================================
 */
void conn_init_global ( void ){
    
    conn_global = (_conn *)calloc(1, sizeof(_conn));
    if(!conn_global){
        FLOG_ERR("conn_global init error");
        exit(-1);
    }
    conn_global->maxrlim = MAXRLIM; 

    conn_global->server_ip = "localhost";
    conn_global->server_port = 2345;

    conn_global->unix_sock = 0;
    conn_global->path = "/tmp/fdbcd.sock";

    conn_global->pg_host = "localhost";
    conn_global->pg_port = 5432;

    conn_global->do_daemonize = 0;
    conn_global->pid_file = "/var/run/fdbcd.pid";

    conn_global->process_num = 2;

    conn_global->factor = 1.25;
    conn_global->maxbytes = 6 * 1024 * 1024 ;
    conn_global->delaytime = 0;
    conn_global->cache_method = D_MEM;

    conn_global->fdbc = "0.0.4";

    conn_global->dmaxbytes = 2 * 1024 * 1024;
    conn_global->deptype = D_MMAP;
    conn_global->hasdep = H_TRUE;
    conn_global->quotient = 2;
    conn_global->deprule = NULL;
    conn_global->mmap_path = "/usr/local/fdbc/cache";
    conn_global->mmdb_length = DEFAULT_MMAP_BYTE;


    conn_global->maxrq = MAXRQ;
    conn_global->limit_rows = LIMITROW;
    /*
    if(conn_global->deptype == D_MMAP){
        stat(conn_global->mmap_path, &sb);
        if(!S_ISDIR(sb.st_mode)){
            DEBUG("cache is not dir %s, mode:%d", conn_global->mmap_path, sb.st_mode);
            exit(-1);
        }
        myuid = getuid();
        
        if(sb.st_uid != myuid && myuid != 0){
            DEBUG("cache dir can't read!");
            exit(-1);
        }
           
    }  */
}		/* -----  end of function conn_init_global  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  conn_get_global
 *  Description:  
 * =====================================================================================
 */
void conn_get_global (  ){
   
    conn_global->maxrlim = atoi(conf_get("max_openfile"));
    
    conn_global->server_ip = conf_get("server_ip");
    conn_global->server_port = atoi(conf_get("server_port"));

    conn_global->unix_sock = atoi(conf_get("unix_sock"));
    conn_global->path = conf_get("path");

    conn_global->pg_host = conf_get("pg_host");
    conn_global->pg_port = atoi(conf_get("pg_port"));

    conn_global->do_daemonize = atoi(conf_get("do_daemonize"));
    conn_global->pid_file = conf_get("pid_file");

    conn_global->process_num = atoi(conf_get("process_num"));

    conn_global->factor = 1.25;
    conn_global->maxbytes = 6 * 1024 * 1024;
    conn_global->delaytime = atoi(conf_get("cache_delaytime"));
    conn_global->mmap_path = conf_get("cache_path");
    if(strcmp(conf_get("cache_method"), "mmap") == 0)
        conn_global->cache_method = D_MMAP;
    else
        conn_global->cache_method = D_MEM;

    initDeposit(); 
    
    
    return ;
}		/* -----  end of function conn_get_global  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initDeposit
 *  Description:  
 * =====================================================================================
 */
void initDeposit ( ){
    if(strcmp(conf_get("deposit_enable_cache"), "on")==0){
        conn_global->hasdep = H_TRUE;
    }else{
        conn_global->hasdep = H_FREE;
    }
    
    if(strcmp(conf_get("deposit_method"),"mmap")==0)
        conn_global->deptype = D_MMAP;
    else 
        conn_global->deptype = D_MEM;

    if(atol(conf_get("deposit_maxbytes")) <=0 )return;
    conn_global->dmaxbytes = atol(conf_get("deposit_maxbytes"));
     
    if(atoi(conf_get("deposit_quotient")) <= 0) return;
    conn_global->quotient = atoi(conf_get("deposit_quotient"));

    conn_global->deprule = conf_get("deposit_rule");
     
}		/* -----  end of function initDeposit  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pathCheck
 *  Description:  
 * =====================================================================================
 */
void pathCheck ( ){
    struct stat sb;
    if(conn_global->deptype == D_MMAP){
        stat(conn_global->mmap_path, &sb);
        if(!S_ISDIR(sb.st_mode)){
            FLOG_ERR("cache is not dir %s, mode:%d", conn_global->mmap_path, sb.st_mode);
            exit(-1);
        }
        
        if(files_iswrite(conn_global->mmap_path) != 1){
            FLOG_ERR("cache dir can't read! dir:%s", conn_global->mmap_path);
            exit(-1);
        }
           
    }
}		/* -----  end of function pathCheck  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  alignByte
 *  Description:  
 * =====================================================================================
 */
ub4 alignByte ( ub4 len ){
    ub4 _lens;
    _lens = len;
    if (_lens % CHUNK_ALIGN_BYTES)
            _lens += CHUNK_ALIGN_BYTES - (_lens % CHUNK_ALIGN_BYTES);
    return _lens;
}		/* -----  end of function alignByte  ----- */
 /* vim: set ts=4 sw=4: */
