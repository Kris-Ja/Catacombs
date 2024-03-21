#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"

#define Xsize 7
#define Ysize 2
#define Zsize 7
#define nr_of_tex 5
#define nr_of_obj 3

glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
float pitch = 0;
float yaw = 0;
float flashlight = 1;
float speed = 0;
float speed_left = 0;
float speed_y = 0;
float speed_yaw = 0;
float speed_pitch = 0;
float aspectRatio = 1;
float leverRotation[4];
float leverSpeed[4];
bool leverFlag[4];

ShaderProgram* sp; //Pointer to the shader program
GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint floor0;
GLuint floor1;
GLuint floor2;
GLuint floor3;
GLuint lamp0;
GLuint lamp1;
GLuint lamp2;
GLuint lamp3;
GLuint lever0;
GLuint lever1;
GLuint lever2;

float* verttab[nr_of_tex];
float* texCordtab[nr_of_tex];
float* normtab[nr_of_tex];
float* c1tab[nr_of_tex];
float* c2tab[nr_of_tex];
float* c3tab[nr_of_tex];
int nr_vr[nr_of_tex];

float* vertt[nr_of_obj];
float* normt[nr_of_obj];
float* text[nr_of_obj];
float* c1t[nr_of_obj];
float* c2t[nr_of_obj];
float* c3t[nr_of_obj];
int tsize[nr_of_obj];

float lights[3*Xsize*Ysize*Zsize+2];
int nr_lights;

std::ifstream odczyt;
void weirdObjinto3v(std::string& k, float& v, float& t, float& n)
{
    int p = k.find('/');
    int l = k.find('/', p + 1);
    v = std::stoi(k.substr(0, p));
    t = std::stoi(k.substr(p + 1, l - p - 1));
    n = std::stoi(k.substr(l + 1));
}
void setObjFile(std::string name, int obj_id)
{
    tsize[obj_id] = 0;
    vertt[obj_id] = new float[50000];
    normt[obj_id] = new float[50000];
    text[obj_id] = new float[50000];
    c1t[obj_id] = new float[50000];
    c2t[obj_id] = new float[50000];
    c3t[obj_id] = new float[50000];
    float temp1;
    float temp2;
    float temp3;
    std::vector<glm::vec3> vert;
    std::vector<glm::vec3> norm;
    std::vector<glm::vec2> tex;
    std::string linia;
    odczyt.open(name);
    while (!odczyt.eof())
    {
        odczyt >> linia;
        /*if (linia.size() == 1 && linia[0] == 'o')
        {
            vert.clear();
            norm.clear();
            tex.clear();
        }*/
        if (linia.size() == 1 && linia[0] == 'v')
        {
            odczyt >> temp1;
            odczyt >> temp2;
            odczyt >> temp3;
            vert.push_back(glm::vec3(temp1, temp2, temp3));
        }
        if (linia.size() == 2 && linia[0] == 'v' && linia[1] == 'n')
        {
            odczyt >> temp1;
            odczyt >> temp2;
            odczyt >> temp3;
            norm.push_back(glm::vec3(temp1, temp2, temp3));
        }
        if (linia.size() == 2 && linia[0] == 'v' && linia[1] == 't')
        {
            odczyt >> temp1;
            odczyt >> temp2;
            tex.push_back(glm::vec2(temp1, temp2));
        }
        if (linia.size() == 1 && linia[0] == 'f')
        {
            for (int i = 0; i < 3; i++)
            {
                odczyt >> linia;
                weirdObjinto3v(linia, temp1, temp2, temp3);
                temp1--;
                temp2--;
                temp3--;
                vertt[obj_id][tsize[obj_id] * 4] = vert[temp1].x;
                vertt[obj_id][tsize[obj_id] * 4 + 1] = vert[temp1].y;
                vertt[obj_id][tsize[obj_id] * 4 + 2] = vert[temp1].z;
                if (obj_id == 0)
                {
                    vertt[obj_id][tsize[obj_id] * 4] += 1.0f / 2.0f;
                    vertt[obj_id][tsize[obj_id] * 4 + 2] -= 1.0f / 2.0f;
                    vertt[obj_id][tsize[obj_id] * 4] *= 3.0f / 4.0f;
                    vertt[obj_id][tsize[obj_id] * 4 + 2] *= 3.0f / 4.0f;
                }
                if (obj_id == 2)
                {
                    vertt[obj_id][tsize[obj_id] * 4] *= 1.0f / 4.0f;
                    vertt[obj_id][tsize[obj_id] * 4+1] *= 1.0f / 4.0f;
                    vertt[obj_id][tsize[obj_id] * 4 + 2] *= 1.0f / 4.0f;
                    vertt[obj_id][tsize[obj_id] * 4+1] += 0.75;
                }
                vertt[obj_id][tsize[obj_id] * 4 + 3] = 1.0f;
                text[obj_id][tsize[obj_id] * 2] = tex[temp2].x;
                text[obj_id][tsize[obj_id] * 2 + 1] = tex[temp2].y;
                normt[obj_id][tsize[obj_id] * 4] = norm[temp3].x;
                normt[obj_id][tsize[obj_id] * 4 + 1] = norm[temp3].y;
                normt[obj_id][tsize[obj_id] * 4 + 2] = norm[temp3].z;
                normt[obj_id][tsize[obj_id] * 4 + 3] = 1.0f;
                tsize[obj_id]++;
            }
        }
    }
    odczyt.close();
}

//Error processing callback procedure
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

