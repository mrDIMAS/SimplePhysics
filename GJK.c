//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "GJK.h"
#include "MathLib.h"

/*
====================================
GJK_GetSupport
====================================
*/
TSupport GJK_GetSupport( TShape * shape1, TShape * shape2, TVec3 dir ) {
    TSupport sup;
    sup.firstShapeSupport = ConvexShape_GetFarthestPointInDirection( shape1, dir );
    sup.minkowskiDifference = Vec3_Sub( sup.firstShapeSupport, ConvexShape_GetFarthestPointInDirection( shape2, Vec3_Negate( dir )));
    return sup;
}

/*
====================================
GJK_ProcessLine
====================================
*/
bool GJK_ProcessLine( TSimplex * simplex, TVec3 * outDirection ) {
    TVec3 a = simplex->points[1].minkowskiDifference;
    TVec3 b = simplex->points[0].minkowskiDifference;
    TVec3 ab = Vec3_Sub( b, a );
    TVec3 aO = Vec3_Negate( a );
   
    if( Helper_SameDirection( ab, aO )) {
        *outDirection = Vec3_Cross( Vec3_Cross( ab, aO ), ab );
    } else {
        Simplex_RemovePoint( simplex, 0 );
        *outDirection = aO;
    }
    return false;
}

/*
====================================
GJK_ProcessTriangle
====================================
*/ 
bool GJK_ProcessTriangle( TSimplex * simplex, TVec3 * outDirection ) {
    TVec3 a = simplex->points[2].minkowskiDifference;
    TVec3 b = simplex->points[1].minkowskiDifference;
    TVec3 c = simplex->points[0].minkowskiDifference;
    TVec3 aO = Vec3_Negate( a );
    TVec3 ab = Vec3_Sub( b, a );
    TVec3 ac = Vec3_Sub( c, a );
    TVec3 abc = Vec3_Cross( ab, ac );
    TVec3 acNormal = Vec3_Cross( abc, ac );
    TVec3 abNormal = Vec3_Cross( ab, abc );
   
    if( Helper_SameDirection( acNormal, aO )) {
        if( Helper_SameDirection( ac, aO )) {
            Simplex_RemovePoint( simplex, 1 );
            *outDirection = Vec3_Cross( Vec3_Cross(ac, aO), ac );
        } else {
            if( Helper_SameDirection( ab, aO )) {
                Simplex_RemovePoint( simplex, 0 );
                *outDirection = Vec3_Cross( Vec3_Cross(ab, aO), ab);
            } else {
                Simplex_RemovePoint( simplex, 1 );
                Simplex_RemovePoint( simplex, 0 );
                *outDirection = aO;
            }
        }
    } else {
        if( Helper_SameDirection( abNormal, aO )) {
            if( Helper_SameDirection( ab, aO )) {
                Simplex_RemovePoint( simplex, 0 );
                *outDirection = Vec3_Cross( Vec3_Cross(ab, aO), ab);
            } else {
                Simplex_RemovePoint( simplex, 1 );
                Simplex_RemovePoint( simplex, 0 );
                *outDirection = aO;
            }
        } else {
            if( Helper_SameDirection( abc, aO )) {
                *outDirection = Vec3_Cross(Vec3_Cross(abc, aO), abc);
            } else {
                *outDirection = Vec3_Cross(Vec3_Cross( Vec3_Negate( abc ), aO), Vec3_Negate( abc ) );
            }
        }
    }
   
    return false;
}
 
