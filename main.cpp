#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>   // The GL Header File
#include <OpenGL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
// #include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

int width = 800, height = 600;
int sampleSize = 10;
float coordMultiplier = 1.0;
float rotationAngle = -30;
int vertCount, horzCount;
int lightSize;
GLuint gVertexAttribBuffer, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

GLuint gProgram[2];
struct Vertex
{
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Normal
{
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct BezierSurface
{
    BezierSurface(
        float cp1,float cp2,float cp3,float cp4,
        float cp5,float cp6,float cp7,float cp8,
        float cp9,float cp10,float cp11,float cp12,
        float cp13,float cp14,float cp15,float cp16
    ){
        cps[0] = cp1; cps[1] = cp2;
        cps[2] = cp3; cps[3] = cp4;
        cps[4] = cp5; cps[5] = cp6;
        cps[6] = cp7; cps[7] = cp8;
        cps[8] = cp9; cps[9] = cp10;
        cps[10] = cp11; cps[11] = cp12;
        cps[12] = cp13; cps[13] = cp14;
        cps[14] = cp15; cps[15] = cp16;
    }
    float cps[16];
};


vector<BezierSurface> surfaces;
glm::vec3 lightPos[5];
glm::vec3 color[5];
// vector<Vertex> gVertices;
// vector<Normal> gNormals;

void drawModel()
{
	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, (sampleSize-1) * (sampleSize-1) * 6, GL_UNSIGNED_INT, 0);
}


void display(){

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(rotationAngle), glm::vec3(1, 0, 0));
    glm::mat4 perspective = glm::perspective(45.0f,(GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    glm::mat4 viewing = glm::lookAt(glm::vec3(0,0,2), glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 transMat = perspective * viewing  * rotate;

    for(int i = 0; i < surfaces.size(); i++){
        glm::mat4 cps = glm::make_mat4x4(surfaces[i].cps);

        glUniform1i(glGetUniformLocation(gProgram[0], "sampleSize"),sampleSize);
        glUniform1f(glGetUniformLocation(gProgram[0], "coordMultiplier"),coordMultiplier);

        glUniform1i(glGetUniformLocation(gProgram[0], "surfaceindex"),i);
        glUniform1i(glGetUniformLocation(gProgram[0], "bezierX"),horzCount/4);
        glUniform1i(glGetUniformLocation(gProgram[0], "bezierY"),vertCount/4);

        glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "modelingMat"), 1, GL_FALSE, glm::value_ptr(rotate));
        glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "transMat"), 1, GL_FALSE, glm::value_ptr(transMat));
        glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "cps"), 1, GL_FALSE, glm::value_ptr(cps));

        glUniform1i(glGetUniformLocation(gProgram[0], "lightSize"),lightSize);
        glUniform3fv(glGetUniformLocation(gProgram[0],"lightPosition"),5,glm::value_ptr(lightPos[0]));
        glUniform3fv(glGetUniformLocation(gProgram[0],"color"),5,glm::value_ptr(color[0]));
        drawModel();
    }


}

bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

void createVS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    // printf("VS compile log: %s\n", output);

    glAttachShader(program, vs);
}

void createFS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    // printf("FS compile log: %s\n", output);

    glAttachShader(program, fs);
}

void initShaders()
{
    gProgram[0] = glCreateProgram();

    createVS(gProgram[0], "vert.glsl");
    createFS(gProgram[0], "frag.glsl");
   
    glLinkProgram(gProgram[0]);
    glUseProgram(gProgram[0]);
}

void initVBO()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);
    // cout << "vao = " << vao << endl;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer);
	glGenBuffers(1, &gIndexBuffer);

	assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
}

