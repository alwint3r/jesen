#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t jesen_err_t;

#define JESEN_ERR_NONE 0
#define JESEN_ERR_BASE 0x10000
#define JESEN_ERR_UNKNOWN (JESEN_ERR_BASE + 1)
#define JESEN_ERR_NOT_FOUND (JESEN_ERR_BASE + 2)
#define JESEN_ERR_ALLOC (JESEN_ERR_BASE + 3)
#define JESEN_ERR_INVALID_ARGS (JESEN_ERR_BASE + 4)
#define JESEN_ERR_SERIALIZE (JESEN_ERR_BASE + 5)
#define JESEN_ERR_INVALID_VALUE_TYPE (JESEN_ERR_BASE + 6)
#define JESEN_ERR_ALREADY_ATTACHED (JESEN_ERR_BASE + 7)
#define JESEN_ERR_WRONG_TYPE (JESEN_ERR_BASE + 8)
#define JESEN_ERR_OUT_OF_RANGE (JESEN_ERR_BASE + 9)
#define JESEN_ERR_BUFFER_TOO_SMALL (JESEN_ERR_BASE + 10)
#define JESEN_ERR_PARSE (JESEN_ERR_BASE + 11)
#define JESEN_ERR_MUTATION_FAILED (JESEN_ERR_BASE + 12)
#define JESEN_ERR_NOT_OWNED (JESEN_ERR_BASE + 13)

typedef struct jesen_node jesen_node_t;

jesen_err_t jesen_object_create(jesen_node_t **out);
jesen_err_t jesen_object_create_to(jesen_node_t *parent, const char *name,
                                   jesen_node_t **out);

jesen_err_t jesen_object_add_string(jesen_node_t *node, const char *key,
                                    const char *value, size_t value_len);
jesen_err_t jesen_object_add_int32(jesen_node_t *node, const char *key,
                                   int32_t value);
jesen_err_t jesen_object_add_double(jesen_node_t *node, const char *key,
                                    double value);
jesen_err_t jesen_object_add_bool(jesen_node_t *node, const char *key,
                                  bool value);
jesen_err_t jesen_object_add_null(jesen_node_t *node, const char *key);
jesen_err_t jesen_object_remove(jesen_node_t *node, const char *key);

jesen_err_t jesen_array_create(jesen_node_t **out);
jesen_err_t jesen_array_create_to(jesen_node_t *parent, const char *name,
                                  jesen_node_t **out);

jesen_err_t jesen_array_get_value(jesen_node_t *array, uint32_t index,
                                  jesen_node_t **out);

jesen_err_t jesen_array_set_value(jesen_node_t *array, uint32_t index,
                                  jesen_node_t *value);
jesen_err_t jesen_array_add_double(jesen_node_t *array, double value);

jesen_err_t jesen_array_add_int32(jesen_node_t *array, int32_t value);

jesen_err_t jesen_array_add_bool(jesen_node_t *array, bool value);
jesen_err_t jesen_array_add_string(jesen_node_t *array, const char *value,
                                   size_t value_len);
jesen_err_t jesen_array_remove(jesen_node_t *array, uint32_t index);
jesen_err_t jesen_array_get_int32(jesen_node_t *array, uint32_t index,
                                  int32_t *out);
jesen_err_t jesen_array_get_double(jesen_node_t *array, uint32_t index,
                                   double *out);
jesen_err_t jesen_array_get_bool(jesen_node_t *array, uint32_t index,
                                 bool *out);
jesen_err_t jesen_array_get_string(jesen_node_t *array, uint32_t index,
                                   char *out, size_t out_max, size_t *out_len);
jesen_err_t jesen_array_get_object_value(jesen_node_t *array, uint32_t index,
                                         const char *key, jesen_node_t **out);
jesen_err_t jesen_array_get_object_int32(jesen_node_t *array, uint32_t index,
                                         const char *key, int32_t *out);
jesen_err_t jesen_array_get_object_double(jesen_node_t *array, uint32_t index,
                                          const char *key, double *out);
jesen_err_t jesen_array_get_object_bool(jesen_node_t *array, uint32_t index,
                                        const char *key, bool *out);
jesen_err_t jesen_array_get_object_string(jesen_node_t *array, uint32_t index,
                                          const char *key, char *out,
                                          size_t out_max, size_t *out_len);

jesen_err_t jesen_node_find(const jesen_node_t *node, const char *key,
                            jesen_node_t **out);
jesen_err_t jesen_object_get_int32(const jesen_node_t *object, const char *key,
                                   int32_t *out);
jesen_err_t jesen_object_get_double(const jesen_node_t *object, const char *key,
                                    double *out);
jesen_err_t jesen_object_get_bool(const jesen_node_t *object, const char *key,
                                  bool *out);
jesen_err_t jesen_object_get_string(const jesen_node_t *object, const char *key,
                                    char *out, size_t out_max, size_t *out_len);
jesen_err_t jesen_object_get_array_value(const jesen_node_t *object,
                                         const char *key, uint32_t index,
                                         jesen_node_t **out);
jesen_err_t jesen_object_get_array_int32(const jesen_node_t *object,
                                         const char *key, uint32_t index,
                                         int32_t *out);
jesen_err_t jesen_object_get_array_double(const jesen_node_t *object,
                                          const char *key, uint32_t index,
                                          double *out);
jesen_err_t jesen_object_get_array_bool(const jesen_node_t *object,
                                        const char *key, uint32_t index,
                                        bool *out);
jesen_err_t jesen_object_get_array_string(const jesen_node_t *object,
                                          const char *key, uint32_t index,
                                          char *out, size_t out_max,
                                          size_t *out_len);

jesen_err_t jesen_value_get_string(const jesen_node_t *node, char *out,
                                   size_t out_max, size_t *out_len);
jesen_err_t jesen_value_get_int32(const jesen_node_t *node, int32_t *out);
jesen_err_t jesen_value_get_double(const jesen_node_t *node, double *out);
jesen_err_t jesen_value_get_bool(const jesen_node_t *node, bool *out);

jesen_err_t jesen_value_is_null(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_int32(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_bool(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_double(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_string(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_array(const jesen_node_t *node, bool *out);
jesen_err_t jesen_value_is_object(const jesen_node_t *node, bool *out);

jesen_err_t jesen_node_get_parent(const jesen_node_t *node,
                                  jesen_node_t **out_parent);
jesen_err_t jesen_node_detach(jesen_node_t *node);
jesen_err_t jesen_array_size(const jesen_node_t *array, size_t *out_size);
jesen_err_t jesen_object_size(const jesen_node_t *object, size_t *out_size);

jesen_err_t jesen_node_assign_to(jesen_node_t *parent, const char *name,
                                 jesen_node_t *node);
jesen_err_t jesen_serialize(const jesen_node_t *node, char *out_buf,
                            size_t out_buf_len);
jesen_err_t jesen_parse(const char *buf, size_t buf_len, jesen_node_t **out);
jesen_err_t jesen_destroy(jesen_node_t *node);

#ifdef __cplusplus
}
#endif
