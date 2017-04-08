/*
 *  This file is part of SST/macroscale:
 *               The macroscale architecture simulator from the SST suite.
 *  Copyright (c) 2009 Sandia Corporation.
 *  This software is distributed under the BSD License.
 *  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 *  the U.S. Government retains certain rights in this software.
 *  For more information, see the LICENSE file in the top
 *  SST/macroscale directory.
 */

#ifndef sstmac_skeletons_otf2_MPI_CALLS_H_
#define sstmac_skeletons_otf2_MPI_CALLS_H_

enum MPI_CALL_ID {
    ID_NULL = 0,
    ID_MPIX_Comm_agree,
    ID_MPI_File_set_errhandler,
    ID_MPI_Rsend,
    ID_MPIX_Comm_failure_ack,
    ID_MPI_File_set_info,
    ID_MPI_Rsend_init,
    ID_MPIX_Comm_failure_get_acked,
    ID_MPI_File_set_size,
    ID_MPI_Scan,
    ID_MPIX_Comm_revoke,
    ID_MPI_File_set_view,
    ID_MPI_Scatter,
    ID_MPIX_Comm_shrink,
    ID_MPI_File_sync,
    ID_MPI_Scatterv,
    ID_MPI_Abort,
    ID_MPI_File_write,
    ID_MPI_Send,
    ID_MPI_Accumulate,
    ID_MPI_File_write_all,
    ID_MPI_Send_init,
    ID_MPI_Add_error_class,
    ID_MPI_File_write_all_begin,
    ID_MPI_Sendrecv,
    ID_MPI_Add_error_code,
    ID_MPI_File_write_all_end,
    ID_MPI_Sendrecv_replace,
    ID_MPI_Add_error_string,
    ID_MPI_File_write_at,
    ID_MPI_Ssend,
    ID_MPI_Address,
    ID_MPI_File_write_at_all,
    ID_MPI_Ssend_init,
    ID_MPI_Aint_add,
    ID_MPI_File_write_at_all_begin,
    ID_MPI_Start,
    ID_MPI_Aint_diff,
    ID_MPI_File_write_at_all_end,
    ID_MPI_Startall,
    ID_MPI_Allgather,
    ID_MPI_File_write_ordered,
    ID_MPI_Status_set_cancelled,
    ID_MPI_Allgatherv,
    ID_MPI_File_write_ordered_begin,
    ID_MPI_Status_set_elements,
    ID_MPI_Alloc_mem,
    ID_MPI_File_write_ordered_end,
    ID_MPI_Status_set_elements_x,
    ID_MPI_Allreduce,
    ID_MPI_File_write_shared,
    ID_MPI_T_category_changed,
    ID_MPI_Alltoall,
    ID_MPI_Finalize,
    ID_MPI_T_category_get_categories,
    ID_MPI_Alltoallv,
    ID_MPI_Finalized,
    ID_MPI_T_category_get_cvars,
    ID_MPI_Alltoallw,
    ID_MPI_Free_mem,
    ID_MPI_T_category_get_info,
    ID_MPI_Attr_delete,
    ID_MPI_Gather,
    ID_MPI_T_category_get_num,
    ID_MPI_Attr_get,
    ID_MPI_Gatherv,
    ID_MPI_T_category_get_pvars,
    ID_MPI_Attr_put,
    ID_MPI_Get,
    ID_MPI_T_cvar_get_info,
    ID_MPI_Barrier,
    ID_MPI_Get_accumulate,
    ID_MPI_T_cvar_get_num,
    ID_MPI_Bcast,
    ID_MPI_Get_address,
    ID_MPI_T_cvar_handle_alloc,
    ID_MPI_Bsend,
    ID_MPI_Get_count,
    ID_MPI_T_cvar_handle_free,
    ID_MPI_Bsend_init,
    ID_MPI_Get_elements,
    ID_MPI_T_cvar_read,
    ID_MPI_Buffer_attach,
    ID_MPI_Get_elements_x,
    ID_MPI_T_cvar_write,
    ID_MPI_Buffer_detach,
    ID_MPI_Get_library_version,
    ID_MPI_T_enum_get_info,
    ID_MPI_Cancel,
    ID_MPI_Get_processor_name,
    ID_MPI_T_enum_get_item,
    ID_MPI_Cart_coords,
    ID_MPI_Get_version,
    ID_MPI_T_finalize,
    ID_MPI_Cart_create,
    ID_MPI_Graph_create,
    ID_MPI_T_init_thread,
    ID_MPI_Cart_get,
    ID_MPI_Graph_get,
    ID_MPI_T_pvar_get_info,
    ID_MPI_Cart_map,
    ID_MPI_Graph_map,
    ID_MPI_T_pvar_get_num,
    ID_MPI_Cart_rank,
    ID_MPI_Graph_neighbors,
    ID_MPI_T_pvar_handle_alloc,
    ID_MPI_Cart_shift,
    ID_MPI_Graph_neighbors_count,
    ID_MPI_T_pvar_handle_free,
    ID_MPI_Cart_sub,
    ID_MPI_Graphdims_get,
    ID_MPI_T_pvar_read,
    ID_MPI_Cartdim_get,
    ID_MPI_Grequest_complete,
    ID_MPI_T_pvar_readreset,
    ID_MPI_Close_port,
    ID_MPI_Grequest_start,
    ID_MPI_T_pvar_reset,
    ID_MPI_Comm_accept,
    ID_MPI_Group_compare,
    ID_MPI_T_pvar_session_create,
    ID_MPI_Comm_call_errhandler,
    ID_MPI_Group_difference,
    ID_MPI_T_pvar_session_free,
    ID_MPI_Comm_compare,
    ID_MPI_Group_excl,
    ID_MPI_T_pvar_start,
    ID_MPI_Comm_connect,
    ID_MPI_Group_free,
    ID_MPI_T_pvar_stop,
    ID_MPI_Comm_create,
    ID_MPI_Group_incl,
    ID_MPI_T_pvar_write,
    ID_MPI_Comm_create_errhandler,
    ID_MPI_Group_intersection,
    ID_MPI_Test,
    ID_MPI_Comm_create_group,
    ID_MPI_Group_range_excl,
    ID_MPI_Test_cancelled,
    ID_MPI_Comm_create_keyval,
    ID_MPI_Group_range_incl,
    ID_MPI_Testall,
    ID_MPI_Comm_delete_attr,
    ID_MPI_Group_rank,
    ID_MPI_Testany,
    ID_MPI_Comm_disconnect,
    ID_MPI_Group_size,
    ID_MPI_Testsome,
    ID_MPI_Comm_dup,
    ID_MPI_Group_translate_ranks,
    ID_MPI_Topo_test,
    ID_MPI_Comm_dup_with_info,
    ID_MPI_Group_union,
    ID_MPI_Type_commit,
    ID_MPI_Comm_free,
    ID_MPI_Iallgather,
    ID_MPI_Type_contiguous,
    ID_MPI_Comm_free_keyval,
    ID_MPI_Iallgatherv,
    ID_MPI_Type_create_darray,
    ID_MPI_Comm_get_attr,
    ID_MPI_Iallreduce,
    ID_MPI_Type_create_hindexed,
    ID_MPI_Comm_get_errhandler,
    ID_MPI_Ialltoall,
    ID_MPI_Type_create_hindexed_block,
    ID_MPI_Comm_get_info,
    ID_MPI_Ialltoallv,
    ID_MPI_Type_create_hvector,
    ID_MPI_Comm_get_name,
    ID_MPI_Ialltoallw,
    ID_MPI_Type_create_indexed_block,
    ID_MPI_Comm_get_parent,
    ID_MPI_Ibarrier,
    ID_MPI_Type_create_keyval,
    ID_MPI_Comm_group,
    ID_MPI_Ibcast,
    ID_MPI_Type_create_resized,
    ID_MPI_Comm_idup,
    ID_MPI_Ibsend,
    ID_MPI_Type_create_struct,
    ID_MPI_Comm_join,
    ID_MPI_Iexscan,
    ID_MPI_Type_create_subarray,
    ID_MPI_Comm_rank,
    ID_MPI_Igather,
    ID_MPI_Type_delete_attr,
    ID_MPI_Comm_remote_group,
    ID_MPI_Igatherv,
    ID_MPI_Type_dup,
    ID_MPI_Comm_remote_size,
    ID_MPI_Improbe,
    ID_MPI_Type_extent,
    ID_MPI_Comm_set_attr,
    ID_MPI_Imrecv,
    ID_MPI_Type_free,
    ID_MPI_Comm_set_errhandler,
    ID_MPI_Ineighbor_allgather,
    ID_MPI_Type_free_keyval,
    ID_MPI_Comm_set_info,
    ID_MPI_Ineighbor_allgatherv,
    ID_MPI_Type_get_attr,
    ID_MPI_Comm_set_name,
    ID_MPI_Ineighbor_alltoall,
    ID_MPI_Type_get_contents,
    ID_MPI_Comm_size,
    ID_MPI_Ineighbor_alltoallv,
    ID_MPI_Type_get_envelope,
    ID_MPI_Comm_spawn,
    ID_MPI_Ineighbor_alltoallw,
    ID_MPI_Type_get_extent,
    ID_MPI_Comm_spawn_multiple,
    ID_MPI_Info_create,
    ID_MPI_Type_get_extent_x,
    ID_MPI_Comm_split,
    ID_MPI_Info_delete,
    ID_MPI_Type_get_name,
    ID_MPI_Comm_split_type,
    ID_MPI_Info_dup,
    ID_MPI_Type_get_true_extent,
    ID_MPI_Comm_test_inter,
    ID_MPI_Info_free,
    ID_MPI_Type_get_true_extent_x,
    ID_MPI_Compare_and_swap,
    ID_MPI_Info_get,
    ID_MPI_Type_hindexed,
    ID_MPI_Dims_create,
    ID_MPI_Info_get_nkeys,
    ID_MPI_Type_hvector,
    ID_MPI_Dist_graph_create,
    ID_MPI_Info_get_nthkey,
    ID_MPI_Type_indexed,
    ID_MPI_Dist_graph_create_adjacent,
    ID_MPI_Info_get_valuelen,
    ID_MPI_Type_lb,
    ID_MPI_Dist_graph_neighbors,
    ID_MPI_Info_set,
    ID_MPI_Type_match_size,
    ID_MPI_Dist_graph_neighbors_count,
    ID_MPI_Init,
    ID_MPI_Type_set_attr,
    ID_MPI_Errhandler_create,
    ID_MPI_Init_thread,
    ID_MPI_Type_set_name,
    ID_MPI_Errhandler_free,
    ID_MPI_Initialized,
    ID_MPI_Type_size,
    ID_MPI_Errhandler_get,
    ID_MPI_Intercomm_create,
    ID_MPI_Type_size_x,
    ID_MPI_Errhandler_set,
    ID_MPI_Intercomm_merge,
    ID_MPI_Type_struct,
    ID_MPI_Error_class,
    ID_MPI_Iprobe,
    ID_MPI_Type_ub,
    ID_MPI_Error_string,
    ID_MPI_Irecv,
    ID_MPI_Type_vector,
    ID_MPI_Exscan,
    ID_MPI_Ireduce,
    ID_MPI_Unpack,
    ID_MPI_Fetch_and_op,
    ID_MPI_Ireduce_scatter,
    ID_MPI_Unpack_external,
    ID_MPI_File_c2f,
    ID_MPI_Ireduce_scatter_block,
    ID_MPI_Unpublish_name,
    ID_MPI_File_call_errhandler,
    ID_MPI_Irsend,
    ID_MPI_Wait,
    ID_MPI_File_close,
    ID_MPI_Is_thread_main,
    ID_MPI_Waitall,
    ID_MPI_File_create_errhandler,
    ID_MPI_Iscan,
    ID_MPI_Waitany,
    ID_MPI_File_delete,
    ID_MPI_Iscatter,
    ID_MPI_Waitsome,
    ID_MPI_File_f2c,
    ID_MPI_Iscatterv,
    ID_MPI_Win_allocate,
    ID_MPI_File_get_amode,
    ID_MPI_Isend,
    ID_MPI_Win_allocate_shared,
    ID_MPI_File_get_atomicity,
    ID_MPI_Issend,
    ID_MPI_Win_attach,
    ID_MPI_File_get_byte_offset,
    ID_MPI_Keyval_create,
    ID_MPI_Win_call_errhandler,
    ID_MPI_File_get_errhandler,
    ID_MPI_Keyval_free,
    ID_MPI_Win_complete,
    ID_MPI_File_get_group,
    ID_MPI_Lookup_name,
    ID_MPI_Win_create,
    ID_MPI_File_get_info,
    ID_MPI_Mprobe,
    ID_MPI_Win_create_dynamic,
    ID_MPI_File_get_position,
    ID_MPI_Mrecv,
    ID_MPI_Win_create_errhandler,
    ID_MPI_File_get_position_shared,
    ID_MPI_Neighbor_allgather,
    ID_MPI_Win_create_keyval,
    ID_MPI_File_get_size,
    ID_MPI_Neighbor_allgatherv,
    ID_MPI_Win_delete_attr,
    ID_MPI_File_get_type_extent,
    ID_MPI_Neighbor_alltoall,
    ID_MPI_Win_detach,
    ID_MPI_File_get_view,
    ID_MPI_Neighbor_alltoallv,
    ID_MPI_Win_fence,
    ID_MPI_File_iread,
    ID_MPI_Neighbor_alltoallw,
    ID_MPI_Win_flush,
    ID_MPI_File_iread_all,
    ID_MPI_Op_commute,
    ID_MPI_Win_flush_all,
    ID_MPI_File_iread_at,
    ID_MPI_Op_create,
    ID_MPI_Win_flush_local,
    ID_MPI_File_iread_at_all,
    ID_MPI_Op_free,
    ID_MPI_Win_flush_local_all,
    ID_MPI_File_iread_shared,
    ID_MPI_Open_port,
    ID_MPI_Win_free,
    ID_MPI_File_iwrite,
    ID_MPI_Pack,
    ID_MPI_Win_free_keyval,
    ID_MPI_File_iwrite_all,
    ID_MPI_Pack_external,
    ID_MPI_Win_get_attr,
    ID_MPI_File_iwrite_at,
    ID_MPI_Pack_external_size,
    ID_MPI_Win_get_errhandler,
    ID_MPI_File_iwrite_at_all,
    ID_MPI_Pack_size,
    ID_MPI_Win_get_group,
    ID_MPI_File_iwrite_shared,
    ID_MPI_Pcontrol,
    ID_MPI_Win_get_info,
    ID_MPI_File_open,
    ID_MPI_Probe,
    ID_MPI_Win_get_name,
    ID_MPI_File_preallocate,
    ID_MPI_Publish_name,
    ID_MPI_Win_lock,
    ID_MPI_File_read,
    ID_MPI_Put,
    ID_MPI_Win_lock_all,
    ID_MPI_File_read_all,
    ID_MPI_Query_thread,
    ID_MPI_Win_post,
    ID_MPI_File_read_all_begin,
    ID_MPI_Raccumulate,
    ID_MPI_Win_set_attr,
    ID_MPI_File_read_all_end,
    ID_MPI_Recv,
    ID_MPI_Win_set_errhandler,
    ID_MPI_File_read_at,
    ID_MPI_Recv_init,
    ID_MPI_Win_set_info,
    ID_MPI_File_read_at_all,
    ID_MPI_Reduce,
    ID_MPI_Win_set_name,
    ID_MPI_File_read_at_all_begin,
    ID_MPI_Reduce_local,
    ID_MPI_Win_shared_query,
    ID_MPI_File_read_at_all_end,
    ID_MPI_Reduce_scatter,
    ID_MPI_Win_start,
    ID_MPI_File_read_ordered,
    ID_MPI_Reduce_scatter_block,
    ID_MPI_Win_sync,
    ID_MPI_File_read_ordered_begin,
    ID_MPI_Register_datarep,
    ID_MPI_Win_test,
    ID_MPI_File_read_ordered_end,
    ID_MPI_Request_free,
    ID_MPI_Win_unlock,
    ID_MPI_File_read_shared,
    ID_MPI_Request_get_status,
    ID_MPI_Win_unlock_all,
    ID_MPI_File_seek,
    ID_MPI_Rget,
    ID_MPI_Win_wait,
    ID_MPI_File_seek_shared,
    ID_MPI_Rget_accumulate,
    ID_MPI_Wtick,
    ID_MPI_File_set_atomicity,
    ID_MPI_Rput,
    ID_MPI_Wtime
};

#endif /* MPI_CALLS_H_ */
