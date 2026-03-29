#include "list.h"
#include <stdlib.h>

struct node *list_insert(struct node *head, int value)
{
    struct node *new = malloc(sizeof(struct node));
    if (new == NULL)
        return NULL;
    new->value = value;
    new->next = head;
    return new;
}

int list_contains(struct node *list, int value)
{
    while (list != NULL) {
        if (list->value == value)
            return 1;
        list = list->next;
    }
    return 0;
}

void list_free(struct node *list)
{
    while (list != NULL) {
        struct node *next = list->next;
        free(list);
        list = next;
    }
}