struct room
{
    int last;
    int next;
    bool mazePart;
    bool somsiad[6];
};
room pokoje[Xsize][Ysize][Zsize];
void setBegin()
{
    for (int i = 0; i < Xsize; i++)
        for (int y = 0; y < Ysize; y++)
            for (int r = 0; r < Zsize; r++)
            {
                pokoje[i][y][r].mazePart = 0;
                pokoje[i][y][r].last = -1;
                pokoje[i][y][r].next = -1;
                for (int k = 0; k < 6; k++)
                    pokoje[i][y][r].somsiad[k] = 0;
            }
}
void losuj(int i, int y, int r)
{
    bool dobre;
    do {
        dobre = true;
        pokoje[i][y][r].next = rand() % 6;
        if (r == 0 && pokoje[i][y][r].next == 1)dobre = false;
        if (r == Zsize - 1 && pokoje[i][y][r].next == 0)dobre = false;
        if (i == 0 && pokoje[i][y][r].next == 5)dobre = false;
        if (i == Xsize - 1 && pokoje[i][y][r].next == 3)dobre = false;
        if (y == 0 && pokoje[i][y][r].next == 4)dobre = false;
        if (y == Ysize - 1 && pokoje[i][y][r].next == 2)dobre = false;
        if (pokoje[i][y][r].last == pokoje[i][y][r].next)dobre = false;
    } while (dobre == false);
}
void pathNew(int i, int y, int r)
{
    short lastDir;
    int a = i, b = y, c = r;
    bool pathProgress = true;
    while (pathProgress)
    {
        losuj(a, b, c);
        switch (pokoje[a][b][c].next)
        {
        case 0: c++;
            lastDir = 1;
            break;
        case 1: c--;
            lastDir = 0;
            break;
        case 2: b++;
            lastDir = 4;
            break;
        case 3: a++;
            lastDir = 5;
            break;
        case 4: b--;
            lastDir = 2;
            break;
        case 5: a--;
            lastDir = 3;
            break;
        }
        if (pokoje[a][b][c].mazePart)
        {
            while (pokoje[i][y][r].mazePart == false)
            {
                pokoje[i][y][r].mazePart = true;
                pokoje[i][y][r].somsiad[pokoje[i][y][r].next] = true;
                switch (pokoje[i][y][r].next)
                {
                case 0: r++;
                    pokoje[i][y][r].somsiad[1] = true;
                    break;
                case 1: r--;
                    pokoje[i][y][r].somsiad[0] = true;
                    break;
                case 2: y++;
                    pokoje[i][y][r].somsiad[4] = true;
                    break;
                case 3: i++;
                    pokoje[i][y][r].somsiad[5] = true;
                    break;
                case 4: y--;
                    pokoje[i][y][r].somsiad[2] = true;
                    break;
                case 5: i--;
                    pokoje[i][y][r].somsiad[3] = true;
                    break;
                }
            }
            pathProgress = false;
            break;
        }
        else if (pokoje[a][b][c].next >= 0)
        {
            while (pokoje[a][b][c].next >= 0)
                switch (pokoje[a][b][c].next)
                {
                case 0: pokoje[a][b][c].next = -1;
                    c++;
                    break;
                case 1: pokoje[a][b][c].next = -1;
                    c--;
                    break;
                case 2: pokoje[a][b][c].next = -1;
                    b++;
                    break;
                case 3: pokoje[a][b][c].next = -1;
                    a++;
                    break;
                case 4: pokoje[a][b][c].next = -1;
                    b--;
                    break;
                case 5: pokoje[a][b][c].next = -1;
                    a--;
                    break;
                }
        }
        else
            pokoje[a][b][c].last = lastDir;
    }
}
void mazeSetter()
{
    int i, y, r;
    pokoje[rand() % Xsize][rand() % Ysize][rand() % Zsize].mazePart = 1;
    for (int s = 0; s < 10; s++)
    {
        i = rand() % Xsize;
        y = rand() % Ysize;
        r = rand() % Zsize;
        if (pokoje[i][y][r].mazePart == false)
        {
            pathNew(i, y, r);
        }
    }
    for (int i = 0; i < Xsize; i++)
        for (int y = 0; y < Ysize; y++)
            for (int r = 0; r < Zsize; r++)
                if (pokoje[i][y][r].mazePart == false)
                    pathNew(i, y, r);
    //rest;
}

float linePlane(glm::vec3 n, glm::vec3 pos, glm::vec3 a, glm::vec3 b) {
    float denominator = glm::dot(n, b - a);
    if (denominator == 0)return -1.0f;
    return (glm::dot(n, pos - a) / denominator);
}

void switchLevers(){
	glm::vec3 direction = glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
	float r = linePlane( glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), pos, pos + 1.5f * direction);
	if(r > 0 && r < 1) {
		glm::vec3 p = pos + 1.5f * direction * r;
		if(p.x > -0.6 && p.x < 0.6 && p.z > -0.6 && p.z < 0.6 ){
			if(leverSpeed[0] > 0.0f) leverSpeed[0] = -0.5f;
			else if(leverSpeed[0] < 0.0f) leverSpeed[0] = 0.5f;
			else leverSpeed[0] = leverRotation[0]>0.0f ? -0.5f : 0.5f;
		}
		if(p.x > -0.6 && p.x < 0.6 && p.z > 4*(Zsize-1)-0.6 && p.z < 4*(Zsize-1)+0.6 ){
			if(leverSpeed[1] > 0.0f) leverSpeed[1] = -0.5f;
			else if(leverSpeed[1] < 0.0f) leverSpeed[1] = 0.5f;
			else leverSpeed[1] = leverRotation[1]>0.0f ? -0.5f : 0.5f;
		}
		if(p.x > 4*(Xsize-1)-0.6 && p.x < 4*(Xsize-1)+0.6 && p.z > -0.6 && p.z < 0.6 ){
			if(leverSpeed[2] > 0.0f) leverSpeed[2] = -0.5f;
			else if(leverSpeed[2] < 0.0f) leverSpeed[2] = 0.5f;
			else leverSpeed[2] = leverRotation[2]>0.0f ? -0.5f : 0.5f;
		}
		if(p.x > 4*(Xsize-1)-0.6 && p.x < 4*(Xsize-1)+0.6 && p.z > 4*(Zsize-1)-0.6 && p.z < 4*(Zsize-1)+0.6 ){
			if(leverSpeed[3] > 0.0f) leverSpeed[3] = -0.5f;
			else if(leverSpeed[3] < 0.0f) leverSpeed[3] = 0.5f;
			else leverSpeed[3] = leverRotation[3]>0.0f ? -0.5f : 0.5f;
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_yaw = 3;
        if (key == GLFW_KEY_RIGHT) speed_yaw = -3;
        if (key == GLFW_KEY_W) speed = 3;
        if (key == GLFW_KEY_S) speed = -3;
        if (key == GLFW_KEY_A) speed_left = -2.5;
        if (key == GLFW_KEY_D) speed_left = 2.5;
        if (key == GLFW_KEY_UP) speed_pitch = 3;
        if (key == GLFW_KEY_DOWN) speed_pitch = -3;
        if (key == GLFW_KEY_E) switchLevers();
        if (key == GLFW_KEY_F) flashlight = flashlight ? 0 : 1;
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_yaw = 0;
        if (key == GLFW_KEY_RIGHT) speed_yaw = 0;
        if (key == GLFW_KEY_W) speed = 0;
        if (key == GLFW_KEY_S) speed = 0;
        if (key == GLFW_KEY_A) speed_left = 0;
        if (key == GLFW_KEY_D) speed_left = 0;
        if (key == GLFW_KEY_UP) speed_pitch = 0;
        if (key == GLFW_KEY_DOWN) speed_pitch = 0;
    }
}
GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Read into computers memory
    std::vector<unsigned char> image;   //Allocate memory 
    unsigned width, height;   //Variables for image size
    //Read the image
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import to graphics card memory
    glGenTextures(1, &tex); //Initialize one handle
    glBindTexture(GL_TEXTURE_2D, tex); //Activate handle
    //Copy image to graphics cards memory reprezented by the active handle
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}


