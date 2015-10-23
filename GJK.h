//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _GJK_H_
#define _GJK_H_

#include "Simplex.h"
#include "Shape.h"

TSupport GJK_GetSupport( TShape * shape1, TShape * shape2, TVec3 dir );
bool GJK_ProcessLine( TSimplex * simplex, TVec3 * outDirection );
bool GJK_ProcessTriangle( TSimplex * simplex, TVec3 * outDirection );
bool GJK_ProcessTetrahedron( TSimplex * simplex, TVec3 * outDirection );
bool GJK_ProcessSimplex( TSimplex * simplex, TVec3 * outDirection );
void GJK_FixSimplex( TSimplex * simplex, TShape * shape1, TShape * shape2 );
bool GJK_IsIntersects( TShape * shape1, TShape * shape2, TSimplex * finalSimplex );

#endif