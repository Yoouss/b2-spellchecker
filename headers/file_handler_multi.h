#include "common.h"
#include <stdint.h>

#ifndef FILE_HANDLER_MULTI_H
#define FILE_HANDLER_MULTI_H

int load_dictionaries_multi(const char *path, Dictionary_t **dicts, size_t *dict_count);

void* load_dictionaries_thread(void* args);

#endif