#include "sllist.h"
#include <unistd.h>
#include <stdlib.h>

void createSLList(SLList *list, int dataSize) {
    list->size = 0;
    list->dataSize = dataSize;
    Job *sentinel = malloc(sizeof(Job));
    sentinel->pid = -1;
    sentinel->next = sentinel;
    list->sentinel = sentinel;
}

void deleteSLList(SLList *list) {
    Job *curr = list->sentinel->next;
    while (curr != list->sentinel) {
        Job *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list->sentinel);
}

void insertNode(SLList *list, Job *job){
    Job *curr = list->sentinel;
    while (curr->next != list->sentinel) {
        curr = curr->next;
    }
    curr->next = job;
    job->next = list->sentinel;
    list->size++;
}

void removeNode(SLList *list, int pid){
    Job *curr = list->sentinel;
    while (curr->next != list->sentinel) {
        if (curr->next->pid == pid) {
            Job *next = curr->next->next;
            free(curr->next);
            curr->next = next;
            list->size--;
            return;
        }
        curr = curr->next;
    }
}

int findNode(SLList *list, int pid){
    Job *curr = list->sentinel;
    int index = 0;
    while (curr->next != list->sentinel) {
        if (curr->next->pid == pid) {
            return index;
        }
        curr = curr->next;
        index++;
    }
    return -1;
}

Job* getNode(SLList *list, int index){
    if (index < 0 || index >= list->size) {
        return NULL;
    }
    Job *curr = list->sentinel;
    int i = 0;
    while (curr->next != list->sentinel) {
        if (i == index) {
            return curr->next;
        }
        curr = curr->next;
        i++;
    }
    return NULL;
}