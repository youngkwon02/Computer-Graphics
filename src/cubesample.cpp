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

float rightUpperAngle = 0.0f;
float leftUpperAngle = 0.0f;

float rightLowerAngle = 0.0f;
float leftLowerAngle = 0.0f;


int isDrawingCar = false;

int rightUpperDirect = false;
int leftUpperDirect = false;

int rightLowerDirect = false;
int leftLowerDirect = false;

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

//----------------------------------------------------------------------------

void drawCar(glm::mat4 carMat)
{
	glm::mat4 carBody, carTop, carWheel, pvmMat;
	glm::vec3 wheelPos[4];

	

	wheelPos[0] = glm::vec3(0.3, 0.24, -0.6); // rear right
	wheelPos[1] = glm::vec3(0.3, -0.24, -0.6); // rear left
	wheelPos[2] = glm::vec3(-0.3, 0.24, -0.6); // front right
	wheelPos[3] = glm::vec3(-0.3, -0.24, -0.6); // front left

	// car body
	carBody = glm::scale(carMat, glm::vec3(1, 0.6, 0.2));
	pvmMat = projectMat * viewMat * carBody;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// car top
	carTop = glm::translate(carMat, glm::vec3(0, 0, 0.2));  //P*V*C*T*S*v  바디의 높이만큼 올렸음
	carTop = glm::scale(carTop, glm::vec3(0.5, 0.6, 0.2));
	pvmMat = projectMat * viewMat * carTop;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// car wheel
	for (int i = 0; i < 4; i++)
	{
		carWheel = glm::translate(carMat, wheelPos[i]);  //P*V*C*T*S*v
		carWheel = glm::scale(carWheel, glm::vec3(0.2, 0.1, 0.2));
		carWheel = glm::rotate(carWheel, -rotAngle*2.0f, glm::vec3(1, 0, 0));
		pvmMat = projectMat * viewMat * carWheel;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}
}

