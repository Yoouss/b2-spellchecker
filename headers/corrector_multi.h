#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "input.h"

#ifndef CORRECTOR_MULTI_H
#define CORRECTOR_MULTI_H

char* get_word_correction_multi(char* wrong_word, Dictionary_t* dict);

void* get_word_correction_thread(void* args);

#endif