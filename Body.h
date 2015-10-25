//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _BODY_H_
#define _BODY_H_

#include "Shape.h"
#include "Polytope.h"
#include "Simplex.h"
#include "GJK.h"
#include "MathLib.h"
#include "EPA.h"

#define MAX_CONTACTS (8)

typedef struct TBody {
    TVec3 position;
    TVec3 linearVelocity;
    TShape * shape;
    TContact contacts[ MAX_CONTACTS ];
    int contactCount;
    TVec3 anisotropicFriction;
    int collisionGroup;
    TVec3 gravity;
} TBody;

TBody * Body_Create( TShape * shape );
void Body_Free( TBody * body );
void Body_ClearContacts( TBody * body );
void Body_AddContact( TBody * body, TContact contact );
void Body_ProcessCollision( TBody * body1, TBody * body2, TShape * shape );
void Body_SolveCollision( TBody * body1, TBody * body2 );
void Body_SetGravity( TBody * body, TVec3 gravity );

#endif