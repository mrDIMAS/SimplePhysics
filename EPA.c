//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "EPA.h"
#include "Body.h"

/*
====================================
EPA_ComputeContact
====================================
*/
bool EPA_ComputeContact( TPolytope * polytope, TShape * shape1, TShape * shape2, TBody * body2, TContact * outContact ) {
for( int i = 0; i < gSimulationParameters.epaConvergenceLimit; i++ ) {      
        TTriangle closestTriangle = Polytope_GetClosestTriangleToOrigin( polytope );
        if( closestTriangle.numInPolytope < 0 ) {
            return false; 
        }
        TSupport p = GJK_GetSupport( shape1, shape2, closestTriangle.normal );
        float d = Vec3_Dot( p.minkowskiDifference, closestTriangle.normal );
        if( d - closestTriangle.dist < gSimulationParameters.epaTolerance ) {      
            if( d < gSimulationParameters.epaPDTolerance ) {
                return false;
            }      
            TVec3 proj = Math_ProjectOriginOntoPlane( closestTriangle.a.minkowskiDifference, closestTriangle.normal );
            // get collision point in world coodinates
            float u, v, w;
            Math_GetBarycentricCoords( proj, closestTriangle.a.minkowskiDifference, closestTriangle.b.minkowskiDifference, closestTriangle.c.minkowskiDifference, &u, &v, &w );
            TVec3 a = Vec3_Scale( closestTriangle.a.firstShapeSupport, u );
            TVec3 b = Vec3_Scale( closestTriangle.b.firstShapeSupport, v );
            TVec3 c = Vec3_Scale( closestTriangle.c.firstShapeSupport, w );
            outContact->pointOnB = Vec3_Add( Vec3_Add( a, b ), c );
            outContact->normal = Vec3_Normalize( Vec3_Negate( closestTriangle.normal ));
            outContact->penetrationDepth = d;
            outContact->bodyB = body2;
            outContact->userInfo = shape2->triangle ? shape2->triangle->userInfo : NULL;
            return true;
        } else {                
            TEdgeList edgeList;            
            while( true ) {
                int seenFace = Polytope_GetFirstFaceSeenFromPoint( polytope, p.minkowskiDifference );
                if( seenFace < 0 ) {
                    break;
                } else {
                    Polytope_RemoveFace( polytope, seenFace );
                }
            }        
            EdgeList_Create( &edgeList, polytope );    
            EdgeList_Fill( &edgeList, polytope );
            EdgeList_MarkHoles( &edgeList );
            Polytope_PatchHoles( polytope, &edgeList, Polytope_AddVertex( polytope, p ) );              
        }
    }
    return false;
    /*
    if( polytope->faceCount == 0 ) {
        return false;
    }
    
    TTriangle closestTriangle;
    for( int i = 0; i < gSimulationParameters.epaConvergenceLimit; i++ ) {      
        closestTriangle = Polytope_GetClosestTriangleToOrigin( polytope );
        TSupport p = GJK_GetSupport( shape1, shape2, closestTriangle.normal );
        float d = fabsf( Vec3_Dot( p.minkowskiDifference, closestTriangle.normal ));
        if( d - closestTriangle.dist < gSimulationParameters.epaTolerance ) {      

            // get collision point in world coodinates
            float u, v, w;
            TVec3 proj = Math_ProjectOriginOntoPlane( closestTriangle.a.minkowskiDifference, closestTriangle.normal );    
            Math_GetBarycentricCoords( proj, closestTriangle.a.minkowskiDifference, closestTriangle.b.minkowskiDifference, closestTriangle.c.minkowskiDifference, &u, &v, &w );
            TVec3 a = Vec3_Scale( closestTriangle.a.firstShapeSupport, u );
            TVec3 b = Vec3_Scale( closestTriangle.b.firstShapeSupport, v );
            TVec3 c = Vec3_Scale( closestTriangle.c.firstShapeSupport, w );
            outContact->pointOnB = Vec3_Add( Vec3_Add( a, b ), c );
            
            outContact->normal = Vec3_Negate( Vec3_Normalize( closestTriangle.normal ));
            outContact->penetrationDepth = closestTriangle.dist;
            
            TVec3 offset;
            offset = Vec3_Scale( outContact->normal, outContact->penetrationDepth );
            (void)offset;
            
            outContact->bodyB = body2;
            outContact->userInfo = shape2->triangle ? shape2->triangle->userInfo : NULL;          
            
            return true;
        } else {                
            TEdgeList edgeList;            
            while( true ) {
                int seenFace = Polytope_GetFirstFaceSeenFromPoint( polytope, p.minkowskiDifference );
                if( seenFace < 0 ) {
                    break;
                } else {
                    Polytope_RemoveFace( polytope, seenFace );
                }
            }        
            EdgeList_Create( &edgeList, polytope );    
            EdgeList_Fill( &edgeList, polytope );
            EdgeList_MarkHoles( &edgeList );
            Polytope_PatchHoles( polytope, &edgeList, Polytope_AddVertex( polytope, p ) );              
        }
    }

    
    return false;*/
}