//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glEnable(GL_FRAMEBUFFER_SRGB);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    sp = new ShaderProgram("v_shader.glsl", NULL, "f_shader.glsl");
    tex0 = readTexture("Bricks076A_1K_Color.png");
    tex1 = readTexture("Bricks076A_1K_Normal.png");
    tex2 = readTexture("Bricks076A_1K_Displacement.png");
    tex3 = readTexture("Bricks076A_1K_AmbientOcclusion.png");//Bricks076A
    floor0 = readTexture("Rocks025_1K_Color.png");//DiamondPlate006B
    floor1 = readTexture("Rocks025_1K_Normal.png");//PavingStones046
    floor2 = readTexture("Rocks025_1K_Displacement.png");//Rocks025
    floor3 = readTexture("Rocks025_1K_AmbientOcclusion.png");
    lamp0 = readTexture("DiamondPlate006B_1K_Color.png");
    lamp1 = readTexture("DiamondPlate006B_1K_Normal.png");
    lamp2 = readTexture("DiamondPlate006B_1K_Displacement.png");
    lamp3 = readTexture("DiamondPlate006B_1K_AmbientOcclusion.png");
    lever0 = readTexture("Wood035_1K_Color.png");
    lever1 = readTexture("Wood035_1K_Normal.png");
    lever2 = readTexture("Wood035_1K_Displacement.png");
}
//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Place any code here that needs to be executed once, after the main loop ends************
    delete sp;
    //************Place any code here that needs to be executed once, after the main loop ends************
}
void addCubeWall(float x_przes, float y_przes, float z_przes, float x_scale, float y_scale, float z_scale, int side, int tex)
{
    for (int i = 0; i < 6; i++)
    {
        verttab[tex][nr_vr[tex] * 4 + i * 4] = myCubeVertices[side][i * 4] * x_scale + x_przes;
        verttab[tex][nr_vr[tex] * 4 + i * 4 + 1] = myCubeVertices[side][i * 4 + 1] * y_scale + y_przes;
        verttab[tex][nr_vr[tex] * 4 + i * 4 + 2] = myCubeVertices[side][i * 4 + 2] * z_scale + z_przes;
        verttab[tex][nr_vr[tex] * 4 + i * 4 + 3] = myCubeVertices[side][i * 4 + 3];
        if (x_scale != 1 || z_scale != 1)
        {
            texCordtab[tex][nr_vr[tex] * 2 + i * 2] = 1 - (verttab[tex][nr_vr[tex] * 4 + i * 4] + 1 - float(int((verttab[tex][nr_vr[tex] * 4 + i * 4] + 1) / 4) * 4.0f)) / 2;
            texCordtab[tex][nr_vr[tex] * 2 + i * 2 + 1] = 1 - (verttab[tex][nr_vr[tex] * 4 + i * 4 + 2] + 1 - float(int((verttab[tex][nr_vr[tex] * 4 + i * 4 + 2] + 1) / 4) * 4.0f)) / 2;
        }
        else
        {
            texCordtab[tex][nr_vr[tex] * 2 + i * 2] = myCubeTexCoords[side][i * 2];
            texCordtab[tex][nr_vr[tex] * 2 + i * 2 + 1] = myCubeTexCoords[side][i * 2 + 1];
        }
        normtab[tex][nr_vr[tex] * 4 + i * 4] = myCubeNormals[side][i * 4];
        normtab[tex][nr_vr[tex] * 4 + i * 4 + 1] = myCubeNormals[side][i * 4 + 1];
        normtab[tex][nr_vr[tex] * 4 + i * 4 + 2] = myCubeNormals[side][i * 4 + 2];
        normtab[tex][nr_vr[tex] * 4 + i * 4 + 3] = myCubeNormals[side][i * 4 + 3];
        c1tab[tex][nr_vr[tex] * 4 + i * 4] = myCubeC1[side][i * 4];
        c1tab[tex][nr_vr[tex] * 4 + i * 4 + 1] = myCubeC1[side][i * 4 + 1];
        c1tab[tex][nr_vr[tex] * 4 + i * 4 + 2] = myCubeC1[side][i * 4 + 2];
        c1tab[tex][nr_vr[tex] * 4 + i * 4 + 3] = myCubeC1[side][i * 4 + 3];
        c2tab[tex][nr_vr[tex] * 4 + i * 4] = myCubeC2[side][i * 4];
        c2tab[tex][nr_vr[tex] * 4 + i * 4 + 1] = myCubeC2[side][i * 4 + 1];
        c2tab[tex][nr_vr[tex] * 4 + i * 4 + 2] = myCubeC2[side][i * 4 + 2];
        c2tab[tex][nr_vr[tex] * 4 + i * 4 + 3] = myCubeC2[side][i * 4 + 3];
        c3tab[tex][nr_vr[tex] * 4 + i * 4] = myCubeC3[side][i * 4];
        c3tab[tex][nr_vr[tex] * 4 + i * 4 + 1] = myCubeC3[side][i * 4 + 1];
        c3tab[tex][nr_vr[tex] * 4 + i * 4 + 2] = myCubeC3[side][i * 4 + 2];
        c3tab[tex][nr_vr[tex] * 4 + i * 4 + 3] = myCubeC3[side][i * 4 + 3];
    }
    nr_vr[tex] += 6;
}
void transformRampStair()
{
    for (int i = 0; i < tsize[0]; i++)
    {
        vertt[0][tsize[0] * 4 + i * 4] = -vertt[0][i * 4 + 2];
        vertt[0][tsize[0] * 4 + i * 4 + 1] = vertt[0][i * 4 + 1] - 1;
        vertt[0][tsize[0] * 4 + i * 4 + 2] = vertt[0][i * 4];
        vertt[0][tsize[0] * 4 + i * 4 + 3] = 1.0f;
        text[0][tsize[0] * 2 + i * 2] = text[0][i * 2];
        text[0][tsize[0] * 2 + i * 2 + 1] = text[0][i * 2 + 1];
        normt[0][tsize[0] * 4 + i * 4] = -normt[0][i * 4 + 2];
        normt[0][tsize[0] * 4 + i * 4 + 1] = normt[0][i * 4 + 1];
        normt[0][tsize[0] * 4 + i * 4 + 2] = normt[0][i * 4];
        normt[0][tsize[0] * 4 + i * 4 + 3] = 1.0f;
    }
    tsize[0] *= 2;
    for (int i = 0; i < tsize[0]; i++)
    {
        vertt[0][tsize[0] * 4 + i * 4] = -vertt[0][i * 4];
        vertt[0][tsize[0] * 4 + i * 4 + 1] = vertt[0][i * 4 + 1] + 2;
        vertt[0][tsize[0] * 4 + i * 4 + 2] = -vertt[0][i * 4 + 2];
        vertt[0][tsize[0] * 4 + i * 4 + 3] = 1.0f;
        text[0][tsize[0] * 2 + i * 2] = text[0][i * 2];
        text[0][tsize[0] * 2 + i * 2 + 1] = text[0][i * 2 + 1];
        normt[0][tsize[0] * 4 + i * 4] = -normt[0][i * 4];
        normt[0][tsize[0] * 4 + i * 4 + 1] = normt[0][i * 4 + 1];
        normt[0][tsize[0] * 4 + i * 4 + 2] = -normt[0][i * 4 + 2];
        normt[0][tsize[0] * 4 + i * 4 + 3] = 1.0f;
    }
    tsize[0] *= 2;
}
void addWalls(bool roof, bool floor)
{
    for (int i = 0; i < Xsize; i++)
        for (int y = 0; y < Ysize; y++)
            for (int r = 0; r < Zsize; r++)
            {
                if (roof)
                {
                    if (!pokoje[i][y][r].somsiad[2])
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 3, 0);
                    if (pokoje[i][y][r].somsiad[3])
                        addCubeWall(i * 4 + 2, y * 4, r * 4, 1, 1, 1, 3, 0);
                    if (pokoje[i][y][r].somsiad[0])
                        addCubeWall(i * 4, y * 4, r * 4 + 2, 1, 1, 1, 3, 0);
                }
                else if (floor)
                {
                    if (!pokoje[i][y][r].somsiad[4])
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 2, 1);
                    else
                    {
                        addCubeWall(i * 4 + 1.0f / 2, y * 4 - 201.0f / 100, r * 4, 1.0f / 2, 1, 1, 3, 1);
                        addCubeWall(i * 4 - 1.0f / 2, y * 4 - 201.0f / 100, r * 4 - 3.0f / 4, 1.0f / 2, 1, 1.0f / 4, 3, 1);
                        addCubeWall(i * 4 + 1.0f / 2, y * 4, r * 4, 1.0f / 2, 1, 1, 2, 1);
                        addCubeWall(i * 4 - 1.0f / 2, y * 4, r * 4 - 3.0f / 4, 1.0f / 2, 1, 1.0f / 4, 2, 1);
                    }
                    if (pokoje[i][y][r].somsiad[3])
                        addCubeWall(i * 4 + 2, y * 4, r * 4, 1, 1, 1, 2, 1);
                    if (pokoje[i][y][r].somsiad[0])
                        addCubeWall(i * 4, y * 4, r * 4 + 2, 1, 1, 1, 2, 1);
                }
                else
                {
                    if (pokoje[i][y][r].somsiad[2])
                    {
                        addCubeWall(i * 4, y * 4 + 2, r * 4, 1, 1, 1, 0, 2);
                        addCubeWall(i * 4, y * 4 + 2, r * 4, 1, 1, 1, 1, 2);
                        addCubeWall(i * 4, y * 4 + 2, r * 4, 1, 1, 1, 4, 2);
                        addCubeWall(i * 4, y * 4 + 2, r * 4, 1, 1, 1, 5, 2);
                    }
                    if (!pokoje[i][y][r].somsiad[1])
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 0, 2);
                    if (!pokoje[i][y][r].somsiad[5])
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 4, 2);
                    if (pokoje[i][y][r].somsiad[0])
                    {
                        addCubeWall(i * 4, y * 4, r * 4 + 2, 1, 1, 1, 4, 2);
                        addCubeWall(i * 4, y * 4, r * 4 + 2, 1, 1, 1, 5, 2);
                    }
                    else
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 1, 2);
                    if (pokoje[i][y][r].somsiad[3])
                    {
                        addCubeWall(i * 4 + 2, y * 4, r * 4, 1, 1, 1, 0, 2);
                        addCubeWall(i * 4 + 2, y * 4, r * 4, 1, 1, 1, 1, 2);
                    }
                    else
                        addCubeWall(i * 4, y * 4, r * 4, 1, 1, 1, 5, 2);
                }
            }
}
void addRamps()
{
    for (int i = 0; i < Xsize; i++)
        for (int y = 0; y < Ysize; y++)
            for (int r = 0; r < Zsize; r++)
            {
                if (pokoje[i][y][r].somsiad[2])
                {
                    for (int k = 0; k < tsize[0]; k++)
                    {
                        verttab[3][nr_vr[3] * 4 + k * 4] = vertt[0][k * 4] + i * 4;
                        verttab[3][nr_vr[3] * 4 + k * 4 + 1] = vertt[0][k * 4 + 1] + y * 4;
                        verttab[3][nr_vr[3] * 4 + k * 4 + 2] = vertt[0][k * 4 + 2] + r * 4;
                        verttab[3][nr_vr[3] * 4 + k * 4 + 3] = vertt[0][k * 4 + 3];
                        texCordtab[3][nr_vr[3] * 2 + k * 2] = text[0][k * 2];
                        texCordtab[3][nr_vr[3] * 2 + k * 2 + 1] = text[0][k * 2 + 1];
                        normtab[3][nr_vr[3] * 4 + k * 4] = normt[0][k * 4];
                        normtab[3][nr_vr[3] * 4 + k * 4 + 1] = normt[0][k * 4 + 1];
                        normtab[3][nr_vr[3] * 4 + k * 4 + 2] = normt[0][k * 4 + 2];
                        normtab[3][nr_vr[3] * 4 + k * 4 + 3] = normt[0][k * 4 + 3];
                    }
                    nr_vr[3] += tsize[0];
                }

            }
}
void addLamps()
{
    for (int i = 0; i < Xsize; i++)
        for (int y = 0; y < Ysize; y++)
            for (int r = 0; r < Zsize; r++)
            {
                if (pokoje[i][y][r].somsiad[0])
                {
                    lights[nr_lights * 3] = i * 4.0f;
                    lights[nr_lights * 3 + 1] = y * 4+0.75;
                    lights[nr_lights * 3 + 2] = r * 4 + 2;
                    nr_lights++;
                    for (int k = 0; k < tsize[0]; k++)
                    {
                        verttab[4][nr_vr[4] * 4 + k * 4] = vertt[2][k * 4] + i * 4;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 1] = vertt[2][k * 4 + 1] + y * 4+0.2;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 2] = vertt[2][k * 4 + 2] + r * 4+2;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 3] = vertt[2][k * 4 + 3];
                        texCordtab[4][nr_vr[4] * 2 + k * 2] = text[2][k * 2];
                        texCordtab[4][nr_vr[4] * 2 + k * 2 + 1] = text[2][k * 2 + 1];
                        normtab[4][nr_vr[4] * 4 + k * 4] = normt[2][k * 4];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 1] = normt[2][k * 4 + 1];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 2] = normt[2][k * 4 + 2];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 3] = normt[2][k * 4 + 3];
                    }
                    nr_vr[4] += tsize[2];
                }
                if (pokoje[i][y][r].somsiad[3])
                {
                    lights[nr_lights * 3] = i * 4+2;
                    lights[nr_lights * 3 + 1] = y * 4 + 0.75;
                    lights[nr_lights * 3 + 2] = r * 4;
                    nr_lights++;
                    for (int k = 0; k < tsize[0]; k++)
                    {
                        verttab[4][nr_vr[4] * 4 + k * 4] = vertt[2][k * 4] + i * 4+2;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 1] = vertt[2][k * 4 + 1] + y * 4+0.2;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 2] = vertt[2][k * 4 + 2] + r * 4;
                        verttab[4][nr_vr[4] * 4 + k * 4 + 3] = vertt[2][k * 4 + 3];
                        texCordtab[4][nr_vr[4] * 2 + k * 2] = text[2][k * 2];
                        texCordtab[4][nr_vr[4] * 2 + k * 2 + 1] = text[2][k * 2 + 1];
                        normtab[4][nr_vr[4] * 4 + k * 4] = normt[2][k * 4];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 1] = normt[2][k * 4 + 1];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 2] = normt[2][k * 4 + 2];
                        normtab[4][nr_vr[4] * 4 + k * 4 + 3] = normt[2][k * 4 + 3];
                    }
                    nr_vr[4] += tsize[2];
                }
            }
}
void drawLever(glm::mat4 P, glm::mat4 V)
{
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 A;
    //front 1 back 0 up 3 down 2 left 5 right 4
    sp->use();

    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniform1i(sp->u("flashlight"), flashlight);
    glUniform1i(sp->u("n_lights"), nr_lights);
	glUniform3fv(sp->u("lightPos"), nr_lights, lights);
	glUniform1i(sp->u("obj_type"), 7);
    for (int obj = 0; obj < 4; obj++)
    {
        A = glm::translate(M, glm::vec3(obj / 2 * (Xsize - 1) * 4.0, -1.07f, obj % 2 * (Zsize - 1) * 4.0));
        A = glm::rotate(A, leverRotation[obj], glm::vec3(1.0f, 0.0f, 0.0f));
        A = glm::translate(A, glm::vec3(0.0f, 0.1f, 0.0f));
        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(A));
        glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
        glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertt[1]); //Specify source of the data for the attribute vertex
        glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
        glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, text[1]); //Specify source of the data for the attribute texCoord0
        //glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
        //glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normt[1]); //Specify source of the data for the attribute normal

		glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
        glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
        glBindTexture(GL_TEXTURE_2D, lever0);

        glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap0 with the 0-th texturing unit
        glActiveTexture(GL_TEXTURE1); //Assign texture tex0 to the 0-th texturing unit
        glBindTexture(GL_TEXTURE_2D, lever1);

        glUniform1i(sp->u("textureMap2"), 2); // Associate sampler textureMap2 with the 2-nd texturing unit
        glActiveTexture(GL_TEXTURE2); //Assign texture tex2 to the 2-nd texturing unit
        glBindTexture(GL_TEXTURE_2D, lever2);
 
        glEnableVertexAttribArray(sp->a("c1"));
        glVertexAttribPointer(sp->a("c1"), 4, GL_FLOAT, false, 0, c1t[1]);

        glEnableVertexAttribArray(sp->a("c2"));
        glVertexAttribPointer(sp->a("c2"), 4, GL_FLOAT, false, 0, c2t[1]);

        glEnableVertexAttribArray(sp->a("c3"));
        glVertexAttribPointer(sp->a("c3"), 4, GL_FLOAT, false, 0, c3t[1]);
 
        glDrawArrays(GL_TRIANGLES, 0, tsize[1]); //Draw the object

        glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
        glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute texCoord0
        //glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
        glDisableVertexAttribArray(sp->a("c1"));
        glDisableVertexAttribArray(sp->a("c2"));
        glDisableVertexAttribArray(sp->a("c3"));
    }
}

