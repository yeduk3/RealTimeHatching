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
#define TEST_TAM

GLFWwindow *window;

// Test_TAM //

Program testTAMprogram;
GLuint testTAMVAO, testTAMVBO;

std::vector<glm::vec4> testPoint{
    glm::vec4(-1.0, -1.0, 0.0, 0.0), // lb
    glm::vec4(-1.0, 1.0, 0.0, 1.0),  // lt
    glm::vec4(1.0, -1.0, 1.0, 0.0),  // rb
    glm::vec4(1.0, 1.0, 1.0, 1.0),   // rt
};

// TAM Texture //

// tone: 0 is lightest, (TONE_COUNT-1) is darkest
// mip: 0 is finest, (MIPMAP_COUNT-1) is coarsest // it is reverse order of the paper
const int TONE_COUNT = 6;
const int MIPMAP_COUNT = 4;
GLuint TAMTexture[TONE_COUNT];
std::string filename = "TAM/tone0mip0.png";

const float PI = 3.1415926535f;
float fovy = 45 * PI / 180.0f;

// Lapped Texture? //
ObjData obj;

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

// init //

void init(GLFWwindow *window)
{
    // texture load
    tamTexLoad();

#ifdef TEST_TAM

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

#else

    obj.setPrefix("obj");
    obj.loadObject("teapot.obj");

#endif
}

// render //

void render(GLFWwindow *window)
{
#ifdef TEST_TAM
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
#else

#endif
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

    GLFWwindow *window = glfwCreateWindow(640, 480, "TAM", 0, 0);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW Init Error" << std::endl;
        return -1;
    }

    init(window);

    // render

    while (!glfwWindowShouldClose(window))
    {
        render(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}