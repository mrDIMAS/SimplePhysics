//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "MathLib.h"

/*
====================================
Math_LeastSignificantComponent
====================================
*/
int Math_LeastSignificantComponent( TVec3 v ) {
    const float epsilon = 0.0001f;
    if( v.x >= epsilon && v.y > epsilon && v.z > epsilon ) {
        return 0;
    } else if ( v.x > epsilon && v.y >= epsilon && v.z > epsilon ) {
        return 1;
    } else if ( v.x > epsilon && v.y > epsilon && v.z >= epsilon ) {
        return 2;
    } else {
        return -1; // should never happen
    }
}

/*
====================================
Mat3_AxisAngle
====================================
*/
TMat3 Mat3_AxisAngle( TVec3 axis, float angle ) {
    float cosTheta = cosf( angle );
    float sinTheta = sinf( angle );
    float oneMinusCosTheta = 1.0f - cosTheta;
    return (TMat3) { 
        .m[0][0] = cosTheta + axis.x * axis.x * oneMinusCosTheta,
        .m[0][1] = axis.x * axis.y * oneMinusCosTheta - axis.z * sinTheta,
        .m[0][2] = axis.x * axis.z * oneMinusCosTheta + axis.y * sinTheta,                     
        .m[1][0] = axis.y * axis.x * oneMinusCosTheta + axis.z * sinTheta,
        .m[1][1] = cosTheta + axis.y * axis.y * oneMinusCosTheta,
        .m[1][2] = axis.y * axis.z * oneMinusCosTheta - axis.x * sinTheta,                     
        .m[2][0] = axis.z * axis.x * oneMinusCosTheta - axis.y * sinTheta,
        .m[2][1] = axis.z * axis.y * oneMinusCosTheta + axis.x * sinTheta,
        .m[2][2] = cosTheta + axis.z * axis.z * oneMinusCosTheta 
    };
}

/*
====================================
Vec3_Rotate
====================================
*/
TVec3 Vec3_Transform( TVec3 v, TMat3 mat ) {
    return (TVec3) { 
         .x = mat.m[0][0] * v.x + mat.m[1][0] * v.y + mat.m[2][0] * v.z,
         .y = mat.m[0][1] * v.x + mat.m[1][1] * v.y + mat.m[2][1] * v.z,
         .z = mat.m[0][2] * v.x + mat.m[1][2] * v.y + mat.m[2][2] * v.z 
    };
}

/*
====================================
Helper_GetFarthestPointInDirection
====================================
*/
TVec3 Helper_GetFarthestPointInDirection( TVec3 * points, int count, TVec3 dir ) {
    int nFarthest = 0;
    float maxDot = -FLT_MAX;
    for( int i = 0; i < count; ++i ) {
        float dot = Vec3_Dot( dir, points[i] );
        if( dot > maxDot ) {
            nFarthest = i;
            maxDot = dot;
        }
    }
    return points[ nFarthest ];
}

/*
====================================
Math_GetBarycentricCoords
====================================
*/
void Math_GetBarycentricCoords( TVec3 p, TVec3 a, TVec3 b, TVec3 c, float *u,float *v,float *w ) {
     TVec3 v0 = Vec3_Sub( b, a );
     TVec3 v1 = Vec3_Sub( c, a );
     TVec3 v2 = Vec3_Sub( p, a );
     float d00 = Vec3_Dot( v0, v0 );
     float d01 = Vec3_Dot( v0, v1 );
     float d11 = Vec3_Dot( v1, v1 );
     float d20 = Vec3_Dot( v2, v0 );
     float d21 = Vec3_Dot( v2, v1 );
     float denom = d00 * d11 - d01 * d01;
     *v = (d11 * d20 - d01 * d21) / denom;
     *w = (d00 * d21 - d01 * d20) / denom;
     *u = 1.0f - *v - *w; 
}

/*
====================================
Vec3_Project
====================================
*/
TVec3 Vec3_Project( TVec3 a, TVec3 planeNormal ) {
    float nlen = Vec3_SqrLength( planeNormal );
    // normal vector is degenerated 
    if( nlen < 0.001f ) {
        return Vec3_Zero();
    }
    float t = Vec3_Dot( a, planeNormal ) / nlen;   
    return Vec3_Sub( a, Vec3_Scale( planeNormal, t ));
}