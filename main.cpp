#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#include <myprogram.hpp>
#include <objreader.hpp>

// To test TAM, define it. otherwise don't.
// #define TEST_TAM

// To Combine TAM Texture, define it. otherwise don't.
#define TEXTURE_COMBINE

GLFWwindow *window;

// Callbacks //

const float PI = 3.1415926535f;
float fovy = 45 * PI / 180.0f;

namespace comparator
{
    float max(const float &a, const float &b)
    {
        return a > b ? a : b;
    }
    float min(const float &a, const float &b)
    {
        return a > b ? b : a;
    }
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    fovy -= yoffset / 10;
    fovy = comparator::max(0.01f, comparator::min(fovy, PI - 0.01f));
}

float cameraTheta, cameraPhi;

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    static double lastX = 0;
    static double lastY = 0;
    // when left mouse button clicked
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        // rotate 180 degree per each width/height dragging
        cameraTheta -= dx / w * PI; // related with y-axis rotation
        cameraPhi -= dy / h * PI;   // related with x-axis rotation
        cameraPhi = comparator::max(-PI / 2 + 0.01f, comparator::min(cameraPhi, PI / 2 - 0.01f));
        // printf("%.3f %.3f\n", cameraTheta, cameraPhi);
    }
    // whenever, save current cursor position as previous one
    lastX = xpos;
    lastY = ypos;
}

int toneLevel = 0;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        toneLevel = comparator::min(5, comparator::max(0, toneLevel + 1));
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        toneLevel = comparator::min(5, comparator::max(0, toneLevel - 1));
    }
}

// TAM Texture Load //

// tone: 0 is lightest, (TONE_COUNT-1) is darkest
// mip: 0 is finest, (MIPMAP_COUNT-1) is coarsest // it is reverse order of the paper
const int TONE_COUNT = 6;
const int MIPMAP_COUNT = 4;
GLuint TAMTexture[TONE_COUNT];
std::string filename = "TAM/tone0mip0.png";

void tamTexLoad()
{
    stbi_set_flip_vertically_on_load(1);
    glGenTextures(TONE_COUNT, TAMTexture);
    for (int tone = 0; tone < TONE_COUNT; tone++)
    {
        glBindTexture(GL_TEXTURE_2D, TAMTexture[tone]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, MIPMAP_COUNT - 1);
        for (int mip = 0; mip < MIPMAP_COUNT; mip++)
        {
            filename[8] = '0' + tone;
            filename[12] = '0' + mip;
            int x, y, n;
            unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 0);
            std::cout << filename << " Image: x = " << x << ", y = " << y << ", n = " << n << std::endl;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, mip, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }
}

// Test_TAM //

Program testTAMprogram;
GLuint testTAMVAO, testTAMVBO;

std::vector<glm::vec4> testPoint{
    glm::vec4(-1.0, -1.0, 0.0, 0.0), // lb
    glm::vec4(-1.0, 1.0, 0.0, 1.0),  // lt
    glm::vec4(1.0, -1.0, 1.0, 0.0),  // rb
    glm::vec4(1.0, 1.0, 1.0, 1.0),   // rt
};

