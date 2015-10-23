//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "DynamicsWorld.h"
#include "Common.h"
#include "Body.h"

TDynamicsWorld gDynamicsWorld;

/*
====================================
Dynamics_SolveCollisions
====================================
*/
void Dynamics_SolveCollisions() {
    static TVec3 gravity = { .x = 0.0f, .y = -0.000777, .z = 0.0f };
    for_each( TBody, body, gDynamicsWorld.bodies ) {
        Body_ClearContacts( body );
        bool bodyIsDynamic = body->shape->triMesh == NULL;
        if( bodyIsDynamic ) {
            body->position = Vec3_Add( body->position, body->linearVelocity );  
            body->linearVelocity = Vec3_Add( body->linearVelocity, gravity );                                           
            for_each( TBody, otherBody, gDynamicsWorld.bodies ) {
                if( body != otherBody ) {
                    Body_SolveCollision( body, otherBody );
                }
            }     

        }
    }
}

/*
====================================
Dynamics_TraceRay
====================================
*/
TBody * Dynamics_TraceRay( TVec3 rayBegin, TVec3 rayDir, bool infiniteRay, int outCountContacts, TSPRayTraceResult * outResultList ) {
    for_each( TBody, body, gDynamicsWorld.bodies ) {
        TShape * shape = body->shape;
        Shape_TraceRay( shape, rayBegin, rayDir, infiniteRay, outCountContacts, outResultList );
        for( int i = 0; i < outCountContacts; i++ ) {
            outResultList[i].body = body;
        }
        if( outCountContacts > 0 ) {
            return body;
        }
    }
    return NULL;
}