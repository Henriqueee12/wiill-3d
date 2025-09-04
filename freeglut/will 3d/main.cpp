#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

static int slices = 16;
static int stacks = 16;
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    // define a area de enquadramento da cena
    gluOrtho2D(-3, 3, -3, 3);
    glLineWidth(100);
    glBegin(GL_LINES);
      glVertex2f(-2.0f, 0.0f);
      glVertex2f(2.0f, 0.0f);
      glEnd();
      glFlush();


}
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("GLUT Shapes");
    glClearColor(0, 0, 1, 0);
    glutDisplayFunc(display);
    glutMainLoop();
    return EXIT_SUCCESS;
}
