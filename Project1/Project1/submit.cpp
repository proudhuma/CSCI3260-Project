#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
//#include "Dependencies\glui\glui.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "shader_install.h"
#include "Loading_func.h"
#include "Bind_Data.h"

using namespace std;
using glm::vec3;
using glm::mat4;
// ============================= //
// window size
int Win_w, Win_h;
float camera_fov = 45.0;
unsigned int amount = 500;

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

GLuint lightboxVAO;
GLuint lightboxVBO;
GLuint lightboxVBO_IDX;
GLuint skyboxVAO;
GLuint skyboxVBO;
extern GLuint earthVao;
extern GLuint planetCVao;
extern GLuint planetBVao;
extern GLuint objDVao;
extern GLuint objGVao[500];

extern int drawEarthSize;
extern int drawPlanetCSize;
extern int drawPlanetBSize;
extern int drawObjDSize;
extern int drawObjGSize[500];


// view matrix
glm::mat4 common_viewM;
glm::mat4 common_projection;

float earth_innRot_Degree = 0.0f;

// ============================= //

const float M_PI = 3.14159265;
float radius = 30.0f;
float initViewHorizontal = -90.0f;
float initViewVertical = -90.0f;

float cameraX = 0;
float cameraY = 0;
float cameraZ = radius;

float viewRotateDegree[3] = { 0.0f, 0.0f, 0.0f };

float a_brightness = 0.3f;
float d_brightness = 0.0f;
float s_brightness = 0.6f;

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

void sendDataToOpenGL()
{
	//Load objects and bind to VAO & VBO
	bindEarth("obj/planet.obj");
	bindPlanetB("obj/planet.obj");
	bindPlanetC("obj/planet.obj");
	bindObjD("obj/helicopter2.obj");
	bindObjG("obj/rock.obj");
	// load all textures
	LoadAllTextures();

	// light source box
	const GLfloat cubic[] =
	{
		-1.0f, -1.0f, -1.0f, // position 0
		+1.0f, +1.0f, +1.0f, // color

		-1.0f, +1.0f, -1.0f, // position 1
		+1.0f, +1.0f, +1.0f, // color

		+1.0f, -1.0f, -1.0f, // position 2
		+1.0f, +1.0f, +1.0f, // color

		+1.0f, +1.0f, -1.0f, // postiion 3
		+1.0f, +1.0f, +1.0f, // color

		-1.0f, -1.0f, +1.0f, // position 4
		+1.0f, +1.0f, +1.0f, // color

		-1.0f, +1.0f, +1.0f, // position 5
		+1.0f, +1.0f, +1.0f, // color

		+1.0f, -1.0f, +1.0f, // position 6
		+1.0f, +1.0f, +1.0f, // color

		+1.0f, +1.0f, +1.0f, // postiion 7
		+1.0f, +1.0f, +1.0f, // color

	};
	GLushort indices_1[] = {
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
		0, 2, 4,
		2, 4, 6,
		1, 3, 5,
		3, 5, 7,
		2, 3, 6,
		3, 6, 7,
		0, 1, 4,
		1, 4, 5,
	};

	glGenVertexArrays(1, &lightboxVAO);
	glGenBuffers(1, &lightboxVBO);
	glGenBuffers(1, &lightboxVBO_IDX);

	glBindVertexArray(lightboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubic), cubic, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightboxVBO_IDX);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1, GL_STATIC_DRAW);

	//vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	//vertex color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

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
void set_lighting()
{
	glUseProgram(programID);

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
	glm::vec3 lightPosition(0.0, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(programID, "lightColor");
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
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.5f, +10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

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
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 10.0f, +10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

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

	GLuint Skb_ModelUniformLocation = glGetUniformLocation(skyboxProgramID, "M");
	glm::mat4 Skb_ModelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(Skb_ModelUniformLocation, 1, GL_FALSE, &Skb_ModelMatrix[0][0]);
	// remove any translation component of the view matrix
	glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(vec3(cameraX, cameraY, cameraZ), vec3(cameraX, cameraY, cameraZ - 1.0f), vec3(0, 1, 0))));
	glm::mat4 projection = glm::perspective(45.0f, (float)Win_w / (float)Win_h, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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
	GLfloat scale_fact = 1.0f;

	glUseProgram(lightboxProgramID);

	glBindVertexArray(lightboxVAO);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, yLightPos, zLightPos)); // light position
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(lightboxProgramID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(lightboxProgramID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(lightboxProgramID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// ================================ //
	// view matrix
	common_viewM = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// projection matrix
	common_projection = glm::perspective(camera_fov, 1.0f, 0.1f, 200.0f);
	
	//=== draw ===//
	drawSkybox();
	
	// set lighting parameters
	set_lighting();
	// draw earth
	drawEarth();
	// draw planet C
	drawPlanetC();
	// draw planet B
	drawPlanetB();
	// draw obj D
	drawObjD();
	drawObjG();
	drawLightbox();

	glutSwapBuffers();
	glutPostRedisplay();
}

void Shaders_Installer()
{
	programID = installShaders("shader/Common.vs", "shader/Common.frag");
	planetC_programID = installShaders("shader/planetC.vs", "shader/planetC.frag");
	skyboxProgramID = installShaders("shader/Skybox.vs", "shader/Skybox.frag");
	earthProgramID = installShaders("shader/Earth.vs", "shader/Earth.frag");
	lightboxProgramID = installShaders("shader/Lightbox.vs", "shader/Lightbox.frag");
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
	
	glutPostRedisplay();
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);
}

int main(int argc, char *argv[])
{
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

	// initialize openGL
	initializedGL();
	// draw
	glutDisplayFunc(paintGL);

	glutMouseFunc(Mouse_Wheel_Func);

	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);

	glutReshapeFunc(reshape);
	/*Enter the GLUT event processing loop which never returns.*/
	glutMainLoop();

	return 0;
}