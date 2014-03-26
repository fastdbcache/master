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
 *         Name:  clu_find_an
 *  Description:  
 * =====================================================================================
 */
void clu_find_an (  ){

    return <+return_value+>;
}		/* -----  end of function clu_find_an  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_reset_nodes
 *  Description:  
 * =====================================================================================
 */
void clu_reset_nodes ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_reset_nodes  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_set_anno
 *  Description:  
 * =====================================================================================
 */
void clu_set_anno ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_set_anno  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_bcast_anno
 *  Description:  
 * =====================================================================================
 */
void clu_bcast_anno ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_bcast_anno  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_uuid
 *  Description:  
 * =====================================================================================
 */
void clu_fetch_uuid ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_fetch_uuid  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_fetch_cdb
 *  Description:  
 * =====================================================================================
 */
void clu_fetch_cdb ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_fetch_cdb  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_push
 *  Description:  
 * =====================================================================================
 */
void clu_node_push ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_node_push  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clu_node_pop
 *  Description:  
 * =====================================================================================
 */
void clu_node_pop ( <+argument_list+> ){
    return <+return_value+>;
}		/* -----  end of function clu_node_pop  ----- */
 /* vim: set ts=4 sw=4: */


