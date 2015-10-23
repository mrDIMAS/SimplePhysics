//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Octree.h"
#include "GJK.h"
#include <float.h>

/*
====================================
Octree_Build
====================================
*/ 
void Octree_Build( TOctree * octree, TVec3 offset, TVec3 * vertices, TSPFace * faces, int faceCount ) {
    octree->root = calloc( 1, sizeof( TOctreeNode ));

    octree->root->min = Vec3_Set( FLT_MAX, FLT_MAX, FLT_MAX );
    octree->root->max = Vec3_Set( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( int i = 0; i < faceCount; i++ ) {
        TVec3 a = Vec3_Add( vertices[ faces[ i ].a ], offset );
        TVec3 b = Vec3_Add( vertices[ faces[ i ].b ], offset );
        TVec3 c = Vec3_Add( vertices[ faces[ i ].c ], offset );
        
        if( a.x < octree->root->min.x ) octree->root->min.x = a.x;
        if( b.x < octree->root->min.x ) octree->root->min.x = b.x;
        if( c.x < octree->root->min.x ) octree->root->min.x = c.x;

        if( a.y < octree->root->min.y ) octree->root->min.y = a.y;
        if( b.y < octree->root->min.y ) octree->root->min.y = b.y;
        if( c.y < octree->root->min.y ) octree->root->min.y = c.y;

        if( a.z < octree->root->min.z ) octree->root->min.z = a.z;
        if( b.z < octree->root->min.z ) octree->root->min.z = b.z;
        if( c.z < octree->root->min.z ) octree->root->min.z = c.z;

        if( a.x > octree->root->max.x ) octree->root->max.x = a.x;
        if( b.x > octree->root->max.x ) octree->root->max.x = b.x;
        if( c.x > octree->root->max.x ) octree->root->max.x = c.x;

        if( a.y > octree->root->max.y ) octree->root->max.y = a.y;
        if( b.y > octree->root->max.y ) octree->root->max.y = b.y;
        if( c.y > octree->root->max.y ) octree->root->max.y = c.y;

        if( a.z > octree->root->max.z ) octree->root->max.z = a.z;
        if( b.z > octree->root->max.z ) octree->root->max.z = b.z;
        if( c.z > octree->root->max.z ) octree->root->max.z = c.z;
    }

    octree->resultFaceCount = faceCount * 8;
    octree->resultFaceList = calloc( octree->resultFaceCount, sizeof( TSPFace ));

    Octree_BuildRecursiveInternal( octree->root, offset, vertices, faces, faceCount );
}
 
static inline float squared(float v) {
    return v * v;
}

/*
====================================
Octree_NodeIntersectSphere
====================================
*/
char Octree_NodeIntersectSphere( TOctreeNode * node, TVec3 pos, float radius ) {
    float r2 = squared( radius );
    float dmin = 0;

    if( pos.x < node->min.x ) {
        dmin += squared( pos.x - node->min.x );
    } else if( pos.x > node->max.x ) {
        dmin += squared( pos.x - node->max.x );
    }

    if( pos.y < node->min.y ) {
        dmin += squared( pos.y - node->min.y );
    } else if( pos.y > node->max.y ) {
        dmin += squared( pos.y - node->max.y );
    }

    if( pos.z < node->min.z ) {
        dmin += squared( pos.z - node->min.z );
    } else if( pos.z > node->max.z ) {
        dmin += squared( pos.z - node->max.z );
    }

    char sphereInside = (pos.x >= node->min.x) && (pos.x <= node->max.x) &&
                        (pos.y >= node->min.y) && (pos.y <= node->max.y) &&
                        (pos.z >= node->min.z) && (pos.z <= node->max.z);

    return dmin <= r2 || sphereInside;
}

/*
====================================
Octree_TraceSphereRecursiveInternal
====================================
*/
void Octree_TraceSphereRecursiveInternal( TOctree * octree, TOctreeNode * node, TVec3 pos, float radius ) {
    if( Octree_NodeIntersectSphere( node, pos, radius ) ) {
        if( node->split ) {
            for( int i = 0; i < 8; i++ ) {
                Octree_TraceSphereRecursiveInternal( octree, node->childs[i], pos, radius );
            }
        } else {
            for( int i = 0; i < node->faceCount; i++ ) {
                octree->resultFaceList[ octree->resultFaceCount++ ] = node->faces[ i ];
            }
        }
    }
}

/*
====================================
Octree_TraceSphere
====================================
*/
void Octree_TraceSphere( TOctree * octree, TVec3 pos, float radius ) {
    octree->resultFaceCount = 0;
    Octree_TraceSphereRecursiveInternal( octree, octree->root, pos, radius );
}

/*
====================================
Octree_IsPointInsideNode
====================================
*/
static bool inline Octree_IsPointInsideNode( TOctreeNode * node, TVec3 * point ) {
    return	point->x >= node->min.x && point->x <= node->max.x && point->y >= node->min.y && point->y <= node->max.y && point->z >= node->min.z && point->z <= node->max.z;
}

/*
====================================
Octree_BuildRecursiveInternal
====================================
*/
void Octree_BuildRecursiveInternal( TOctreeNode * node, TVec3 offset, TVec3 * vertices, TSPFace * faces, int faceCount ) {
    if( faceCount < 96 ) {        
        node->faceCount = faceCount;
        node->faces = malloc( sizeof( TSPFace ) * faceCount );
        memcpy( node->faces, faces, sizeof( TSPFace ) * faceCount );
        return;
    }

    Octree_SplitNode( node );

    for( int childNum = 0; childNum < 8; childNum++ ) {
        TOctreeNode * child = node->childs[childNum];

        TShape * box = ConvexShape_CreateBox( child->min, child->max );
        
        int tempFaceCount = 0;
        TSPFace * tempFaces = NULL;    
        for( int i = 0; i < faceCount; i++ ) {            
            TTriangleShape triangle = { 
                .v[0] = vertices[ faces[i].a ], 
                .v[1] = vertices[ faces[i].b ], 
                .v[2] = vertices[ faces[i].c ]
            };
            
            TShape shape = {  
                .position = offset, 
                .triangle = &triangle, 
                .localScale = Vec3_Set( 1.0f, 1.0f, 1.0f )
            };
            
            if( GJK_IsIntersects( &shape, box, NULL )) {
                int last = tempFaceCount++;
                tempFaces = realloc( tempFaces, tempFaceCount * sizeof( TSPFace ) );
                tempFaces[ last ] = faces[i];
            }
        }

        Octree_BuildRecursiveInternal( child, offset, vertices, tempFaces, tempFaceCount );

        free( tempFaces );
        
        ConvexShape_Delete( box );
    }
}

/*
====================================
Octree_BuildRecursiveInternal
====================================
*/
void Octree_SplitNode( TOctreeNode * node ) {
    TVec3 center = Vec3_Middle( node->min, node->max );

    for(int i = 0; i < 8; i++) {
        node->childs[i] = calloc( 1, sizeof( TOctreeNode ));
    }    

    node->childs[0]->min = Vec3_Set( node->min.x, node->min.y, node->min.z );    
    node->childs[0]->max = Vec3_Set( center.x, center.y, center.z ); 

    node->childs[1]->min = Vec3_Set( center.x, node->min.y, node->min.z );
    node->childs[1]->max = Vec3_Set( node->max.x, center.y, center.z );

    node->childs[2]->min = Vec3_Set( node->min.x, node->min.y, center.z );
    node->childs[2]->max = Vec3_Set( center.x, center.y, node->max.z );

    node->childs[3]->min = Vec3_Set( center.x, node->min.y, center.z );
    node->childs[3]->max = Vec3_Set( node->max.x, center.y, node->max.z );

    node->childs[4]->min = Vec3_Set( node->min.x, center.y, node->min.z );
    node->childs[4]->max = Vec3_Set( center.x, node->max.y, center.z );

    node->childs[5]->min = Vec3_Set( center.x, center.y, node->min.z );
    node->childs[5]->max = Vec3_Set( node->max.x, node->max.y, center.z );

    node->childs[6]->min = Vec3_Set( node->min.x, center.y, center.z );
    node->childs[6]->max = Vec3_Set( center.x, node->max.y, node->max.z );

    node->childs[7]->min = Vec3_Set( center.x, center.y, center.z );
    node->childs[7]->max = Vec3_Set( node->max.x, node->max.y, node->max.z );

    node->split = true;
    
    const float bias = 1.015f;
    for( int i = 0; i < 8; i++ ) {
        node->childs[i]->max = Vec3_Scale( node->childs[i]->max, bias );
        node->childs[i]->min = Vec3_Scale( node->childs[i]->min, bias );
    }
}

/*
====================================
Octree_TraceAABBRecursiveInternal
====================================
*/
bool Octree_IsNodeIntersectAABB( TOctreeNode * node, TVec3 min, TVec3 max ) {
    return Octree_IsPointInsideNode( node, &min ) || Octree_IsPointInsideNode( node, &max );
}

/*
====================================
Octree_TraceAABBRecursiveInternal
====================================
*/
void Octree_TraceAABBRecursiveInternal( TOctree * octree, TOctreeNode * node, TVec3 min, TVec3 max ) {
    if( Octree_IsNodeIntersectAABB( node, min, max ) ) {
        if( node->split ) {
            for( int i = 0; i < 8; i++ ) {
                Octree_TraceAABBRecursiveInternal( octree, node->childs[i], min, max );
            }
        } else {
            for( int i = 0; i < node->faceCount; i++ ) {
                octree->resultFaceList[ octree->resultFaceCount++ ] = node->faces[ i ];
            }
        }
    }
}

/*
====================================
Octree_TraceAABB
====================================
*/
void Octree_TraceAABB( TOctree * octree, TVec3 min, TVec3 max ) {
    octree->resultFaceCount = 0;
    Octree_TraceAABBRecursiveInternal( octree, octree->root, min, max );
}

/*
====================================
Octree_TraceRayRecursiveInternal
====================================
*/
bool Octree_TraceRayRecursiveInternal( TOctree * octree, TOctreeNode * node, TVec3 rayBegin, TVec3 rayDir ) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    if( rayDir.x >= 0 ) {
        tmin = (node->min.x - rayBegin.x) / rayDir.x;
        tmax = (node->max.x - rayBegin.x) / rayDir.x;
    } else {
        tmin = (node->max.x - rayBegin.x) / rayDir.x;
        tmax = (node->min.x - rayBegin.x) / rayDir.x;
    }
    if( rayDir.y >= 0 ) {
        tymin = (node->min.y - rayBegin.y) / rayDir.y;
        tymax = (node->max.y - rayBegin.y) / rayDir.y;
    } else {
        tymin = (node->max.y - rayBegin.y) / rayDir.y;
        tymax = (node->min.y - rayBegin.y) / rayDir.y;
    }
    if( (tmin > tymax) || (tymin > tmax) ) {
        return false;
    }
    if( tymin > tmin ) {
        tmin = tymin;
    }
    if( tymax < tmax ) {
        tmax = tymax;
    }
    if( rayDir.z >= 0 ) {
        tzmin = (node->min.z - rayBegin.z) / rayDir.z;
        tzmax = (node->max.z - rayBegin.z) / rayDir.z;
    } else {
        tzmin = (node->max.z - rayBegin.z) / rayDir.z;
        tzmax = (node->min.z - rayBegin.z) / rayDir.z;
    }
    if( (tmin > tzmax) || (tzmin > tmax) ) {
        return false;
    }
    if( tzmin > tmin ) {
        tmin = tzmin;
    }
    if( tzmax < tmax ) {
        tmax = tzmax;
    }
    if( (tmin < 1.0f) && (tmax > 0.0f)) {
        if( node->split ) {
            for( int i = 0; i < 8; i++ ) {
                Octree_TraceRayRecursiveInternal( octree, node->childs[i], rayBegin, rayDir );
            }
        } else {
            for( int i = 0; i < node->faceCount; i++ ) {
                octree->resultFaceList[ octree->resultFaceCount++ ] = node->faces[ i ];
            }
        }
    }
    return true;
}

void Octree_TraceRay( TOctree * octree, TVec3 rayBegin, TVec3 rayDir ) {
    octree->resultFaceCount = 0;
    Octree_TraceRayRecursiveInternal( octree, octree->root, rayBegin, rayDir );
}




