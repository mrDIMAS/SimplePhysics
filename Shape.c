//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Shape.h"
#include "MathLib.h"

/*
====================================
ConvexShape_GetFarthestPointInDirection
====================================
*/
TVec3 ConvexShape_GetFarthestPointInDirection( TShape * shape, TVec3 dir ) {
    TVec3 farthest;
    if( shape->convex ) {
        farthest = Helper_GetFarthestPointInDirection( shape->convex->points, shape->convex->count, dir );
    } else if( shape->sphere ) {
        farthest = Vec3_Scale( Vec3_Normalize( dir ), shape->sphere->radius );
    } else if( shape->triangle ) {        
        farthest = Helper_GetFarthestPointInDirection( shape->triangle->v, 3, dir );
    } else {
        farthest = Vec3_Zero();
    }
    return Vec3_Mul( Vec3_Add( farthest, shape->position ), shape->localScale );
}

/*
====================================
Shape_Create
====================================
*/
TShape * Shape_Create( ) {
    TShape * shape = calloc( 1, sizeof( TShape ));
    shape->localScale = Vec3_Set( 1.0f, 1.0f, 1.0f );
    return shape;
}

/*
====================================
ConvexShape_CreateSphere
====================================
*/
TShape * ConvexShape_CreateSphere( TVec3 position, float radius ) {
    TShape * shape = Shape_Create();
    shape->sphere = calloc( 1, sizeof( TSphereShape ));    
    shape->position = position;
    shape->sphere->radius = radius;
    
    shape->boundingRadius = radius;
    shape->center = Vec3_Set( 0.0f, 0.0, 0.0f );
    
    return shape; 
}

/*
====================================
ConvexShape_Delete
====================================
*/ 
TShape * ConvexShape_CreateBox( TVec3 min, TVec3 max ) {
    TShape * shape = Shape_Create();
    shape->convex = calloc( 1, sizeof( TConvexShape ));    

    shape->convex->count = 8;
    shape->convex->points = malloc( shape->convex->count * sizeof( TVec3 ));
    
    shape->convex->points[0] = Vec3_Set( min.x, min.y, min.z );   
    shape->convex->points[1] = Vec3_Set( min.x, min.y, max.z );
    shape->convex->points[2] = Vec3_Set( max.x, min.y, max.z );
    shape->convex->points[3] = Vec3_Set( max.x, min.y, min.z );   

    shape->convex->points[4] = Vec3_Set( min.x, max.y, min.z );   
    shape->convex->points[5] = Vec3_Set( min.x, max.y, max.z );
    shape->convex->points[6] = Vec3_Set( max.x, max.y, max.z );
    shape->convex->points[7] = Vec3_Set( max.x, max.y, min.z ); 
    
    shape->boundingRadius = Vec3_Distance( max, min ) / 2;
    shape->center = Vec3_Set( 0.0f, 0.0, 0.0f );
    
    return shape;
}

