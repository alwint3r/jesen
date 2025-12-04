#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
	#if defined(JESEN_DLL)
		#if defined(JESEN_DLL_EXPORTS)
			#define JESEN_API __declspec(dllexport)
		#else
			#define JESEN_API __declspec(dllimport)
		#endif
	#else
		#define JESEN_API
	#endif
#elif defined(JESEN_DLL) && defined(__GNUC__) && __GNUC__ >= 4
	#define JESEN_API __attribute__((visibility("default")))
#else
	#define JESEN_API
#endif
//clang-format on

/**
 * @brief Ownership-aware wrapper around cJSON for building and querying JSON.
 *
 * All APIs return a `jesen_err_t`; success is `JESEN_ERR_NONE`.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Error code returned by Jesen APIs. */
typedef int32_t jesen_err_t;

/** Success. */
#define JESEN_ERR_NONE 0

/** Base offset for Jesen-specific error values. */
#define JESEN_ERR_BASE 0x10000

/** Unknown failure (catch-all). */
#define JESEN_ERR_UNKNOWN (JESEN_ERR_BASE + 1)

/** Requested key or index was not found. */
#define JESEN_ERR_NOT_FOUND (JESEN_ERR_BASE + 2)

/** Allocation failed. */
#define JESEN_ERR_ALLOC (JESEN_ERR_BASE + 3)

/** Invalid argument passed (e.g., NULL pointer, wrong size). */
#define JESEN_ERR_INVALID_ARGS (JESEN_ERR_BASE + 4)

/** Serialization failed to fit in the provided buffer. */
#define JESEN_ERR_SERIALIZE (JESEN_ERR_BASE + 5)

/** Node holds a value of the wrong type for the requested operation. */
#define JESEN_ERR_INVALID_VALUE_TYPE (JESEN_ERR_BASE + 6)

/** Node is already attached to a parent. */
#define JESEN_ERR_ALREADY_ATTACHED (JESEN_ERR_BASE + 7)

/** Operation applied to a node of the wrong structural type. */
#define JESEN_ERR_WRONG_TYPE (JESEN_ERR_BASE + 8)

/** Index was outside the bounds of an array. */
#define JESEN_ERR_OUT_OF_RANGE (JESEN_ERR_BASE + 9)

/** Provided buffer was too small for the requested operation. */
#define JESEN_ERR_BUFFER_TOO_SMALL (JESEN_ERR_BASE + 10)

/** JSON parsing failed. */
#define JESEN_ERR_PARSE (JESEN_ERR_BASE + 11)

/** Underlying mutation (insert/replace/remove) failed. */
#define JESEN_ERR_MUTATION_FAILED (JESEN_ERR_BASE + 12)

/** Node does not belong to the expected parent. */
#define JESEN_ERR_NOT_OWNED (JESEN_ERR_BASE + 13)

/** Opaque wrapper around a cJSON node with parent/child/sibling links. */
typedef struct jesen_node jesen_node_t;

/**
 * @brief Create a new unattached JSON object.
 * @param[out] out Receives the allocated object wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_create(jesen_node_t **out);

/**
 * @brief Create an object and attach it to a parent object property.
 * @param parent Destination parent object.
 * @param name   Property name to assign.
 * @param[out] out Receives the attached child wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_create_to(jesen_node_t *parent,
                                             const char *name,
                                             jesen_node_t **out);

/**
 * @brief Add a string property to an object.
 * @param node  Target object.
 * @param key   Property name.
 * @param value String data (not required to be null-terminated).
 * @param value_len Number of bytes from `value` to copy.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_add_string(jesen_node_t *node,
                                              const char *key,
                                              const char *value,
                                              size_t value_len);

/**
 * @brief Add a 32-bit integer property to an object.
 * @param node Target object.
 * @param key  Property name.
 * @param value Integer value to store.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_add_int32(jesen_node_t *node,
                                             const char *key, int32_t value);

/**
 * @brief Add a double property to an object.
 * @param node Target object.
 * @param key  Property name.
 * @param value Double value to store.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_add_double(jesen_node_t *node,
                                              const char *key, double value);

/**
 * @brief Add a boolean property to an object.
 * @param node Target object.
 * @param key  Property name.
 * @param value Boolean value to store.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_add_bool(jesen_node_t *node, const char *key,
                                            bool value);

/**
 * @brief Add a null property to an object.
 * @param node Target object.
 * @param key  Property name.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_add_null(jesen_node_t *node,
                                            const char *key);

/**
 * @brief Remove a property from an object and free its subtree.
 * @param node Target object.
 * @param key  Property name.
 * @return JESEN_ERR_NONE on success or JESEN_ERR_NOT_FOUND if missing.
 */
