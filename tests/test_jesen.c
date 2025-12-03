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

  char buf[16];
  size_t out_len = 0;
  EXPECT_OK(jesen_object_get_string(root, "s", buf, sizeof buf, &out_len));
  assert(out_len == 5);
  assert(strcmp(buf, "hello") == 0);

  int32_t int_val = 0;
  EXPECT_OK(jesen_object_get_int32(root, "i", &int_val));
  assert(int_val == 42);

  double dbl_val = 0;
  EXPECT_OK(jesen_object_get_double(root, "d", &dbl_val));
  assert(dbl_val == 3.14);

  bool bool_val = false;
  EXPECT_OK(jesen_object_get_bool(root, "b", &bool_val));
  assert(bool_val == true);

  jesen_node_t *null_node = NULL;
  EXPECT_OK(jesen_node_find(root, "n", &null_node));
  bool is_null = false;
  EXPECT_OK(jesen_value_is_null(null_node, &is_null));

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

  double dbl_out = 0;
  EXPECT_OK(jesen_array_get_double(arr, 0, &dbl_out));
  assert(dbl_out == 1.5);

  int32_t int_out = 0;
  EXPECT_OK(jesen_array_get_int32(arr, 1, &int_out));
  assert(int_out == 7);

  bool bool_val = true;
  EXPECT_OK(jesen_array_get_bool(arr, 2, &bool_val));
  assert(bool_val == false);

  char buf[8];
  size_t out_len = 0;
  EXPECT_OK(jesen_array_get_string(arr, 3, buf, sizeof buf, &out_len));
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
  assert(jesen_node_assign_to(parent1, "again", child) ==
         JESEN_ERR_ALREADY_ATTACHED);

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

  int32_t intval = 0;
  EXPECT_OK(jesen_object_get_int32(root, "a", &intval));
  assert(intval == 1);

  bool val = false;
  EXPECT_OK(jesen_object_get_array_bool(root, "b", 0, &val));
  assert(val == true);

  EXPECT_OK(jesen_destroy(root));
}

static void test_nested_getters(void) {
  jesen_node_t *root = NULL;
  EXPECT_OK(jesen_object_create(&root));

  jesen_node_t *nums = NULL;
  EXPECT_OK(jesen_array_create_to(root, "nums", &nums));
  EXPECT_OK(jesen_array_add_int32(nums, 10));
  EXPECT_OK(jesen_array_add_int32(nums, 20));

  int32_t second = 0;
  EXPECT_OK(jesen_object_get_array_int32(root, "nums", 1, &second));
  assert(second == 20);

  jesen_node_t *arr_obj = NULL;
  EXPECT_OK(jesen_array_create(&arr_obj));
  jesen_node_t *inner_obj = NULL;
  EXPECT_OK(jesen_object_create(&inner_obj));
  EXPECT_OK(jesen_object_add_int32(inner_obj, "x", 99));
  EXPECT_OK(jesen_node_assign_to(arr_obj, "ignored", inner_obj));

  int32_t xval = 0;
  EXPECT_OK(jesen_array_get_object_int32(arr_obj, 0, "x", &xval));
  assert(xval == 99);

  EXPECT_OK(jesen_destroy(root));
  EXPECT_OK(jesen_destroy(arr_obj));
}

int main(void) {
  test_object_ops();
  test_array_ops();
  test_assign_and_detach();
  test_parse_wrapper();
  test_nested_getters();
  printf("All tests passed\n");
  return 0;
}
