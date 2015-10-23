//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _CONVEX_H_
#define _CONVEX_H_

#include "Common.h"
#include "SimplePhysics.h"
#include "Octree.h"

typedef struct TSphereShape {
    float radius;
} TSphereShape; 
 
typedef struct TConvexShape {
    TVec3 * points;
    int count;
    TSPFace * faces;
    int faceCount;
} TConvexShape;

typedef struct TTriangleShape {
    TVec3 v[3];
    void * userInfo;
} TTriangleShape;

typedef struct TTriangleMeshShape {
    TVec3 * vertices;
    int vertexCount;
    TSPFace * faces;
    int faceCount;
    TOctree octree;
} TTriangleMeshShape;

typedef struct TShape {
    TConvexShape * convex;
    TSphereShape * sphere;
    TTriangleMeshShape * triMesh;
    TTriangleShape * triangle;
    
    TVec3 position; 
    TVec3 localScale;

    // for octree tracing
    TVec3 center;
    float boundingRadius;
    TVec3 min;
    TVec3 max;
} TShape;

TShape * Shape_Create( );
TShape * ConvexShape_CreateSphere( TVec3 position, float radius );
TShape * ConvexShape_CreateBox( TVec3 min, TVec3 max );
TShape * ConvexShape_CreateFromMemory( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces );
float ConvexShape_CalculateCircumcircleRadius( TShape * shape );
TVec3 ConvexShape_CalculateCenter( TShape * shape );
void ConvexShape_Delete( TShape * shape );
TVec3 ConvexShape_GetFarthestPointInDirection( TShape * shape, TVec3 dir );
TShape * TriangleMesh_CreateFromMemory( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces );

void Shape_TraceRay( TShape * shape, TVec3 rayBegin, TVec3 rayDir, bool infiniteRay, int outCountContacts, TSPRayTraceResult * outResultList );

#endif