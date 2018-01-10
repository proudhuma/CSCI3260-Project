#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Dependencies\glui\glui.h"
#include "Dependencies\include\irrKlang.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "shader_install.h"
#include "Loading_func.h"
#include "Bind_Data.h"
using namespace irrklang;
using namespace std;
using glm::vec3;
using glm::mat4;
// ============================= //
// window size
int Win_w, Win_h;
float camera_fov = 45.0;
unsigned int amount = 500;
GLuint rock_amount = 5000;
mat4 *rock_RandMatrices;

int  mainWindowID;
GLint programID;
GLint skyboxProgramID;
GLint planetC_programID;
GLint earthProgramID;
GLint lightboxProgramID;
GLint rockProgramID;

GLuint TextureEarth_0;
GLuint TextureEarth_1;
GLuint TexturePlanetC_1;
GLuint TexturePlanetC_0;
GLuint TexturePlanetB;
GLuint TextureObjD;
GLuint TextureObjG;
GLuint TextureSkybox;
GLuint TextureBox;


GLuint skyboxVAO;
GLuint skyboxVBO;
extern GLuint earthVao;
extern GLuint planetCVao;
extern GLuint planetBVao;
extern GLuint objDVao;
extern GLuint objGVao[500];
extern GLuint boxVao;

extern int drawEarthSize;
extern int drawPlanetCSize;
extern int drawPlanetBSize;
extern int drawObjDSize;
extern int drawObjGSize[500];
extern int drawBoxSize;


// view matrix
glm::mat4 common_viewM;
glm::mat4 common_projection;
glm::mat4 ViewSwitch_Mat;

float earth_innRot_Degree = 0.0f;
float light_Rot_Degree = 0.0f;

// ============================= //
int fogon = 1;
float fogr = 0.5;
float fogg = 0.5;
float fogb= 0.5;


const float M_PI = 3.14159265;
float radius = 30.0f;
float initViewHorizontal = -90.0f;
float initViewVertical = -90.0f;

float cameraX = 0;
float cameraY = 0;
float cameraZ = radius;
float precameraX = 0;
float precameraY = 0;
float precameraZ = 0;
float viewRotateDegree[3] = { 0.0f, 0.0f, 0.0f };

float a_brightness = 0.5f;
float d_brightness = 0.0f;
float s_brightness = 0.1f;
float speed = 2.0f;
float cameraRotation_x = 0.0f;
float cameraRotation_y = 0.0f;
float threex;
float threey;
float threez;
float dx;
float dy;
float dz;

bool doPharse = false;
bool changeView = false;

int cameraCounter = 0;

//new stuffs
float helispeed = 2.0f;
float heliscale = 0.004f;
float orbitscale = 1.0f;

float earthPositionX = -8.0f;
float earthPositionY = 6.0f;
float earthPositionZ = -10.0f;

glm::vec4 heliPosition(0.0f);
glm::vec4 heliPosition1(0.0f);
glm::vec3 heliHead;
bool startPaint = false;
int starCounter = 0;
float heliSet[1000];
int i = 0;
int timeCounter = 0;

void keyboard(unsigned char key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	if (key == 'a') {
		precameraX = 0.0f;
		precameraY = 0.0f;
		precameraZ = 25.0f;
		threex = 0.0f;
		threey = 1.0f;
		threez = 0.0f;
	}
	if (key == 's') {
		precameraX = 0.0f;
		precameraY = 25.0f;
		precameraZ = 0.0f;
		threex = 0.0f;
		threey = 0.0f;
		threez = -1.0f;
	}
	if (key == 'd') {
		precameraX = 20.0f;
		precameraY = 20.0f;
		precameraZ = 20.0f;
		threex = -1.0f;
		threey = 1.0f;
		threez = -1.0f;
	}
	if (key == 'v') {
		cameraCounter++;
		if (cameraCounter % 2 == 1)
			changeView = true;
		else
			changeView = false;
	}
	if (key == 'b') {
		starCounter++;
		if (starCounter % 2 == 1)
			startPaint = true;
		else
			startPaint = false;
	}

	if (key == 'p') {
		doPharse = true;
	}
}

void reshape(int w, int h)
{
	Win_w = w;
	Win_h = h;
}