JESEN_API jesen_err_t jesen_object_remove(jesen_node_t *node, const char *key);

/**
 * @brief Create a new unattached JSON array.
 * @param[out] out Receives the allocated array wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_create(jesen_node_t **out);

/**
 * @brief Create an array and attach it to a parent object property.
 * @param parent Destination parent object.
 * @param name   Property name to assign.
 * @param[out] out Receives the attached child wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_create_to(jesen_node_t *parent,
                                            const char *name,
                                            jesen_node_t **out);

/**
 * @brief Get the array element at `index` using the existing wrapper.
 * @param array Source array.
 * @param index Zero-based index.
 * @param[out] out Receives the owned child wrapper (no new allocation).
 * @return JESEN_ERR_NONE on success or an error code (e.g., OUT_OF_RANGE).
 */
JESEN_API jesen_err_t jesen_array_get_value(jesen_node_t *array, uint32_t index,
                                            jesen_node_t **out);

/**
 * @brief Replace the element at `index` with `value`.
 * @param array Destination array.
 * @param index Zero-based index to replace.
 * @param value Node to insert; must be unattached on entry.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_set_value(jesen_node_t *array, uint32_t index,
                                            jesen_node_t *value);

/**
 * @brief Append a double to an array.
 * @param array Destination array.
 * @param value Value to append.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_add_double(jesen_node_t *array, double value);

/**
 * @brief Append a 32-bit integer to an array.
 * @param array Destination array.
 * @param value Value to append.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_add_int32(jesen_node_t *array, int32_t value);

/**
 * @brief Append a boolean to an array.
 * @param array Destination array.
 * @param value Value to append.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_add_bool(jesen_node_t *array, bool value);

/**
 * @brief Append a string to an array.
 * @param array Destination array.
 * @param value String data (not required to be null-terminated).
 * @param value_len Number of bytes from `value` to copy.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_add_string(jesen_node_t *array,
                                             const char *value,
                                             size_t value_len);

/**
 * @brief Remove the element at `index`, freeing its subtree.
 * @param array Target array.
 * @param index Zero-based index to remove.
 * @return JESEN_ERR_NONE on success or JESEN_ERR_OUT_OF_RANGE if invalid.
 */
JESEN_API jesen_err_t jesen_array_remove(jesen_node_t *array, uint32_t index);

/**
 * @brief Typed getter for an int32 array element.
 * @param array Source array.
 * @param index Zero-based index.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_int32(jesen_node_t *array, uint32_t index,
                                            int32_t *out);

/**
 * @brief Typed getter for a double array element.
 * @param array Source array.
 * @param index Zero-based index.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_double(jesen_node_t *array,
                                             uint32_t index, double *out);

/**
 * @brief Typed getter for a boolean array element.
 * @param array Source array.
 * @param index Zero-based index.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_bool(jesen_node_t *array, uint32_t index,
                                           bool *out);

/**
 * @brief Typed getter for a string array element.
 * @param array Source array.
 * @param index Zero-based index.
 * @param[out] out Buffer to receive the string (with terminator).
 * @param out_max Size of `out` in bytes.
 * @param[out] out_len Receives the string length (excluding terminator).
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_string(jesen_node_t *array,
                                             uint32_t index, char *out,
                                             size_t out_max, size_t *out_len);

/**
 * @brief Get an object stored at `index` then return its child named `key`.
 * @param array Source array.
 * @param index Zero-based index.
 * @param key   Object property name.
 * @param[out] out Receives the existing child wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_object_value(jesen_node_t *array,
                                                   uint32_t index,
                                                   const char *key,
                                                   jesen_node_t **out);

/**
 * @brief Typed getter for an int32 field on an object stored in an array.
 * @param array Source array.
 * @param index Zero-based index.
 * @param key   Object property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_object_int32(jesen_node_t *array,
                                                   uint32_t index,
                                                   const char *key,
                                                   int32_t *out);

/**
 * @brief Typed getter for a double field on an object stored in an array.
 * @param array Source array.
 * @param index Zero-based index.
 * @param key   Object property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_object_double(jesen_node_t *array,
                                                    uint32_t index,
                                                    const char *key,
                                                    double *out);

/**
 * @brief Typed getter for a boolean field on an object stored in an array.
 * @param array Source array.
 * @param index Zero-based index.
 * @param key   Object property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_object_bool(jesen_node_t *array,
                                                  uint32_t index,
                                                  const char *key, bool *out);

/**
 * @brief Typed getter for a string field on an object stored in an array.
 * @param array Source array.
 * @param index Zero-based index.
 * @param key   Object property name.
 * @param[out] out Buffer to receive the string (with terminator).
 * @param out_max Size of `out` in bytes.
 * @param[out] out_len Receives the string length (excluding terminator).
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_get_object_string(jesen_node_t *array,
                                                    uint32_t index,
                                                    const char *key, char *out,
                                                    size_t out_max,
                                                    size_t *out_len);

/**
 * @brief Find an immediate child of `node` by key.
 * @param node Object to search.
 * @param key  Property name.
 * @param[out] out Receives the existing child wrapper.
 * @return JESEN_ERR_NONE on success or JESEN_ERR_NOT_FOUND if missing.
 */
