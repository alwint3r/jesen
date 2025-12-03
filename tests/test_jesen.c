#include "jesen.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define EXPECT_OK(expr) assert((expr) == JESEN_ERR_NONE)

static void test_object_ops(void) {
  jesen_node_t *root = NULL;
  EXPECT_OK(jesen_object_create(&root));

  EXPECT_OK(jesen_object_add_string(root, "s", "hello", 5));
  EXPECT_OK(jesen_object_add_int32(root, "i", 42));
  EXPECT_OK(jesen_object_add_double(root, "d", 3.14));
  EXPECT_OK(jesen_object_add_bool(root, "b", true));
  EXPECT_OK(jesen_object_add_null(root, "n"));

  size_t obj_size = 0;
  EXPECT_OK(jesen_object_size(root, &obj_size));
  assert(obj_size == 5);

  jesen_node_t *str_node = NULL;
  EXPECT_OK(jesen_node_find(root, "s", &str_node));
  bool is_string = false;
  EXPECT_OK(jesen_value_is_string(str_node, &is_string));
  assert(is_string);
  char buf[16];
  size_t out_len = 0;
  EXPECT_OK(jesen_value_get_string(str_node, buf, sizeof buf, &out_len));
  assert(out_len == 5);
  assert(strcmp(buf, "hello") == 0);

  jesen_node_t *int_node = NULL;
  EXPECT_OK(jesen_node_find(root, "i", &int_node));
  bool is_num = false;
  EXPECT_OK(jesen_value_is_int32(int_node, &is_num));
  assert(is_num);
  int32_t int_val = 0;
  EXPECT_OK(jesen_value_get_int32(int_node, &int_val));
  assert(int_val == 42);

  jesen_node_t *bool_node = NULL;
  EXPECT_OK(jesen_node_find(root, "b", &bool_node));
  bool is_bool = false;
  EXPECT_OK(jesen_value_is_bool(bool_node, &is_bool));
  assert(is_bool);
  bool bool_val = false;
  EXPECT_OK(jesen_value_get_bool(bool_node, &bool_val));
  assert(bool_val == true);

  jesen_node_t *null_node = NULL;
  EXPECT_OK(jesen_node_find(root, "n", &null_node));
  bool is_null = false;
  EXPECT_OK(jesen_value_is_null(null_node, &is_null));
  assert(is_null);

  EXPECT_OK(jesen_destroy(root));
}

static void test_array_ops(void) {
  jesen_node_t *arr = NULL;
  EXPECT_OK(jesen_array_create(&arr));

  EXPECT_OK(jesen_array_add_double(arr, 1.5));
  EXPECT_OK(jesen_array_add_int32(arr, 7));
  EXPECT_OK(jesen_array_add_bool(arr, false));
  EXPECT_OK(jesen_array_add_string(arr, "abc", 3));

  size_t size = 0;
  EXPECT_OK(jesen_array_size(arr, &size));
  assert(size == 4);

  jesen_node_t *val = NULL;
  EXPECT_OK(jesen_array_get_value(arr, 0, &val));
  bool is_num = false;
  EXPECT_OK(jesen_value_is_double(val, &is_num));
  assert(is_num);

  EXPECT_OK(jesen_array_get_value(arr, 2, &val));
  bool is_bool = false;
  EXPECT_OK(jesen_value_is_bool(val, &is_bool));
  assert(is_bool);
  bool bool_val = true;
  EXPECT_OK(jesen_value_get_bool(val, &bool_val));
  assert(bool_val == false);

  EXPECT_OK(jesen_array_get_value(arr, 3, &val));
  bool is_str = false;
  EXPECT_OK(jesen_value_is_string(val, &is_str));
  assert(is_str);
  char buf[8];
  size_t out_len = 0;
  EXPECT_OK(jesen_value_get_string(val, buf, sizeof buf, &out_len));
  assert(out_len == 3);
  assert(strcmp(buf, "abc") == 0);

  EXPECT_OK(jesen_array_remove(arr, 1));
  EXPECT_OK(jesen_array_size(arr, &size));
  assert(size == 3);

  EXPECT_OK(jesen_destroy(arr));
}

static void test_assign_and_detach(void) {
  jesen_node_t *parent1 = NULL;
  jesen_node_t *parent2 = NULL;
  jesen_node_t *child = NULL;
  EXPECT_OK(jesen_object_create(&parent1));
  EXPECT_OK(jesen_object_create(&parent2));
  EXPECT_OK(jesen_object_create(&child));

  EXPECT_OK(jesen_node_assign_to(parent1, "child", child));
  assert(jesen_node_assign_to(parent1, "again", child) == JESEN_ERR_ALREADY_ATTACHED);

  EXPECT_OK(jesen_node_detach(child));
  EXPECT_OK(jesen_node_assign_to(parent2, "moved", child));

  jesen_node_t *parent_lookup = NULL;
  EXPECT_OK(jesen_node_get_parent(child, &parent_lookup));
  assert(parent_lookup == parent2);

  EXPECT_OK(jesen_destroy(parent1));
  EXPECT_OK(jesen_destroy(parent2));
}

static void test_parse_wrapper(void) {
  const char *json = "{\"a\":1,\"b\":[true]}";
  jesen_node_t *root = NULL;
  EXPECT_OK(jesen_parse(json, strlen(json), &root));

  jesen_node_t *a = NULL;
  EXPECT_OK(jesen_node_find(root, "a", &a));
  int32_t intval = 0;
  EXPECT_OK(jesen_value_get_int32(a, &intval));
  assert(intval == 1);

  jesen_node_t *b = NULL;
  EXPECT_OK(jesen_node_find(root, "b", &b));
  bool is_array = false;
  EXPECT_OK(jesen_value_is_array(b, &is_array));
  assert(is_array);

  size_t arr_size = 0;
  EXPECT_OK(jesen_array_size(b, &arr_size));
  assert(arr_size == 1);

  jesen_node_t *elem0 = NULL;
  EXPECT_OK(jesen_array_get_value(b, 0, &elem0));
  bool val = false;
  EXPECT_OK(jesen_value_get_bool(elem0, &val));
  assert(val == true);

  jesen_node_t *elem_parent = NULL;
  EXPECT_OK(jesen_node_get_parent(elem0, &elem_parent));
  assert(elem_parent == b);

  EXPECT_OK(jesen_destroy(root));
}

int main(void) {
  test_object_ops();
  test_array_ops();
  test_assign_and_detach();
  test_parse_wrapper();
  printf("All tests passed\n");
  return 0;
}
