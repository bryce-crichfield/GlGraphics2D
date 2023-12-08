#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <Polygon.h>

int main()
{
    if (! glfwInit())
        return -1;

    // glfwWindowHint(GLFW_SAMPLES, 4);
    // glfwWindowHint(GLFW_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    Polygon polygonRenderer;
    glm::mat4 view;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0, 0, 0.25, 1);

    while (! glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        view = glm::ortho(0.0f, (float) width, (float) height, 0.0f, -1000000.0f, 0.01f);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        polygonRenderer.setColor(1.0f, 0.0f, 0.0f);
        polygonRenderer.setThickness(10.0f);

        polygonRenderer.drawLine(50, 50, 200, 200);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        polygonRenderer.draw(glm::value_ptr(view));
    }

    glfwTerminate();
    return 0;
}