
#include <OpenGL/gl.h>  // GLUT, include glu.h and gl.h
#include <GLUT/GLUT.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#define GE 6.67259
#define RAD 100

#include "glm.hpp"
#include "norm.hpp"
using namespace glm;
using namespace std;


/* Global variables */
char title[] = "3D Shapes";

typedef struct particulas{
    double x,y,z; // se va a comenza
    dvec3 pos;
    double mass;
    dvec3 vel;
    dvec3 force;
    void Drawparticulas();
    particulas(double px,double py,double pz,double pfactor){
        pos=dvec3(px,py,pz);
        mass=pfactor;
        
    }
    void DrawPoints();
    
    void Integrate( double dt )
    {
        dvec3 a = force / mass;
        
        // semi-implicit euler
        vel = vel + a * dt;
        pos = pos + vel * dt;
    }
    
    
};
double camx,camy,camz;



vector <particulas*> mundo;

/* Initialize OpenGL Graphics */
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}
void particulas::DrawPoints(){
    glBegin(GL_POINTS);// Specify point drawing
    glColor3f(1,1,1);
    glVertex3f(pos.x, pos.y, pos.z);
    glEnd();
}

void particulas::Drawparticulas()
{
    // clear the drawing buffer.
    glPushMatrix();
    //glLoadIdentity();
    glTranslatef(x,y,z);
    //escala al tamaño pedido
    glScalef(mass,mass,mass);
    glBegin(GL_QUADS);        // Draw The Cube Using quads
    glColor3f(0.0f,1.0f,0.0f);    // Color Blue
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)
    glColor3f(1.0f,0.5f,0.0f);    // Color Orange
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)
    glColor3f(1.0f,0.0f,0.0f);    // Color Red
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)
    glColor3f(1.0f,0.0f,1.0f);    // Color Violet
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)
    glEnd();            // End Drawing The Cube
    
    glPopMatrix();
}

void animation(void)
{

}

const double BigGee = 1.0;
void Integrate( double dt )
{
    for( size_t i = 0; i < mundo.size(); ++i )
    {
        particulas* p1 = mundo[i];
        p1->force = dvec3( 0 );
        
        for( size_t j = 0; j < mundo.size(); ++j )
        {
            if( i == j )
            {
                continue;
            }
            
            const particulas* p2 = mundo[j];
            
            // http://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
            const dvec3 rHat = glm::normalize( p2->pos - p1->pos );
            const double r2 = glm::distance2( p1->pos, p2->pos );
            p1->force += BigGee * ( ( p1->mass * p2->mass ) / r2 ) * rHat;
        }
    }
    
    for( size_t i = 0; i < mundo.size(); ++i )
    {
        mundo[i]->Integrate( dt );
    }
}



/* Handler for window-repaint event. Called back when the window first appears and
 whenever the window needs to be re-painted. */
void display() {
    
    static int last = glutGet(GLUT_ELAPSED_TIME);
    int cur = glutGet(GLUT_ELAPSED_TIME);
    double dt = ( cur - last ) / 1000.0;
    last = cur;
    
    Integrate(dt);

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
    // Render a color-cube consisting of 6 quads with different colors
    glLoadIdentity();
    
    gluLookAt (0.0, 200, 250, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(camx,0.0,1.0,0.0);
    glRotatef(camy,1.0,0,1.0);
    // Reset the model-view matrix
     // Move right and into the screen
  
    mundo[0]->Drawparticulas();
    for(int i=1;i<mundo.size();i++)
        mundo[i]->DrawPoints();
    glBegin(GL_LINES);
    glVertex3f(-RAD, 0.0, -RAD);
    glVertex3f(RAD, 0, -RAD);
    
    glVertex3f(RAD, 0, -RAD);
    glVertex3f(RAD, 0, RAD);
    
    glVertex3f(RAD, 0, RAD);
    glVertex3f(-RAD, 0, RAD);
    
    glVertex3f(-RAD, 0, RAD);
    glVertex3f(-RAD, 0, -RAD);
    
    glEnd();
    
    
   
    glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
    
}
void keyboard_function( unsigned char ch , int x , int y)
{
    if(ch==GLUT_KEY_RIGHT){
        camx+=5;
    }
    else if(ch==GLUT_KEY_LEFT){
        camx-=5;
    }
    else if (ch==GLUT_KEY_DOWN){
        
    }
    else if(ch==GLUT_KEY_UP)
    {
        
    }
    else exit(0);
    
}

/* Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLdouble aspect = (GLdouble)width / (GLdouble)height;
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 1000.0f);
}
long long num=0;
void timer_function( int value)
{
    num++;
    if(num>10)
    animation();
    
    glutPostRedisplay();
    glutTimerFunc( 200,timer_function , 0);
}


/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    mundo.resize(2000);
    mundo[0]=new particulas(0,0,0,5); // centro
    camx=45;
    camy=camz=-10;
    for(int i=1;i<mundo.size();i++){
        double x= -(RAD+100)/2  + rand()%(RAD+100);
        double y= -(RAD+100)/2  + rand()%(RAD+100);
        double z= -(RAD+100)/2  + rand()%(RAD+100);
        double r= 1+rand()%3;
        //  printf("%lf %lf %lf %lf\n",x,y,z,r);
        mundo[i]=new particulas(x,y,z,r);
    }    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(800, 600);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title
    glutKeyboardFunc( keyboard_function);
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event
    initGL();                       // Our own OpenGL initialization
    glutTimerFunc (1, timer_function,0);
    glutMainLoop();                 // Enter the infinite event-processing loop
    return 0;
}