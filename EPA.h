//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _EPA_H_
#define _EPA_H_

#include "Polytope.h"
#include "Shape.h"

typedef struct TContact {
    TVec3 normal;
    float penetrationDepth;
    TVec3 pointOnB;
    struct TBody * bodyB;
    int triangleIndex;
    void * userInfo;
} TContact;

bool EPA_ComputeContact( TPolytope * polytope, TShape * shape1, TShape * shape2, struct TBody * body2, TContact * outContact );

#endif