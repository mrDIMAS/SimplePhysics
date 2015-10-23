//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>

/*
====================================
List_Create
====================================
*/
void List_Create( TList * list ) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/*
====================================
List_Free
====================================
*/
void List_Free( TList * list ) {
	TListNode * current = list->head;
    while( current ) {
		TListNode * del = current;
        current = current->next;
		free( del );
    }
}

/*
====================================
List_Add
====================================
*/
void List_Add( TList * list, void * data ) {
    TListNode * newElement = malloc( sizeof( TListNode ));
    newElement->data = data;
    newElement->next = NULL;
    newElement->prev = list->tail;
    if( list->tail ) {
        list->tail->next = newElement;
    }
    if( !list->head ) {
        list->head = newElement;
    }
    list->tail = newElement;
    list->size++;
}

/*
====================================
List_Find
====================================
*/
TListNode * List_Find( TList * list, void * data ) {
    for( TListNode * node = list->head; node; node = node->next ) {
        if( node->data == data ) {
            return node;
        }
    }
    return 0;    
}

/*
====================================
List_Count
====================================
*/
int List_Count( TList * list ) {
    TListNode * current = list->head;
    int count = 0;
    while( current ) {
        count++;
        current = current->next;
    }
    
    return count;
}

/*
====================================
List_RemoveNth
====================================
*/
void List_RemoveNth( TList * list, int n, char freeData ) {
    int i = 0;
    TListNode * current = list->head;
    while( current ) {
        if( i == n ) {
            if( current->next ) {
                current->next->prev = current->prev;
            }
            if( current->prev ) {
                current->prev->next = current->next;
            }
            if( current == list->head ) {
                if( current->next ) {
                    list->head = current->next;
                } else {
                    list->head = NULL;
                }
            }
            if( current == list->tail ) {
                if( current->prev ) {
                    list->tail = current->prev;
                } else {
                    list->tail = NULL;
                }
            }
            free( current );
            if( freeData ) {
                free( current->data );
            }
            list->size--;
            break;
        }
        i++;
        current = current->next;
    }
}

/*
====================================
List_Clear
====================================
*/
void List_Clear( TList * list, char freeData ) {
    TListNode * current = list->head;
    while( current ) {
        if( freeData ) {
            if( current->data ) {
                free( current->data );
            }
        }
        TListNode * next = current->next;
        free( current );
        current = next;
    }
    list->size = 0;
}

/*
====================================
List_GetNodeData
====================================
*/
void * List_GetNodeData( TList * list, int n ) {
    TListNode * current = list->head;
    int count = 0;
    while( current ) {
        if( count == n ) {
            return current->data;
        }
        count++;
        current = current->next;
    }
    return NULL;
}

/*
====================================
List_Remove
====================================
*/
TListNode * List_Remove( TList * list, void * data ) {
    TListNode * current = list->head;
	TListNode * nextOfDeleted = NULL;
    while( current ) {
        if( current->data == data ) {
            if( current->next ) {
                current->next->prev = current->prev;
            }
            if( current->prev ) {
                current->prev->next = current->next;
            }
			nextOfDeleted = current->next;
            if( current == list->head ) {
                if( current->next ) {
                    list->head = current->next;
                } else {
                    list->head = NULL;
                }
            }
            if( current == list->tail ) {
                if( current->prev ) {
                    list->tail = current->prev;
                } else {
                    list->tail = NULL;
                }
            }
            free( current );
            list->size--;
            break;
        }
        current = current->next;
    }
	return nextOfDeleted;
}