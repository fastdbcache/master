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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_init
 *  Description:  
 * =====================================================================================
 */
void clu_init (  ){
    int clu_fd;
    if(!conn_global->cluster_listen) return;
    if(conn_global->cluster_port == 0) return;

    clu_fd = Socket_bind(conn_global->cluster_listen, conn_global->cluster_port);
    if(clu_fd == -1){
        FLOG_WARN("cluster listen error");
        return;
    }

    
    
}		/* -----  end of function clu_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_find_lead
 *  Description:  for all server 
 * =====================================================================================
 */
void clu_find_lead (  ){

    return <+return_value+>;
}		/* -----  end of function clu_find_lead  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_reset_nodes
 *  Description:  for leader
 * =====================================================================================
 */
void clu_reset_nodes ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_reset_nodes  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_set_lead 
 *  Description:  for proposer and voters
 * =====================================================================================
 */
void clu_set_lead ( <+argument_list+> ){

    return <+return_value+>;
}		/* -----  end of function clu_set_lead  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_bcast_lead
 *  Description:  for who is lead, because lead will bcast the leader command
 *  and lead time
 * =====================================================================================
 */
void clu_bcast_lead ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_bcast_lead  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_uuid
 *  Description:  for lead,  fetch from all proposers and voters 
 * =====================================================================================
 */
void clu_fetch_uuid ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_fetch_uuid  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_cdb
 *  Description:  fetch cdb from all server 
 * =====================================================================================
 */
void clu_fetch_cdb ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_fetch_cdb  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_push
 *  Description:  node push cdb to leader 
 * =====================================================================================
 */
void clu_node_push ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_node_push  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_pop
 *  Description:  get from lead cdb info 
 * =====================================================================================
 */
void clu_node_pop ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_node_pop  ----- */

 /* vim: set ts=4 sw=4: */


