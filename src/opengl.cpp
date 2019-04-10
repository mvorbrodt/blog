#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	cout << "Vendor          : " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer        : " << glGetString(GL_RENDERER) << endl;
	cout << "Version         : " << glGetString(GL_VERSION) << endl;
	cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << "Extensions      : " << glGetString(GL_EXTENSIONS) << endl;

	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, 640.0 / 480.0, 1.0, 100.0);

	float rotation = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -3.0);
		glRotatef(rotation, 0.0, 1.0, 0.0);

		glutWireTeapot(1.0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		rotation += 0.5;
	}

	glfwTerminate();
}
