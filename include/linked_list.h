#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "stdint.h"
#include "memory.h"

struct _NODE { 
    void *data; 
    uint32_t size;
    struct _NODE *next; 
};


struct LINKED_LIST {    
    struct _NODE *head;  
    struct _NODE *tail;
    void (*Add)(struct LINKED_LIST *list, void *data, size_t size);
    void (*Del)(struct LINKED_LIST *list, void *data, size_t size);
}; 

typedef struct _NODE _NODE_t; 
typedef struct LINKED_LIST LINKED_LIST_t;

LINKED_LIST_t *LINKED_LIST_New(void); 
void LINKED_LIST_Free(LINKED_LIST_t *); 
#endif


#ifdef LINKED_LIST_IMPL
void LINKED_LIST_Add(LINKED_LIST_t* list, void *data, size_t size){
    if (list == NULL){
        return ;
    }
    _NODE_t* node = (_NODE_t*) malloc(sizeof(_NODE_t));
    node->data =  (void*) malloc(size);
    node->size = size;
    node->next = NULL;
    memcpy(node->data, &*data, size);
    if (list->tail == NULL){
        list->tail = node;
        list->head = node;
        return ;
    }
    list->tail->next = node;
    list->tail = node;
}

void LINKED_LIST_Del(LINKED_LIST_t* list, void *data, size_t size){
    if (list == NULL){
        return ;
    }
    _NODE_t t = {};
    _NODE_t *pre = &t;
    _NODE_t *curr = list->head;
    while (curr != NULL){
        if (curr->size == size) {
            if (memcmp(curr->data, data, size)){
                pre->next = curr->next;
                break;
            }
        }
        pre->next = curr;
        curr = curr->next;
    }
    if (curr != NULL){
        free(curr->data);
        free(curr);
    }
}

LINKED_LIST_t *LINKED_LIST_New(void){
    LINKED_LIST_t* _linkedlist = (LINKED_LIST_t*) malloc(sizeof(LINKED_LIST_t));
    _linkedlist->head = NULL;
    _linkedlist->tail = NULL;
    _linkedlist->Add = LINKED_LIST_Add;
    _linkedlist->Del = LINKED_LIST_Del;
  return _linkedlist;
}



void LINKED_LIST_Free(LINKED_LIST_t *list){
    if (list == NULL){
        return ;
    }
    _NODE_t *curr = list->head;
    while (curr != NULL){
        _NODE_t *tmp = curr;
        curr = curr->next;
        free(tmp->data);
        free(tmp);
    }
    free(list);
}
#endif