//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

#include "Memory.h"

typedef struct SMemoryNode {
    void * data;
    int size;
    struct SMemoryNode * next;
    struct SMemoryNode * prev;
} TMemoryNode;


TMemoryNode * g_rootAllocNode = NULL;
TMemoryNode * g_lastAllocNode = NULL;
int totalAllocatedMemory = 0;

void * Memory_CommonAllocation( int size, bool clear ) {
    void * block = malloc( size );
    if( clear ) {
        memset( block, 0, size );
    }
    totalAllocatedMemory += size;
    TMemoryNode * newAllocNode = malloc( sizeof( TMemoryNode ));
    newAllocNode->data = block;
    newAllocNode->next = NULL;
    newAllocNode->size = size;
    newAllocNode->prev = g_lastAllocNode;
    if( g_lastAllocNode ) {
        g_lastAllocNode->next = newAllocNode;
    }
    if( !g_rootAllocNode ) {
        g_rootAllocNode = newAllocNode;
    }
    g_lastAllocNode = newAllocNode;
    
    return block;
}

void * Memory_Allocate( int size ) {
    return Memory_CommonAllocation( size, false );
}

void * Memory_AllocateClean( int size ) {
    return Memory_CommonAllocation( size, true );
}

int Memory_GetAllocated( void ) {
    return totalAllocatedMemory;
}

void * Memory_Reallocate( void * data, int newSize ) {
    // find existing memory block
    TMemoryNode * current = g_rootAllocNode;
    void * newData = 0;
    while( current ) {
        if( current->data == data ) {
            newData = realloc( data, newSize );
            // got pointer to another place, rewrite it in the memblock
            if( newData != data ) {
                current->data = newData;
            }
        }
        current = current->next;
    }
    return newData;
}

void Memory_Free( void * data ) {
    TMemoryNode * current = g_rootAllocNode;
    while( current ) {
        if( current->data == data ) {
            if( current->next ) {
                current->next->prev = current->prev;
            }
            if( current->prev ) {
                current->prev->next = current->next;
            }
            if( current == g_rootAllocNode ) {
                if( current->next ) {
                    g_rootAllocNode = current->next;
                } else {
                    g_rootAllocNode = NULL;
                }
            }
            if( current == g_lastAllocNode ) {
                if( current->prev ) {
                    g_lastAllocNode = current->prev;
                } else {
                    g_lastAllocNode = NULL;
                }
            }
            totalAllocatedMemory -= current->size;
            free( current );
            free( data );
            return;
        }
        current = current->next;
    }
}


void Memory_CollectGarbage( ) {
    TMemoryNode *next, *current;
    for( current = g_rootAllocNode; current; current = next ) {
        next = current->next;
        totalAllocatedMemory -= current->size;
        free( current->data );
        free( current );
        current = next;
    }
}