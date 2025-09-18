#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <time.h>

static int slices = 16;
static int stacks = 16;
float r, g, b, x, y;
bool check = true;

void mouse(int button, int state, int mousex, int mousey )
{
    if(button == GLUT_LEFT_BUTTON)
    {
        check = true;
        x = mousex;
        y = 480-mousey;
        r = (rand()%10)/10.0;
        g = (rand()%10)/10.0;
        b = (rand()%10)/10.0;
    }
    else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        glClearColor(1, 1, 1, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        check = false;
    }
    glutPostRedisplay();
}
void display(void)
{
    glColor3f(r, g, b);
    glPointSize(50);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
    if(check)
    {
        glBegin(GL_POINTS);
        glVertex2i(x,y);
        glEnd();
    }
    glFlush();
}
int main(int argc, char** argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Interacao via Mouse");
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
}
