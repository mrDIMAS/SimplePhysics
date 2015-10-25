//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _MATH_LIB_H_
#define _MATH_LIB_H_

#include "Common.h"

typedef struct TMat3 {
    float m[3][3];
} TMat3;

#ifndef M_PI
#   define M_PI 3.14159
#endif

int Math_LeastSignificantComponent( TVec3 v );
TMat3 Mat3_AxisAngle( TVec3 axis, float angle );
TVec3 Vec3_Transform( TVec3 v, TMat3 mat );
void Helper_GetMinMax( int count, TVec3 * points, TVec3 * min, TVec3 * max );
TVec3 Helper_GetFarthestPointInDirection( TVec3 * points, int count, TVec3 dir );
void Math_GetBarycentricCoords( TVec3 p, TVec3 a, TVec3 b, TVec3 c, float *u,float *v,float *w );
TVec3 Vec3_Project( TVec3 a, TVec3 planeNormal );

/*
====================================
Helper_SameDirection
====================================
*/
static inline bool Helper_SameDirection( TVec3 a, TVec3 b ) {
    return Vec3_Dot( a, b ) > 0;
}

/*
====================================
Math_ProjectOriginOntoPlane
====================================
*/
static inline TVec3 Math_ProjectOriginOntoPlane( TVec3 planePoint, TVec3 planeNormal ) {
    /*
    planeNormal = Vec3_Normalize( planeNormal );
    return Vec3_Scale( planeNormal, Vec3_Dot( planePoint, planeNormal ));
    */
    TVec3 o = Vec3_Zero();
    TVec3 pq1 = Vec3_Sub( o, planePoint );
    float t = Vec3_Dot( pq1, planeNormal );
    return Vec3_Sub( o, Vec3_Scale( planeNormal, t ));
}
 
/*
====================================
Math_DistanceToOrigin
====================================
*/
static inline float Math_DistanceToOrigin( TVec3 normal, TVec3 point ) {
    return Vec3_Dot( normal, point );
}
  
/*
====================================
Math_Sqr
====================================
*/ 
static inline float Math_Sqr( float v ) {
    return v * v;
}


#endif