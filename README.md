# Jesen

Jesen is a thin C wrapper around JSON libraries that adds a small, ownership-aware node layer and convenient APIs for building, querying, serializing, and parsing JSON documents. 

Right now, jesen only wraps [cJSON](https://github.com/DaveGamble/cJSON).

## Features

- Create JSON objects and arrays, add primitive values, and attach subtrees with explicit ownership tracking.
- Parse JSON strings into a wrapper tree; serialize back with `jesen_serialize`.
- Type-checked getters for objects and arrays, including nested convenience helpers (e.g., `jesen_object_get_array_int32`, `jesen_array_get_object_string`).
- Detach/reparent nodes safely with `jesen_node_detach`, and inspect structure with `jesen_array_size` / `jesen_object_size`.
- Error codes distinguish invalid args, type mismatches, ownership issues, parse errors, buffer limits, and mutation failures.

## Building & Testing
The project uses only the C standard library and cJSON (vendored in `cJSON/`). You can build this library with c99 standard at minimum.

Build and run the simple test suite:
```sh
clang -std=c11 -I. tests/test_jesen.c jesen_cjson.c cJSON/cJSON.c -o /tmp/test_jesen
/tmp/test_jesen
```
You should see `All tests passed`.

## Usage Sketch
```c
jesen_node_t *root = NULL;
jesen_object_create(&root);
jesen_object_add_string(root, "name", "alice", 5);
jesen_node_t *nums = NULL;
jesen_array_create_to(root, "nums", &nums);
jesen_array_add_int32(nums, 1);
jesen_array_add_int32(nums, 2);

int32_t second = 0;
jesen_object_get_array_int32(root, "nums", 1, &second); // second == 2

char buf[128];
jesen_serialize(root, buf, sizeof buf); // JSON string in buf
jesen_destroy(root);
```

## Attribution
This project depends on cJSON by Dave Gamble and contributors. The cJSON source is included under its own license in `cJSON/`.
