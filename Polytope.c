//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Polytope.h"
#include "MathLib.h"

#define POLYTOPE_MAX_FACE_COUNT ( 4096 )
#define POLYTOPE_MAX_EDGE_COUNT ( 3 * POLYTOPE_MAX_FACE_COUNT )
#define POLYTOPE_MAX_VERTEX_COUNT ( 3 * POLYTOPE_MAX_FACE_COUNT )

// these pre-allocated buffers gives noticeable boost
static TEdge gEdgeCache[ POLYTOPE_MAX_EDGE_COUNT ];
static TSupport gPolytopeVertexCache[ POLYTOPE_MAX_VERTEX_COUNT ];
static TFace gPolytopeFaceCache[ POLYTOPE_MAX_FACE_COUNT ];
 
 
/*
====================================
Polytope_ComputeFaceNormal
====================================
*/
void Polytope_ComputeFaceNormal( TPolytope * polytope, int n ) {
    TVec3 a = polytope->vertices[ polytope->faces[ n ].a ].minkowskiDifference;
    TVec3 b = polytope->vertices[ polytope->faces[ n ].b ].minkowskiDifference;
    TVec3 c = polytope->vertices[ polytope->faces[ n ].c ].minkowskiDifference;
    polytope->faces[n].normal = Vec3_Cross( Vec3_Sub( b, a ), Vec3_Sub( c, a ) );
    polytope->faces[n].degenerated = Vec3_SqrLength( polytope->faces[n].normal ) < 0.000001;
}

/*
====================================
Polytope_SetFromSimplex
====================================
*/
void Polytope_SetFromSimplex( TPolytope * polytope, TSimplex * simplex ) {
    polytope->vertexCount = 4;
    polytope->vertices = &gPolytopeVertexCache[0];
    //memset( polytope->vertices, 0, sizeof( TSupport ) * POLYTOPE_MAX_VERTEX_COUNT );
    for( int i = 0; i < polytope->vertexCount; i++ ) {
        polytope->vertices[i] = simplex->points[i];
    }
    
    polytope->faceCount = 4;
    polytope->faces = &gPolytopeFaceCache[0];
    
    polytope->faces[0] = (TFace) { 0, 1, 2 };
    polytope->faces[1] = (TFace) { 0, 3, 1 };
    polytope->faces[2] = (TFace) { 0, 2, 3 };
    polytope->faces[3] = (TFace) { 2, 1, 3 };
    
    Polytope_ComputeFaceNormal( polytope, 0 );
    Polytope_ComputeFaceNormal( polytope, 1 );
    Polytope_ComputeFaceNormal( polytope, 2 );
    Polytope_ComputeFaceNormal( polytope, 3 );
}

/*
====================================
EdgeList_Create
====================================
*/
void EdgeList_Create( TEdgeList * edgeList, TPolytope * polytope ) {
    edgeList->count = polytope->faceCount * 3;
    if( edgeList->count > POLYTOPE_MAX_EDGE_COUNT ) {
        printf( "Edge count exceeded!\n" );
    }
    edgeList->edges = &gEdgeCache[0];
}

/*
====================================
EdgeList_Fill
====================================
*/
void EdgeList_Fill( TEdgeList * edgeList, TPolytope * polytope ) {
    for( int i = 0, j = 0; i < polytope->faceCount; i++, j += 3 ) {
        edgeList->edges[j+0] = (TEdge) { .a = polytope->faces[i].a, .b = polytope->faces[i].b, .free = true };
        edgeList->edges[j+1] = (TEdge) { .a = polytope->faces[i].b, .b = polytope->faces[i].c, .free = true };
        edgeList->edges[j+2] = (TEdge) { .a = polytope->faces[i].c, .b = polytope->faces[i].a, .free = true };
    }
}

