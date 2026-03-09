#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "common.h"
#include "io.h"

int word_in_dictionary(char* word, Dictionary_t* dict) {
    return 0;
}

// À enlever après implémentation
void test_word_in_dictionary(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;
    char* dictionaryPath = "fr.dict.test";
    load_dictionaries(dictionaryPath, &dicts, &dicts_count);

    CU_ASSERT_PTR_NOT_NULL(dicts);
    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", dicts));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", dicts));
    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, dicts)); // cas extrême 1 : on ne donne pas de mot à chercher
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL)); // cas extrême 2 : pas de dictionnaire trouvé 
}