void testTAMInit(GLFWwindow *window)
{
    // texture load
    tamTexLoad();

    glfwSetScrollCallback(window, scrollCallback);

    testTAMprogram.loadShader("testTAM.vert", "testTAM.frag");
    testTAMprogram.linkShader();

    glGenBuffers(1, &testTAMVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testTAMVBO);
    glBufferData(GL_ARRAY_BUFFER, testPoint.size() * sizeof(glm::vec4), testPoint.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &testTAMVAO);
    glBindVertexArray(testTAMVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void testTAMRender(GLFWwindow *window)
{

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(testTAMprogram.programID);
    glBindVertexArray(testTAMVAO);
    glBindBuffer(GL_ARRAY_BUFFER, testTAMVBO);

    glm::mat4 modelMat(1);
    glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projMat = glm::perspective(fovy, w / (float)h, 0.0001f, 1000.0f);
    GLuint mmLoc = glGetUniformLocation(testTAMprogram.programID, "modelMat");
    GLuint vmLoc = glGetUniformLocation(testTAMprogram.programID, "viewMat");
    GLuint pmLoc = glGetUniformLocation(testTAMprogram.programID, "projMat");
    glUniformMatrix4fv(mmLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(vmLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(pmLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    GLuint ttLoc = glGetUniformLocation(testTAMprogram.programID, "tamTexture");
    glUniform1d(ttLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TAMTexture[2]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, testPoint.size());

    glfwSwapBuffers(window);
}

// Cylindrical Mapping //

ObjData obj;
Program cmProgram;

GLuint cmVAO;
GLuint cmVertexVBO, cmNormalVBO, cmElementVBO;

// Texture Combine //

Program tcProgram;

GLuint tcVAO;
GLuint tcTexture024, tcTexture135;

void textureCombineInit(GLFWwindow *window)
{
    tamTexLoad();

    tcProgram.loadShader("texturecombine.vert", "texturecombine.frag");
}
void textureCombineRender(GLFWwindow *window)
{
}

// init //

float distX, distY;
float maxX, minX, maxY, minY;

void init(GLFWwindow *window)
{
    // texture load
    tamTexLoad();

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);

    obj.setPrefix("obj");
    obj.loadObject("sphere.obj");
    if (!obj.isOk)
    {
        std::cerr << "Obj read failed" << std::endl;
        return;
    }

    // cmProgram.loadShader("cylindricalmap.vert", "cylindricalmap.frag");
    cmProgram.loadShader("cylindricalmap.vert", "spheremap.frag");
    cmProgram.linkShader();

    glGenVertexArrays(1, &cmVAO);
    glBindVertexArray(cmVAO);

    glGenBuffers(1, &cmVertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cmVertexVBO);
    glBufferData(GL_ARRAY_BUFFER, obj.nVertices * sizeof(obj.vertices[0]), obj.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glGenBuffers(1, &cmNormalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cmNormalVBO);
    glBufferData(GL_ARRAY_BUFFER, obj.nSyncedNormals * sizeof(obj.normals[0]), obj.syncedNormals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glGenBuffers(1, &cmElementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.nElements3 * sizeof(obj.elements3[0]), obj.elements3.data(), GL_STATIC_DRAW);

    maxX = -100, minX = 100, maxY = -100, minY = 100;
    for (auto v : obj.vertices)
    {
        minX = comparator::min(minX, v.x);
        maxX = comparator::max(maxX, v.x);
        minY = comparator::min(minY, v.y);
        maxY = comparator::max(maxY, v.y);
    }
    distX = maxX - minX, distY = maxY - minY;
    std::cout << "distX: " << distX << ", distY: " << distY << std::endl;
}

// render //

void render(GLFWwindow *window)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(cmProgram.programID);

    // glm::mat4 modelMat = glm::translate(glm::vec3(0, -1.4, 0));
    glm::mat4 modelMat(1);
    glm::vec3 eye(0, 0, 5);
    glm::mat4 rotateX = glm::rotate(cameraTheta, glm::vec3(0, 1, 0));
    glm::mat4 rotateY = glm::rotate(cameraPhi, glm::vec3(1, 0, 0));
    glm::vec3 eyePosition = rotateX * rotateY * glm::vec4(eye, 1);
    glm::mat4 viewMat = glm::lookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projMat = glm::perspective(fovy, w / (float)h, 0.0001f, 1000.0f);
    GLuint mmLoc = glGetUniformLocation(cmProgram.programID, "modelMat");
    GLuint vmLoc = glGetUniformLocation(cmProgram.programID, "viewMat");
    GLuint pmLoc = glGetUniformLocation(cmProgram.programID, "projMat");
    glUniformMatrix4fv(mmLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(vmLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(pmLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    GLuint ttLoc = glGetUniformLocation(cmProgram.programID, "tamTexture");
    glUniform1d(ttLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TAMTexture[toneLevel]);

    GLuint raLoc = glGetUniformLocation(cmProgram.programID, "radius");
    glUniform1f(raLoc, distX / 2);
    GLuint heLoc = glGetUniformLocation(cmProgram.programID, "height");
    glUniform1f(heLoc, distY);
    GLuint ofLoc = glGetUniformLocation(cmProgram.programID, "offset");
    glm::vec2 offset(minX + distX / 2, minY + distY / 2);
    glUniform2fv(ofLoc, 1, glm::value_ptr(offset));

    glBindVertexArray(cmVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmElementVBO);

    // short -> 16 bit
    glDrawElements(GL_TRIANGLES, obj.nElements3 * 3, GL_UNSIGNED_SHORT, 0);

    glfwSwapBuffers(window);
}

// main //

int main()
{

    if (!glfwInit())
    {
        std::cout << "GLFW Init Error" << std::endl;
        return -1;
    }

// If MacOS
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // forward compatibility: necessary for OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // set as core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow *window = glfwCreateWindow(128, 128, "TAM", 0, 0);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW Init Error" << std::endl;
        return -1;
    }

#if defined(TEST_TAM)
    testTAMInit(window);
#elif defined(TEXTURE_COMBINE)
    textureCombineInit(window);
#else
    init(window);
#endif

    // render

    while (!glfwWindowShouldClose(window))
    {
#if defined(TEST_TAM)
        testTAMRender(window);
#elif defined(TEXTURE_COMBINE)
        textureCombineRender(window);
#else
        render(window);
#endif
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}