void drawMaze(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
    //front 1 back 0 up 3 down 2 left 5 right 4
    sp->use();

    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    glUniform1i(sp->u("flashlight"), flashlight);
    glUniform1i(sp->u("n_lights"), nr_lights);
	glUniform3fv(sp->u("lightPos"), nr_lights, lights);
    for (int nr_tex = 0; nr_tex < 5; nr_tex++)
    {
		glUniform1i(sp->u("obj_type"), nr_tex);
        glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
        glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verttab[nr_tex]); //Specify source of the data for the attribute vertex
        glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
        glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCordtab[nr_tex]); //Specify source of the data for the attribute texCoord0

        if (nr_tex == 1) {
            glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, floor0);

            glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE1); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, floor1);

            glUniform1i(sp->u("textureMap2"), 2); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE2); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, floor2);

            glUniform1i(sp->u("textureMap3"), 3); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE3); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, floor3);
        }
        else if (nr_tex == 4) {
            glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, lamp0);

            glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE1); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, lamp1);

            glUniform1i(sp->u("textureMap2"), 2); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE2); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, lamp2);

            glUniform1i(sp->u("textureMap3"), 3); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE3); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, lamp3);
        }
        else {
            glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, tex0);

            glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap0 with the 0-th texturing unit
            glActiveTexture(GL_TEXTURE1); //Assign texture tex0 to the 0-th texturing unit
            glBindTexture(GL_TEXTURE_2D, tex1);

            glUniform1i(sp->u("textureMap2"), 2); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE2); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, tex2);

            glUniform1i(sp->u("textureMap3"), 3); // Associate sampler textureMap2 with the 2-nd texturing unit
            glActiveTexture(GL_TEXTURE3); //Assign texture tex2 to the 2-nd texturing unit
            glBindTexture(GL_TEXTURE_2D, tex3);
        }

        glEnableVertexAttribArray(sp->a("c1"));
        glVertexAttribPointer(sp->a("c1"), 4, GL_FLOAT, false, 0, c1tab[nr_tex]);

        glEnableVertexAttribArray(sp->a("c2"));
        glVertexAttribPointer(sp->a("c2"), 4, GL_FLOAT, false, 0, c2tab[nr_tex]);

        glEnableVertexAttribArray(sp->a("c3"));
        glVertexAttribPointer(sp->a("c3"), 4, GL_FLOAT, false, 0, c3tab[nr_tex]);

        glDrawArrays(GL_TRIANGLES, 0, nr_vr[nr_tex]); //Draw the object

        glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
        glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute texCoord0
        //glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
        glDisableVertexAttribArray(sp->a("c1"));
        glDisableVertexAttribArray(sp->a("c2"));
        glDisableVertexAttribArray(sp->a("c3"));
    }
}
//Drawing procedure
void drawScene(GLFWwindow* window, glm::vec3 pos, glm::vec3 direction, float yaw, float pitch) {
    //************Place any code here that draws something inside the window******************l

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 V = glm::lookAt(pos, pos + direction, glm::vec3(0.0f, 1.0f, 0.0f)); //compute view matrix
    glm::mat4 P = glm::perspective(50.0f * PI / 100.0f, aspectRatio, 0.1f, 16.0f); //compute projection matrix

    drawMaze(P, V, M);
    //drawRamps(P, V, M);
    //drawLamps(P, V, M);
    drawLever(P, V);

    glfwSwapBuffers(window); //Copy back buffer to front buffer
}
bool insideTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    if (glm::dot(glm::cross(b - a, p - a), glm::cross(c - b, p - b)) < 0.0f || glm::dot(glm::cross(a - c, p - c), glm::cross(c - b, p - b)) < 0.0f)return 0;
    return 1;
}
glm::vec3 newPos(glm::vec3 pos, glm::vec3 new_pos) {
    if (new_pos == pos)return pos;
    glm::vec3 t[3];
    float r;
    if (glm::distance(new_pos, pos) > 0.2f) return pos;
    for (int j = 0; j < nr_of_tex; j++)
        for (int i = 0; i < 4 * nr_vr[j]; i += 12) {
            t[0] = glm::vec3(verttab[j][i], verttab[j][i + 1], verttab[j][i + 2]);
            t[1] = glm::vec3(verttab[j][i + 4], verttab[j][i + 5], verttab[j][i + 6]);
            t[2] = glm::vec3(verttab[j][i + 8], verttab[j][i + 9], verttab[j][i + 10]);
            bool _1 = abs(t[0].x - pos.x) > 1.01 || abs(t[0].y - pos.y) > 1.01 || abs(t[0].z - pos.z) > 1.01;
            bool _2 = abs(t[1].x - pos.x) > 1.01 || abs(t[1].y - pos.y) > 1.01 || abs(t[1].z - pos.z) > 1.01;
            bool _3 = abs(t[2].x - pos.x) > 1.01 || abs(t[2].y - pos.y) > 1.01 || abs(t[2].z - pos.z) > 1.01;
            if (_1 && _2 && _3)continue;
            if (j != 0 && j != 1) {
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.2f, 0.0f, -0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.2f * r - 0.2f, 0.0f, 0.2f * r - 0.2f), t[0], t[1], t[2])) {
                        new_pos.x += 0.0001 + 0.2 * r;
                        new_pos.z += 0.0001 + 0.2 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.2f, 0.0f, 0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.2f * r + 0.2f, 0.0f, -0.2f * r + 0.2f), t[0], t[1], t[2])) {
                        new_pos.x -= 0.0001 + 0.2 * r;
                        new_pos.z -= 0.0001 + 0.2 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.2f, 0.0f, -0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.2f * r + 0.2f, 0.0f, 0.2f * r - 0.2f), t[0], t[1], t[2])) {
                        new_pos.z += 0.0001 + 0.2 * r;
                        new_pos.x -= 0.0001 + 0.2 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.2f, 0.0f, 0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.2f * r - 0.2f, 0.0f, -0.2f * r + 0.2f), t[0], t[1], t[2])) {
                        new_pos.z -= 0.0001 + 0.2 * r;
                        new_pos.x += 0.0001 + 0.2 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.25f, 0.0f, 0.0f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.25f * r - 0.25f, 0.0f, 0.0f), t[0], t[1], t[2])) {
                        new_pos.x += 0.0001 + 0.25 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.25f, 0.0f, 0.0f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.25f * r + 0.25f, 0.0f, 0.0f), t[0], t[1], t[2])) {
                        new_pos.x -= 0.0001 + 0.25 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, 0.0f, -0.25f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, 0.0f, 0.25f * r - 0.25f), t[0], t[1], t[2])) {
                        new_pos.z += 0.0001 + 0.25 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, 0.0f, 0.25f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, 0.0f, -0.25f * r + 0.25f), t[0], t[1], t[2])) {
                        new_pos.z -= 0.0001 + 0.25 * r;
                    }
                }


                glm::vec3 t[3];
                t[0] = glm::vec3(verttab[j][i], verttab[j][i + 1] + 0.4f, verttab[j][i + 2]);
                t[1] = glm::vec3(verttab[j][i + 4], verttab[j][i + 5] + 0.4f, verttab[j][i + 6]);
                t[2] = glm::vec3(verttab[j][i + 8], verttab[j][i + 9] + 0.4f, verttab[j][i + 10]);
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.2f, 0.0f, -0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.2f * r - 0.2f, 0.0f, 0.2f * r - 0.2f), t[0], t[1], t[2])) {
                        new_pos.x += 0.0001 + 0.2 * r;
                        new_pos.z += 0.0001 + 0.2 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.2f, 0.0f, 0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.2f * r + 0.2f, 0.0f, -0.2f * r + 0.2f), t[0], t[1], t[2])) {
                        new_pos.x -= 0.0001 + 0.2 * r;
                        new_pos.z -= 0.0001 + 0.2 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.2f, 0.0f, -0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.2f * r + 0.2f, 0.0f, 0.2f * r - 0.2f), t[0], t[1], t[2])) {
                        new_pos.z += 0.0001 + 0.2 * r;
                        new_pos.x -= 0.0001 + 0.2 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.2f, 0.0f, 0.2f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.2f * r - 0.2f, 0.0f, -0.2f * r + 0.2f), t[0], t[1], t[2])) {
                        new_pos.z -= 0.0001 + 0.2 * r;
                        new_pos.x += 0.0001 + 0.2 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(-0.25f, 0.0f, 0.0f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.25f * r - 0.25f, 0.0f, 0.0f), t[0], t[1], t[2])) {
                        new_pos.x += 0.0001 + 0.25 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.25f, 0.0f, 0.0f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(-0.25f * r + 0.25f, 0.0f, 0.0f), t[0], t[1], t[2])) {
                        new_pos.x -= 0.0001 + 0.25 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, 0.0f, -0.25f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, 0.0f, 0.25f * r - 0.25f), t[0], t[1], t[2])) {
                        new_pos.z += 0.0001 + 0.25 * r;
                    }
                }
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, 0.0f, 0.25f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, 0.0f, -0.25f * r + 0.25f), t[0], t[1], t[2])) {
                        new_pos.z -= 0.0001 + 0.25 * r;
                    }
                }

            }
            if (j != 2) {
                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, -0.7f, 0.0f), new_pos);
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, 0.7f * r - 0.7f, 0.0f), t[0], t[1], t[2])) {
                        speed_y = 0.0f;
                        if (r < 0.9)new_pos.y += 0.0001 + 0.7 * r;
                        else return pos;
                        //new_pos.y += 0.0001 + 0.7 * r;
                    }
                }

                r = linePlane(glm::cross(t[1] - t[0], t[2] - t[1]), t[0], new_pos + glm::vec3(0.0f, 0.2f, 0.0f), new_pos + glm::vec3(0.0f, -0.1f, 0.0f));
                if (r > 0 && r < 1) {
                    if (insideTriangle(new_pos + glm::vec3(0.0f, -0.2f * r + 0.2f, 0.0f), t[0], t[1], t[2])) {
                        speed_y = 0.0f;
                        return pos;//new_pos.y -= 0.0001 + 0.2 * r;
                    }
                }
            }
        }
    return new_pos;
}
void calcTBN(int x){
	for(int i = 0; i < nr_vr[x]; i+=3){
		glm::vec3 n1 = glm::vec3(normtab[x][4*i], normtab[x][4*i+1], normtab[x][4*i+2]);
		glm::vec3 n2 = glm::vec3(normtab[x][4*i+4], normtab[x][4*i+5], normtab[x][4*i+6]);
		glm::vec3 n3 = glm::vec3(normtab[x][4*i+8], normtab[x][4*i+9], normtab[x][4*i+10]);
		glm::vec3 v1 = glm::vec3(verttab[x][4*i], verttab[x][4*i+1], verttab[x][4*i+2]);
		glm::vec3 v2 = glm::vec3(verttab[x][4*i+4], verttab[x][4*i+5], verttab[x][4*i+6]);
		glm::vec3 v3 = glm::vec3(verttab[x][4*i+8], verttab[x][4*i+9], verttab[x][4*i+10]);
		glm::vec2 t1 = glm::vec2(texCordtab[x][2*i], texCordtab[x][2*i+1]);
		glm::vec2 t2 = glm::vec2(texCordtab[x][2*i+2], texCordtab[x][2*i+3]);
		glm::vec2 t3 = glm::vec2(texCordtab[x][2*i+4], texCordtab[x][2*i+5]);
		float f = 1.0f/((t2.x-t1.x) * (t3.y-t1.y) - (t3.x-t1.x) * (t2.y-t1.y));
		glm::vec3 t = glm::vec3(
			((t3.y-t1.y)*(v2.x-v1.x) - (t2.y-t1.y)*(v3.x-v1.x)) * f,
			((t3.y-t1.y)*(v2.y-v1.y) - (t2.y-t1.y)*(v3.y-v1.y)) * f,
			((t3.y-t1.y)*(v2.z-v1.z) - (t2.y-t1.y)*(v3.z-v1.z)) * f
		);

		glm::vec3 b = glm::normalize(glm::cross(n1, t));
		c1tab[x][4*i] = c1tab[x][4*i+4] = c1tab[x][4*i+8] = t.x;
		c1tab[x][4*i+1] = c1tab[x][4*i+5] = c1tab[x][4*i+9] = b.x;
		c1tab[x][4*i+2] = n1.x;
		c1tab[x][4*i+6] = n2.x;
		c1tab[x][4*i+10] = n3.x;
		c1tab[x][4*i+3] = c1tab[x][4*i+7] = c1tab[x][4*i+11] = 0.0f;
		c2tab[x][4*i] = c2tab[x][4*i+4] = c2tab[x][4*i+8] = t.y;
		c2tab[x][4*i+1] = c2tab[x][4*i+5] = c2tab[x][4*i+9] = b.y;
		c2tab[x][4*i+2] = n1.y;
		c2tab[x][4*i+6] = n2.y;
		c2tab[x][4*i+10] = n3.y;
		c2tab[x][4*i+3] = c2tab[x][4*i+7] = c2tab[x][4*i+11] = 0.0f;
		c3tab[x][4*i] = c3tab[x][4*i+4] = c3tab[x][4*i+8] = t.z;
		c3tab[x][4*i+1] = c3tab[x][4*i+5] = c3tab[x][4*i+9] = b.z;
		c3tab[x][4*i+2] = n1.z;
		c3tab[x][4*i+6] = n2.z;
		c3tab[x][4*i+10] = n3.z;
		c3tab[x][4*i+3] = c3tab[x][4*i+7] = c3tab[x][4*i+11] = 0.0f;
	}
	return;
}
void calcLeverTBN(int x){
	for(int i = 0; i < tsize[x]; i+=3){		
		glm::vec3 n1 = glm::vec3(normt[x][4*i], normt[x][4*i+1], normt[x][4*i+2]);
		glm::vec3 n2 = glm::vec3(normt[x][4*i+4], normt[x][4*i+5], normt[x][4*i+6]);
		glm::vec3 n3 = glm::vec3(normt[x][4*i+8], normt[x][4*i+9], normt[x][4*i+10]);
		glm::vec3 v1 = glm::vec3(vertt[x][4*i], vertt[x][4*i+1], vertt[x][4*i+2]);
		glm::vec3 v2 = glm::vec3(vertt[x][4*i+4], vertt[x][4*i+5], vertt[x][4*i+6]);
		glm::vec3 v3 = glm::vec3(vertt[x][4*i+8], vertt[x][4*i+9], vertt[x][4*i+10]);
		glm::vec2 t1 = glm::vec2(text[x][2*i], text[x][2*i+1]);
		glm::vec2 t2 = glm::vec2(text[x][2*i+2], text[x][2*i+3]);
		glm::vec2 t3 = glm::vec2(text[x][2*i+4], text[x][2*i+5]);
		float f = 1.0f/((t2.x-t1.x) * (t3.y-t1.y) - (t3.x-t1.x) * (t2.y-t1.y));
		glm::vec3 t = glm::vec3(
			((t3.y-t1.y)*(v2.x-v1.x) - (t2.y-t1.y)*(v3.x-v1.x)) * f,
			((t3.y-t1.y)*(v2.y-v1.y) - (t2.y-t1.y)*(v3.y-v1.y)) * f,
			((t3.y-t1.y)*(v2.z-v1.z) - (t2.y-t1.y)*(v3.z-v1.z)) * f
		);
		glm::vec3 b = glm::vec3(
			(-(t3.x-t1.x)*(v2.x-v1.x) + (t2.y-t1.x)*(v3.x-v1.x)) * f,
			(-(t3.x-t1.x)*(v2.y-v1.y) + (t2.y-t1.x)*(v3.y-v1.y)) * f,
			(-(t3.x-t1.x)*(v2.z-v1.z) + (t2.y-t1.x)*(v3.z-v1.z)) * f
		);
		b = glm::normalize(glm::cross(n1, t));
		c1t[x][4*i] = c1t[x][4*i+4] = c1t[x][4*i+8] = t.x;
		c1t[x][4*i+1] = c1t[x][4*i+5] = c1t[x][4*i+9] = b.x;
		c1t[x][4*i+2] = n1.x;
		c1t[x][4*i+6] = n2.x;
		c1t[x][4*i+10] = n3.x;
		c1t[x][4*i+3] = c1t[x][4*i+7] = c1t[x][4*i+11] = 0.0f;
		c2t[x][4*i] = c2t[x][4*i+4] = c2t[x][4*i+8] = t.y;
		c2t[x][4*i+1] = c2t[x][4*i+5] = c2t[x][4*i+9] = b.y;
		c2t[x][4*i+2] = n1.y;
		c2t[x][4*i+6] = n2.y;
		c2t[x][4*i+10] = n3.y;
		c2t[x][4*i+3] = c2t[x][4*i+7] = c2t[x][4*i+11] = 0.0f;
		c3t[x][4*i] = c3t[x][4*i+4] = c3t[x][4*i+8] = t.z;
		c3t[x][4*i+1] = c3t[x][4*i+5] = c3t[x][4*i+9] = b.z;
		c3t[x][4*i+2] = n1.z;
		c3t[x][4*i+6] = n2.z;
		c3t[x][4*i+10] = n3.z;
		c3t[x][4*i+3] = c3t[x][4*i+7] = c3t[x][4*i+11] = 0.0f;
	}
	return;
}
void preProcessing()
{
    for (int i = 0; i < nr_of_obj; i++)
    {
        switch (i)
        {
        case 0:
            setObjFile("struct_large_curved_ramp.obj", i);
            break;
        case 1:
            setObjFile("prop_floor_lever.obj", i);
            break;
        case 2:
            setObjFile("prop_floor_barrel.obj", i);
            break;
        }
    }
    for (int i = 0; i < nr_of_tex; i++)
    {
        verttab[i] = new float[500000];
        texCordtab[i] = new float[500000];
        normtab[i] = new float[500000];
        c1tab[i] = new float[500000];
        c2tab[i] = new float[500000];
        c3tab[i] = new float[500000];
        switch (i)
        {
        case 0://roof
            addWalls(1, 0);
            break;
        case 1://floor
            addWalls(0, 1);
            break;
        case 2://wall wool
            addWalls(0, 0);
            break;
        case 3:
            transformRampStair();
            addRamps();
            break;
        case 4:
            addLamps();
            break;
        }

    }
}
int main(void) {
    srand(0);
    setBegin();
    mazeSetter();
    preProcessing();
    calcTBN(3);
    calcTBN(4);
    calcLeverTBN(1);
    GLFWwindow* window; //Pointer to object that represents the application window

    glfwSetErrorCallback(error_callback);//Register error processing callback procedure

    if (!glfwInit()) { //Initialize GLFW library
        fprintf(stderr, "Can't initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

    if (!window) //If no window is opened then close the program
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
    glfwSwapInterval(1); //During vsync wait for the first refresh

    GLenum err;
    if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
        fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window); //Call initialization procedure
    for(int i=0; i<4; i++) leverRotation[i] = 0.8f;
    for(int i=0; i<4; i++) leverSpeed[i] = 0.0f;
    glfwSetTime(0); //Zero the timer
    //Main application loop
    while (!glfwWindowShouldClose(window))
    {
        float G = 8.0f;
        speed_y -= G * glfwGetTime();
        glm::vec3 new_pos = pos + glm::vec3(
            (-cos(yaw) * speed_left + sin(yaw) * speed) * glfwGetTime(),
            speed_y * glfwGetTime(),//sin(pitch) * speed * glfwGetTime()
            (sin(yaw) * speed_left + cos(yaw) * speed) * glfwGetTime());
        glm::vec3 view_direction = glm::vec3(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
        yaw += speed_yaw * glfwGetTime();
        pitch += speed_pitch * glfwGetTime();
        pos = newPos(pos, new_pos);
        while (yaw > 2 * PI)yaw -= 2 * PI;
        while (yaw < -2 * PI)yaw += 2 * PI;
        if (pitch > PI / 2 - 0.01)pitch = PI / 2 - 0.01;
        if (pitch < -PI / 2 + 0.01)pitch = -PI / 2 + 0.01;
        for(int i=0; i<4; i++){
			leverRotation[i] += leverSpeed[i] * glfwGetTime();
			if(leverRotation[i] < -0.8){
				leverSpeed[i] = 0;
				leverRotation[i] = -0.8;
				leverFlag[i] = 1; 
			}else if(leverRotation[i] > 0.8){
				leverSpeed[i] = 0;
				leverRotation[i] = 0.8;
			}
		}
        glfwSetTime(0); //Zero the timer
        drawScene(window, pos, view_direction, yaw, pitch); //Execute drawing procedure
        glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
    }
    freeOpenGLProgram(window);

    glfwDestroyWindow(window); //Delete OpenGL context and the window.
    glfwTerminate(); //Free GLFW resources
    exit(EXIT_SUCCESS);
}
