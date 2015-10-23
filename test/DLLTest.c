//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "../SimplePhysics.h"
#include "../Vector3.h"

// this headers for rendering 
#include <windows.h>
#include "glut.h"
#include "gl/gl.h" 
#include <stdlib.h>
#include <stdio.h>

float sceneAngle = 0;
 
TSPShape currentShape1;
TSPShape currentShape2;
TSPShape currentShape3;
TSPShape currentShape4;

TSPBody body1;
TSPBody body2;
TSPBody body3;
TSPBody body4;

void Renderer_DrawWireBox( TVec3 min, TVec3 max ) {
    TVec3 v0 = Vec3_Set( min.x, min.y, min.z );   
    TVec3 v1 = Vec3_Set( min.x, min.y, max.z );
    TVec3 v2 = Vec3_Set( max.x, min.y, max.z );
    TVec3 v3 = Vec3_Set( max.x, min.y, min.z );   

    TVec3 v4 = Vec3_Set( min.x, max.y, min.z );   
    TVec3 v5 = Vec3_Set( min.x, max.y, max.z );
    TVec3 v6 = Vec3_Set( max.x, max.y, max.z );
    TVec3 v7 = Vec3_Set( max.x, max.y, min.z ); 
    
    glDisable( GL_LIGHTING );
    
    glBegin( GL_LINES );

    glVertex3f( v0.x, v0.y, v0.z );
    glVertex3f( v1.x, v1.y, v1.z );
 
    glVertex3f( v1.x, v1.y, v1.z );
    glVertex3f( v2.x, v2.y, v2.z );

    glVertex3f( v2.x, v2.y, v2.z ); 
    glVertex3f( v3.x, v3.y, v3.z );
      
    glVertex3f( v3.x, v3.y, v3.z );
    glVertex3f( v0.x, v0.y, v0.z );

    glVertex3f( v4.x, v4.y, v4.z );
    glVertex3f( v5.x, v5.y, v5.z );
 
    glVertex3f( v5.x, v5.y, v5.z );
    glVertex3f( v6.x, v6.y, v6.z );

    glVertex3f( v6.x, v6.y, v6.z ); 
    glVertex3f( v7.x, v7.y, v7.z );
      
    glVertex3f( v7.x, v7.y, v7.z );
    glVertex3f( v4.x, v4.y, v4.z );
  
    glVertex3f( v4.x, v4.y, v4.z );
    glVertex3f( v0.x, v0.y, v0.z );
 
    glVertex3f( v1.x, v1.y, v1.z );
    glVertex3f( v5.x, v5.y, v5.z );

    glVertex3f( v2.x, v2.y, v2.z ); 
    glVertex3f( v6.x, v6.y, v6.z );
      
    glVertex3f( v3.x, v3.y, v3.z );
    glVertex3f( v7.x, v7.y, v7.z );
 
    glEnd();
    
    glEnable( GL_LIGHTING );
}
 
/*
====================================
Renderer_DrawFaceList
====================================
*/ 
void Renderer_DrawFaceList( TVec3 * vertices, int faceCount, TSPFace * faces ) {
    glBegin(GL_TRIANGLES);
    for( int i = 0; i < faceCount; i++ ) {    
        
        TVec3 a = vertices[ faces[ i ].a ];
        TVec3 b = vertices[ faces[ i ].b ];
        TVec3 c = vertices[ faces[ i ].c ];
        
        TVec3 normal = Vec3_Cross( Vec3_Sub( b, a), Vec3_Sub( c, a ));
        
        glNormal3f( normal.x, normal.y, normal.z );
        
        glVertex3f( a.x, a.y, a.z );
        glVertex3f( b.x, b.y, b.z );
        glVertex3f( c.x, c.y, c.z );           
        
    }
    glEnd();
}

/*
====================================
Renderer_DrawShape
====================================
*/ 
void Renderer_DrawBody( TSPBody body ) {
    TSPShape shape = SP_Body_GetShape( body );
    TVec3 pos = SP_Body_GetPosition( body );
    TVec3 scale = SP_Shape_GetLocalScale( shape );
    
    glPushMatrix();        
    glScalef( scale.x, scale.y, scale.z );
    glTranslatef( pos.x, pos.y, pos.z );     
    
    if( SP_Shape_GetType( shape ) == SPSHAPE_CONVEX || SP_Shape_GetType( shape ) == SPSHAPE_TRIMESH ) {
        Renderer_DrawFaceList( SP_Shape_GetVertices( shape ), SP_Shape_GetFaceCount( shape ), SP_Shape_GetFaces( shape ) );        
    } else if( SP_Shape_GetType( shape ) == SPSHAPE_SPHERE ) {         
        glutSolidSphere( SP_Shape_Sphere_GetRadius( shape ), 32, 32 );             
    };
    
    glPopMatrix();
}

