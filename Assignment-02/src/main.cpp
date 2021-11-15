#include "initShader.h"
#include "sphere.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

enum eShadeMode { NO_LIGHT, GOURAUD, PHONG, NUM_LIGHT_MODE };

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f);

int shadeMode = NO_LIGHT;
int isTexture = false;
int isRotate = false;

GLuint projectMatrixID;
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint shadeModeID;
GLuint textureModeID;

float rotAngle = 0.0f;
float rightArmAngle = glm::radians(180.0f);
float leftArmAngle = glm::radians(0.0f);
float rightUpperLegAngle = glm::radians(20.0f);
float leftUpperLegAngle = glm::radians(-20.0f);


int rightUpperLeg_up = false;
int leftUpperLeg_up = true;


Sphere sphere(0, 0); // Whatever you pass, it would be rassigned

// OpenGL initialization
void init()
{
	// Create a vertex array object
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// Create and initialize a buffer object
	GLuint buffer[1];
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	int vertSize = sizeof(sphere.verts[0]) * sphere.verts.size();
	int normalSize = sizeof(sphere.normals[0]) * sphere.normals.size();
	int texSize = sizeof(sphere.texCoords[0]) * sphere.texCoords.size();
	glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize + texSize,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, sphere.verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normalSize, sphere.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize + normalSize, texSize, sphere.texCoords.data());

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize + normalSize));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	shadeModeID = glGetUniformLocation(program, "shadeMode");
	glUniform1i(shadeModeID, shadeMode);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// Load the texture using BMP File
	GLuint Texture = loadBMP_custom("wood.bmp");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(program, "sphereTexture");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawMan(glm::mat4 worldMat) {

	// Body 
	modelMat = glm::scale(worldMat, glm::vec3(1.4, 0.6, 0.6));
	modelMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	// Head
	modelMat = glm::translate(worldMat, glm::vec3(-1.0, 0, 0.15));
	modelMat = glm::scale(modelMat, glm::vec3(0.4, 0.4, 0.4));
	modelMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());


	glm::vec3 armPosArr[4];
	// Arms Position Setting
	armPosArr[0] = glm::vec3(-0.6, -0.7, 0.35);
	armPosArr[1] = glm::vec3(-0.3, 0.7, 0.35);

	//left arm
	modelMat = glm::translate(worldMat, armPosArr[0]);
	modelMat = glm::rotate(modelMat, leftArmAngle, glm::vec3(0, 1, 0));
	modelMat = glm::scale(modelMat, glm::vec3(1.0, 0.3, 0.4));
	modelMat = glm::translate(modelMat, glm::vec3(0.4, 0, 0));
	modelMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	//right arm
	modelMat = glm::translate(worldMat, armPosArr[1]);  //P*V*C*T*S*R*T*v
	modelMat = glm::rotate(modelMat, rightArmAngle, glm::vec3(0, 1, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5, 0.2, 0.2));
	modelMat = glm::translate(modelMat, glm::vec3(0.4, 0, 0));
	modelMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());


	glm::vec3 legPosArr[4];
	// Legs Position Setting
	// Upper Legs
	legPosArr[0] = glm::vec3(0.9, 0.15, 0.2);
	legPosArr[1] = glm::vec3(0.9, -0.15, 0.2);
	// Lower Legs
	legPosArr[2] = glm::vec3(0.8, 0, 0.2);
	legPosArr[3] = glm::vec3(0.8, 0, 0.2);

	//Left Upper Leg
	glm::mat4 modelLeft;
	modelLeft = glm::translate(worldMat, legPosArr[0]);
	modelLeft = glm::rotate(modelLeft, leftUpperLegAngle, glm::vec3(0, 1, 0));
	modelLeft = glm::scale(modelLeft, glm::vec3(0.65, 0.4, 0.3));
	modelLeft = glm::translate(modelLeft, glm::vec3(-0.1, 0, 0));
	modelMat = projectMat * viewMat * modelLeft;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	//Left Lower Leg
	glm::mat4 modelLeftLow;
	modelLeftLow = glm::translate(modelLeft, legPosArr[2]);
	modelLeftLow = glm::rotate(modelLeftLow, glm::radians(10.0f), glm::vec3(0, 1, 0));
	modelLeftLow = glm::scale(modelLeftLow, glm::vec3(0.7, 1, 0.7));
	modelMat = projectMat * viewMat * modelLeftLow;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	//Right Upper Leg
	glm::mat4 modelRight;
	modelRight = glm::translate(worldMat, legPosArr[1]);
	modelRight = glm::rotate(modelRight, rightUpperLegAngle, glm::vec3(0, 1, 0));
	modelRight = glm::scale(modelRight, glm::vec3(0.65, 0.4, 0.3));
	modelRight = glm::translate(modelRight, glm::vec3(-0.05, 0, 0));
	modelMat = projectMat * viewMat * modelRight;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	//Right Lower Leg
	glm::mat4 modelRightLow;
	modelRightLow = glm::translate(modelRight, legPosArr[3]);
	modelRightLow = glm::rotate(modelRightLow, glm::radians(10.0f), glm::vec3(0, 1, 0));
	modelRightLow = glm::scale(modelRightLow, glm::vec3(0.7, 1, 0.7));
	modelMat = projectMat * viewMat * modelRightLow;
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());
}

void display(void)
{
	glm::mat4 worldMat;
	worldMat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawMan(worldMat);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());

	glutSwapBuffers();
}

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	if (isRotate && abs(currTime - prevTime) >= 25)
	{
		float time = abs(currTime - prevTime);
		float speed = 360.0f / 1000.0f;

		rightArmAngle += glm::radians(time * 360.0f / 4000.0f);
		leftArmAngle += glm::radians(time * 360.0f / 4000.0f);

		if (leftUpperLegAngle <= glm::radians(-20.0f)) {
			leftUpperLeg_up = true;
		}
		else if (leftUpperLegAngle >= glm::radians(20.0f)) {
			leftUpperLeg_up = false;
		}

		if (rightUpperLegAngle <= glm::radians(-20.0f)) {
			rightUpperLeg_up = true;
		}
		else if (rightUpperLegAngle >= glm::radians(20.0f)) {
			rightUpperLeg_up = false;
		}

		if (leftUpperLeg_up) {
			leftUpperLegAngle += glm::radians(time * 360.0f / 3600.0f);
		}
		else {
			leftUpperLegAngle -= glm::radians(time * 360.0f / 3600.0f);
		}

		if (rightUpperLeg_up) {
			rightUpperLegAngle += glm::radians(time * 360.0f / 3600.0f);
		}
		else {
			rightUpperLegAngle -= glm::radians(time * 360.0f / 3600.0f);
		}

		prevTime = currTime;
		glutPostRedisplay();
	}
	else if (!isRotate)
	{
		prevTime = currTime;
	}
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'l': case 'L':
		shadeMode = (++shadeMode % NUM_LIGHT_MODE);
		glUniform1i(shadeModeID, shadeMode);
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		isRotate = !isRotate;
		glutPostRedisplay();
		break;
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 033:
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}


void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
