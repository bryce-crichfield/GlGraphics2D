#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

#include <Line.h>
#include <Rect.h>

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

    Line lineRenderer;
    Rect rectRenderer;
    glm::mat4 view = glm::ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f);

    while (! glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        view = glm::ortho(0.0f, (float) width, (float) height, 0.0f, -1.0f, 1.0f);
        glViewport(0, 0, width, height);

        glfwSwapBuffers(window);
        glClearColor(0, 0, .25f, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        lineRenderer.setColor(1,1,0);
        lineRenderer.line(100,100,100,200);
        lineRenderer.render(glm::value_ptr(view));

        rectRenderer.setColor(1,0,0, 1);
        rectRenderer.rect(200,200,100,100);
        rectRenderer.draw(glm::value_ptr(view));

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}