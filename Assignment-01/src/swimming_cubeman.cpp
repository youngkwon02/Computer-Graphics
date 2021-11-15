//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;

float rotAngle = 0.0f;
int isDrawingCar = false;


float leftUpperArmAngle = glm::radians(0.0f);
float leftLowerArmAngle = glm::radians(0.0f);
float leftUpperLegAngle = glm::radians(30.0f);
float leftLowerLegAngle = 0.0f;

float rightUpperArmAngle = glm::radians(180.0f);
float rightLowerArmAngle = 0.0f;
float rightUpperLegAngle = glm::radians(-30.0f);
float rightLowerLegAngle = glm::radians(-60.0f);

int leftLowerArmDirection = true;
int rightLowerArmDirection = false;
int leftUpperLegDirection = true;
int rightUpperLegDirection = false;
int leftLowerLegDirection = true;
int rightLowerLegDirection = false;

int isUpperAngle = false;
float personPos = 0.0f;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f); // param1 : angle, param2 : left-right, param3: near, param4 : far  
	viewMat = glm::lookAt(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // param1 : eye position, param2 : origin point, param3 : up vector

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}


void drawPerson(glm::mat4 personMat) {
	glm::mat4 modelBody, modelHead, modelLeftArm, modelLeftForeArm, modelRightArm, modelRightForeArm,
		modelLeftUpperLeg, modelLeftLowerLeg, modelRightUpperLeg, modelRightLowerLeg, pvmMat;

	// Body
	modelBody = glm::scale(personMat, glm::vec3(0.4, 0.5, 0.2f));
	pvmMat = projectMat * viewMat * modelBody;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Head
	modelHead = glm::translate(modelBody, glm::vec3(0, 0.7f, 0));
	modelHead = glm::scale(modelHead, glm::vec3(0.5, 0.5, 0.7));
	pvmMat = projectMat * viewMat * modelHead;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Right Arm
	modelRightArm = glm::translate(modelBody, glm::vec3(0.70, 0.25f, 0.0));
	modelRightArm = glm::rotate(modelRightArm, rightUpperArmAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	modelRightArm = glm::scale(modelRightArm, glm::vec3(0.4, 0.9, 0.6));
	pvmMat = projectMat * viewMat * modelRightArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);


	// Right Forearm
	modelRightForeArm = glm::translate(modelRightArm, glm::vec3(0, -0.8, 0));
	modelRightForeArm = glm::rotate(modelRightForeArm, rightLowerArmAngle * 1.0f, glm::vec3(1, 0, 0));
	modelRightForeArm = glm::scale(modelRightForeArm, glm::vec3(1, 1.0, 0.8));
	pvmMat = projectMat * viewMat * modelRightForeArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Left Arm
	modelLeftArm = glm::translate(modelBody, glm::vec3(-0.70, 0.25f, 0));
	modelLeftArm = glm::rotate(modelLeftArm, leftUpperArmAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	modelLeftArm = glm::scale(modelLeftArm, glm::vec3(0.4, 0.9, 0.4));
	pvmMat = projectMat * viewMat * modelLeftArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Left Forearm
	modelLeftForeArm = glm::translate(modelLeftArm, glm::vec3(0, -0.8, 0));
	modelLeftForeArm = glm::rotate(modelLeftForeArm, leftLowerArmAngle * 1.0f, glm::vec3(1.0, 0, 0));
	modelLeftForeArm = glm::scale(modelLeftForeArm, glm::vec3(0.8, 1.0, 0.8));
	pvmMat = projectMat * viewMat * modelLeftForeArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Right Upper Leg
	modelRightUpperLeg = glm::translate(modelBody, glm::vec3(0.25, -0.9, 0));
	modelRightUpperLeg = glm::scale(modelRightUpperLeg, glm::vec3(0.4, (0.4 / 0.5), 0.6));
	modelRightUpperLeg = glm::rotate(modelRightUpperLeg, rightUpperLegAngle * 1.0f, glm::vec3(1, 0, 0));
	pvmMat = projectMat * viewMat * modelRightUpperLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// RIght Lower Leg
	modelRightLowerLeg = glm::translate(modelRightUpperLeg, glm::vec3(0, -0.9, 0));
	modelRightLowerLeg = glm::scale(modelRightLowerLeg, glm::vec3(1, 0.8, 1));
	modelRightLowerLeg = glm::rotate(modelRightLowerLeg, rightLowerLegAngle * 1.0f, glm::vec3(1, 0, 0));
	pvmMat = projectMat * viewMat * modelRightLowerLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Left Upper Leg
	modelLeftUpperLeg = glm::translate(modelBody, glm::vec3(-0.25, -0.9, 0));
	modelLeftUpperLeg = glm::scale(modelLeftUpperLeg, glm::vec3(0.4, (0.4 / 0.5), 0.6));
	modelLeftUpperLeg = glm::rotate(modelLeftUpperLeg, leftUpperLegAngle * 1.0f, glm::vec3(1, 0, 0));
	pvmMat = projectMat * viewMat * modelLeftUpperLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Left Lower Leg
	modelLeftLowerLeg = glm::translate(modelLeftUpperLeg, glm::vec3(0, -0.9, -0.0));
	modelLeftLowerLeg = glm::scale(modelLeftLowerLeg, glm::vec3(1, 0.8, 1));
	modelLeftLowerLeg = glm::rotate(modelLeftLowerLeg, leftLowerLegAngle * 1.0f, glm::vec3(1, 0, 0));
	pvmMat = projectMat * viewMat * modelLeftLowerLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

}


void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	worldMat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	if (isUpperAngle) {
		worldMat = glm::rotate(worldMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		worldMat = glm::rotate(worldMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	worldMat = glm::translate(worldMat, glm::vec3(0.0f, personPos, 0.0f));
	drawPerson(worldMat);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		rotAngle += glm::radians(t * 360.0f / 10000.0f);


		if (rightLowerArmAngle >= glm::radians(90.0f)) {
			rightLowerArmDirection = true;
		}
		else if (rightLowerArmAngle <= glm::radians(0.0f)) {
			rightLowerArmDirection = false;
		}

		if (leftLowerArmAngle >= glm::radians(90.0f)) {
			leftLowerArmDirection = true;
		}
		else if (leftLowerArmAngle <= glm::radians(0.0f)) {
			leftLowerArmDirection = false;
		}

		if (rightUpperLegAngle >= glm::radians(30.0f)) {
			rightUpperLegDirection = true;
		}
		else if (rightUpperLegAngle <= glm::radians(-30.0f)) {
			rightUpperLegDirection = false;
		}

		if (leftUpperLegAngle >= glm::radians(30.0f)) {
			leftUpperLegDirection = true;
		}
		else if (leftUpperLegAngle <= glm::radians(-30.0f)) {
			leftUpperLegDirection = false;
		}

		if (rightLowerLegAngle >= glm::radians(0.0f)) {
			rightLowerLegDirection = true;
		}
		else if (rightLowerLegAngle <= glm::radians(-60.0f)) {
			rightLowerLegDirection = false;
		}

		if (leftLowerLegAngle >= glm::radians(0.0f)) {
			leftLowerLegDirection = true;
		}
		else if (leftLowerLegAngle <= glm::radians(-60.0f)) {
			leftLowerLegDirection = false;
		}


		leftUpperArmAngle -= glm::radians(t * 360.0f / 5000.0f);
		//rightUpperArmAngle -= glm::radians(t * 360.0f / 5000.0f);
		leftLowerArmDirection ? leftLowerArmAngle -= glm::radians(t * 360.0f / 10000.0f) : leftLowerArmAngle += glm::radians(t * 360.0f / 10000.0f);
		//rightLowerArmDirection ? rightLowerArmAngle -= glm::radians(t * 360.0f / 10000.0f) : rightLowerArmAngle += glm::radians(t * 360.0f / 10000.0f);



		leftUpperLegDirection ? leftUpperLegAngle -= glm::radians(t * 360.0f / 10000.0f) : leftUpperLegAngle += glm::radians(t * 360.0f / 10000.0f);
		rightUpperLegDirection ? rightUpperLegAngle -= glm::radians(t * 360.0f / 10000.0f) : rightUpperLegAngle += glm::radians(t * 360.0f / 10000.0f);

		// 고급 애니메이션
		leftLowerLegDirection ? leftLowerLegAngle -= glm::radians(t * 360.0f / 10000.0f) : leftLowerLegAngle += glm::radians(t * 360.0f / 10000.0f);
		rightLowerLegDirection ? rightLowerLegAngle -= glm::radians(t * 360.0f / 10000.0f) : rightLowerLegAngle += glm::radians(t * 360.0f / 10000.0f);



		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a': case 'A':
		personPos += 0.1f;
		break;
	case 'd': case 'D':
		personPos -= 0.1f;
		break;
	case 'w': case 'W':
		isUpperAngle = !isUpperAngle;
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Swimming cubeman");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
