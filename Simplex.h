//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _SIMPLEX_H_
#define _SIMPLEX_H_

#include "Common.h"

typedef struct TSupport {
    TVec3 firstShapeSupport;
    TVec3 minkowskiDifference;
} TSupport;
 
typedef struct TSimplex {
    TSupport points[4];
    int size;
} TSimplex;

void Simplex_RemovePoint( TSimplex * s, int num );
void Simplex_AddPoint( TSimplex * s, TSupport p );
 

#endif