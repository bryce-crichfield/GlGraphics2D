#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <Graphics.h>

int main()
{
    if (! glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    Graphics graphics;

    glClearColor(0, 0, 0.25, 1);
    while (! glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        graphics.setViewport(width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        graphics.setColor(1.0f, 0.0f, 0.0f);
        graphics.setThickness(10.0f);
        graphics.drawLine(50, 50, 200, 200);

        graphics.setColor(0.0f, 1.0f, 0.0f);
        graphics.setThickness(5.0f);
        graphics.setRoundness(2.0f);
        graphics.drawRect(200, 200, 50, 50);

        graphics.flush();
    }

    glfwTerminate();
    return 0;
}