JESEN_API jesen_err_t jesen_node_find(const jesen_node_t *node, const char *key,
                                      jesen_node_t **out);

/**
 * @brief Typed getter for an object property expected to be an int32.
 * @param object Source object.
 * @param key    Property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_int32(const jesen_node_t *object,
                                             const char *key, int32_t *out);

/**
 * @brief Typed getter for an object property expected to be a double.
 * @param object Source object.
 * @param key    Property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_double(const jesen_node_t *object,
                                              const char *key, double *out);

/**
 * @brief Typed getter for an object property expected to be a boolean.
 * @param object Source object.
 * @param key    Property name.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_bool(const jesen_node_t *object,
                                            const char *key, bool *out);

/**
 * @brief Typed getter for an object property expected to be a string.
 * @param object Source object.
 * @param key    Property name.
 * @param[out] out Buffer to receive the string (with terminator).
 * @param out_max Size of `out` in bytes.
 * @param[out] out_len Receives the string length (excluding terminator).
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_string(const jesen_node_t *object,
                                              const char *key, char *out,
                                              size_t out_max, size_t *out_len);

/**
 * @brief Get an array stored on an object and return its element at `index`.
 * @param object Source object.
 * @param key    Property name.
 * @param index  Zero-based index into the array.
 * @param[out] out Receives the existing child wrapper.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_array_value(const jesen_node_t *object,
                                                   const char *key,
                                                   uint32_t index,
                                                   jesen_node_t **out);

/**
 * @brief Typed getter for an int32 inside an array stored on an object
 * property.
 * @param object Source object.
 * @param key    Property name.
 * @param index  Zero-based index into the array.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_array_int32(const jesen_node_t *object,
                                                   const char *key,
                                                   uint32_t index,
                                                   int32_t *out);

/**
 * @brief Typed getter for a double inside an array stored on an object
 * property.
 * @param object Source object.
 * @param key    Property name.
 * @param index  Zero-based index into the array.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_array_double(const jesen_node_t *object,
                                                    const char *key,
                                                    uint32_t index,
                                                    double *out);

/**
 * @brief Typed getter for a boolean inside an array stored on an object
 * property.
 * @param object Source object.
 * @param key    Property name.
 * @param index  Zero-based index into the array.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_array_bool(const jesen_node_t *object,
                                                  const char *key,
                                                  uint32_t index, bool *out);

/**
 * @brief Typed getter for a string inside an array stored on an object
 * property.
 * @param object Source object.
 * @param key    Property name.
 * @param index  Zero-based index into the array.
 * @param[out] out Buffer to receive the string (with terminator).
 * @param out_max Size of `out` in bytes.
 * @param[out] out_len Receives the string length (excluding terminator).
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_get_array_string(const jesen_node_t *object,
                                                    const char *key,
                                                    uint32_t index, char *out,
                                                    size_t out_max,
                                                    size_t *out_len);

/**
 * @brief Read a string value from a node.
 * @param node Source node.
 * @param[out] out Buffer to receive the string (with terminator).
 * @param out_max Size of `out` in bytes.
 * @param[out] out_len Receives the string length (excluding terminator).
 * @return JESEN_ERR_NONE on success or JESEN_ERR_INVALID_VALUE_TYPE if not a
 *         string.
 */
