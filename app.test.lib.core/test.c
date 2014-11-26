#include <stdio.h>

#include <lib.test/test.h>

#include "test-defines.h"
#include "test-error.h"
#include "test-object.h"
#include "test-ascii.h"
#include "test-memory.h"
#include "test-permute.h"
#include "test-combinations.h"
#include "test-convert.h"
#include "test-random.h"
#include "test-stopwatch.h"
#include "test-string.h"
#include "test-list.h"
#include "test-tree.h"
#include "test-tainio-tree.h"
#include "test-file.h"
#include "test-file-reader.h"
#include "test-file-writer.h"
#include "test-print.h"
#include "test-topological.h"
#include "test-index.h"
#include "test-big-int.h"

bool (*tests[]) (Test *test) =
{
        &test_defines,
        &test_size_t_add,
        &test_size_t_mul,
        &test_size_t_pow_invalid_argument,
        &test_size_t_pow_function_call_1,
        &test_size_t_pow_function_call_2,
        &test_size_t_pow,
        &test_size_t_private_max,
        &test_unsigned_long_long_add,
        &test_unsigned_long_long_mul,
        &test_unsigned_long_long_private_max,
        &test_unsigned_long_long_bit_get,
        &test_unsigned_long_long_bit_set,
        &test_unsigned_long_long_bit_most_significant,
        &test_error,
        &test_error_add_invalid_error,
        &test_error_add,
        &test_error_reset,
        &test_error_count,
        &test_error_at,
        &test_object_id_overflow,
        &test_object_id,
        &test_object_id_set_counter,
        &test_ascii_is_white_space,
        &test_ascii_is_digit,
        &test_ascii_is_digit_octal,
        &test_ascii_is_digit_hexadecimal,
        &test_memory_create_invalid_argument,
        &test_memory_create_overflow_1,
        &test_memory_create_overflow_2,
        &test_memory_create_overflow_3,
        &test_memory_create_commit_limit,
        &test_memory_create_total_create_limit,
        &test_memory_create_out_of_memory,
        &test_memory_create_1,
        &test_memory_create_2,
        &test_memory_create_3,
        &test_memory_size_invalid_argument,
        &test_memory_size,
        &test_memory_grow_invalid_argument_1,
        &test_memory_grow_invalid_argument_2,
        &test_memory_grow_invalid_operation,
        &test_memory_grow_overflow_1,
        &test_memory_grow_overflow_2,
        &test_memory_grow_overflow_3,
        &test_memory_grow_commit_limit,
        &test_memory_grow_total_create_limit,
        &test_memory_grow_out_of_memory,
        &test_memory_grow_1,
        &test_memory_grow_2,
        &test_memory_destroy_invalid_argument,
        &test_memory_destroy,
        &test_memory_total_create_size,
        &test_memory_total_destroy_size,
        &test_permute_create_invalid_argument,
        &test_permute_create_overflow_1,
        &test_permute_create_overflow_2,
        &test_permute_create_function_call_1,
        &test_permute_create_function_call_2,
        &test_permute_create,
        &test_permute_destroy_invalid_argument,
        &test_permute_destroy,
        &test_permute_next_invalid_argument,
        &test_permute_next_1,
        &test_permute_next_2,
        &test_combinations_a_b_invalid_argument_1,
        &test_combinations_a_b_invalid_argument_2,
        &test_combinations_a_b_invalid_argument_3,
        &test_combinations_a_b_invalid_argument_4,
        &test_combinations_a_b_invalid_argument_5,
        &test_combinations_a_b,
        &test_convert_string_to_int_invalid_argument_1,
        &test_convert_string_to_int_invalid_argument_2,
        &test_convert_string_to_int_invalid_argument_3,
        &test_convert_string_to_int_invalid_operation_1,
        &test_convert_string_to_int_invalid_operation_2,
        &test_convert_string_to_int_invalid_operation_3,
        &test_convert_string_to_int_invalid_operation_4,
        &test_convert_string_to_int_overflow_1,
        &test_convert_string_to_size_t_invalid_argument,
        &test_convert_string_to_size_t_function_call,
        &test_convert_string_to_size_t_overflow,
        &test_convert_string_to_size_t,
        &test_convert_string_to_unsigned_long_long_invalid_argument_1,
        &test_convert_string_to_unsigned_long_long_invalid_argument_2,
        &test_convert_string_to_unsigned_long_long_invalid_argument_3,
        &test_convert_string_to_unsigned_long_long_invalid_operation_1,
        &test_convert_string_to_unsigned_long_long_invalid_operation_2,
        &test_convert_string_to_unsigned_long_long_overflow,
        &test_convert_string_to_unsigned_long_long,
        &test_random_open_invalid_operation,
        &test_random_bytes_invalid_operation,
        &test_random_bytes_invalid_argument_1,
        &test_random_bytes_invalid_argument_2,
        &test_random_bytes,
        &test_random_value,
        &test_random_close_invalid_operation,
        &test_stopwatch_start_invalid_argument,
        &test_stopwatch_start_invalid_operation,
        &test_stopwatch_stop_invalid_argument,
        &test_stopwatch_stop_invalid_operation_1,
        &test_stopwatch_stop_invalid_operation_2,
        &test_stopwatch_reset_invalid_argument,
        &test_stopwatch_reset,
        &test_stopwatch_elapsed_time_invalid_argument_1,
        &test_stopwatch_elapsed_time_invalid_argument_2,
        &test_stopwatch_elapsed_time_invalid_operation_1,
        &test_stopwatch_elapsed_time_invalid_operation_2,
        &test_stopwatch_elapsed_time_invalid_operation_3,
        &test_stopwatch_elapsed_time,
        &test_string_create_invalid_argument,
        &test_string_create_overflow,
        &test_string_create_function_call,
        &test_string_create_1,
        &test_string_create_2,
        &test_string_create_3,
        &test_string_create_with_size_invalid_operation,
        &test_string_create_with_size_function_call,
        &test_string_create_with_size,
        &test_string_destroy_invalid_argument,
        &test_string_destroy,
        &test_string_append_invalid_argument_1,
        &test_string_append_invalid_argument_2,
        &test_string_append_invalid_argument_3,
        &test_string_append_overflow_1,
        &test_string_append_overflow_2,
        &test_string_append_function_call,
        &test_string_append_1,
        &test_string_append_2,
        &test_string_append_3,
        &test_string_append_4,
        &test_string_append_5,
        &test_string_append_6,
        &test_string_append_7,
        &test_string_append_8,
        &test_string_append_9,
        &test_string_equals_invalid_argument_1,
        &test_string_equals_invalid_argument_2,
        &test_string_equals_1,
        &test_string_equals_2,
        &test_string_equals_3,
        &test_string_begins_with_invalid_argument_1,
        &test_string_begins_with_invalid_argument_2,
        &test_string_begins_with,
        &test_string_ends_with_invalid_argument_1,
        &test_string_ends_with_invalid_argument_2,
        &test_string_ends_with,
        &test_string_index_invalid_argument_1,
        &test_string_index_invalid_argument_2,
        &test_string_index_overflow,
        &test_string_index,
        &test_string_split_invalid_argument,
        &test_string_split_overflow,
        &test_string_split_function_call_1,
        &test_string_split_function_call_2,
        &test_string_split_1,
        &test_string_split_2,
        &test_string_split_3,
        &test_string_split_4,
        &test_string_split_5,
        &test_string_split_6,
        &test_string_split_7,
        &test_string_split_8,
        &test_string_split_9,
        &test_string_split_10,
        &test_string_split_11,
        &test_string_split_12,
        &test_string_split_13,
        &test_string_split_14,
        &test_string_split_15,
        &test_string_split_16,
        &test_string_split_17,
        &test_string_split_18,
        &test_list_create,
        &test_list_destroy_invalid_argument,
        &test_list_destroy,
        &test_list_insert_invalid_argument,
        &test_list_insert_overflow,
        &test_list_insert_function_call,
        &test_list_insert_1,
        &test_list_insert_2,
        &test_list_insert_3,
        &test_list_append_invalid_argument,
        &test_list_append_overflow,
        &test_list_append_function_call,
        &test_list_append_1,
        &test_list_append_2,
        &test_list_append_3,
        &test_list_position_invalid_argument_1,
        &test_list_position_invalid_argument_2,
        &test_list_position_1,
        &test_list_position_2,
        &test_list_remove_invalid_argument,
        &test_list_remove,
        &test_list_first,
        &test_list_last,
        &test_list_count_invalid_argument,
        &test_list_count,
        &test_list_find_invalid_argument,
        &test_list_find,
        &test_tree_create_function_call_1,
        &test_tree_create_function_call_2,
        &test_tree_create,
        &test_tree_node_insert_function_call_1,
        &test_tree_node_insert_function_call_2,
        &test_tree_node_insert_invalid_operation,
        &test_tree_insert_invalid_argument_1,
        &test_tree_insert_invalid_argument_2,
        &test_tree_insert_overflow,
        &test_tree_insert_function_call_1,
        &test_tree_insert_function_call_2,
        &test_tree_insert_invalid_operation,
        &test_tree_insert_1,
        &test_tree_insert_2,
        &test_tree_insert_3,
        &test_tree_insert_4,
        &test_tree_insert_5,
        &test_tree_insert_6,
        &test_tree_search_invalid_argument_1,
        &test_tree_search_invalid_argument_2,
        &test_tree_search_1,
        &test_tree_search_2,
        &test_tree_search_3,
        &test_tree_search_4,
        &test_tree_search_5,
        &test_tree_search_6,
        &test_tree_search_7,
        &test_tree_iterator_path_size_1,
        &test_tree_iterator_path_size_2,
        &test_tree_iterator_create_function_call_1,
        &test_tree_iterator_create_function_call_2,
        &test_tree_iterator_create,
        &test_tree_iterator_next_invalid_argument,
        &test_tree_iterator_next_overflow,
        &test_tree_iterator_next_1,
        &test_tree_iterator_next_2,
        &test_tree_iterator_next_3,
        &test_tree_iterator_destroy_invalid_argument,
        &test_tree_iterator_destroy,
        &test_tree_count_invalid_argument,
        &test_tree_count,
        &test_tainio_tree_create_invalid_argument_1,
        &test_tainio_tree_create_invalid_argument_2,
        &test_tainio_tree_create_function_call_1,
        &test_tainio_tree_create_function_call_2,
        &test_tainio_tree_create,
        &test_tainio_tree_destroy_invalid_argument,
        &test_tainio_tree_next_invalid_argument_1,
        &test_tainio_tree_next_invalid_argument_2,
        &test_tainio_tree_next_invalid_operation,
        &test_tainio_tree_next_overflow,
        &test_tainio_tree_next_function_call_1,
        &test_tainio_tree_next_function_call_2,
        &test_tainio_tree_next_function_call_3,
        &test_tainio_tree_next_function_call_1_2,
        &test_tainio_tree_next,
        &test_file_path_size_1,
        &test_file_path_size_2,
        &test_file_directory_current_path_function_call,
        &test_file_directory_current_path_system_call,
        &test_file_directory_current_path,
        &test_file_path_is_valid,
        &test_file_name_from_path_invalid_operation,
        &test_file_name_from_path,
        &test_file_directory_sub_path_invalid_operation,
        &test_file_directory_sub_path_function_call,
        &test_file_directory_sub_path,
        &test_file_directory_open_invalid_operation,
        &test_file_directory_open_system_call,
        &test_file_directory_open_function_call_1,
        &test_file_directory_open_function_call_2,
        &test_file_directory_open_function_call_3,
        &test_file_directory_open_current_1,
        &test_file_directory_open_current_2,
        &test_file_directory_open,
        &test_file_directory_close_invalid_argument,
        &test_file_directory_close,
        &test_file_directory_read_invalid_argument,
        &test_file_directory_read_1,
        &test_file_directory_read_2,
        &test_file_directory_read_3,
        &test_file_directory_read_4,
        &test_file_directory_read_5,
        &test_file_directory_read_6,
        &test_file_directory_find_invalid_argument,
        &test_file_directory_find,
        &test_file_open,
        &test_file_close,
        &test_file_readline_f1,
        &test_file_readline_f2,
        &test_file_readline_f3,
        &test_file_remove,
        &test_file_exists,
        &test_file_reader_create_function_call_1,
        &test_file_reader_create_function_call_2,
        &test_file_reader_create_system_call_1,
        &test_file_reader_create_system_call_2,
        &test_file_reader_create_system_call_3,
        &test_file_reader_create,
        &test_file_reader_destroy_invalid_argument,
        &test_file_writer_create_invalid_argument_1,
        &test_file_writer_create_invalid_argument_2,
        &test_file_writer_create_function_call_1,
        &test_file_writer_create_function_call_2,
        &test_file_writer_create_system_call,
        &test_file_writer_create_append,
        &test_file_writer_create_truncate,
        &test_file_writer_write_invalid_argument_1,
        &test_file_writer_write_invalid_argument_2,
        &test_file_writer_write_invalid_argument_3,
        &test_file_writer_write_system_call,
        &test_file_writer_write_args_invalid_argument_1,
        &test_file_writer_write_args_invalid_argument_2,
        &test_file_writer_write_args_system_call,
        &test_file_writer_write_args,
        &test_print,
        &test_print_silent,
        &test_print_log_begin_invalid_argument,
        &test_print_log_begin_invalid_operation,
        &test_print_log_begin_function_call,
        &test_print_log_end_invalid_operation,
        &test_print_log_1,
        &test_print_log_2,
        &test_topological_create_function_call_1,
        &test_topological_create_function_call_2,
        &test_topological_create_function_call_3,
        &test_topological_create,
        &test_topological_destroy_invalid_argument,
        &test_topological_destroy,
        &test_topological_add_vertex_invalid_argument_1,
        &test_topological_add_vertex_invalid_argument_2,
        &test_topological_add_vertex_invalid_operation,
        &test_topological_add_vertex_function_call,
        &test_topological_add_vertex,
        &test_topological_contains_vertex_invalid_argument_1,
        &test_topological_contains_vertex_invalid_argument_2,
        &test_topological_contains_vertex,
        &test_topological_set_edge_invalid_argument_1,
        &test_topological_set_edge_invalid_argument_2,
        &test_topological_set_edge_invalid_argument_3,
        &test_topological_set_edge_invalid_operation_1,
        &test_topological_set_edge_invalid_operation_2,
        &test_topological_set_edge_function_call,
        &test_topological_set_edge_1,
        &test_topological_set_edge_2,
        &test_topological_set_edge_3,
        &test_topological_set_edge_4,
        &test_topological_sort_invalid_argument,
        &test_topological_sort_function_call_1,
        &test_topological_sort_function_call_2,
        &test_topological_sort_1,
        &test_topological_sort_2,
        &test_topological_sort_3,
        &test_topological_sort_4,
        &test_topological_sort_5,
        &test_topological_sort_6,
        &test_topological_sort_7,
        &test_topological_sort_8,
        &test_topological_sort_9,
        &test_topological_sort_10,
        &test_topological_sort_11,
        &test_topological_sort_12,
        &test_topological_sort_13,
        &test_topological_sort_14,
        &test_topological_get_edges_invalid_argument_1,
        &test_topological_get_edges_invalid_argument_2,
        &test_topological_get_edges_invalid_operation,
        &test_topological_get_edges_function_call_1,
        &test_topological_get_edges_function_call_2,
        &test_topological_get_edges,
        &test_topological_count_vertices_invalid_argument,
        &test_topological_count_vertices,
        &test_index_create_invalid_argument,
        &test_index_create_overflow_1,
        &test_index_create_function_call_1,
        &test_index_create_function_call_2,
        &test_index_create_function_call_3,
        &test_index_create_function_call_4,
        &test_index_create_1,
        &test_index_create_2,
        &test_index_create_3,
        &test_index_create_4,
        &test_index_create_5,
        &test_index_create_6,
        &test_big_int_create_function_call_1,
        &test_big_int_create_function_call_2,
        &test_big_int_create_1,
        &test_big_int_create_2,
        &test_big_int_set_invalid_argument,
        &test_big_int_set,
        &test_big_int_add_invalid_argument_1,
        &test_big_int_add_invalid_argument_2,
        &test_big_int_add_invalid_argument_3,
        &test_big_int_add_invalid_operation_1,
        &test_big_int_add_invalid_operation_2,
        &test_big_int_add_invalid_operation_3,
        &test_big_int_add_invalid_operation_4,
        &test_big_int_add_function_call_1,
        &test_big_int_add_function_call_2,
        &test_big_int_add_1,
        &test_big_int_add_2,
        &test_big_int_add_3,
        &test_big_int_add_4,
        &test_big_int_destroy_invalid_argument,
        &test_big_int_mul_invalid_argument_1,
        &test_big_int_mul_invalid_argument_2,
        &test_big_int_mul_invalid_argument_3,
        &test_big_int_mul_invalid_operation_1,
        &test_big_int_mul_invalid_operation_2,
        &test_big_int_mul_function_call_1,
        &test_big_int_mul_function_call_2,
        &test_big_int_mul_function_call_3,
        &test_big_int_mul_1,
        &test_big_int_mul_2,
        &test_big_int_copy_invalid_argument_1,
        &test_big_int_copy_invalid_argument_2,
        &test_big_int_copy_invalid_operation,
        &test_big_int_copy_function_call,
        &test_big_int_copy,
        &test_big_int_equals_string_invalid_argument_1,
        &test_big_int_equals_string_invalid_argument_2,
        &test_big_int_equals_string,
        &test_big_int_to_value_invalid_argument_1,
        &test_big_int_to_value_invalid_argument_2,
        &test_big_int_to_value,
        NULL
};

int main (int argc, char **argv)
{
        return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
