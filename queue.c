#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *first = malloc(sizeof(struct list_head));
    if (first == NULL)
        return NULL;  // check if malloc get the spaces
    first->prev = first;
    first->next = first;
    return first;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *head = l->next;
    while (head != l) {
        element_t *first = container_of(head, element_t, list);
        head = head->next;
        free(first->value);
        free(first);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_head = malloc(sizeof(element_t));
    if (!new_head)
        return false;
    int char_size = 0;
    while (*(s + char_size))
        char_size += 1;
    new_head->value = malloc(char_size + 1);
    if (!new_head->value) {
        free(new_head);
        return false;
    }
    strncpy(new_head->value, s, char_size);
    new_head->value[char_size] = '\0';
    list_add(&new_head->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_tail = malloc(sizeof(element_t));
    if (!new_tail)
        return false;
    int char_size = 0;
    while (*(s + char_size))
        char_size += 1;
    new_tail->value = malloc(char_size + 1);
    if (!new_tail->value) {
        free(new_tail);
        return false;
    }
    strncpy(new_tail->value, s, char_size);
    new_tail->value[char_size] = '\0';
    list_add_tail(&new_tail->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    struct list_head *header = head->next;
    element_t *first = container_of(header, element_t, list);
    strncpy(sp, first->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    header = header->next;
    head->next = header;
    header->prev = head;
    return first;
}
/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    struct list_head *header = head->prev;
    element_t *first = container_of(header, element_t, list);
    strncpy(sp, first->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    header = header->prev;
    header->next = head;
    head->prev = header;
    return first;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;
    int count = 0;
    struct list_head *iter = head->next;
    while (iter != head) {
        count++;
        iter = iter->next;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return false;
    struct list_head *forward = head->next, *backward = head->prev;
    while (forward != backward && forward->prev != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    element_t *target = container_of(forward, element_t, list);
    if (forward == backward) {
        backward = backward->prev;
        forward = forward->next;
    } else
        forward = forward->next;
    free(target->value);
    free(target);
    backward->next = forward;
    forward->prev = backward;
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *header = head->next, *memory = NULL;
    int count = 1;
    while (header != head) {
        if (!memory) {
            count = 1;
            memory = header;
            header = header->next;
        } else {
            element_t *basic = container_of(memory, element_t, list);
            element_t *dup = container_of(header, element_t, list);
            if (strcasecmp(basic->value, dup->value) != 0 &&
                memory->next != header) {
                struct list_head *tmp = header->prev;
                memory->prev->next = header;
                header->prev = memory->prev;
                tmp->next = NULL;
                while (memory) {
                    struct list_head *next_target = memory->next;
                    element_t *target = container_of(memory, element_t, list);
                    free(target->value);
                    free(target);
                    memory = next_target;
                }
            } else if (strcasecmp(basic->value, dup->value) != 0 &&
                       memory->next == header)
                memory = NULL;
            else {
                header = header->next;
                count++;
            }
        }
    }
    if (count > 1) {
        struct list_head *tmp = header->prev;
        memory->prev->next = header;
        header->prev = memory->prev;
        tmp->next = NULL;
        while (memory) {
            struct list_head *next_target = memory->next;
            element_t *target = container_of(memory, element_t, list);
            free(target->value);
            free(target);
            memory = next_target;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    // l = [bear bear gerbil gerbil meerkat fish]
    struct list_head *first = head->next, *second = first->next;
    while (first != head && second != head) {
        first->prev->next = second;
        second->next->prev = first;
        first->next = second->next;
        second->prev = first->prev;
        first->prev = second;
        second->next = first;
        first = first->next;
        second = first->next;
    }
    head->next = (head->next->prev == head) ? head->next : head->next->prev;
    head->prev = (head->prev->next == head) ? head->prev : head->prev->next;
    // l = [bear bear gerbil meerkat]
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *forward = head->next, *backward = head->prev;
    head->next = backward;
    head->prev = forward;
    while (forward != head) {
        struct list_head *tmp = forward->next;
        forward->next = forward->prev;
        forward->prev = tmp;
        forward = tmp;
    }
}

struct list_head *compare_sort(struct list_head *head,
                               struct list_head *left,
                               struct list_head *right)
{
    // do the comparison and sort
    // problem right one element left one element
    for (struct list_head *merge = NULL; left || right != head;) {
        // right list hasn't reach the end (go back to the head of sorted list)
        // or left node has found its position for inserting
        element_t *left_element = container_of(left, element_t, list);
        element_t *right_element = container_of(right, element_t, list);
        if (right == head || (left && strcasecmp(right_element->value,
                                                 left_element->value) >= 0)) {
            if (!merge) {
                head = merge = left;
            } else {
                // merge is among right list
                // insert left node after the node pointed by merge
                left->prev = merge;
                left->next = merge->next;
                merge->next = left;
                left->next->prev = left;
                // shift merge pointer
                merge = merge->next;
            }
            left = NULL;
        } else {
            if (!merge) {
                head = merge = right;
            } else {
                // when merge points to the left element
                // (left sublist contains only one element)
                if (merge == merge->next) {
                    // push merge on to right's head
                    merge->next = right;
                    merge->prev = right->prev;
                    right->prev->next = merge;
                    right->prev = merge;
                }
                // shift merge pointer
                merge = merge->next;
            }
            right = right->next;
        }
    }
    return head;
}

struct list_head *divide(struct list_head *head)
{
    // Divide the list into (1,n-1)
    if (!head || head->next == head)
        return head;
    struct list_head *left = head;
    struct list_head *right = left->next;
    left->next = left;
    left->prev->next = right;
    right->prev = left->prev;
    left->prev = left;
    left = divide(left);
    right = divide(right);
    return compare_sort(head, left, right);
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    // Implement q_sort in quick sort
    head->prev->next = head->next;
    head->next->prev = head->prev;
    struct list_head *header = divide(head->next);
    head->next = header;
    head->prev = header->prev;
    header->prev->next = head;
    header->prev = head;
}
