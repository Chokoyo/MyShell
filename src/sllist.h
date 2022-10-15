#include <stdbool.h>

#ifndef SLLIST_H
#define SLLIST_H


typedef void (*FreeFunction)(void *);

typedef struct job
{
    int pid;
    char *command;
    struct job *next;
} Job;

typedef struct sllist
{
    int size;
    int dataSize;
    Job *sentinel;
} SLList;

// Methods for Node
bool hasNext(Job *job);

// Methods for SLList
void createSLList(SLList *list, int dataSize);
void deleteSLList(SLList *list);
void insertNode(SLList *list, Job *job);
void removeNode(SLList *list, int pid);
int findNode(SLList *list, int pid);
Job* getNode(SLList *list, int index);

#endif