void Mouse_Wheel_Func(int button, int state, int x, int y)
{
	if ((button == 3) || (button == 4))
	{
		if (state == GLUT_UP) return;
		if (button == 3)
		{
			radius -= 1.0f;
			cameraX = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraY = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraZ = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
		else
		{
			radius += 1.0f;
			cameraX = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraY = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraZ = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
	}
}

void move(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		helispeed += 0.5f;
		break;
	case GLUT_KEY_DOWN:
		helispeed -= 0.5f;
		break;
	case GLUT_KEY_LEFT:
		orbitscale += 0.1f;
		break;
	case GLUT_KEY_RIGHT:
		orbitscale -= 0.1f;
		break;
	}

	glutPostRedisplay();

}

/*void ProcessMouse(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
		zoom += 1.0f;
		glutPostRedisplay();
	}
	if (state == GLUT_UP &&button == GLUT_WHEEL_DOWN) {
		zoom -= 1.0f;
		glutPostRedisplay();
	}
}*/

void PassiveMouse(int x, int y)
{
	//TODO: Use Mouse to do interactive events and animation
	cameraRotation_x = 0.045*x;
	cameraRotation_y = 0.045*y;

}

void LoadAllTextures()
{
	TextureEarth_0 = loadBMP2Texture("texture/earth.bmp");
	TextureEarth_1 = loadBMP2Texture("normal_map/earth_normal.bmp");
	TexturePlanetB = loadBMP2Texture("texture/grass.bmp");
	TexturePlanetC_0 = loadBMP2Texture("texture/glass_a.bmp");
	TexturePlanetC_1 = loadBMP2Texture("texture/apple.bmp");
	TextureObjD = loadBMP2Texture("texture/helicopter.bmp");
	//for (GLuint i = 0; i < 500; i++)
		TextureObjG = loadBMP2Texture("texture/brickwall.bmp");
	TextureBox = loadBMP2Texture("texture/sea_skybox/front.bmp");
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		printf("Reading image %s\n", faces[i]);

		unsigned char header[54];
		unsigned int dataPos;
		unsigned int imageSize;
		unsigned int width, height;
		unsigned char * data;

		FILE * file = fopen(faces[i], "rb");
		if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", faces[i]); getchar(); return 0; }

		if (fread(header, 1, 54, file) != 54) {
			printf("Not a correct BMP file\n");
			return 0;
		}
		if (header[0] != 'B' || header[1] != 'M') {
			printf("Not a correct BMP file\n");
			return 0;
		}
		if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
		if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);
		if (imageSize == 0)    imageSize = width*height * 3;
		if (dataPos == 0)      dataPos = 54;

		data = new unsigned char[imageSize];
		fread(data, 1, imageSize, file);
		fclose(file);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;

}

vec3 ExtractCameraPos(const mat4 & a_modelView)
{
	// Get the 3 basis vector planes at the camera origin and transform them into model space.
	//  
	// NOTE: Planes have to be transformed by the inverse transpose of a matrix
	//       Nice reference here: http://www.opengl.org/discussion_boards/showthread.php/159564-Clever-way-to-transform-plane-by-matrix
	//
	//       So for a transform to model space we need to do:
	//            inverse(transpose(inverse(MV)))
	//       This equals : transpose(MV) - see Lemma 5 in http://mathrefresher.blogspot.com.au/2007/06/transpose-of-matrix.html
	//
	// As each plane is simply (1,0,0,0), (0,1,0,0), (0,0,1,0) we can pull the data directly from the transpose matrix.
	//  
	mat4 modelViewT = transpose(a_modelView);

	// Get plane normals 
	vec3 n1(modelViewT[0]);
	vec3 n2(modelViewT[1]);
	vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	// Get the intersection of these 3 planes
	// http://paulbourke.net/geometry/3planes/
	vec3 n2n3 = cross(n2, n3);
	vec3 n3n1 = cross(n3, n1);
	vec3 n1n2 = cross(n1, n2);

	vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}

void CreateRand_ModelM() {
	rock_RandMatrices = new mat4[rock_amount];
	srand(glutGet(GLUT_ELAPSED_TIME));
	GLfloat radius = 6.0f;
	GLfloat offset = 1.2f;
	for (GLuint i = 0; i < rock_amount; i++)
	{
		mat4 model;
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)rock_amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 50.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 50.0f - offset;
		GLfloat y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (GLint)(2 * offset * 100)) / 50.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 20) / 2000.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		rock_RandMatrices[i] = model;
	}
}


