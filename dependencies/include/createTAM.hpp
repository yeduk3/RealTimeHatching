#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <random>
#include <ctime>

#include <myprogram.hpp>

std::vector<glm::vec4> testPoint;

const glm::vec2 strokeTexLT(0.00, 0.95);
const glm::vec2 strokeTexRT(0.95, 0.95);
const glm::vec2 strokeTexLB(0.00, 0.05);
const glm::vec2 strokeTexRB(0.95, 0.05);

Program program;

GLuint createTAMVAO;
GLuint createTAMVBO;

GLuint strokeTexture;

struct Stroke
{
    // Line's length.
    // It is between `0.3` ~ `1.0` which means multiple of the screen's width.
    float length;
    // Start position of the stroke
    glm::vec2 position;
    // Line's width by pixel.
    // When `isVertical` is `false`(default), it is the height.
    // When `isVertical` is `true`, it is the width.
    float lineWidth;
    // Offset of direction of line.
    float perturbation;
    // Direction of the line.
    // `false` by default.
    bool isVertical;

    friend std::ostream &operator<<(std::ostream &os, const Stroke &s)
    {
        os << "Length: " << s.length << std::endl;
        os << "Position: (" << s.position.x << ", " << s.position.y << ")" << std::endl;
        os << "Line Width: " << s.lineWidth << std::endl;
        os << "Perturbation: " << s.perturbation;

        return os;
    }
};

Stroke createStroke()
{
    Stroke stroke;
    stroke.length = (rand() % 701 + 300) / (float)1000.0f;
    stroke.position.x = (rand() % 1001 - 500) / (float)500.0f;
    stroke.position.y = (rand() % 1001 - 500) / (float)500.0f;
    stroke.lineWidth = 0.05;
    stroke.perturbation = (rand() % 7 - 3) * 3.141592f / (float)180.0f;
    stroke.isVertical = false;

    return stroke;
}

void createTAMInit(GLFWwindow *window)
{
    program.loadShader("stroke.vert", "stroke.frag");
    program.loadGeomShader("stroke.geom");
    program.linkShader();

    srand(time(NULL));

    for (int i = 0; i < 100; i++)
    {
        Stroke s = createStroke();
        std::cout << s << std::endl;
        glm::vec2 rt(s.position.x + s.length * 2, s.position.y);
        glm::vec2 lb(s.position.x, s.position.y - s.lineWidth);
        glm::vec2 rb(rt.x, lb.y);
        glm::mat4 rotate = glm::rotate(s.perturbation, glm::vec3(0, 0, 1));
        // glm::mat4 rotate = glm::rotate(0.0f, glm::vec3(0, 0, 1));
        glm::mat4 translate = glm::translate(glm::vec3(s.position, 0));
        rt = translate * rotate * glm::inverse(translate) * glm::vec4(rt, 0, 1);
        lb = translate * rotate * glm::inverse(translate) * glm::vec4(lb, 0, 1);
        rb = translate * rotate * glm::inverse(translate) * glm::vec4(rb, 0, 1);

        testPoint.push_back(glm::vec4(s.position, strokeTexLT));
        testPoint.push_back(glm::vec4(rt, strokeTexRT));
        testPoint.push_back(glm::vec4(rb, strokeTexRB));
        testPoint.push_back(glm::vec4(s.position, strokeTexLT));
        testPoint.push_back(glm::vec4(rb, strokeTexRB));
        testPoint.push_back(glm::vec4(lb, strokeTexLB));
    }

    // Send Data to Buffer //

    glGenBuffers(1, &createTAMVBO);
    glBindBuffer(GL_ARRAY_BUFFER, createTAMVBO);
    glBufferData(GL_ARRAY_BUFFER, testPoint.size() * sizeof(glm::vec4), testPoint.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &createTAMVAO);
    glBindVertexArray(createTAMVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float)));

    // Original Texture //

    stbi_set_flip_vertically_on_load(true);
    int x, y, n;
    unsigned char *data = stbi_load("TestBrush.png", &x, &y, &n, 0);
    // unsigned char *data = stbi_load("Thin Brushes Pack/O4YINI0.jpg", &x, &y, &n, 0);
    std::cout << "Image: x = " << x << ", y = " << y << ", n = " << n << std::endl;
    // ... process data if not NULL ...
    // ... x = width, y = height, n = # 8-bit components per pixel ...
    // ... replace '0' with '1'..'4' to force that many components per pixel
    // ... but 'n' will always be the number that it would have been if you said 0

    glGenTextures(1, &strokeTexture);
    glBindTexture(GL_TEXTURE_2D, strokeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // texture minify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // texture magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);      // GL_CLAMP or GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

void createTAMRender(GLFWwindow *window)
{

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program.programID);
    glBindVertexArray(createTAMVAO);
    glBindBuffer(GL_ARRAY_BUFFER, createTAMVBO);

    GLuint strokeTexLoc = glGetUniformLocation(program.programID, "strokeTex");
    glUniform1d(strokeTexLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, strokeTexture);

    glDrawArrays(GL_TRIANGLES, 0, testPoint.size());

    glfwSwapBuffers(window);
}