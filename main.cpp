#include <iostream>
#include <string>
#include "glad.c"
#include "glfw3.h"
#include "model.h"
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "buffers.h"
#include "cubemap.h"
#include "suvatic.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"

int WINW = 1280;
int WINH = 720;

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

glm::vec2 boxBase[]{
    glm::vec2(-1, 1),
    glm::vec2(1, 1),
    glm::vec2(1, -1),
    glm::vec2(-1, -1)
};

glm::vec2 boxColliders[]{
    glm::vec2(-1, 1),
    glm::vec2(1, 1),
    glm::vec2(1, -1),
    glm::vec2(-1, -1)
};

glm::vec2 boxPos(3);


// a triangle?
float trigVertices[]{
    -1, 1, 0,
    1, 1, 0,
    1, -1, 0,
};

int trigIndices[]{
    0, 2, 1,
};

glm::vec2 trigBase[]{
    glm::vec2(-1.0f, 1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(1.0f, -1.0f),
};

glm::vec2 trigColliders[]{
    glm::vec2(-1, 1),
    glm::vec2(1, 1),
    glm::vec2(1, -1),
};

glm::vec2 trigPos(0.0f);
float trigRot = 0.0f;

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

    VAO trigVAO;
    EBO trigEBO(trigIndices, sizeof(trigIndices));
    VBO trigVBO(trigVertices, sizeof(trigVertices));
    trigVAO.Bind();
    trigVAO.LinkVBO(trigVBO, 0, 3, 3, 0);
    trigEBO.Bind();
    trigVAO.Unbind();
    trigEBO.Unbind();

    // VV render loop i think VV
    while (!glfwWindowShouldClose(window))
    {
        //delta tiem
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();


        //in🅱️ut
        processInput(window); // gets input
        glfwGetWindowSize(window, &WINW, &WINH); // resizes window (if it happenes)

        trigRot += 45 * deltaTime;

        // updating box colliders
        for(int i = 0; i < 4; i++){
            boxColliders[i] = boxBase[i] + boxPos;
            //std::cout << boxColliders[i].x << " , " << boxColliders[i].y << std::endl;
        }

        bool hasCollided = checkForIntersection(&boxColliders[0], 4, &trigColliders[0], 3);


        //ren🅱️ering
        glClearColor(0.1, 0.1, 0.1, 1); // grey background
        glClear(GL_COLOR_BUFFER_BIT); // clearing background

        // resetting matricies
        glm::mat4 world = glm::mat4(1.0f); // update position in this
        world = glm::translate(world, glm::vec3(boxPos, 0));

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(-(float)WINW / 100.0f, (float)WINW / 100.0f, -(float)WINH / 100.0f, (float)WINH / 100.0f, -1.0f, 1.0f);

        myShader.Use();

        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);

        if(hasCollided){
            myShader.SetVec3("colour", glm::vec3(1, 0, 0));
        }
        else{
            myShader.SetVec3("colour", glm::vec3(0, 1, 1));
        }
        boxVAO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // resetting matricies
        world = glm::mat4(1.0f); // update position in this
        world = glm::translate(world, glm::vec3(trigPos, 0));

        world = glm::rotate(world, glm::radians(trigRot), glm::vec3(0, 0, 1));

        glm::mat3 trigTransform(1.0f);
        trigTransform = glm::translate(trigTransform, glm::vec2(trigPos));
        trigTransform = glm::rotate(trigTransform, glm::radians(trigRot));

        for(int i = 0; i < 3; i++){
            trigColliders[i] = trigTransform * glm::vec3(trigBase[i], 1);
            //std::cout << trigColliders[i].x << " , " << trigColliders[i].y << std::endl;
        }

        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);

        // collision!
        
        if(hasCollided){
            myShader.SetVec3("colour", glm::vec3(0, 1, 0));
        }
        else{
            myShader.SetVec3("colour", glm::vec3(1, 1, 0));
        }
        trigVAO.Bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);


        //re🅱️resh
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) boxPos.y += (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) boxPos.x -= (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) boxPos.y -= (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) boxPos.x += (float)(5 * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) trigPos.y += (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) trigPos.x -= (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) trigPos.y -= (float)(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) trigPos.x += (float)(5 * deltaTime);
}