#include <iostream>
#include <string>
#include "glad.c"
#include "glfw3.h"
#include "model.h"
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "buffers.h"
#include "cubemap.h"
#include "physics.cpp"
//#include "glm/gtx/rotate_vector.hpp"
//#include "glm/gtx/matrix_transform_2d.hpp"

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

glm::vec2 boxPos(3);

physicsObject box(glm::vec2(3, 3), 1, &boxVertices[0], 4);


// a triangle?
float trigVertices[]{
    -1, 1, 0,
    1, 1, 0,
    1, -1, 0,
};

int trigIndices[]{
    0, 2, 1
};

physicsObject triangle(glm::vec2(0, 0), 0.5, &trigVertices[0], 3);



float pentVertices[]{ // points go clockwise
    1, 0, 0, // to de right
    0.31, -0.95, 0, // bottom right
    -0.81, -0.59, 0, // bottom left
    -0.81, 0.59, 0, // top left
    0.31, 0.95, 0 // top right
};

int pentIndices[]{ // clockwise handedness
    0, 1, 2,
    0, 2, 3,
    0, 3, 4
};

physicsObject pentagon(glm::vec2(0, -3), 2, &pentVertices[0], 5);

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

    float origVertices[] = {0, 0, 0,
                            0, 0, 0,
                            0, 0, 0};

    int origIndices[] = {0, 1, 2};

    VAO origVAO;
    EBO origEBO(origIndices, sizeof(origIndices));
    VBO origVBO(origVertices, sizeof(origVertices));
    origVAO.Bind();
    origVAO.LinkVBO(origVBO, 0, 3, 3, 0);
    origEBO.Bind();
    origVAO.Unbind();
    origEBO.Unbind();

    VAO pentVAO;
    EBO pentEBO(pentIndices, sizeof(pentIndices));
    VBO pentVBO(pentVertices, sizeof(pentVertices));
    pentVAO.Bind();
    pentVAO.LinkVBO(pentVBO, 0, 3, 3, 0);
    pentEBO.Bind();
    pentVAO.Unbind();
    pentEBO.Unbind();

    glEnable(GL_PROGRAM_POINT_SIZE);

    // VV render loop i think VV
    while (!glfwWindowShouldClose(window))
    {
        // simp!!
        simpVertices[0] = simplex[0].x;
        simpVertices[1] = simplex[0].y;

        simpVertices[3] = simplex[1].x;
        simpVertices[4] = simplex[1].y;

        simpVertices[6] = simplex[2].x;
        simpVertices[7] = simplex[2].y;

        VAO simpVAO;
        EBO simpEBO(simpIndices, sizeof(simpIndices));
        VBO simpVBO(simpVertices, sizeof(simpVertices));
        simpVAO.Bind();
        simpVAO.LinkVBO(simpVBO, 0, 3, 3, 0);
        simpEBO.Bind();
        simpVAO.Unbind();
        simpEBO.Unbind();

        supVertices[0] = supports[0].x;
        supVertices[1] = supports[0].y;

        supVertices[3] = supports[1].x;
        supVertices[4] = supports[1].y;

        supVertices[6] = supports[2].x;
        supVertices[7] = supports[2].y;

        supVertices[9] = supports[3].x;
        supVertices[10] = supports[3].y;

        supVertices[12] = supports[4].x;
        supVertices[13] = supports[4].y;

        supVertices[15] = supports[5].x;
        supVertices[16] = supports[5].y;

        VAO supVAO;
        VBO supVBO(supVertices, sizeof(supVertices));
        supVAO.Bind();
        supVAO.LinkVBO(supVBO, 0, 3, 3, 0);
        supVAO.Unbind();



        //🅱️elta tiem
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();


        //in🅱️ut
        processInput(window); // gets input
        glfwGetWindowSize(window, &WINW, &WINH); // resizes window (if it happenes)

        // 🅱️ysics
        UpdatePhysics(deltaTime);
        /* no collisions for now
        trigRot -= 45 * deltaTime;
        glm::vec2 offset = {0, 0};
        bool hasCollided = checkForIntersection(&boxVertices[0], 4, boxPos, 0, &trigVertices[0], 3, trigPos, trigRot, &offset);
        trigPos += offset;
        */
        bool hasCollided = GJK(*pStart, *(*pStart).pNext);
        //triangle.rotation += 0.1;

       // looping
        if(triangle.position.x > 25) triangle.position.x = -25;
        if(pentagon.position.x > 25) pentagon.position.x = -25;
        if(box.position.x > 25) box.position.x = -25;

        if(triangle.position.x < -25) triangle.position.x = 25;
        if(pentagon.position.x < -25) pentagon.position.x = 25;
        if(box.position.x < -25) box.position.x = 25;


        if(triangle.position.y > 15) triangle.position.y = -15;
        if(pentagon.position.y > 15) pentagon.position.y = -15;
        if(box.position.y > 15) box.position.y = -15;

        if(triangle.position.y < -15) triangle.position.y = 15;
        if(pentagon.position.y < -15) pentagon.position.y = 15;
        if(box.position.y < -15) box.position.y = 15;

        //ren🅱️ering
        glClearColor(0.1, 0.1, 0.1, 1); // grey background
        glClear(GL_COLOR_BUFFER_BIT); // clearing background

        // resetting matricies
        glm::mat4 world = glm::mat4(1.0f); // update position in this
        world = glm::translate(world, glm::vec3(box.position, 0));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(-(float)WINW / 50.0f, (float)WINW / 50.0f, -(float)WINH / 50.0f, (float)WINH / 50.0f, -1.0f, 1.0f);
        myShader.Use();
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        myShader.SetVec3("colour", glm::vec3(0, 1, 1));
        boxVAO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // resetting matricies
        world = glm::mat4(1.0f); // update position in this
        world = glm::translate(world, glm::vec3(triangle.position, 0));
        world = glm::rotate(world, glm::radians(triangle.rotation), glm::vec3(0, 0, 1));
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        if(hasCollided){
            myShader.SetVec3("colour", glm::vec3(1, 1, 1));
        }
        else{
            myShader.SetVec3("colour", glm::vec3(1, 1, 0));
        }
        trigVAO.Bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // pentagon
        world = glm::mat4(1.0f); // update position in this
        world = glm::translate(world, glm::vec3(pentagon.position, 0));
        world = glm::rotate(world, glm::radians(pentagon.rotation), glm::vec3(0, 0, 1));
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        // collision!
        if(hasCollided){
            myShader.SetVec3("colour", glm::vec3(1, 0, 1));
        }
        else{
            myShader.SetVec3("colour", glm::vec3(1, 0, 1));
        }
        pentVAO.Bind();
        //glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // simp
        world = glm::mat4(1.0f);
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        myShader.SetVec3("colour", glm::vec3(1, 0, 1));
        simpVAO.Bind();
        if(hasCollided || 1) glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, 0);

        // orig
        world = glm::mat4(1.0f);
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        myShader.SetVec3("colour", glm::vec3(1, 1, 1));
        origVAO.Bind();
        glDrawElements(GL_POINTS, 3, GL_UNSIGNED_INT, 0);

        // sup
        world = glm::mat4(1.0f);
        myShader.SetMat4("world", world);
        myShader.SetMat4("projection", projection);
        myShader.SetVec3("colour", glm::vec3(1, 0, 0));
        supVAO.Bind();
        if(hasCollided || 1) glDrawArrays(GL_POINTS, 0, 6);

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

    float speed = 5;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) box.AddForce(glm::vec2(0, 1) * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) box.AddForce(glm::vec2(-1, 0) * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) box.AddForce(glm::vec2(0, -1) * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) box.AddForce(glm::vec2(1, 0) * speed);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) triangle.AddForce(glm::vec2(0, 1) * speed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) triangle.AddForce(glm::vec2(-1, 0) * speed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) triangle.AddForce(glm::vec2(0, -1) * speed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) triangle.AddForce(glm::vec2(1, 0) * speed);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) pentagon.AddForce(glm::vec2(0, 1) * speed);
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) pentagon.AddForce(glm::vec2(-1, 0) * speed);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) pentagon.AddForce(glm::vec2(0, -1) * speed);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) pentagon.AddForce(glm::vec2(1, 0) * speed);
}