void changebuffer()
{
    int vertexCount =sampleSize * sampleSize;

	gVertexDataSizeInBytes = vertexCount * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = vertexCount * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = (sampleSize-1) * (sampleSize-1) * 6 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat [vertexCount  * 3];
	GLfloat* normalData = new GLfloat [vertexCount  * 3];
	GLuint* indexData = new GLuint [ (sampleSize-1) * (sampleSize-1) * 6 ];

	for (int i = 0; i < vertexCount ; ++i)
	{
        vertexData[3*i] = 0;
 		vertexData[3*i+1] = 0;
		vertexData[3*i+2] = 0;
	}

	for (int i = 0; i < vertexCount; ++i)
	{
		normalData[3*i] = 1;
		normalData[3*i+1] = 1;
		normalData[3*i+2] = 1;
	}

    for(int i = 0, k=0 ; i <(sampleSize-1) * (sampleSize-1) ; i++){
        if((k+1) % sampleSize == 0) k++;
        indexData[6*i] = k;
		indexData[6*i + 1] = k + sampleSize;
        indexData[6*i + 2] = k + sampleSize +1;

        indexData[6*i + 3] = k;
		indexData[6*i+4] = k + 1;
        indexData[6*i+5] = k + sampleSize +1;
        k++;
    }

    // for(int i=0; i<(sampleSize-1) * (sampleSize-1) * 2;i++ ){
    //     cout <<  indexData[3*i] <<  " " <<  indexData[3*i+1] << " " << indexData[3*i+2] <<endl;
    // }

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying; can free now
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}


void init() 
{

    glEnable(GL_DEPTH_TEST);
    initVBO();
    changebuffer();
    initShaders();
}


void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    width = w;
    height = h;

    glViewport(0, 0, w, h);
}


void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
       rotationAngle += 10;
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
       rotationAngle -= 10;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
       coordMultiplier += 0.1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
       if(coordMultiplier > 0.1) coordMultiplier -= 0.1;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
       if(sampleSize < 80) {
            sampleSize += 2;
            changebuffer();
       }
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
       if(sampleSize > 2) {
            sampleSize -= 2;
            changebuffer();
       }
    }
}

bool parseInput(string inputFile){
    fstream myfile;

    // Open the input 
    myfile.open(inputFile.c_str(), std::ios::in);
    if(myfile.is_open()){
        myfile >> lightSize;
        for(int i=0; i<lightSize;i++){
            float xPos, yPos, zPos, rInt, gInt, bInt;
            myfile >> xPos >> yPos >> zPos >> rInt >> gInt >> bInt;
            lightPos[i] = glm::vec3(xPos,yPos,zPos);
            color[i] = glm::vec3(rInt,gInt,bInt);
        }
        myfile >> vertCount >> horzCount;
        int surfaceCount = (vertCount/4)*(horzCount/4);
        for(int k=0;k<surfaceCount;k++) surfaces.push_back(BezierSurface(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
        for(int v=0;v<vertCount;v++){
            for(int h=0;h<horzCount;h++){
                int indY = v/4;
                int indx = h/4;
                int index = (v/4) * (horzCount/4) + indx;
                myfile >> surfaces[index].cps[(v%4)*4 + (h%4)];
            }
        }
        // for(int s=0;s<surfaceCount;s++){
        // for(int i = 0;i<4 ; i++){
        //     for(int k=0; k<4 ; k++){
        //         cout<<surfaces[s].cps[4*i+k]<<" ";
        //     }
        //     cout<<endl;
        // }
        // }
        return true;
    }
    else{
        return false;
    }
    return false;
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    if(argc!=2){
        cout<<"usage: ./main <input-file>"<<endl;
        exit(1);
    }

    string inputfile = argv[1];
    if(!parseInput(inputfile)) {
        cout <<"could not read input file"<<endl;
        exit(1);
    }

    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

     if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // char rendererInfo[512] = {0};
    // strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
    // strcat(rendererInfo, " - ");
    // strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
    char title[] = "CENG469_HW1";
    glfwSetWindowTitle(window, title);

    init();

    glfwSetKeyCallback(window, keyboard);

    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}