/*
====================================
GJK_ProcessTetrahedron
====================================
*/ 
bool GJK_ProcessTetrahedron( TSimplex * simplex, TVec3 * outDirection ) {
    TVec3 a = simplex->points[3].minkowskiDifference;
    TVec3 b = simplex->points[2].minkowskiDifference;
    TVec3 c = simplex->points[1].minkowskiDifference;
    TVec3 d = simplex->points[0].minkowskiDifference;
   
    TVec3 ac = Vec3_Sub( c, a );
    TVec3 ab = Vec3_Sub( b, a );
    TVec3 ad = Vec3_Sub( d, a );
   
    TVec3 acd = Vec3_Cross( ad, ac );
    TVec3 abd = Vec3_Cross( ab, ad );
    TVec3 abc = Vec3_Cross( ac, ab );
   
    TVec3 aO = Vec3_Negate( a );
   
    if( Helper_SameDirection( abc, aO )) {
        if( Helper_SameDirection( Vec3_Cross( abc, ac ), aO )) {
            Simplex_RemovePoint( simplex, 2 );
            Simplex_RemovePoint( simplex, 0 );
            *outDirection = Vec3_Cross( Vec3_Cross( ac, aO ), ac );
        } else if( Helper_SameDirection( Vec3_Cross( ab, abc ), aO )) {
            Simplex_RemovePoint( simplex, 1 );
            Simplex_RemovePoint( simplex, 0 );
            *outDirection = Vec3_Cross( Vec3_Cross( ab, aO ), ab );
        } else {
            Simplex_RemovePoint( simplex, 0 );
            *outDirection = abc;
        }
    } else if( Helper_SameDirection( acd, aO )) {
        if( Helper_SameDirection( Vec3_Cross( acd, ad ), aO )) {
            Simplex_RemovePoint( simplex, 2 );
            Simplex_RemovePoint( simplex, 1 );
            *outDirection = Vec3_Cross( Vec3_Cross( ad, aO ), ad );
        } else if ( Helper_SameDirection( Vec3_Cross( ac, acd ), aO )) {
            Simplex_RemovePoint( simplex, 2 );
            Simplex_RemovePoint( simplex, 0 );
            *outDirection = Vec3_Cross( Vec3_Cross( ac, aO ), ac );
        } else {
           Simplex_RemovePoint( simplex, 2 );
            *outDirection = acd;
        }
    } else if( Helper_SameDirection( abd, aO )) {
        if( Helper_SameDirection( Vec3_Cross( abd, ab ), aO )) {
            Simplex_RemovePoint( simplex, 1 );
            Simplex_RemovePoint( simplex, 0 );
            *outDirection = Vec3_Cross( Vec3_Cross( ab, aO ), ab );
        } else if( Helper_SameDirection( Vec3_Cross( ad, abd ), aO )) {
            Simplex_RemovePoint( simplex, 2 );
            Simplex_RemovePoint( simplex, 1 );
            *outDirection = Vec3_Cross( Vec3_Cross( ad, aO ), ad );
        } else {
            Simplex_RemovePoint( simplex, 1 );
            *outDirection = abd;
        }
    } else {
        return true;
    }
   
    return false;
}

/*
====================================
GJK_ProcessSimplex
====================================
*/ 
bool GJK_ProcessSimplex( TSimplex * simplex, TVec3 * outDirection ) {
    if( simplex->size == 2 ) {
        return GJK_ProcessLine( simplex, outDirection );
    } else if ( simplex->size == 3 ) {
        return GJK_ProcessTriangle( simplex, outDirection );
    } else if( simplex->size == 4 ) {
        return GJK_ProcessTetrahedron( simplex, outDirection );
    }
    return false;
}