void drawPerson(glm::mat4 personMat) {
	glm::mat4 modelBody, modelhead, modelLeftArm, modelLeftForeArm, modelRightArm, modelRightForeArm, 
		modelLeftUpLeg, modelLeftLowLeg, modelRightUpLeg, modelRightLowLeg, pvmMat;

	// person body
	modelBody = glm::scale(personMat, glm::vec3(0.4, 0.5, 0.4));
	pvmMat = projectMat * viewMat * modelBody;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person head
	modelhead = glm::translate(modelBody, glm::vec3(0, 0.7f, 0));
	modelhead = glm::scale(modelhead, glm::vec3(0.5, (0.2/0.5), 0.5));
	pvmMat = projectMat * viewMat * modelhead;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person right arm
	modelRightArm = glm::translate(modelBody, glm::vec3(0, 0.05, 0.70));
	modelRightArm = glm::scale(modelRightArm, glm::vec3(0.4, (0.3 / 0.5), 0.4));
	modelRightArm = glm::rotate(modelRightArm, rightUpperAngle * 1.0f, glm::vec3(0.0f, 0.0f, 0.1f));
	pvmMat = projectMat * viewMat * modelRightArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	
	// person right hand
	modelRightForeArm = glm::translate(modelRightArm, glm::vec3(0, -0.95, 0));
	modelRightForeArm = glm::scale(modelRightForeArm, glm::vec3(1, 0.9, 1));
	modelRightForeArm = glm::rotate(modelRightForeArm, rightLowerAngle * 1.0f, glm::vec3(0.0f, 0.0f, 0.1f));
	pvmMat = projectMat * viewMat * modelRightForeArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person left arm
	modelLeftArm = glm::translate(modelBody, glm::vec3(0, 0.05, -0.70)); 
	modelLeftArm = glm::scale(modelLeftArm, glm::vec3(0.4, (0.3 / 0.5), 0.4));
	modelLeftArm = glm::rotate(modelLeftArm, leftUpperAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelLeftArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person left hand
	modelLeftForeArm = glm::translate(modelLeftArm, glm::vec3(0, -0.95, 0)); 
	modelLeftForeArm = glm::scale(modelLeftForeArm, glm::vec3(1, 0.9, 1));
	modelLeftForeArm = glm::rotate(modelLeftForeArm, leftLowerAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelLeftForeArm;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person right leg
	modelRightUpLeg = glm::translate(modelBody, glm::vec3(0, -0.9, 0.25));
	modelRightUpLeg = glm::scale(modelRightUpLeg, glm::vec3(0.4, (0.4 / 0.5), 0.4));
	modelRightUpLeg = glm::rotate(modelRightUpLeg, leftUpperAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelRightUpLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person right foot
	modelRightLowLeg = glm::translate(modelRightUpLeg, glm::vec3(0, -0.9, 0));
	modelRightLowLeg = glm::scale(modelRightLowLeg, glm::vec3(1, 0.8, 1));
	modelRightLowLeg = glm::rotate(modelRightLowLeg, leftLowerAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelRightLowLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person left leg
	modelLeftUpLeg = glm::translate(modelBody, glm::vec3(0, -0.9, -0.25)); 
	modelLeftUpLeg = glm::scale(modelLeftUpLeg, glm::vec3(0.4, (0.4 / 0.5), 0.4));
	modelLeftUpLeg = glm::rotate(modelLeftUpLeg, rightUpperAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelLeftUpLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// person left foot
	modelLeftLowLeg = glm::translate(modelLeftUpLeg, glm::vec3(0, -0.9, -0.0));
	modelLeftLowLeg = glm::scale(modelLeftLowLeg, glm::vec3(1, 0.8, 1));
	modelLeftLowLeg = glm::rotate(modelLeftLowLeg, rightLowerAngle * 1.0f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelLeftLowLeg;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

}


void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//worldMat = glm::rotate(glm::mat4(1.0f), rotAngle, glm::vec3(1.0f, -1.0f, 1.0f)); // param1 : 기존 매트릭스(이전 트랜스포메이션?), param1 : rotate angle, param3 : 회전축
	worldMat = glm::translate(glm::mat4(1.0f), glm::vec3(personPos, 0.0f, 0.0f));
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
		rotAngle += glm::radians(t*360.0f / 10000.0f);

		if (rightUpperAngle >= glm::radians(30.0f)) rightUpperDirect = true;
		else if (rightUpperAngle <= glm::radians(-50.0f)) rightUpperDirect = false;
		
		rightUpperDirect ? rightUpperAngle -= glm::radians(t * 360.0f / 5000.0f) : rightUpperAngle += glm::radians(t * 360.0f / 5000.0f);

		if (leftUpperAngle <= glm::radians(-30.0f)) leftUpperDirect = true;
		else if (leftUpperAngle >= glm::radians(50.0f)) leftUpperDirect = false;
		
		leftUpperDirect ? leftUpperAngle += glm::radians(t * 360.0f / 5000.0f) : leftUpperAngle -= glm::radians(t * 360.0f / 5000.0f);


		if (rightLowerAngle >= glm::radians(10.0f)) rightLowerDirect = true;
		else if (rightLowerAngle <= glm::radians(-30.0f)) rightLowerDirect = false;

		rightLowerDirect ? rightLowerAngle -= glm::radians(t * 360.0f / 10000.0f) : rightLowerAngle += glm::radians(t * 360.0f / 10000.0f);

		if (leftLowerAngle <= glm::radians(-10.0f)) leftLowerDirect = true;
		else if (leftLowerAngle >= glm::radians(30.0f)) leftLowerDirect = false;

		leftLowerDirect ? leftLowerAngle += glm::radians(t * 360.0f / 10000.0f) : leftLowerAngle -= glm::radians(t * 360.0f / 10000.0f);


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
		personPos -= 0.01f;
		break;
	case 'd': case 'D':
		personPos += 0.01f;
		break;
	case 'c': case 'C':
		isDrawingCar = !isDrawingCar;
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
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Car");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
