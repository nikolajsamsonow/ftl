#ifndef YET_ANOTHER_MUTATOR_LIBRARY_H
#define YET_ANOTHER_MUTATOR_LIBRARY_H

#include <cstddef>
#include <cstdint>

struct afl_t;

extern "C" {
__attribute__((visibility("default"))) void *afl_custom_init(afl_t *afl, unsigned int seed);
__attribute__((visibility("default"))) size_t afl_custom_fuzz(void *data, uint8_t *buf, size_t buf_size, uint8_t **out_buf, uint8_t *add_buf, size_t add_buf_size, size_t max_size);
//size_t afl_custom_post_process(void *data, uint8_t *buf, size_t buf_size, uint8_t **out_buf);
//int32_t afl_custom_init_trim(void *data, uint8_t *buf, size_t buf_size);
//size_t afl_custom_trim(void *data, uint8_t **out_buf);
//int32_t afl_custom_post_trim(void *data, int success);
__attribute__((visibility("default"))) size_t afl_custom_havoc_mutation(void *data, uint8_t *buf, size_t buf_size, uint8_t **out_buf, size_t max_size);
__attribute__((visibility("default"))) uint8_t afl_custom_havoc_mutation_probability(void *data);
//uint8_t afl_custom_queue_get(void *data, const uint8_t *filename);
//void afl_custom_queue_new_entry(void *data, const uint8_t *filename_new_queue, const uint8_t *filename_orig_queue);
__attribute__((visibility("default"))) void afl_custom_deinit(void *data);
}

#endif //YET_ANOTHER_MUTATOR_LIBRARY_H