/*
====================================
GJK_FixSimplex
====================================
*/
void GJK_FixSimplex( TSimplex * simplex, TShape * shape1, TShape * shape2 ) {
    const float epsilon = 0.0001f;
    static const TVec3 directions[6] = {
        { .x =  1.0f,  .y =  0.0f,  .z =  0.0f }, 
        { .x = -1.0f,  .y =  0.0f,  .z =  0.0f }, 
        { .x =  0.0f,  .y =  1.0f,  .z =  0.0f }, 
        { .x =  0.0f,  .y = -1.0f,  .z =  0.0f }, 
        { .x =  0.0f,  .y =  0.0f,  .z =  1.0f }, 
        { .x =  0.0f,  .y =  0.0f,  .z = -1.0f }
    };    
    static const TVec3 axes[3] = {
        { .x =  1.0f,  .y = 0.0f,   .z = 0.0f  },
        { .x =  0.0f,  .y = 1.0f,   .z = 0.0f  },
        { .x =  0.0f,  .y = 0.0f,   .z = 1.0f  }
    };
    switch( simplex->size ) {
        // case 'fall-through' is need to continuously add new points to the simplex
        
        // our simplex is a point
        case 1: {
            TSupport additionalSupport;
            for( int i = 0; i < 6; i++ ) {
                additionalSupport = GJK_GetSupport( shape1, shape2, directions[i] );
                if( Vec3_SqrDistance( additionalSupport.minkowskiDifference, simplex->points[0].minkowskiDifference ) > epsilon ) {
                    Simplex_AddPoint( simplex, additionalSupport );
                    break;
                }
            }
        }

        // our simplex is a line
        case 2: {
            TVec3 line = Vec3_Sub( simplex->points[1].minkowskiDifference, simplex->points[0].minkowskiDifference );            
            int lsc = Math_LeastSignificantComponent( line );
            TVec3 dir = Vec3_Cross( line, axes[lsc] );            
            TMat3 mat60 = Mat3_AxisAngle( line, M_PI / 3 );            
            for( int i = 0; i < 6; i++ ) {
                TSupport additionalSupport = GJK_GetSupport( shape1, shape2, dir );
                if( Vec3_SqrLength( additionalSupport.minkowskiDifference ) > epsilon ) {
                    Simplex_AddPoint( simplex, additionalSupport );
                    break;
                }                
                dir = Vec3_Transform( dir, mat60 );
            }
        }
        
        // our simplex is a triangle
        case 3: {
            TVec3 ab = Vec3_Sub( simplex->points[1].minkowskiDifference, simplex->points[0].minkowskiDifference );
            TVec3 ac = Vec3_Sub( simplex->points[2].minkowskiDifference, simplex->points[0].minkowskiDifference );
            TVec3 normal = Vec3_Cross( ab, ac );
            TSupport additionalSupport = GJK_GetSupport( shape1, shape2, normal );
            if( Vec3_SqrLength( additionalSupport.minkowskiDifference ) < epsilon ) {
                normal = Vec3_Negate( normal );
                additionalSupport = GJK_GetSupport( shape1, shape2, normal );
            }
            Simplex_AddPoint( simplex, additionalSupport );
        }
    }
}
 
/*
====================================
GJK_IsIntersects
====================================
*/
bool GJK_IsIntersects( TShape * shape1, TShape * shape2, TSimplex * finalSimplex ) {
    TVec3 dir = Vec3_Set( 1, 1, 1 );
   
    TSimplex simplex = { 0 };
    Simplex_AddPoint( &simplex, GJK_GetSupport( shape1, shape2, dir ) );
   
    dir = Vec3_Negate( dir );
    
    for( int i = 0; i < gSimulationParameters.gjkConvergenceLimit; i++ ) {
        TSupport lastSupport = GJK_GetSupport( shape1, shape2, dir );              
        if( Helper_SameDirection( dir, lastSupport.minkowskiDifference )) {
            Simplex_AddPoint( &simplex, lastSupport );            
            if( GJK_ProcessSimplex( &simplex, &dir )) {
                if( finalSimplex ) {
                    *finalSimplex = simplex;
                }
                return true;
            }            
        } else {
            return false;
        }
    }
    
    // The GJK algorithm can end up with a non-tetrahedron result, this happens when point, line or plane of triangle 
    // contains the origin, so we must 'blow-up' simplex to a tetrahedron before pass simplex to EPA
    if( finalSimplex ) {
        if( simplex.size > 0 ) {
            GJK_FixSimplex( &simplex, shape1, shape2 );
            if( simplex.size == 4 ) {            
                *finalSimplex = simplex;            
                return true;
            } else {
                // should never happen
                return false;
            }
        } else {
            return false;
        }
    } else {
        return true;
    }
}