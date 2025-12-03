#include "cJSON/cJSON.h"
#include "jesen.h"
#include <stdlib.h>
#include <string.h>

struct jesen_node {
  cJSON *cjson;
  jesen_node_t *parent;
  jesen_node_t *sibling;
  jesen_node_t *child;
};

static jesen_err_t jesen_free(jesen_node_t *node);
static jesen_err_t jesen_build_wrapper_tree(cJSON *json, jesen_node_t *parent,
                                            jesen_node_t **out);
static jesen_node_t *jesen_find_child_wrapper(jesen_node_t *parent,
                                              cJSON *child_cjson,
                                              jesen_node_t **out_prev);

// { } -> is the root
// { "a": { } } -> "a" is the child of root.
// { "a": { }, "b": { } } -> "b" is also the child of root.
// How to clean up when the root is cleaned up?
// when "a" inserted into root, it became the child of root.
// when "b" inserted into root, it became the child of root, and the sibling of
// "a".

// to generalize this.
// at first, a root has no child. a root may not have a sibling.
// when a child is added to root, it is assigned to root->child. n-1th child.
// when a nth child is added to root, the n-1th child is assigned tn nth child
// 's sibling and nth child is assnged to root->child.

jesen_err_t jesen_object_create(jesen_node_t **out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_CreateObject();
  created->parent = NULL;
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  *out = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_create_to(jesen_node_t *parent, const char *name,
                                   jesen_node_t **out) {
  if (!out || !parent) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_AddObjectToObject(parent->cjson, name);
  created->parent = parent;
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->sibling = parent->child;
  created->child = NULL;
  parent->child = created;

  *out = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_node_assign_to(jesen_node_t *parent, const char *name,
                                 jesen_node_t *node) {
  if (!parent || !name || !node) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!node->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (node->parent) {
    return JESEN_ERR_ALREADY_ATTACHED;
  }

  if (cJSON_IsArray(parent->cjson)) {
    if (!cJSON_AddItemToArray(parent->cjson, node->cjson)) {
      return JESEN_ERR_MUTATION_FAILED;
    }
  } else if (cJSON_IsObject(parent->cjson)) {
    if (!cJSON_AddItemToObject(parent->cjson, name, node->cjson)) {
      return JESEN_ERR_MUTATION_FAILED;
    }
  } else {
    return JESEN_ERR_WRONG_TYPE;
  }

  node->sibling = parent->child;
  node->parent = parent;
  parent->child = node;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_create(jesen_node_t **out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_CreateArray();
  created->parent = NULL;
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  *out = created;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_create_to(jesen_node_t *parent, const char *name,
                                  jesen_node_t **out) {
  if (!parent || !name || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_AddArrayToObject(parent->cjson, name);
  created->parent = parent;
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->sibling = parent->child;
  created->child = NULL;
  parent->child = created;

  *out = created;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_add_double(jesen_node_t *node, const char *key,
                                    double value) {
  if (!node || !key) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson =
      cJSON_AddNumberToObject(node->cjson, key, (const double)value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->child = NULL;
  created->parent = node;
  created->sibling = node->child;
  node->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_add_int32(jesen_node_t *node, const char *key,
                                   int32_t value) {
  if (!node || !key) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_AddNumberToObject(node->cjson, key, (double)value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->child = NULL;
  created->parent = node;
  created->sibling = node->child;
  node->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_add_bool(jesen_node_t *node, const char *key,
                                  bool value) {
  if (!node || !key) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_AddBoolToObject(node->cjson, key, value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->child = NULL;
  created->parent = node;
  created->sibling = node->child;
  node->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_add_null(jesen_node_t *node, const char *key) {
  if (!node || !key) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_AddNullToObject(node->cjson, key);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  created->child = NULL;
  created->parent = node;
  created->sibling = node->child;
  node->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_add_string(jesen_node_t *node, const char *key,
                                    const char *value, size_t value_len) {
  if (!node || !key || !value) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  char *tmp = (char *)calloc(value_len + 1, sizeof(char));
  if (!tmp) {
    return JESEN_ERR_ALLOC;
  }
  memcpy(tmp, value, value_len);
  tmp[value_len] = '\0';

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    free(tmp);
    return JESEN_ERR_ALLOC;
  }

  cJSON *string_json = cJSON_CreateString(tmp);
  free(tmp);
  if (!string_json) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  if (!cJSON_AddItemToObject(node->cjson, key, string_json)) {
    cJSON_Delete(string_json);
    free(created);
    return JESEN_ERR_MUTATION_FAILED;
  }

  created->cjson = string_json;
  created->child = NULL;
  created->parent = node;
  created->sibling = node->child;
  node->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_remove(jesen_node_t *node, const char *key) {
  if (!node || !key) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  cJSON *target = cJSON_GetObjectItem(node->cjson, key);
  if (!target) {
    return JESEN_ERR_NOT_FOUND;
  }

  jesen_node_t *prev = NULL;
  jesen_node_t *wrapped = jesen_find_child_wrapper(node, target, &prev);
  if (!wrapped) {
    return JESEN_ERR_NOT_OWNED;
  }

  if (prev) {
    prev->sibling = wrapped->sibling;
  } else {
    node->child = wrapped->sibling;
  }

  cJSON_DeleteItemFromObject(node->cjson, key);
  wrapped->parent = NULL;
  wrapped->sibling = NULL;
  jesen_free(wrapped);

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_add_double(jesen_node_t *array, double value) {
  if (!array || !array->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_CreateNumber(value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  if (!cJSON_AddItemToArray(array->cjson, created->cjson)) {
    cJSON_Delete(created->cjson);
    free(created);
    return JESEN_ERR_MUTATION_FAILED;
  }

  created->child = NULL;
  created->parent = array;
  created->sibling = array->child;
  array->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_add_int32(jesen_node_t *array, int32_t value) {
  if (!array || !array->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_CreateNumber(value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  if (!cJSON_AddItemToArray(array->cjson, created->cjson)) {
    cJSON_Delete(created->cjson);
    free(created);
    return JESEN_ERR_MUTATION_FAILED;
  }

  created->child = NULL;
  created->parent = array;
  created->sibling = array->child;
  array->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_add_bool(jesen_node_t *array, bool value) {
  if (!array || !array->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    return JESEN_ERR_ALLOC;
  }

  created->cjson = cJSON_CreateBool(value);
  if (!created->cjson) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  if (!cJSON_AddItemToArray(array->cjson, created->cjson)) {
    cJSON_Delete(created->cjson);
    free(created);
    return JESEN_ERR_MUTATION_FAILED;
  }

  created->child = NULL;
  created->parent = array;
  created->sibling = array->child;
  array->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_add_string(jesen_node_t *array, const char *value,
                                   size_t value_len) {
  if (!array || !array->cjson || !value) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  char *tmp = (char *)calloc(value_len + 1, sizeof(char));
  if (!tmp) {
    return JESEN_ERR_ALLOC;
  }
  memcpy(tmp, value, value_len);
  tmp[value_len] = '\0';

  jesen_node_t *created = (jesen_node_t *)calloc(1, sizeof *created);
  if (!created) {
    free(tmp);
    return JESEN_ERR_ALLOC;
  }

  cJSON *str_item = cJSON_CreateString(tmp);
  free(tmp);
  if (!str_item) {
    free(created);
    return JESEN_ERR_ALLOC;
  }

  if (!cJSON_AddItemToArray(array->cjson, str_item)) {
    cJSON_Delete(str_item);
    free(created);
    return JESEN_ERR_MUTATION_FAILED;
  }

  created->cjson = str_item;
  created->child = NULL;
  created->parent = array;
  created->sibling = array->child;
  array->child = created;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_get_value(jesen_node_t *array, uint32_t index,
                                  jesen_node_t **out) {
  if (!array || !array->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  cJSON *item = cJSON_GetArrayItem(array->cjson, (int)index);
  if (!item) {
    return JESEN_ERR_OUT_OF_RANGE;
  }

  jesen_node_t *wrapped = jesen_find_child_wrapper(array, item, NULL);
  if (!wrapped) {
    return JESEN_ERR_NOT_OWNED;
  }

  *out = wrapped;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_set_value(jesen_node_t *array, uint32_t index,
                                  jesen_node_t *value) {
  if (!array || !array->cjson || !value || !value->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (value->parent) {
    return JESEN_ERR_ALREADY_ATTACHED;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  cJSON *existing = cJSON_GetArrayItem(array->cjson, (int)index);
  if (!existing) {
    return JESEN_ERR_OUT_OF_RANGE;
  }

  jesen_node_t *prev = NULL;
  jesen_node_t *wrapped_existing =
      jesen_find_child_wrapper(array, existing, &prev);
  if (!wrapped_existing) {
    return JESEN_ERR_NOT_OWNED;
  }

  if (!cJSON_ReplaceItemInArray(array->cjson, (int)index, value->cjson)) {
    return JESEN_ERR_MUTATION_FAILED;
  }

  if (prev) {
    prev->sibling = wrapped_existing->sibling;
  } else {
    array->child = wrapped_existing->sibling;
  }

  wrapped_existing->parent = NULL;
  wrapped_existing->sibling = NULL;
  jesen_free(wrapped_existing);

  value->parent = array;
  value->sibling = array->child;
  array->child = value;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_remove(jesen_node_t *array, uint32_t index) {
  if (!array || !array->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  cJSON *item = cJSON_GetArrayItem(array->cjson, (int)index);
  if (!item) {
    return JESEN_ERR_OUT_OF_RANGE;
  }

  jesen_node_t *prev = NULL;
  jesen_node_t *wrapped = jesen_find_child_wrapper(array, item, &prev);
  if (!wrapped) {
    return JESEN_ERR_NOT_OWNED;
  }

  if (prev) {
    prev->sibling = wrapped->sibling;
  } else {
    array->child = wrapped->sibling;
  }

  cJSON_DeleteItemFromArray(array->cjson, (int)index);
  wrapped->parent = NULL;
  wrapped->sibling = NULL;
  jesen_free(wrapped);

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_get_int32(jesen_node_t *array, uint32_t index,
                                  int32_t *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_array_get_value(array, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_int32(node, out);
}

jesen_err_t jesen_array_get_double(jesen_node_t *array, uint32_t index,
                                   double *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_array_get_value(array, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_double(node, out);
}

jesen_err_t jesen_array_get_bool(jesen_node_t *array, uint32_t index,
                                 bool *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_array_get_value(array, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_bool(node, out);
}

jesen_err_t jesen_array_get_string(jesen_node_t *array, uint32_t index,
                                   char *out, size_t out_max, size_t *out_len) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_array_get_value(array, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_string(node, out, out_max, out_len);
}

jesen_err_t jesen_array_get_object_value(jesen_node_t *array, uint32_t index,
                                         const char *key, jesen_node_t **out) {
  if (!array || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *elem = NULL;
  jesen_err_t err = jesen_array_get_value(array, index, &elem);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  if (!cJSON_IsObject(elem->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }
  return jesen_node_find(elem, key, out);
}

jesen_err_t jesen_array_get_object_int32(jesen_node_t *array, uint32_t index,
                                         const char *key, int32_t *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_array_get_object_value(array, index, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_int32(child, out);
}

jesen_err_t jesen_array_get_object_double(jesen_node_t *array, uint32_t index,
                                          const char *key, double *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_array_get_object_value(array, index, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_double(child, out);
}

jesen_err_t jesen_array_get_object_bool(jesen_node_t *array, uint32_t index,
                                        const char *key, bool *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_array_get_object_value(array, index, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_bool(child, out);
}

jesen_err_t jesen_array_get_object_string(jesen_node_t *array, uint32_t index,
                                          const char *key, char *out,
                                          size_t out_max, size_t *out_len) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_array_get_object_value(array, index, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_string(child, out, out_max, out_len);
}

jesen_err_t jesen_node_find(const jesen_node_t *node, const char *key,
                            jesen_node_t **out) {
  if (!node || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(node->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  for (jesen_node_t *child = node->child; child; child = child->sibling) {
    const char *child_key = child->cjson ? child->cjson->string : NULL;
    if (child_key && strcmp(child_key, key) == 0) {
      *out = child;
      return JESEN_ERR_NONE;
    }
  }

  return JESEN_ERR_NOT_FOUND;
}

jesen_err_t jesen_object_get_int32(const jesen_node_t *object, const char *key,
                                   int32_t *out) {
  if (!object || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_node_find(object, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_int32(child, out);
}

jesen_err_t jesen_object_get_double(const jesen_node_t *object, const char *key,
                                    double *out) {
  if (!object || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_node_find(object, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_double(child, out);
}

jesen_err_t jesen_object_get_bool(const jesen_node_t *object, const char *key,
                                  bool *out) {
  if (!object || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_node_find(object, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_bool(child, out);
}

jesen_err_t jesen_object_get_string(const jesen_node_t *object, const char *key,
                                    char *out, size_t out_max,
                                    size_t *out_len) {
  if (!object || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_node_find(object, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_string(child, out, out_max, out_len);
}

jesen_err_t jesen_object_get_array_value(const jesen_node_t *object,
                                         const char *key, uint32_t index,
                                         jesen_node_t **out) {
  if (!object || !key || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *child = NULL;
  jesen_err_t err = jesen_node_find(object, key, &child);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  if (!cJSON_IsArray(child->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }
  return jesen_array_get_value(child, index, out);
}

jesen_err_t jesen_object_get_array_int32(const jesen_node_t *object,
                                         const char *key, uint32_t index,
                                         int32_t *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_object_get_array_value(object, key, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_int32(node, out);
}

jesen_err_t jesen_object_get_array_double(const jesen_node_t *object,
                                          const char *key, uint32_t index,
                                          double *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_object_get_array_value(object, key, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_double(node, out);
}

jesen_err_t jesen_object_get_array_bool(const jesen_node_t *object,
                                        const char *key, uint32_t index,
                                        bool *out) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_object_get_array_value(object, key, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_bool(node, out);
}

jesen_err_t jesen_object_get_array_string(const jesen_node_t *object,
                                          const char *key, uint32_t index,
                                          char *out, size_t out_max,
                                          size_t *out_len) {
  if (!out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  jesen_node_t *node = NULL;
  jesen_err_t err = jesen_object_get_array_value(object, key, index, &node);
  if (err != JESEN_ERR_NONE) {
    return err;
  }
  return jesen_value_get_string(node, out, out_max, out_len);
}

jesen_err_t jesen_value_get_string(const jesen_node_t *node, char *out,
                                   size_t out_max, size_t *out_len) {
  if (!node || !node->cjson || !out || out_max == 0) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsString(node->cjson)) {
    return JESEN_ERR_INVALID_VALUE_TYPE;
  }

  const char *str = node->cjson->valuestring ? node->cjson->valuestring : "";
  size_t len = strlen(str);
  if (len + 1 > out_max) {
    return JESEN_ERR_INVALID_ARGS;
  }
  memcpy(out, str, len + 1);
  if (out_len) {
    *out_len = len;
  }
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_get_int32(const jesen_node_t *node, int32_t *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsNumber(node->cjson)) {
    return JESEN_ERR_INVALID_VALUE_TYPE;
  }

  *out = (int32_t)node->cjson->valueint;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_get_double(const jesen_node_t *node, double *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsNumber(node->cjson)) {
    return JESEN_ERR_INVALID_VALUE_TYPE;
  }

  *out = node->cjson->valuedouble;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_get_bool(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsBool(node->cjson)) {
    return JESEN_ERR_INVALID_VALUE_TYPE;
  }

  *out = cJSON_IsTrue(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_null(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsNull(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_int32(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsNumber(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_bool(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsBool(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_double(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsNumber(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_string(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsString(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_array(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsArray(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_value_is_object(const jesen_node_t *node, bool *out) {
  if (!node || !node->cjson || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }
  *out = cJSON_IsObject(node->cjson);
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_node_get_parent(const jesen_node_t *node,
                                  jesen_node_t **out_parent) {
  if (!node || !out_parent) {
    return JESEN_ERR_INVALID_ARGS;
  }

  *out_parent = node->parent;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_node_detach(jesen_node_t *node) {
  if (!node || !node->parent || !node->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *parent = node->parent;
  jesen_node_t *prev = NULL;
  jesen_node_t *wrapped = jesen_find_child_wrapper(parent, node->cjson, &prev);
  if (!wrapped) {
    return JESEN_ERR_NOT_FOUND;
  }

  if (cJSON_IsArray(parent->cjson)) {
    int idx = 0;
    for (cJSON *cur = parent->cjson->child; cur; cur = cur->next, ++idx) {
      if (cur == node->cjson) {
        cJSON_DetachItemFromArray(parent->cjson, idx);
        break;
      }
    }
  } else if (cJSON_IsObject(parent->cjson)) {
    const char *key = node->cjson->string;
    if (!key) {
      return JESEN_ERR_INVALID_ARGS;
    }
    cJSON_DetachItemFromObject(parent->cjson, key);
  } else {
    return JESEN_ERR_WRONG_TYPE;
  }

  if (prev) {
    prev->sibling = wrapped->sibling;
  } else {
    parent->child = wrapped->sibling;
  }

  wrapped->parent = NULL;
  wrapped->sibling = NULL;

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_array_size(const jesen_node_t *array, size_t *out_size) {
  if (!array || !array->cjson || !out_size) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsArray(array->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  int size = cJSON_GetArraySize(array->cjson);
  if (size < 0) {
    return JESEN_ERR_MUTATION_FAILED;
  }
  *out_size = (size_t)size;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_object_size(const jesen_node_t *object, size_t *out_size) {
  if (!object || !object->cjson || !out_size) {
    return JESEN_ERR_INVALID_ARGS;
  }

  if (!cJSON_IsObject(object->cjson)) {
    return JESEN_ERR_WRONG_TYPE;
  }

  int size = cJSON_GetArraySize(object->cjson);
  if (size < 0) {
    return JESEN_ERR_MUTATION_FAILED;
  }
  *out_size = (size_t)size;
  return JESEN_ERR_NONE;
}

jesen_err_t jesen_serialize(const jesen_node_t *node, char *out_buf,
                            size_t out_buf_len) {
  if (!cJSON_PrintPreallocated(node->cjson, out_buf, (int)out_buf_len, false)) {
    return JESEN_ERR_BUFFER_TOO_SMALL;
  }

  return JESEN_ERR_NONE;
}

static jesen_err_t jesen_build_wrapper_tree(cJSON *json, jesen_node_t *parent,
                                            jesen_node_t **out) {
  if (!json || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  jesen_node_t *node = (jesen_node_t *)calloc(1, sizeof *node);
  if (!node) {
    return JESEN_ERR_ALLOC;
  }

  node->cjson = json;
  node->parent = parent;
  node->child = NULL;
  node->sibling = NULL;

  jesen_node_t *child_head = NULL;
  for (cJSON *child = json->child; child; child = child->next) {
    jesen_node_t *wrapped_child = NULL;
    jesen_err_t err = jesen_build_wrapper_tree(child, node, &wrapped_child);
    if (err != JESEN_ERR_NONE) {
      node->child = child_head;
      jesen_free(node);
      return err;
    }

    wrapped_child->sibling = child_head;
    child_head = wrapped_child;
  }

  node->child = child_head;
  *out = node;

  return JESEN_ERR_NONE;
}

static jesen_node_t *jesen_find_child_wrapper(jesen_node_t *parent,
                                              cJSON *child_cjson,
                                              jesen_node_t **out_prev) {
  jesen_node_t *prev = NULL;
  for (jesen_node_t *cur = parent ? parent->child : NULL; cur;
       prev = cur, cur = cur->sibling) {
    if (cur->cjson == child_cjson) {
      if (out_prev) {
        *out_prev = prev;
      }
      return cur;
    }
  }
  if (out_prev) {
    *out_prev = NULL;
  }
  return NULL;
}

jesen_err_t jesen_parse(const char *buf, size_t buf_len, jesen_node_t **out) {
  if (!buf || !out) {
    return JESEN_ERR_INVALID_ARGS;
  }

  cJSON *json = cJSON_ParseWithLength(buf, buf_len);
  if (!json) {
    return JESEN_ERR_PARSE;
  }

  jesen_node_t *root = NULL;
  jesen_err_t err = jesen_build_wrapper_tree(json, NULL, &root);
  if (err != JESEN_ERR_NONE) {
    cJSON_Delete(json);
    return err;
  }

  *out = root;

  return JESEN_ERR_NONE;
}

static jesen_err_t jesen_free(jesen_node_t *node) {
  if (!node) {
    return JESEN_ERR_NONE;
  }

  jesen_free(node->child);
  jesen_free(node->sibling);

  free(node);

  return JESEN_ERR_NONE;
}

jesen_err_t jesen_destroy(jesen_node_t *node) {
  if (!node || !node->cjson) {
    return JESEN_ERR_INVALID_ARGS;
  }

  cJSON_Delete(node->cjson);
  return jesen_free(node);
}
