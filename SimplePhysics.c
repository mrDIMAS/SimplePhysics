//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "SimplePhysics.h"
#include <stdbool.h>
#include "Body.h"
#include "DynamicsWorld.h"

/*
====================================
SP_Init
====================================
*/ 
int SP_World_Create( void ) {
    return true;
}

/*
====================================
SP_Shutdown
====================================
*/ 
int SP_World_Destroy( void ) {
    DynamicsWorld_CleanUp();
    return true;
} 

/*
====================================
SP_Shape_CreateConvex
====================================
*/ 
TSPShape SP_Shape_CreateConvex( int vertexCount, TVec3 * vertices ) {
    return ConvexShape_CreateFromMemory( vertexCount, vertices, 0, 0 );
}

/*
====================================
SP_Shape_CreateSphere
====================================
*/ 
TSPShape SP_Shape_CreateSphere( float radius ) {
    return ConvexShape_CreateSphere( Vec3_Set( 0, 0, 0 ), radius );
}

/*
====================================
SP_Shape_CreateTriangleMesh
====================================
*/ 
TSPShape SP_Shape_CreateTriangleMesh( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces ) {
    return TriangleMesh_CreateFromMemory( vertexCount, vertices, faceCount, faces );
}

/*
====================================
SP_Shape_GetFaceCount
====================================
*/ 
int SP_Shape_GetFaceCount( TSPShape shape ) {
    TShape * s = shape;
    if( s->convex ) {
        return s->convex->faceCount;
    } else if( s->triMesh ) {
        return s->triMesh->faceCount;
    } 
    return 0;
}

/*
====================================
SP_Shape_GetFaces
====================================
*/ 
TSPFace * SP_Shape_GetFaces( TSPShape shape ) {
    TShape * s = shape;
    if( s->convex ) {
        return s->convex->faces;
    } else if( s->triMesh ) {
        return s->triMesh->faces;
    } 
    return NULL;
}

/*
====================================
SP_Shape_GetVertexCount
====================================
*/ 
int SP_Shape_GetVertexCount( TSPShape shape ) {
    TShape * s = shape;
    if( s->convex ) {
        return s->convex->count;
    } else if( s->triMesh ) {
        return s->triMesh->vertexCount;
    } 
    return 0;
}

/*
====================================
SP_Shape_GetVertices
====================================
*/ 
TVec3 * SP_Shape_GetVertices( TSPShape shape ) {
    TShape * s = shape;
    if( s->convex ) {
        return s->convex->points;
    } else if( s->triMesh ) {
        return s->triMesh->vertices;
    } 
    return NULL;  
}

/*
====================================
SP_Shape_GetLocalScale
====================================
*/
TVec3 SP_Shape_GetLocalScale( TSPShape shape ) {
    return ((TShape*)shape)->localScale;
}

/*
====================================
SP_Shape_SetLocalScale
====================================
*/ 
void SP_Shape_SetLocalScale( TSPShape shape, TVec3 scale ) {
    ((TShape*)shape)->localScale = scale;
}

/*
====================================
SP_Shape_GetBoundingRadius
====================================
*/ 
float SP_Shape_GetBoundingRadius( TSPShape shape ) {
    return ((TShape*)shape)->boundingRadius;
}

/*
====================================
SP_Shape_GetAABBMin
====================================
*/ 
TVec3 SP_Shape_GetAABBMin( TSPShape shape ) {
    return ((TShape*)shape)->min;
}

/*
====================================
SP_Shape_GetAABBMax
====================================
*/ 
TVec3 SP_Shape_GetAABBMax( TSPShape shape ) {
    return ((TShape*)shape)->max;
}

/*
====================================
SP_Shape_GetType
====================================
*/ 
EShapeType SP_Shape_GetType( TSPShape shape ) {
    TShape * s = shape;    
    if( s->convex ) {
        return SPSHAPE_CONVEX;
    } else if( s->sphere ) {
        return SPSHAPE_SPHERE;
    } else if( s->triMesh ) {
        return SPSHAPE_TRIMESH;
    } else {
        return SPSHAPE_BAD;
    }
}

/*
====================================
SP_Shape_Sphere_GetRadius
====================================
*/ 
float SP_Shape_Sphere_GetRadius( TSPShape shape ) {
    TShape * s = shape; 
    if( s->sphere ) {
        return s->sphere->radius;
    } else {
        return -1.0f;
    }
}

