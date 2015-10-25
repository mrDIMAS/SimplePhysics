/*////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//           Simple Physics API         //
//            (C) mrDIMAS 2015          //
////////////////////////////////////////*/

#ifndef _SIMPLE_PHYSICS_H_
#define _SIMPLE_PHYSICS_H_

#ifdef _cplusplus
    extern "C" {
#else
    #include <stdbool.h>
#endif

typedef void * TSPBody;
typedef void * TSPShape;

typedef struct TSPFace {
    int a, b, c;
    void * userInfo;
} TSPFace;

typedef struct TVec3 {
    float x, y, z;
} TVec3;

typedef struct TSPRayTraceResult {
    TVec3 position;
    TVec3 normal;
    TSPBody body;
} TSPRayTraceResult;

typedef enum ESPShapeType {
    SPSHAPE_BAD,
    SPSHAPE_CONVEX,
    SPSHAPE_SPHERE,
    SPSHAPE_TRIMESH,
} ESPShapeType;

/* World creation */
int SP_World_Create( void );
int SP_World_Destroy( void );

/* World simulation */
void SP_World_Simulate();

/* World gravity */
void SP_World_SetGravity( TVec3 gravity );
TVec3 SP_World_GetGravity( );

/* Shape creation */
TSPShape SP_Shape_CreateConvex( int vertexCount, TVec3 * vertices );
TSPShape SP_Shape_CreateSphere( float radius );
TSPShape SP_Shape_CreateTriangleMesh( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces );
TSPShape SP_Shape_CreateFromObj( const char * fn, bool triangleMeshShape );

/* Shape faces */
int SP_Shape_GetFaceCount( TSPShape shape );
TSPFace * SP_Shape_GetFaces( TSPShape shape );

/* Shape vertices */
int SP_Shape_GetVertexCount( TSPShape shape );
TVec3 * SP_Shape_GetVertices( TSPShape shape );

/* Shape scale */
TVec3 SP_Shape_GetLocalScale( TSPShape shape );
void SP_Shape_SetLocalScale( TSPShape shape, TVec3 scale );

/* Shape boundary */
float SP_Shape_GetBoundingRadius( TSPShape shape );
TVec3 SP_Shape_GetAABBMin( TSPShape shape );
TVec3 SP_Shape_GetAABBMax( TSPShape shape );

/* Shape type */
ESPShapeType SP_Shape_GetType( TSPShape shape );

/* Sphere shape */
float SP_Shape_Sphere_GetRadius( TSPShape shape );

/* Body creation */
TSPBody SP_Body_Create( TSPShape shape );

/* Body position */
void SP_Body_SetPosition( TSPBody body, TVec3 position );
TVec3 SP_Body_GetPosition( TSPBody body );

/* Body linear velocity */
void SP_Body_SetLinearVelocity( TSPBody body, TVec3 linVel );
TVec3 SP_Body_GetLinearVelocity( TSPBody body );

/* Body anisotropic friction */
void SP_Body_SetAnisotropicFriction( TSPBody body, TVec3 anisoFrict );
TVec3 SP_Body_GetAnisotropicFriction( TSPBody body );

/* Body gravity */
void SP_Body_SetGravity( TSPBody body, TVec3 gravity );
TVec3 SP_Body_GetGravity( TSPBody body );

/* Body contacts */
int SP_Body_GetContactCount( TSPBody body );
TVec3 SP_Body_GetContactPosition( TSPBody body, int contactN );
TVec3 SP_Body_GetContactNormal( TSPBody body, int contactN );
void * SP_Body_GetContactUserInfo( TSPBody body, int contactN );

/* Body shape */
TSPShape SP_Body_GetShape( TSPBody body );

#ifdef _cplusplus
};
#endif

#endif