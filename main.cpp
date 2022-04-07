#include <iostream>
#include <string>
#include "glad.c"
#include "glfw3.h"
#include "model.h"
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "buffers.h"
#include "cubemap.h"

int WINW = 720;
int WINH = 480;

// a box
float boxVertices[]{
    -1, 1, 0,
    1, 1, 0,
    1, -1, 0,
    -1, -1, 0
};

int boxIndices[]{
    0, 2, 1,
    0, 3, 2
};

glm::vec2 boxColliders[]{
    glm::vec2(-1, 1),
    glm::vec2(1, 1),
    glm::vec2(1, -1),
    glm::vec2(-1, -1)
};


// a triangle?
float trigVertices[]{
    -1, 1, 0,
    1, 1, 0,
    1, -1, 0,
};

int trigIndices[]{
    0, 2, 1,
};

glm::vec2 trigColliders[]{
    glm::vec2(-1, 1),
    glm::vec2(1, 1),
    glm::vec2(1, -1),
};

//prototypes suii
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* GetWindow(void);

//other
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main(void) {
    //init oot of da wae
    GLFWwindow* window = GetWindow();
    if (window == NULL) return -1;

    // shaders
    Shader myShader("shaders/2d.vert", "shaders/2d.frag");


    // buffers
    VAO boxVAO;
    EBO boxEBO(boxIndices, sizeof(boxIndices));
    VBO boxVBO(boxVertices, sizeof(boxVertices));
    boxVAO.Bind();
    boxVAO.LinkVBO(boxVBO, 0, 3, 3, 0);
    boxEBO.Bind();
    boxVAO.Unbind();
    boxEBO.Unbind();
    // VV render loop i think VV
    while (!glfwWindowShouldClose(window))
    {
        //delta tiem
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();


        //in🅱️ut
        processInput(window); // gets input
        glfwGetWindowSize(window, &WINW, &WINH); // resizes window (if it happenes)


        //ren🅱️ering
        glClearColor(0.1, 0.1, 0.1, 1); // grey background
        glClear(GL_COLOR_BUFFER_BIT); // clearing background

        // resetting matricies
        //glm::mat4 world = glm::mat4(1.0f); // aka model matrix

        myShader.Use();
        boxVAO.Bind();
        //boxEBO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //re🅱️resh
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //deinit
    glfwTerminate();
    return 0;
}





GLFWwindow* GetWindow(void) {
    if (!glfwInit()) return NULL; //init lib
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // hints for the window context idk

    // make the window
    GLFWwindow* window = glfwCreateWindow(WINW, WINH, "say hello to deez nuts", NULL, NULL);;

    //error handling
    if (!window) return NULL;

    // ??? make the window context or smthn
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //load up glad + error handling
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "glad error, i am not glad!" << std::endl;
        return NULL;
    }

    //opengl isnt very bright
    glViewport(0, 0, WINW, WINH);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill mode

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

}