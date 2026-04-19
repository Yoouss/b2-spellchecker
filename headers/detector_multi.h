#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "input.h"

#ifndef DETECTOR_MULTI_H
#define DETECTOR_MULTI_H

/**
 * Fonction de démarrage pour chaque thread
 */
void* thread_count_errors(void* arg);

/**fonction trouve le dictionnaire adéquat pour la ligne entrée en utilisant un thread par dictionnaire */
Dictionary_t* find_candidate_dict_for_line_thread(char* line, Dictionary_t* dicts, size_t dictionaries_count);


#endif