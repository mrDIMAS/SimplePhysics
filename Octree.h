//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "SimplePhysics.h"

typedef struct SOctreeNode {
    TSPFace * faces;
    int faceCount;
    char split;
    TVec3 min;
    TVec3 max;
    struct SOctreeNode * childs[8];
} TOctreeNode;

#define OCTREE_MAX_SIMULTANEOUS_THREADS (8)

typedef struct SOctree {
    TSPFace * resultFaceList;
    int resultFaceCount;
    TOctreeNode * root;
} TOctree;

void Octree_Build( TOctree * octree, TVec3 offset, TVec3 * vertices, TSPFace * faces, int faceCount );
void Octree_BuildRecursiveInternal( TOctreeNode * node, TVec3 offset, TVec3 * vertices, TSPFace * faces, int faceCount );
void Octree_SplitNode( TOctreeNode * node );

void Octree_TraceSphere( TOctree * octree, TVec3 pos, float radius );
void Octree_TraceAABB( TOctree * octree, TVec3 min, TVec3 max );
void Octree_TraceRay( TOctree * octree, TVec3 rayBegin, TVec3 rayDir );

#endif