void sendDataToOpenGL()
{
	//Load objects and bind to VAO & VBO
	bindEarth("obj/planet.obj");
	bindPlanetB("obj/planet.obj");
	bindPlanetC("obj/planet.obj");
	bindObjD("obj/Arc170.obj");
	bindObjG("obj/rock.obj");
	bindLightbox("obj/rock.obj");
	// load all textures
	LoadAllTextures();

	// skybox
	GLfloat skyboxVertices[] =
	{
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f
	};
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	vector<const GLchar*> skybox;
	skybox.push_back("texture/skycity_skybox/right.bmp");
	skybox.push_back("texture/skycity_skybox/left.bmp");
	skybox.push_back("texture/skycity_skybox/bottom.bmp");
	skybox.push_back("texture/skycity_skybox/top.bmp");
	skybox.push_back("texture/skycity_skybox/back.bmp");
	skybox.push_back("texture/skycity_skybox/front.bmp");
	TextureSkybox = loadCubemap(skybox);
}

float zLightPos = 10.0f;
float yLightPos = 10.0f;
float xLightPos = 0.0f;
void set_lighting()
{
	glUseProgram(programID);
	// fog color
	GLint fogColorUniformLocation = glGetUniformLocation(programID, "fogcolor");
	glm::vec3 fogColor(fogr, fogg, fogb);
	glUniform3fv(fogColorUniformLocation, 1, &fogColor[0]);
	// fog ON/OFF
	GLint fogONUniformLocation = glGetUniformLocation(programID, "fogON");
	glUniform1i(fogONUniformLocation, fogon);
	// ambient
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glm::vec3 ambientLight(a_brightness, a_brightness, a_brightness);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffusion
	GLint kd = glGetUniformLocation(programID, "coefficient_d");
	glm::vec3 vec_kd(d_brightness, d_brightness, d_brightness);
	glUniform3fv(kd, 1, &vec_kd[0]);
	// specular
	GLint ks = glGetUniformLocation(programID, "coefficient_s");
	glm::vec3 vec_ks(s_brightness, s_brightness, s_brightness);
	glUniform3fv(ks, 1, &vec_ks[0]);
	// eye position
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition(cameraX, cameraY, cameraZ);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// light position
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	glm::vec3 lightPosition(xLightPos, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(programID, "lightColor");
	glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(lightColorUniformLocation, 1, &lightColor[0]);

}

void set_lightingG()
{
	glUseProgram(rockProgramID);
	
	// ambient
	GLint ambientLightUniformLocation = glGetUniformLocation(rockProgramID, "ambientLight");
	glm::vec3 ambientLight(a_brightness, a_brightness, a_brightness);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffusion
	GLint kd = glGetUniformLocation(rockProgramID, "coefficient_d");
	glm::vec3 vec_kd(d_brightness, d_brightness, d_brightness);
	glUniform3fv(kd, 1, &vec_kd[0]);
	// specular
	GLint ks = glGetUniformLocation(rockProgramID, "coefficient_s");
	glm::vec3 vec_ks(s_brightness, s_brightness, s_brightness);
	glUniform3fv(ks, 1, &vec_ks[0]);
	// eye position
	GLint eyePositionUniformLocation = glGetUniformLocation(rockProgramID, "eyePositionWorld");
	vec3 eyePosition(cameraX, cameraY, cameraZ);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// light position
	GLint lightPositionUniformLocation = glGetUniformLocation(rockProgramID, "lightPositionWorld");
	glm::vec3 lightPosition(xLightPos, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(rockProgramID, "lightColor");
	glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(lightColorUniformLocation, 1, &lightColor[0]);
}

void set_lightingEarth()
{
	glUseProgram(earthProgramID);
	// fog color
	GLint fogColorUniformLocation = glGetUniformLocation(earthProgramID, "fogcolor");
	glm::vec3 fogColor(fogr, fogg, fogb);
	glUniform3fv(fogColorUniformLocation, 1, &fogColor[0]);
	// fog ON/OFF
	GLint fogONUniformLocation = glGetUniformLocation(earthProgramID, "fogON");
	glUniform1i(fogONUniformLocation, fogon);
	// ambient
	GLint ambientLightUniformLocation = glGetUniformLocation(earthProgramID, "ambientLight");
	glm::vec3 ambientLight(a_brightness, a_brightness, a_brightness);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffusion
	GLint kd = glGetUniformLocation(earthProgramID, "coefficient_d");
	glm::vec3 vec_kd(d_brightness, d_brightness, d_brightness);
	glUniform3fv(kd, 1, &vec_kd[0]);
	// specular
	GLint ks = glGetUniformLocation(earthProgramID, "coefficient_s");
	glm::vec3 vec_ks(s_brightness, s_brightness, s_brightness);
	glUniform3fv(ks, 1, &vec_ks[0]);
	// eye position
	GLint eyePositionUniformLocation = glGetUniformLocation(earthProgramID, "eyePositionWorld");
	vec3 eyePosition(cameraX, cameraY, cameraZ);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// light position
	GLint lightPositionUniformLocation = glGetUniformLocation(earthProgramID, "lightPositionWorld");
	glm::vec3 lightPosition(xLightPos, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(earthProgramID, "lightColor");
	glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(lightColorUniformLocation, 1, &lightColor[0]);
}

void set_lighting_C()
{
	glUseProgram(planetC_programID);
	// fog color
	GLint fogColorUniformLocation = glGetUniformLocation(planetC_programID, "fogcolor");
	glm::vec3 fogColor(fogr, fogg, fogb);
	glUniform3fv(fogColorUniformLocation, 1, &fogColor[0]);
	// fog ON/OFF
	GLint fogONUniformLocation = glGetUniformLocation(planetC_programID, "fogON");
	glUniform1i(fogONUniformLocation, fogon);
	// ambient
	GLint ambientLightUniformLocation = glGetUniformLocation(planetC_programID, "ambientLight");
	glm::vec3 ambientLight(a_brightness, a_brightness, a_brightness);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffusion
	GLint kd = glGetUniformLocation(planetC_programID, "coefficient_d");
	glm::vec3 vec_kd(d_brightness, d_brightness, d_brightness);
	glUniform3fv(kd, 1, &vec_kd[0]);
	// specular
	GLint ks = glGetUniformLocation(planetC_programID, "coefficient_s");
	glm::vec3 vec_ks(s_brightness, s_brightness, s_brightness);
	glUniform3fv(ks, 1, &vec_ks[0]);
	// eye position
	GLint eyePositionUniformLocation = glGetUniformLocation(planetC_programID, "eyePositionWorld");
	vec3 eyePosition(cameraX, cameraY, cameraZ);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// light position
	GLint lightPositionUniformLocation = glGetUniformLocation(planetC_programID, "lightPositionWorld");
	glm::vec3 lightPosition(xLightPos, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(planetC_programID, "lightColor");
	glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(lightColorUniformLocation, 1, &lightColor[0]);
}

void drawEarth(void)
{
	//earth
	GLfloat scale_fact = 3.0f;

	glUseProgram(earthProgramID);

	glBindVertexArray(earthVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 6.0f, -10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(earthProgramID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(earthProgramID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(earthProgramID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID_0 = glGetUniformLocation(earthProgramID, "myTextureSampler_0");
	GLuint TextureID_1 = glGetUniformLocation(earthProgramID, "myTextureSampler_1");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureEarth_0);
	glUniform1i(TextureID_0, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureEarth_1);
	glUniform1i(TextureID_1, 0);

	glDrawArrays(GL_TRIANGLES, 0, drawEarthSize);
}

void drawPlanetC(void)
{
	//Planet C
	GLfloat scale_fact = 1.5f;

	glUseProgram(planetC_programID);

	glBindVertexArray(planetCVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	//glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, -5.0f, 0.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(planetC_programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(planetC_programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(planetC_programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID_0 = glGetUniformLocation(planetC_programID, "myTextureSampler_0");
	GLuint TextureID_1 = glGetUniformLocation(planetC_programID, "myTextureSampler_1");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetC_0);
	glUniform1i(TextureID_0, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetC_1);
	glUniform1i(TextureID_1, 1);

	glDrawArrays(GL_TRIANGLES, 0, drawPlanetCSize);
}

void drawPlanetB(void)
{
	//Planet B
	GLfloat scale_fact = 0.2f;

	glUseProgram(programID);

	glBindVertexArray(planetBVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_G = glm::rotate(mat4(), (GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f*speed, glm::vec3(0, 1, 0));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 10.0f, -10.0f));
	glm::mat4 trans_G = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.0f, -10.0f));
	glm::mat4 Model = trans_G*rot_G*trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetB);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawPlanetBSize);
}

void drawStar(float x, float y, float z)
{
	GLfloat scale_fact = 0.2f;

	glUseProgram(programID);

	glBindVertexArray(planetBVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	glm::mat4 Model = trans_M * scale_M;

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetB);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawPlanetBSize);
}


void drawObjD(void)
{
	// object D
	GLfloat scale_fact = 0.1f;

	glUseProgram(programID);

	glBindVertexArray(objDVao);

	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(heliscale));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 rot_N = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	glm::mat4 rot_G = glm::rotate(mat4(), (GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f*helispeed, glm::vec3(0, 0, 1));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(15 * orbitscale, 0, 0));
	glm::mat4 trans_G = glm::translate(glm::mat4(1.0f), glm::vec3(earthPositionX, earthPositionY, earthPositionZ));
	glm::mat4 trans_N = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1000, 0));
	glm::mat4 Model = trans_G*rot_G*trans_M* rot_N*rot_M*scale_M;
	glm::mat4 Model1 = trans_G*rot_G*trans_M* rot_N*rot_M*scale_M*trans_N;
	heliPosition1 = Model1 * glm::vec4(0, 0, 0, 1);
	heliPosition = Model * glm::vec4(0, 0, 0, 1);
	heliHead = glm::vec3(heliPosition1.x, heliPosition1.y, heliPosition1.z);
	if (startPaint&&timeCounter % 5 == 0) {
		heliSet[i] = heliPosition.x;
		heliSet[i + 1] = heliPosition.y;
		heliSet[i + 2] = heliPosition.z;
		i += 3;
	}
	//heliPosition = ExtractCameraPos(Model);

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureObjD);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawObjDSize);
}


