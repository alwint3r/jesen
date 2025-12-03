#include "jesen.h"
#include <cstdio>
#include <cstring>

int main() {
  jesen_node_t *root = nullptr;
  jesen_err_t err = jesen_object_create(&root);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed creating a root jesen object: %d\n", (int)err);
    return -1;
  }

  err = jesen_object_add_double(root, "pi", 3.14);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed setting double to object: %d\n", (int)err);
    return -1;
  }

  jesen_node_t *params = nullptr;
  err = jesen_object_create_to(root, "params", &params);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed creating another object to root: %d\n", (int)err);
    return -1;
  }

  err = jesen_object_add_double(params, "r", 1.8);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed setting double to a nested object: %d\n", (int)err);
    return -1;
  }

  jesen_node_t *params2 = nullptr;
  err = jesen_array_create_to(root, "params2", &params2);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed creating array: %d\n", (int)err);
    return -1;
  }

  err = jesen_array_add_double(params2, 2.8);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed adding item to array: %d\n", (int)err);
    return -1;
  }

  err = jesen_array_add_int32(params2, 127);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed adding item to array: %d\n", (int)err);
    return -1;
  }

  constexpr size_t kOutBufMax = 512;
  char outBuf[kOutBufMax]{};
  err = jesen_serialize(root, outBuf, kOutBufMax);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed serializing JSON using jesen_serialize: %d\n",
                (int)err);
    return -1;
  }

  std::printf("Serialized: %s\n", outBuf);
  err = jesen_destroy(root);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed destroying node: %d\n", (int)err);
    return -1;
  }

  jesen_node_t *parsed = nullptr;
  err = jesen_parse(outBuf, strlen(outBuf), &parsed);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed parsing: %d\n", (int)err);
    return -1;
  }

  jesen_node_t *params2_out = nullptr;
  err = jesen_object_get_value(parsed, "params2", &params2_out);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed finding: %d\n", (int)err);
    return -1;
  }

  jesen_node_t *params_item = nullptr;
  err = jesen_array_get_value(params2_out, 0, &params_item);
  if (err != JESEN_ERR_NONE) {
    std::printf("Failed getting array value: %d\n", (int)err);
    return -1;
  }

  int32_t param_value = 0;
  err = jesen_value_get_int32(params_item, &param_value);

  if (err != JESEN_ERR_NONE) {
    std::printf("Failed getting int32 value: %d\n", (int)err);
    return -1;
  }

  std::printf("param_value = %d\n", (int)param_value);

  jesen_destroy(parsed);

  return 0;
}