/*
====================================
Helper_GetMinMax
====================================
*/ 
void Helper_GetMinMax( int count, TVec3 * points, TVec3 * min, TVec3 * max ) {
    *min = Vec3_Set( FLT_MAX, FLT_MAX, FLT_MAX );
    *max = Vec3_Set( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( int i = 0; i < count; i++ ) {
        if( points[i].x < min->x ) min->x = points[i].x;
        if( points[i].y < min->y ) min->y = points[i].y;
        if( points[i].z < min->z ) min->z = points[i].z;
        if( points[i].x > max->x ) max->x = points[i].x;
        if( points[i].y > max->y ) max->y = points[i].y;
        if( points[i].z > max->z ) max->z = points[i].z;
    }
}

/*
====================================
ConvexShape_CalculateCenter
====================================
*/ 
TVec3 ConvexShape_CalculateCenter( TShape * shape ) {
    TVec3 min = Vec3_Zero(), max = Vec3_Zero();
    if( shape->convex ) {
        Helper_GetMinMax( shape->convex->count, shape->convex->points, &min, &max );
    }
    if( shape->triMesh ) {
        Helper_GetMinMax( shape->triMesh->vertexCount, shape->triMesh->vertices, &min, &max );
    }
    if( shape->sphere ) {
        return Vec3_Set( 0.0f, 0.0f, 0.0f );
    } else {
        return Vec3_Middle( max, min );
    }
}

/*
====================================
ConvexShape_Delete
 * - computes radius of sphere that fully contain the shape
====================================
*/ 
float ConvexShape_CalculateCircumcircleRadius( TShape * shape ) {
    TVec3 min = Vec3_Zero(), max = Vec3_Zero();
    if( shape->convex ) {
        Helper_GetMinMax( shape->convex->count, shape->convex->points, &min, &max );
    }
    if( shape->triMesh ) {
        Helper_GetMinMax( shape->triMesh->vertexCount, shape->triMesh->vertices, &min, &max );
    }
    if( shape->sphere ) {
        return shape->sphere->radius;
    } else {
        return Vec3_Distance( max, min ) / 2;
    }
}

/*
====================================
ConvexShape_Delete
====================================
*/ 
TShape * ConvexShape_CreateFromMemory( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces ) {
    TShape * shape = Shape_Create();
    shape->convex = calloc( 1, sizeof( TConvexShape ));    

    shape->convex->count = vertexCount;
    shape->convex->points = calloc( shape->convex->count, sizeof( TVec3 ));
    memcpy( shape->convex->points, vertices, vertexCount * sizeof( TVec3 ));
    
    if( faces && faceCount > 0 ) {
        shape->convex->faceCount = faceCount; 
        shape->convex->faces = calloc( shape->convex->faceCount, sizeof( TSPFace ));
        memcpy( shape->convex->faces, faces, faceCount * sizeof( TSPFace ));
    }
    
    shape->boundingRadius = ConvexShape_CalculateCircumcircleRadius( shape );
    shape->center = ConvexShape_CalculateCenter( shape );
    
    return shape;
}

/*
====================================
ConvexShape_Delete
====================================
*/
void ConvexShape_Delete( TShape * shape ) {
    if( shape->convex ) {
        if( shape->convex->faces ) {
            free( shape->convex->faces );
        }
        if( shape->convex->points ) {
            free( shape->convex->points );
        }
        free( shape->convex );
    }
    if( shape->triMesh ) {
        if( shape->triMesh->faces ) {
            free( shape->triMesh->faces );
        }
        if( shape->triMesh->vertices ) {
            free( shape->triMesh->vertices );
        }
    }
    if( shape->sphere ) {
        free( shape->sphere );
    }
    free( shape );
}

/*
====================================
TriangleMesh_CreateFromMemory
====================================
*/
TShape * TriangleMesh_CreateFromMemory( int vertexCount, TVec3 * vertices, int faceCount, TSPFace * faces ) {
    TShape * shape = Shape_Create();
    TTriangleMeshShape * triMesh = calloc( 1, sizeof( TTriangleMeshShape ));
    
    triMesh->vertexCount = vertexCount;
    triMesh->vertices = calloc( vertexCount, sizeof( TVec3 ));
    memcpy( triMesh->vertices, vertices, vertexCount * sizeof( TVec3 ));

    triMesh->faceCount = faceCount;
    triMesh->faces = calloc( triMesh->faceCount, sizeof( TSPFace ));
    memcpy( triMesh->faces, faces, faceCount * sizeof( TSPFace ));
    
    Octree_Build( &triMesh->octree, Vec3_Set( 0, 0, 0 ), vertices, faces, faceCount );
    
    shape->triMesh = triMesh;
    return shape;
}

/*
====================================
Helper_TraceRay
====================================
*/
void Helper_TraceRay( TVec3 * vertices, int faceCount, TSPFace * faces, TVec3 rayBegin, TVec3 rayDir, bool infiniteRay, int outCountContacts, TSPRayTraceResult * outResultList ) {
    int contactCount = 0;
    for( int i = 0; i < faceCount; i++ ) {     
        TVec3 a = vertices[ faces[i].a ];
        TVec3 b = vertices[ faces[i].b ];
        TVec3 c = vertices[ faces[i].c ];
        TVec3 normal = Vec3_Cross( Vec3_Sub( b, a ), Vec3_Sub( c, a ));
        // solve plane equation 
        float u = -( Vec3_Dot( rayBegin, normal ) - Vec3_Dot( a, normal ));
        float v = Vec3_Dot( rayDir, normal );
        float t = u / v;
        // ray hit
        if( t > 0.0f ) {         
            bool intersectTrianglePlane;
            if( infiniteRay ) {
                intersectTrianglePlane = true;
            } else { // ray is a line segment
                intersectTrianglePlane = ( t >= 0.0f ) && ( t <= 1.0f );
            }
            if( intersectTrianglePlane ) {            
                // find intersection point 
                TVec3 intersectionPoint = Vec3_Add( rayBegin, Vec3_Scale( rayDir, t ));        
                float u, v, w;
                Math_GetBarycentricCoords( intersectionPoint, a, b, c, &u, &v, &w );
                if( (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f) ) {
                    outResultList[contactCount].body = NULL;
                    outResultList[contactCount].normal = normal;
                    outResultList[contactCount].position = intersectionPoint;
                    contactCount++;
                    if( contactCount >= outCountContacts ) {
                        break;
                    }
                }
            } 
        }
    }
}


/*
====================================
Shape_TraceRay
====================================
*/
void Shape_TraceRay( TShape * shape, TVec3 rayBegin, TVec3 rayDir, bool infiniteRay, int outCountContacts, TSPRayTraceResult * outResultList ) {
    if( shape->convex ) {
        Helper_TraceRay( shape->convex->points, shape->convex->faceCount, shape->convex->faces, rayBegin, rayDir, infiniteRay, outCountContacts, outResultList );
    } else if( shape->sphere ) {
        TVec3 d = Vec3_Sub( rayBegin, shape->position );
        float a = Vec3_Dot( rayDir, rayDir );
        float b = 2.0f * Vec3_Dot( rayDir, d );
        float c = Vec3_Dot( d, d ) - shape->sphere->radius * shape->sphere->radius;
        float discriminant = b * b - 4 * a * c;
        if( discriminant > 0.0f ) {
            float discrRoot = sqrt( discriminant );
            // find roots of quadratic equation 
            float r1 = (-b + discrRoot) / 2.0f;
            float r2 = (-b - discrRoot) / 2.0f;
            bool intersection;
            if( infiniteRay ) {
                intersection = true;
            } else {
                intersection = ((r1 >= 0.0f) && (r1 <= 1.0f)) || ((r2 >= 0.0f) && (r2 <= 1.0f));
            }            
            if( intersection ) {    
                // to get correct normals in two intersection points we must ensure that point2 is farthest
                if( r1 > r2 ) {
                    float temp = r1;
                    r1 = r2;
                    r2 = temp;
                }
                outResultList[0].position = Vec3_Add( rayBegin, Vec3_Scale( rayDir, r1 ));        
                outResultList[0].normal = rayDir;
                outResultList[0].body = NULL;
                if( outCountContacts > 1 ) {
                    outResultList[1].position = Vec3_Add( rayBegin, Vec3_Scale( rayDir, r2 ));
                    outResultList[1].normal = Vec3_Negate( rayDir ); // other side of sphere
                    outResultList[1].body = NULL;
                }
            }
        }
    } else if( shape->triMesh ) {
        Helper_TraceRay( shape->triMesh->vertices, shape->triMesh->faceCount, shape->triMesh->faces, rayBegin, rayDir, infiniteRay, outCountContacts, outResultList );
    }
}