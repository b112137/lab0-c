#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "natsort/strnatcmp.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL)
        return;
    list_ele_t *current;
    current = q->head;

    /* visit all the elements of the queue and free each */
    while (current != NULL) {
        q->head = q->head->next;
        if (current->value)
            free(current->value);
        free(current);
        current = q->head;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL)
        return false;

    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;

    newh->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (newh->value == NULL) {
        free(newh);
        return false;
    }
    memset(newh->value, 0, sizeof(char) * (strlen(s) + 1));

    strncpy(newh->value, s, strlen(s));
    newh->next = q->head;
    q->head = newh;
    /* if the queue is empty, q->tail is NULL */
    if (q->tail == NULL)
        q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q == NULL)
        return false;

    list_ele_t *newt;
    newt = malloc(sizeof(list_ele_t));
    if (newt == NULL)
        return false;

    newt->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (newt->value == NULL) {
        free(newt);
        return false;
    }
    memset(newt->value, 0, sizeof(char) * (strlen(s) + 1));

    strncpy(newt->value, s, strlen(s));
    newt->next = NULL;
    /* if the queue is empty, q->head adn q->tail are NULL */
    if (q->head == NULL && q->tail == NULL) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->size == 0)
        return false;

    list_ele_t *delete;
    size_t copy_size;
    delete = q->head;
    copy_size = 0;

    if (sp != NULL) {
        if (delete->value != NULL) {
            if (strlen(delete->value) < bufsize)
                copy_size = strlen(delete->value);
            else
                copy_size = bufsize - 1;
            strncpy(sp, delete->value, copy_size);
        }
        sp[copy_size] = '\0';
    }

    q->head = q->head->next;
    free(delete->value);
    free(delete);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL || q->size <= 1)
        return;

    list_ele_t *insert, *tmp, *finish;
    insert = q->head->next;
    q->tail->next = q->head;
    q->head = q->tail;
    q->tail = q->tail->next;
    q->tail->next = NULL;
    finish = q->tail;

    while (insert != q->head) {
        tmp = insert->next;
        insert->next = finish;
        q->head->next = insert;
        finish = insert;
        insert = tmp;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *merge(list_ele_t *start, size_t lenth)
{
    if (lenth <= 1)
        return start;

    list_ele_t *left, *right, *tmp;
    size_t left_lenth, right_lenth;
    if (lenth % 2 == 0) {
        left_lenth = right_lenth = lenth / 2;
    } else {
        left_lenth = lenth / 2;
        right_lenth = lenth / 2 + 1;
    }

    left = start;
    right = start;
    for (int i = 0; i < left_lenth; i++) {
        if (i < left_lenth - 1)
            right = right->next;
        else {
            tmp = right->next;
            right->next = NULL;
            right = tmp;
        }
    }

    left = merge(left, left_lenth);
    right = merge(right, right_lenth);

    list_ele_t *merge = NULL;
    while (left || right) {
        if (right == NULL ||
            (left && strnatcmp(left->value, right->value) < 0)) {
            if (merge == NULL) {
                start = merge = left;
            } else {
                merge->next = left;
                merge = merge->next;
            }
            left = left->next;
        } else {
            if (merge == NULL) {
                start = merge = right;
            } else {
                merge->next = right;
                merge = merge->next;
            }
            right = right->next;
        }
    }
    return start;
}

void q_sort(queue_t *q)
{
    if (q == NULL || q->size <= 1)
        return;
    q->head = merge(q->head, q->size);
    list_ele_t *tmp;
    tmp = q->head;
    for (size_t i = 0; i < q->size; i++) {
        if (i == q->size - 1)
            q->tail = tmp;
        tmp = tmp->next;
    }
    // while(tmp->next != NULL)
    //     tmp = tmp->next;
    // q->tail = tmp;
}
