/*
 * =====================================================================================
 *
 *       Filename:  cluster_init.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/26/2014 10:32:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cluster_struct.h"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_init
 *  Description:  
 * =====================================================================================
 */
void clu_init (  ){
    int i; 
    
    int clu_fd, client_fd;
    pid_t cpid;
    struct sockaddr_storage client_addr;
	socklen_t client_len;
   
    if(!conn_global->cluster_listen_ip) return;
    if(conn_global->cluster_listen_port == 0) return;

    cpid = fork();
    if(cpid == 0){
        DEBUG("pid:%d", getpid());
        
        clu_fd = Socket_bind(conn_global->cluster_listen_ip, conn_global->cluster_listen_port);
        if(clu_fd == -1){
            FLOG_WARN("cluster listen error");
            return;
        }

        client_fd = accept(clu_fd, (struct sockaddr *)&client_addr, &client_len);

        DEBUG("ip:%s, port:%d, clu_fd:%d",conn_global->cluster_listen_ip, conn_global->cluster_listen_port, clu_fd);   
                 
    }
}		/* -----  end of function clu_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_find_lead
 *  Description:  for all server 
 * =====================================================================================
 */
void clu_find_lead (  ){
   
    if(!conn_global->cluster_leader_ip)return;
    if(conn_global->cluster_leader_port == 0) return;

    if(strcmp(conn_global->cluster_leader_ip, conn_global->cluster_listen_ip) == 0 &&
        conn_global->cluster_leader_port == conn_global->cluster_listen_port)return;

    conn_global->cluster_leader_fd= Client_Init(conn_global->cluster_leader_ip, conn_global->cluster_leader_port);

    if(conn_global->cluster_leader_fd == -1)FLOG_WARN("cluster_leader_fd is -1");

}		/* -----  end of function clu_find_lead  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_find_dbd
 *  Description:  
 * =====================================================================================
 */
void clu_find_dbd (  ){
    FJSON *_jsons, *_tjson;
    int *dbd_fd, *dbd_fd_tmp;
    int dbd_num;
    char host[255]

    if(!conn_global->cluster_fdbd_nodes) return;

    _jsons = calloc(1, sizeof(FJSON)); 
    dbd_num = Json_Root(conn_global->cluster_fdbd_nodes, _jsons);
    if(dbd_num == 0){
        free_json(_jsons);
        return ;
    }
    dbd_fd = calloc(dbd_num, sizeof(int));
    if(!dbd_fd) return;

    dbd_fd_tmp = dbd_fd;

    i=0;
    _tjson = _jsons->next;
    while(_tjson){
       DEBUG("%d. _jsons_len:%d, _jsons:%s",i++, _tjson->string_len, _tjson->string);
       DEBUG("json_number:%d", _tjson->number);
       bzero(host, 254);
       if(_tjson->string_len > 254) continue;
       memcpy(host, _tjson->string, _tjson->string_len);
       dbd_fd_tmp = Client_Init(host, _tjson->number);
       dbd_fd_tmp += 1;

       _tjson = _tjson->next;
       
    }

    return <+return_value+>;
}		/* -----  end of function clu_find_dbd  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_reset_nodes
 *  Description:  for leader
 * =====================================================================================
 */
void clu_reset_nodes (  ){
    return ;
}		/* -----  end of function clu_reset_nodes  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_set_lead 
 *  Description:  for proposer and voters
 * =====================================================================================
 */
void clu_set_lead ( FJSON *_json ){
    FJSON *_tjson;
    char host[255];
    if(!_json->next) return;
    _tjson = _json->next;


    if(strncmp(_tjson->string, conn_global->cluster_listen_ip, _tjson->string_len) == 0 &&
        _tjson->number == conn_global->cluster_listen_port){
        DEBUG("i'm a lead");
        clu_roles = R_LEADER;
    }else{
        if(_tjson->string_len > 254 || _tjson->string_len < 1) return;

        bzero(host, 255);
        
        memcpy(host, _tjson->string, _tjson->string_len);

        clu_lead_fd = Client_Init(host, _tjson->number);
        if(clu_lead_fd == -1) return;
        /* ask node to lead, who is leader */

        Socket_Send(clu_lead_fd, );

        clu_roles = R_NODE; 
    }

    return ;
}		/* -----  end of function clu_set_lead  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_bcast_lead
 *  Description:  for who is lead, because lead will bcast the leader command
 *  and lead time
 * =====================================================================================
 */
void clu_bcast_lead (  ){
    return ;
}		/* -----  end of function clu_bcast_lead  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_uuid
 *  Description:  for lead,  fetch from all proposers and voters 
 * =====================================================================================
 */
void clu_fetch_uuid ( ){
    return ;
}		/* -----  end of function clu_fetch_uuid  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_cdb
 *  Description:  fetch cdb from all server 
 * =====================================================================================
 */
void clu_fetch_cdb (  ){
    return ;
}		/* -----  end of function clu_fetch_cdb  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_push
 *  Description:  node push cdb to leader 
 * =====================================================================================
 */
void clu_node_push (  ){
    return ;
}		/* -----  end of function clu_node_push  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_pop
 *  Description:  get from lead cdb info 
 * =====================================================================================
 */
void clu_node_pop (  ){
    return ;
}		/* -----  end of function clu_node_pop  ----- */

 /* vim: set ts=4 sw=4: */


