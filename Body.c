//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Body.h"
#include "DynamicsWorld.h"

/*
====================================
Body_Create
====================================
*/
TBody * Body_Create( TShape * shape ) {
    TBody * body = Memory_New( TBody );
    body->position = shape->position;
    body->shape = shape;
    body->anisotropicFriction = Vec3_Set( 0.01f, 0.01f, 0.01f );
    List_Add( &gDynamicsWorld.bodies, body );    
    return body;
}

/*
====================================
Body_Free
====================================
*/
void Body_Free( TBody * body ) {
    List_Remove( &gDynamicsWorld.bodies, body );
    ConvexShape_Delete( body->shape );
    Memory_Free( body );
}

/*
====================================
Body_ClearContacts
====================================
*/
void Body_ClearContacts( TBody * body ) {
    body->contactCount = 0;
}

/*
====================================
Body_AddContact
====================================
*/
void Body_AddContact( TBody * body, TContact contact ) {
    if( body->contactCount < MAX_CONTACTS ) {
        body->contacts[body->contactCount++] = contact;
    }
}

/*
====================================
Body_ProcessCollision
====================================
*/
void Body_ProcessCollision( TBody * body1, TBody * body2, TShape * shape ) {
    TSimplex finalSimplex;
    TContact contact;
    TPolytope polytope;
    // select shape
    if( body2 ) {
        shape = body2->shape;
    }
    while( GJK_IsIntersects( body1->shape, shape, &finalSimplex )) {         
        Polytope_SetFromSimplex( &polytope, &finalSimplex );        
        if( EPA_ComputeContact( &polytope, body1->shape, shape, body2, &contact )) {
            TVec3 penetrationVec;
            if( body2 ) {
                TVec3 penetrationVec = Vec3_Scale( contact.normal, -contact.penetrationDepth / 2 );
                body2->position = Vec3_Add( body2->position, penetrationVec );
                body2->linearVelocity = Vec3_Project( body2->linearVelocity, contact.normal );
                body2->linearVelocity = Vec3_Mul( body2->linearVelocity, Vec3_Sub( Vec3_One(), body2->anisotropicFriction ));
                body2->shape->position = body2->position; 
                Body_AddContact( body2, contact );
            }
            penetrationVec = Vec3_Scale( contact.normal, contact.penetrationDepth / 2 );
            body1->position = Vec3_Add( body1->position, penetrationVec );
            body1->linearVelocity = Vec3_Project( body1->linearVelocity, contact.normal );
            body1->linearVelocity = Vec3_Mul( body1->linearVelocity, Vec3_Sub( Vec3_One(), body1->anisotropicFriction ));
            body1->shape->position = body1->position;       
            Body_AddContact( body1, contact );
        } else {  
            break;
        }                 
    }
} 

/*
====================================
Body_SolveCollision
====================================
*/
void Body_SolveCollision( TBody * body1, TBody * body2 ) {
    if( body1->shape->triMesh ) {
        TBody * temp = body1;
        body1 = body2;
        body2 = temp;
    }

    body1->shape->position = body1->position;
    body2->shape->position = body2->position;
    
    if( body2->shape->triMesh ) { 
        TTriangleMeshShape * triMesh = body2->shape->triMesh;                
        Octree_TraceSphere( &triMesh->octree, Vec3_Add( body1->position, body1->shape->center ), body1->shape->boundingRadius );         
        for( int i = 0; i < triMesh->octree.resultFaceCount; i++ ) {            
            TTriangleShape triangle = { 
                .v[0] = triMesh->vertices[ triMesh->octree.resultFaceList[i].a ], 
                .v[1] = triMesh->vertices[ triMesh->octree.resultFaceList[i].b ], 
                .v[2] = triMesh->vertices[ triMesh->octree.resultFaceList[i].c ],
                .userInfo = triMesh->octree.resultFaceList[i].userInfo
            };            
            TShape shape = {  
                .position = body2->position, 
                .triangle = &triangle, 
                .localScale = body2->shape->localScale 
            };            
            Body_ProcessCollision( body1, NULL, &shape );
        }  
    } else {
        float r1pr2 = body1->shape->boundingRadius + body1->shape->boundingRadius;
        bool coarseIntersection = Vec3_SqrDistance( Vec3_Add( body1->position, body1->shape->center ), Vec3_Add( body2->position, body2->shape->center ) ) < ( r1pr2 * r1pr2 );
        if( coarseIntersection ) {
            Body_ProcessCollision( body1, body2, NULL );
        }
    }
}

