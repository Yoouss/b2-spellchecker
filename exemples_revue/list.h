#ifndef LIST_H
#define LIST_H

struct node {
    int value;
    struct node *next;
};

struct node *list_insert(struct node *head, int value);
int list_contains(struct node *list, int value);
void list_free(struct node *list);

#endif