void drawObjG(void)
{
	glm::mat4 *modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glutGet(GLUT_ELAPSED_TIME));
	GLfloat radius = 6.0f;
	GLfloat offset = 0.4f;
	GLfloat displacement;
	for (GLuint i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f + 1; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
	glm::mat4 rockOrbitIni = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, -5.0f, 0.0f));
	glm::mat4 rockOrbit_M = glm::rotate(rockOrbitIni, glm::radians(0.0f), glm::vec3(0, 1, 0));
	glUseProgram(rockProgramID);

	
	for (GLuint i = 0; i < 500; i++) {

		// fog color
		GLint fogColorUniformLocation = glGetUniformLocation(rockProgramID, "fogcolor");
		glm::vec3 fogColor(fogr, fogg, fogb);
		glUniform3fv(fogColorUniformLocation, 1, &fogColor[0]);
		// fog ON/OFF
		GLint fogONUniformLocation = glGetUniformLocation(rockProgramID, "fogON");
		glUniform1i(fogONUniformLocation, fogon);

		glBindVertexArray(objGVao[i]);

		glm::mat4 rockModelMat_temp = modelMatrices[i];
		rockModelMat_temp = rockOrbit_M * rockModelMat_temp;

		GLint M_ID = glGetUniformLocation(rockProgramID, "MM");
		glUniformMatrix4fv(M_ID, 1, GL_FALSE, &rockModelMat_temp[0][0]);
		GLint V_ID = glGetUniformLocation(rockProgramID, "VM");
		glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
		GLint P_ID = glGetUniformLocation(rockProgramID, "PM");
		glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

		GLuint TextureID = glGetUniformLocation(rockProgramID, "myTextureSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureObjG);
		glUniform1i(TextureID, 0);
		glActiveTexture(GL_TEXTURE1);
		glDrawArrays(GL_TRIANGLES, 0, drawObjGSize[i]);
	}
	
}

