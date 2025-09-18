#include <GL/glut.h>
#include <stdlib.h>

GLfloat escala = 0.0f;
void teclado (unsigned char tecla,GLint x, GLint y)
{

    switch(tecla)
    {
    case '+' :
        //escala ++;
        escala=escala+0.05f;
        break;
    case '-' :
        //escala --;
         escala=escala-0.05f;
        break;
    }

    glutPostRedisplay();


}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    //define a matriz de transformação da câmera
    glMatrixMode(GL_PROJECTION);
    //seta a matrix identidade para a câmera
    glLoadIdentity();
    //define área de enquadramento da cena
    gluOrtho2D(-3, 3, -3, 3);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);

    glScalef(escala, escala, 0);

    //define a matrix de transformação dos modelos
    glMatrixMode(GL_MODELVIEW);
    //seta a matrix identidade para o modelos
    glLoadIdentity();

    glLineWidth(10);
    glBegin(GL_LINES);
    glVertex2f(-2.0f, 0.0f);
    glVertex2f(2.0f, 0.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1,1,0);
    glVertex2f(-2.0f,-2.0f);
    glVertex2f(2.0f,-2.0f);
    glColor3f(5,3,9);
    glVertex2f(0.0f,2.0f);
    glEnd();

    glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    //as letras maiuscula sao constantes
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("GLUT Shapes");
    glutKeyboardFunc(teclado);
    glClearColor(0.4, 0, 1, 0);
    glutDisplayFunc(display);
    glutMainLoop();
    return EXIT_SUCCESS;
}
