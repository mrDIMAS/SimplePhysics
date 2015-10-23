//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Simplex.h"

/*
====================================
Simplex_RemovePoint
====================================
*/
void Simplex_RemovePoint( TSimplex * s, int num ) {
    if( num == 0 ) {
        s->points[0] = s->points[1];
        s->points[1] = s->points[2];
        s->points[2] = s->points[3];
    } else if( num == 1 ) {
        s->points[1] = s->points[2];
        s->points[2] = s->points[3];
    } else if( num == 2 ) {
        s->points[2] = s->points[3];
    }
    s->size--;
}
 
/*
====================================
Simplex_AddPoint
====================================
*/
void Simplex_AddPoint( TSimplex * s, TSupport p ) {
    s->points[ s->size ] = p;
    s->size++;
}
 