void drawSkybox(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	glUseProgram(skyboxProgramID);

	// fog color
	GLint fogColorUniformLocation = glGetUniformLocation(skyboxProgramID, "fogcolor");
	glm::vec3 fogColor(fogr, fogg, fogb);
	glUniform3fv(fogColorUniformLocation, 1, &fogColor[0]);
	// fog ON/OFF
	GLint fogONUniformLocation = glGetUniformLocation(skyboxProgramID, "fogON");
	glUniform1i(fogONUniformLocation, fogon);

	//GLint V_ID = glGetUniformLocation(skyboxProgramID, "VM");
	//glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(skyboxProgramID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	GLuint Skb_ModelUniformLocation = glGetUniformLocation(skyboxProgramID, "MM");
	glm::mat4 Skb_ModelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(Skb_ModelUniformLocation, 1, GL_FALSE, &Skb_ModelMatrix[0][0]);
	// remove any translation component of the view matrix
	glm::mat4 VM = glm::mat4(glm::mat3(glm::lookAt(vec3(cameraX+cameraRotation_x, cameraY + cameraRotation_y, cameraZ), vec3(cameraX+cameraRotation_x, cameraY + cameraRotation_y, cameraZ - 1.0f), vec3(0, 1, 0))));
	//glm::mat4 projection = glm::perspective(45.0f, (float)Win_w / (float)Win_h, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "VM"), 1, GL_FALSE, glm::value_ptr(VM));
	//glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(skyboxProgramID, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureSkybox);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

void drawLightbox(void)
{
	// lightbox
	GLfloat scale_fact = 0.2f;

	glUseProgram(programID);
	glBindVertexArray(boxVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	//glm::mat4 rot_M = glm::rotate(mat4(), (GLfloat)glutGet(GLUT_ELAPSED_TIME)*0.001f*speed, glm::vec3(1, 0, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(xLightPos, yLightPos, zLightPos));
	//glm::vec4 v = glm::vec4(0.0f, 0.0f, -5.0f, 1.0f);
	glm::mat4 Model = rot_M *trans_M *  scale_M;
	xLightPos = 0.0f + 10 * glm::cos(light_Rot_Degree);
	yLightPos = 10.0f + 10 * glm::sin(light_Rot_Degree);

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureBox);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawBoxSize);
}



void paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ================================ //
	// view matrix
	if (!changeView)
		common_viewM = glm::lookAt(glm::vec3(precameraX + cameraX - cameraRotation_x, precameraY + cameraY - cameraRotation_y, precameraZ + cameraZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	else
		common_viewM = glm::lookAt(glm::vec3(heliPosition1.x, heliPosition1.y, heliPosition1.z), glm::vec3(earthPositionX, earthPositionY + 4, earthPositionZ), glm::vec3(0, 1, 0));
	//glm::vec3(earthPositionX,earthPositionY,earthPositionZ)

	//common_viewM = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// projection matrix
	common_projection = glm::perspective(camera_fov, 1.0f, 0.1f, 200.0f);

	//=== draw ===//
	drawSkybox();

	// set lighting parameters
	set_lighting();
	set_lighting_C();
	set_lightingEarth();
	// draw earth
	if (!doPharse)
		drawEarth();
	// draw planet C
	drawPlanetC();
	// draw planet B
	drawPlanetB();
	// draw obj D
	dx = heliPosition.x - earthPositionX;
	dy = heliPosition.y - earthPositionY;
	dz = heliPosition.z - earthPositionZ;

	float distance = sqrt(dx*dx + dy*dy + dz*dz);
	//printf("distance is %f", distance);
	if (distance >= 12.5f)
		drawObjD();
	drawObjG();
	drawLightbox();




	if (startPaint) {
		for (int a = 0; a < 400; a = a + 3)
		{
			if (heliSet[a] != NULL)
				drawStar(heliSet[a], heliSet[a + 1], heliSet[a + 2]);
		}
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void Shaders_Installer()
{
	programID = installShaders("shader/Common.vs", "shader/Common.frag");
	planetC_programID = installShaders("shader/planetC.vs", "shader/planetC.frag");
	skyboxProgramID = installShaders("shader/Skybox.vs", "shader/Skybox.frag");
	earthProgramID = installShaders("shader/Earth.vs", "shader/Earth.frag");
	//lightboxProgramID = installShaders("shader/Lightbox.vs", "shader/Lightbox.frag");
	rockProgramID = installShaders("shader/Rock.vs", "shader/Rock.frag");

	if (!checkProgramStatus(programID))
		return;
}

void initializedGL(void)
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// install all shaders
	Shaders_Installer();
	// load required obj and textures
	sendDataToOpenGL();
}

void timerFunction(int id)
{
	earth_innRot_Degree += 0.3;
	light_Rot_Degree += 0.03;
	timeCounter++;
	
	glutPostRedisplay();
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);
}

void controlfogon(int control) {
	printf("callback: %d\n", control);
	fogon = (fogon + 1) % 2;
	printf("fogon: %d\n", fogon);
}

void changegray(int control) {
	printf("callback: %d\n", control);
	fogr = 0.5;
	fogg = 0.5;
	fogb = 0.5;
	printf("fogRGB: %f %f %f\n", fogr, fogg, fogb);
}

void changered(int control) {
	printf("callback: %d\n", control);
	fogr = 0.3;
	fogg = 0.0;
	fogb = 0.0;
	printf("fogRGB: %f %f %f\n", fogr, fogg, fogb);
}

void changegreen(int control) {
	printf("callback: %d\n", control);
	fogr = 0.0;
	fogg = 0.3;
	fogb = 0.0;
	printf("fogRGB: %f %f %f\n", fogr, fogg, fogb);
}

void changeblue(int control) {
	printf("callback: %d\n", control);
	fogr = 0.0;
	fogg = 0.0;
	fogb = 0.3;
	printf("fogRGB: %f %f %f\n", fogr, fogg, fogb);
}

void explore(int control) {
	printf("callback: %d\n", control);
	changeView = false;
}

void vehicle(int control) {
	printf("callback: %d\n", control);
	changeView = true;
	//common_viewM = glm::lookAt(glm::vec3(heliPosition1.x, heliPosition1.y, heliPosition1.z), glm::vec3(earthPositionX, earthPositionY + 4, earthPositionZ), glm::vec3(0, 1, 0));
}

void pointa(int control) {
	printf("callback: %d\n", control);
	changeView = false;
	precameraX = 0.0f;
	precameraY = 0.0f;
	precameraZ = 25.0f;
}

void pointb(int control) {
	printf("callback: %d\n", control);
	changeView = false;
	precameraX = 0.0f;
	precameraY = 25.0f;
	precameraZ = 0.0f;
}

void pointc(int control) {
	printf("callback: %d\n", control);
	changeView = false;
	precameraX = 20.0f;
	precameraY = 20.0f;
	precameraZ = 20.0f;
}


int main(int argc, char *argv[])
{
	// start the sound engine with default parameters
	ISoundEngine* engine = createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engine

				  // play some sound stream, looped
	engine->play2D("bgm.mp3", true);
	/*Initialization of GLUT library*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	int height = GetSystemMetrics(SM_CYSCREEN) * 0.8;
	int width = height; // GetSystemMetrics(SM_CXSCREEN) * 0.8;
	Win_h = height;
	Win_w = width;
	glutInitWindowSize(Win_w, Win_h);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	mainWindowID = glutCreateWindow("i-Navigation");

	GLUI *glui = GLUI_Master.create_glui("GUI Controler", 0);
	glui->set_main_gfx_window(mainWindowID);
	glui->add_separator();
	GLUI_StaticText *infoText = glui->add_statictext("SpaceVehicle-CUHK");
	infoText->set_alignment(GLUI_ALIGN_CENTER);
	glui->add_separator();

	GLUI_Spinner *speed = glui->add_spinner("Vehicle Speed", GLUI_SPINNER_FLOAT, &helispeed);
	speed->set_float_limits(1.0, 10.0);
	speed->set_speed(2.0);
	GLUI_Panel *vppanel = glui->add_panel("View Point", GLUI_PANEL_EMBOSSED);
	GLUI_Panel *fogon = glui->add_panel("Fog ON/OFF", GLUI_PANEL_EMBOSSED);
	GLUI_Panel *fogcolor = glui->add_panel("Fog Color", GLUI_PANEL_EMBOSSED);

	glui->add_button_to_panel(vppanel, "Explore", 1, explore);
	glui->add_button_to_panel(vppanel, "Vehicle", 2, vehicle);
	glui->add_button_to_panel(vppanel, "Viewpoint A", 3, pointa);
	glui->add_button_to_panel(vppanel, "Viewpoint B", 4, pointb);
	glui->add_button_to_panel(vppanel, "Viewpoint C", 5, pointc);

	glui->add_checkbox_to_panel(fogon, "OFF", NULL, 6, controlfogon);
	glui->add_button_to_panel(fogcolor, "gray", 7, changegray);
	glui->add_button_to_panel(fogcolor, "red", 8, changered);
	glui->add_button_to_panel(fogcolor, "green", 9, changegreen);
	glui->add_button_to_panel(fogcolor, "blue", 10, changeblue);



	// initialize openGL
	initializedGL();
	// draw
	glutDisplayFunc(paintGL);

	glutMouseFunc(Mouse_Wheel_Func);
	
	glutPassiveMotionFunc(PassiveMouse);

	glutSpecialFunc(move);
	
	glutKeyboardFunc(keyboard);
	
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);

	glutReshapeFunc(reshape);
	/*Enter the GLUT event processing loop which never returns.*/
	glutMainLoop();
	engine->drop(); // delete engine
	return 0;
}