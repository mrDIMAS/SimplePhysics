//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _DYNAMICS_WORLD_H_
#define _DYNAMICS_WORLD_H_

#include "List.h"
#include "Common.h"

typedef struct TDynamicsWorld {
    TList bodies;
} TDynamicsWorld;

void DynamicsWorld_SolveCollisions();
struct TBody * DynamicsWorld_TraceRay( TVec3 rayBegin, TVec3 rayDir, bool infiniteRay, int outCountContacts, TSPRayTraceResult * outResultList );
void DynamicsWorld_CleanUp( );

extern TDynamicsWorld gDynamicsWorld;

#endif