JESEN_API jesen_err_t jesen_value_get_string(const jesen_node_t *node,
                                             char *out, size_t out_max,
                                             size_t *out_len);

/**
 * @brief Read a 32-bit integer from a node.
 * @param node Source node.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_get_int32(const jesen_node_t *node,
                                            int32_t *out);

/**
 * @brief Read a double from a node.
 * @param node Source node.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_get_double(const jesen_node_t *node,
                                             double *out);

/**
 * @brief Read a boolean from a node.
 * @param node Source node.
 * @param[out] out Receives the value.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_get_bool(const jesen_node_t *node, bool *out);

/**
 * @brief Test whether a node holds JSON null.
 * @param node Source node.
 * @param[out] out Receives true if null.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_null(const jesen_node_t *node, bool *out);

/**
 * @brief Test whether a node holds a numeric value (usable with int32 getter).
 * @param node Source node.
 * @param[out] out Receives true if numeric.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_int32(const jesen_node_t *node, bool *out);

/**
 * @brief Test whether a node holds a boolean.
 * @param node Source node.
 * @param[out] out Receives true if boolean.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_bool(const jesen_node_t *node, bool *out);

/**
 * @brief Test whether a node holds a numeric value (usable with double getter).
 * @param node Source node.
 * @param[out] out Receives true if numeric.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_double(const jesen_node_t *node,
                                            bool *out);

/**
 * @brief Test whether a node holds a string.
 * @param node Source node.
 * @param[out] out Receives true if string.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_string(const jesen_node_t *node,
                                            bool *out);

/**
 * @brief Test whether a node holds an array.
 * @param node Source node.
 * @param[out] out Receives true if array.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_array(const jesen_node_t *node, bool *out);

/**
 * @brief Test whether a node holds an object.
 * @param node Source node.
 * @param[out] out Receives true if object.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_value_is_object(const jesen_node_t *node,
                                            bool *out);

/**
 * @brief Return the parent of a node.
 * @param node Source node.
 * @param[out] out_parent Receives the parent or NULL for roots.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_node_get_parent(const jesen_node_t *node,
                                            jesen_node_t **out_parent);

/**
 * @brief Detach a node from its parent without freeing it.
 * @param node Node to detach.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_node_detach(jesen_node_t *node);

/**
 * @brief Return the number of elements in an array.
 * @param array Source array.
 * @param[out] out_size Receives the count.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_array_size(const jesen_node_t *array,
                                       size_t *out_size);

/**
 * @brief Return the number of properties in an object.
 * @param object Source object.
 * @param[out] out_size Receives the count.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_object_size(const jesen_node_t *object,
                                        size_t *out_size);

/**
 * @brief Attach an unattached node to a parent.
 * @param parent Destination parent (object or array).
 * @param name   Property name for object parents; ignored for arrays.
 * @param node   Node to attach (must be unattached on entry).
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_node_assign_to(jesen_node_t *parent,
                                           const char *name,
                                           jesen_node_t *node);

/**
 * @brief Serialize a node to a preallocated buffer.
 * @param node Source node.
 * @param[out] out_buf Destination buffer (includes terminator).
 * @param out_buf_len Size of `out_buf` in bytes.
 * @return JESEN_ERR_NONE on success or JESEN_ERR_BUFFER_TOO_SMALL if short.
 */
JESEN_API jesen_err_t jesen_serialize(const jesen_node_t *node, char *out_buf,
                                      size_t out_buf_len);

/**
 * @brief Parse JSON text into a new node tree.
 * @param buf Input buffer.
 * @param buf_len Length of `buf` in bytes.
 * @param[out] out Receives the root node; caller must destroy with
 *                 `jesen_destroy`.
 * @return JESEN_ERR_NONE on success or an error code (e.g., JESEN_ERR_PARSE).
 */
JESEN_API jesen_err_t jesen_parse(const char *buf, size_t buf_len,
                                  jesen_node_t **out);

/**
 * @brief Destroy a node and its subtree.
 * @param node Root or detached node to free.
 * @return JESEN_ERR_NONE on success or an error code.
 */
JESEN_API jesen_err_t jesen_destroy(jesen_node_t *node);

#ifdef __cplusplus
}
#endif
