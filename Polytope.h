//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _POLYTOPE_H_
#define _POLYTOPE_H_

#include "Common.h"
#include "Simplex.h"

typedef struct TFace {
    int a, b, c;
    TVec3 normal;
    bool degenerated;
} TFace;

typedef struct TPolytope {
    TSupport * vertices;
    int vertexCount;
    TFace * faces;
    int faceCount;
} TPolytope;
 
typedef struct TEdge {
    int a;
    int b;
    bool free;
} TEdge;
 
typedef struct TEdgeList {
    TEdge * edges;
    int count;
    int freeCount;
} TEdgeList;

typedef struct TTriangle {
    TSupport a, b, c;
    TVec3 normal;
    float dist;
    int numInPolytope;
} TTriangle;

void Polytope_ComputeFaceNormal( TPolytope * polytope, int n );
void Polytope_SetFromSimplex( TPolytope * polytope, TSimplex * simplex );
void EdgeList_Create( TEdgeList * edgeList, TPolytope * polytope );
void EdgeList_Fill( TEdgeList * edgeList, TPolytope * polytope );
void EdgeList_MarkHoles( TEdgeList * edgeList );
int Polytope_ReserveFaces( TPolytope * polytope, int faceCount );
void Polytope_PatchHoles( TPolytope * polytope, TEdgeList * edgeList, int newPointIndex );
void Polytope_RemoveFace( TPolytope * polytope, int n );
int Polytope_AddVertex( TPolytope * polytope, TSupport newSupport );
int Polytope_GetFirstFaceSeenFromPoint( TPolytope * polytope, TVec3 point );
TTriangle Polytope_GetClosestTriangleToOrigin( TPolytope * polytope );

#endif