#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <Line.h>
#include <Polygon.h>

int main()
{
    if (! glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    Line lineRenderer;
    Polygon polygonRenderer;
    glm::mat4 view = glm::ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f);

    while (! glfwWindowShouldClose(window))
    {
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        view = glm::ortho(0.0f, (float) width, (float) height, 0.0f, -1.0f, 1.0f);
        glViewport(0, 0, width, height);

        glfwSwapBuffers(window);
        glClearColor(0, 0, .25f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // enable anti-aliasing
        glEnable(GL_MULTISAMPLE);

        polygonRenderer.setThickness(5);
        polygonRenderer.setRoundness(25);
        polygonRenderer.setColor(1, 0, 0, 1);
        polygonRenderer.drawRect(200, 200, 400, 100);
        
        polygonRenderer.setThickness(1);
        polygonRenderer.setColor(0, 1, 0, 1);
        polygonRenderer.drawLine(0, 0, 1000, 1000);

        polygonRenderer.setRoundness(0);
        polygonRenderer.setColor(0, 0, 1, 1);
        polygonRenderer.fillRect(100, 100, 100, 100);

        polygonRenderer.setColor(1, 1, 0, 1);
        polygonRenderer.fillEllipse(300, 300, 100, 200);

        polygonRenderer.setColor(1, 0, 1, 1);
        polygonRenderer.setThickness(1);
        polygonRenderer.drawEllipse(100, 250, 100, 200);

        polygonRenderer.setColor(0, 1, 1, 1);
        polygonRenderer.setThickness(10);
        polygonRenderer.drawLine(100, 100, 100, 150, true);

        polygonRenderer.draw(glm::value_ptr(view));
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}