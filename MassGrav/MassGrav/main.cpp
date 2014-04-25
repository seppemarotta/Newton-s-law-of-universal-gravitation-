//
//  main.cpp
//  MassGrav
//
//  Created by Giuseppe Marotta Portal on 4/22/14.
//  Copyright (c) 2014 Giuseppe. All rights reserved.
#include <GL/glut.h>

#include <vector>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

struct Particle
{
    dvec3 pos;
    dvec3 vel;
    double mass;
    dvec3 force;
    
    void Integrate( double dt )
    {
        dvec3 a = force / mass;
        
        // semi-implicit euler
        vel = vel + a * dt;
        pos = pos + vel * dt;
    }
};

const double BigGee = 1.0;
void Integrate( vector< Particle >& particles, double dt )
{
    for( size_t i = 0; i < particles.size(); ++i )
    {
        Particle& p1 = particles[i];
        p1.force = dvec3( 0 );
        
        for( size_t j = 0; j < particles.size(); ++j )
        {
            if( i == j )
            {
                continue;
            }
            
            const Particle& p2 = particles[j];
            
            // http://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
            const dvec3 rHat = glm::normalize( p2.pos - p1.pos );
            const double r2 = glm::distance2( p1.pos, p2.pos );
            p1.force += BigGee * ( ( p1.mass * p2.mass ) / r2 ) * rHat;
        }
    }
    
    for( size_t i = 0; i < particles.size(); ++i )
    {
        particles[i].Integrate( dt );
    }
}

vector< Particle > particles;
void display()
{
    static int last = glutGet(GLUT_ELAPSED_TIME);
    int cur = glutGet(GLUT_ELAPSED_TIME);
    double dt = ( cur - last ) / 1000.0;
    last = cur;
    
    Integrate( particles, dt );
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double w = glutGet( GLUT_WINDOW_WIDTH );
    double h = glutGet( GLUT_WINDOW_HEIGHT );
    double ar = w / h;
    glOrtho( -100 * ar, 100 * ar, -100, 100, -1, 1 );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    glPointSize( 5 );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_DOUBLE, sizeof( Particle ), &particles[0].pos[0] );
    glDrawArrays( GL_POINTS, 0, particles.size() );
    glDisableClientState( GL_VERTEX_ARRAY );
    
    glutSwapBuffers();
}

void timer( int value )
{
    glutPostRedisplay();
    glutTimerFunc( 16, timer, 0 );
}

int main(int argc, char **argv)
{
    particles.resize( 2 );
    
    // "sun"
    particles[0].pos = dvec3( 0, 0, 0 );
    particles[0].vel = dvec3( 0, 0, 0 );
    particles[0].mass = 100000;
    
    // "planet"
    particles[1].pos = dvec3( 50, 0, 0 );
    particles[1].vel = dvec3( 0, -20, 0 );
    particles[1].mass = 1;
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "GLUT" );
    
    glutDisplayFunc( display );
    glutTimerFunc( 0, timer, 0 );
    glutMainLoop();
    return 0;
}