/*
====================================
SP_Shape_CreateFromObj
====================================
*/ 
TSPShape SP_Shape_CreateFromObj( const char * fn, bool triangleMeshShape ) {
    FILE * file = fopen( fn, "r" );
    if( !file ) {
        return NULL;
    }
    TVec3 * vertices = NULL;
    int vertexCount = 0;
    TSPFace * faces = NULL;
    int faceCount = 0;
    while( true ) {
        char header[1024];
        int count = fscanf( file, "%s", header );
        if( count == EOF ) {
            break;
        }
        
        if( strcmp( header, "v" ) == 0 ) {
            vertexCount++;
            vertices = realloc( vertices, vertexCount * sizeof( TVec3 ));
            fscanf( file, "%f %f %f\n", &vertices[ vertexCount - 1 ].x, &vertices[ vertexCount - 1 ].y, &vertices[ vertexCount - 1 ].z );
        }
        
        if( strcmp( header, "f" ) == 0 ) {
            int dummy, v0, v1, v2;
            int count = fscanf( file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v0, &dummy, &dummy, &v1, &dummy, &dummy, &v2, &dummy, &dummy );
            if( count != 9 ) {
                return NULL;
            }
            faceCount++;
            faces = realloc( faces, faceCount * sizeof( TFace ));
            faces[ faceCount - 1 ].a = v0 - 1;
            faces[ faceCount - 1 ].b = v1 - 1;
            faces[ faceCount - 1 ].c = v2 - 1;
        }
    }
    
    if( triangleMeshShape ) {
        return TriangleMesh_CreateFromMemory( vertexCount, vertices, faceCount, faces );
    } else {
        return ConvexShape_CreateFromMemory( vertexCount, vertices, faceCount, faces );
    }
}

/*
====================================
SP_Body_Create
====================================
*/ 
TSPBody SP_Body_Create( TSPShape shape ) {
    return Body_Create( shape );
}

/*
====================================
SP_Body_SetPosition
====================================
*/ 
void SP_Body_SetPosition( TSPBody body, TVec3 position ) {
    ((TBody*)body)->position = position;
}

/*
====================================
SP_Body_GetPosition
====================================
*/ 
TVec3 SP_Body_GetPosition( TSPBody body ) {
    return ((TBody*)body)->position;
} 

/*
====================================
SP_Body_GetShape
====================================
*/ 
TSPShape SP_Body_GetShape( TSPBody body ) {
    return ((TBody*)body)->shape;
}

/*
====================================
SP_Body_SetAnisotropicFriction
====================================
*/ 
void SP_Body_SetAnisotropicFriction( TSPBody body, TVec3 anisoFrict ) {
    ((TBody*)body)->anisotropicFriction = Vec3_Clamp( anisoFrict, 0.0f, 1.0f );

}

/*
====================================
SP_Body_SetAnisotropicFriction
====================================
*/ 
TVec3 SP_Body_GetAnisotropicFriction( TSPBody body ) {
    return ((TBody*)body)->anisotropicFriction;
}

/*
====================================
SP_Simulate
====================================
*/ 
void SP_World_Simulate() {
    DynamicsWorld_SolveCollisions();
}

/*
====================================
SP_Body_GetContactCount
====================================
*/ 
int SP_Body_GetContactCount( TSPBody body ) {
    return ((TBody*)body)->contactCount;
}

/*
====================================
SP_Body_GetContactPosition
====================================
*/ 
TVec3 SP_Body_GetContactPosition( TSPBody body, int contactN ) {
    return ((TBody*)body)->contacts[contactN].pointOnB;
}

/*
====================================
SP_Body_GetContactNormal
====================================
*/ 
TVec3 SP_Body_GetContactNormal( TSPBody body, int contactN ) {
    return ((TBody*)body)->contacts[contactN].normal;
}

/*
====================================
SP_Body_GetContactUserInfo
====================================
*/ 
void * SP_Body_GetContactUserInfo( TSPBody body, int contactN ) {
    return ((TBody*)body)->contacts[contactN].userInfo;
}

/*
====================================
SP_Body_SetLinearVelocity
====================================
*/ 
void SP_Body_SetLinearVelocity( TSPBody body, TVec3 linVel ) {
    ((TBody*)body)->linearVelocity = linVel;
}

/*
====================================
SP_Body_GetLinearVelocity
====================================
*/ 
TVec3 SP_Body_GetLinearVelocity( TSPBody body ) {
    return ((TBody*)body)->linearVelocity;
}