/*
====================================
EdgeList_MarkHoles
====================================
*/
void EdgeList_MarkHoles( TEdgeList * edgeList ) {
    for( int i = 0; i < edgeList->count; i++ ) {
        for( int j = 0; j < edgeList->count; j++ ) {
            if( edgeList->edges[i].free && edgeList->edges[j].free ) {
                if( edgeList->edges[j].a == edgeList->edges[i].b && edgeList->edges[j].b == edgeList->edges[i].a ) {
                    edgeList->edges[i].free = false;
                    edgeList->edges[j].free = false;
                }
            }
        }
    }
    edgeList->freeCount = 0;
    for( int i = 0; i < edgeList->count; i++ ) {
        if( edgeList->edges[i].free ) {
            edgeList->freeCount++;
        }
    }
}
 
/*
====================================
Polytope_ReserveFaces
====================================
*/
int Polytope_ReserveFaces( TPolytope * polytope, int faceCount ) {
    int last = polytope->faceCount;
    polytope->faceCount += faceCount;
    return last;
}
 
/*
====================================
Polytope_PatchHoles
====================================
*/
void Polytope_PatchHoles( TPolytope * polytope, TEdgeList * edgeList, int newPointIndex ) {
    int lastFree = Polytope_ReserveFaces( polytope, edgeList->freeCount );
    for( int i = 0; i < edgeList->count; i++ ) {
        if( edgeList->edges[i].free ) {
            polytope->faces[lastFree] = (TFace) { .a = newPointIndex, .b = edgeList->edges[i].b, .c = edgeList->edges[i].a };
            Polytope_ComputeFaceNormal( polytope, lastFree );
            lastFree++;
        }
    }
}
 
/*
====================================
Polytope_RemoveFace
====================================
*/
void Polytope_RemoveFace( TPolytope * polytope, int n ) {
    if( n == 0 ) {
        polytope->faceCount--;
        memmove( polytope->faces, polytope->faces + 1, sizeof( TFace ) * polytope->faceCount );
    } else if( n == polytope->faceCount - 1 ) {
        polytope->faceCount--;
    } else {
        memmove( polytope->faces + n, polytope->faces + n + 1, sizeof( TFace ) * ( polytope->faceCount - n ));
        polytope->faceCount--;
    }
}
 
/*
====================================
Polytope_AddVertex
====================================
*/
int Polytope_AddVertex( TPolytope * polytope, TSupport newSupport ) {
    int last = polytope->vertexCount;
    polytope->vertexCount++;
    polytope->vertices[last] = newSupport;
    return last;
}

/*
====================================
Polytope_GetFirstFaceSeenFromPoint
====================================
*/
int Polytope_GetFirstFaceSeenFromPoint( TPolytope * polytope, TVec3 point ) {
    for( int i = 0; i < polytope->faceCount; i++ ) {
        if( Vec3_Dot( polytope->faces[i].normal, Vec3_Sub( point, polytope->vertices[ polytope->faces[i].a ].minkowskiDifference )) > 0 ) {
            return i;
        }
    }
    return -1;
}

/*
====================================
Polytope_GetClosestTriangleToOrigin
====================================
*/
TTriangle Polytope_GetClosestTriangleToOrigin( TPolytope * polytope ) {
    int closest = -1;
    float closestDistance = FLT_MAX;
    for( int i = 0; i < polytope->faceCount; i++ ) {
        if( polytope->faces[i].degenerated ) {
            continue;
        }        
        float d = fabsf( Math_DistanceToOrigin( polytope->faces[i].normal, polytope->vertices[ polytope->faces[ i ].a ].minkowskiDifference ));
        if( d < closestDistance ) {
            closestDistance = d;
            closest = i;
        }        
    }
    if( closest >= 0 ) {
        return (TTriangle) { 
            .a = polytope->vertices[ polytope->faces[ closest ].a ], .b = polytope->vertices[ polytope->faces[ closest ].b ],
            .c = polytope->vertices[ polytope->faces[ closest ].c ], .normal = polytope->faces[closest].normal, .dist = closestDistance, .numInPolytope = closest 
        };
    } else {
         return (TTriangle) { 
             .numInPolytope = -1 
        };       
    }
}