#include <iostream>
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

void init()
{
	glewExperimental=true;
	glewInit();
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, 640.0 / 480.0, 1.0, 100.0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
}

void draw()
{
	static float rotation{};
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0);
	glRotatef(rotation, 0.0, 1.0, 0.0);
	glutWireTeapot(1.0);
	glutSwapBuffers();
	rotation += 0.5;
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Hello OpenGL");
	init();
	glutIdleFunc(draw);
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutMainLoop();
}