/*
====================================
Renderer_DrawShape
====================================
*/  
void Renderer_Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 0, 0.78, 3.5, 0, 0, 0, 0, 1, 0 );
    glRotatef( sceneAngle, 0, 1, 0 );
    
    Renderer_DrawBody( body1 );
    Renderer_DrawBody( body2 );
    Renderer_DrawBody( body3 );  
    Renderer_DrawBody( body4 );  
    
    glutSwapBuffers();
    glutPostRedisplay();
}
 
/*
====================================
Renderer_Init
====================================
*/  
void Renderer_Init() {
    glClearColor(0.000, 0.110, 0.392, 0.0);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 80, 2560.0f / 1440.0f, 0.01, 1024 );
      
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glEnable( GL_TEXTURE_2D );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
   
    glEnable( GL_CULL_FACE );
    
    float dir[4] = { 1, 1, 1, 0 };
    glLightfv( GL_LIGHT0, GL_POSITION, dir );
   
    glDisable( GL_STENCIL_TEST );
    glCullFace( GL_BACK );    
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.025f );
 
    currentShape1 = SP_Shape_CreateFromObj( "cylinder.obj", false );
    currentShape2 = SP_Shape_CreateFromObj( "trimesh.obj", true );
    currentShape3 = SP_Shape_CreateFromObj( "box.obj", false );
    currentShape4 = SP_Shape_CreateFromObj( "capsule.obj", false );
     
    body1 = SP_Body_Create( currentShape1 );
    SP_Body_SetPosition( body1, Vec3_Set( 0, 200.5, 0 ));
    
    body2 = SP_Body_Create( currentShape2 );    
    
    body3 = SP_Body_Create( currentShape3 );
    SP_Body_SetPosition( body3, Vec3_Set( 0, 1.5, 0 ));
    
    body4 = SP_Body_Create( currentShape4 );
    SP_Body_SetPosition( body4, Vec3_Set( 0.0f, 5, 0 ));  
     
    SP_World_Simulate();
}


/*
====================================
Time_GetMilliseconds
====================================
*/ 
double Time_GetMilliseconds( ) {
    LARGE_INTEGER freq;
    LARGE_INTEGER time;
    QueryPerformanceFrequency ( &freq );        
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart * 1000.0 ) / ( double ) ( freq.QuadPart );
}


void Helper_SetBodyXZSpeed( TSPBody body, float x, float z ) {
    SP_Body_SetLinearVelocity( body, Vec3_Set( x, SP_Body_GetLinearVelocity( body ).y, z ));
}

void Helper_SetBodyYSpeed( TSPBody body, float y ) {
    SP_Body_SetLinearVelocity( body, Vec3_Set( SP_Body_GetLinearVelocity( body ).x, y, SP_Body_GetLinearVelocity( body ).z ));
}

/*
====================================
Renderer_Idle
====================================
*/  
void Renderer_Idle() {
    TSPBody * b = body4;
    
    float moveSpeed = 0.015f;
    
    SP_Body_SetAnisotropicFriction( b, Vec3_Set( 0.1f, 0.0f, 0.1f ));
    
    float xSpeed = 0.0f, zSpeed = 0.0f;
    if( GetAsyncKeyState( 'A' ) ) {
        xSpeed += -moveSpeed;
    }
    if( GetAsyncKeyState( 'D' ) ) {
        xSpeed += moveSpeed;
    }
    if( GetAsyncKeyState( 'W' ) ) {
        zSpeed += -moveSpeed;
    }
    if( GetAsyncKeyState( 'S' ) ) {
        zSpeed += moveSpeed;
    } 
    
    Helper_SetBodyXZSpeed( b, xSpeed, zSpeed );
    
    if( GetAsyncKeyState( VK_SPACE ) && SP_Body_GetContactCount( b ) > 0 ) {
        for( int i = 0; i < SP_Body_GetContactCount( b ); i++ ) {
            if( Vec3_Angle( Vec3_Set( 0, 1, 0 ), SP_Body_GetContactNormal( b, i )) < M_PI / 3.0 ) {
                Helper_SetBodyYSpeed( b, 0.02f );
                break;
            }            
        }
    }
    if( GetAsyncKeyState( 'Q' )  ) {
        sceneAngle += 1.0f;
    }
    if( GetAsyncKeyState( 'E' )  ) {
        sceneAngle -= 1.0f;
    } 
    
    static float scl = 1.0f;
    
    if( GetAsyncKeyState( 'C' )) {
        scl -= 0.01f;
        if( scl < 0.6f ) {
            scl = 0.6f;
        }
    } else {
        scl += 0.01f;
        if( scl > 1.0f ) {
            scl = 1.0f;
        }
    }
    
    SP_Shape_SetLocalScale( SP_Body_GetShape( b ), Vec3_Set( 1.0f, scl, 1.0f ));
    
    double lastTime = Time_GetMilliseconds();
    
    SP_World_Simulate();
    
    printf( "Physics frame time: %f ms\n", Time_GetMilliseconds() - lastTime );   
    
}
 
/*
====================================
main
====================================
*/  
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowSize( GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ) );
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");
    glutDisplayFunc(Renderer_Display);
    glutIdleFunc(  Renderer_Idle );
    
    Renderer_Init();
 
    glutMainLoop();
}

