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



//prototypes suii
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* GetWindow(void);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(char const* path, int wrapping, int minFilter, int maxFilter);


//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::radians(-90.0), 0);


//other
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// this ones are for mouse
float lastX = WINW / 2.0;
float lastY = WINH / 2.0;


struct PointLight {
    glm::vec3 position;
    glm::vec3 colour = glm::vec3(1);
};

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 colour = glm::vec3(1);
};

int main(void) {
    //init oot of da wae
    GLFWwindow* window = GetWindow();
    if (window == NULL) return -1;


    glEnable(GL_DEPTH_TEST); // self explanatory


    glfwSetCursorPosCallback(window, mouse_callback); // mouse function


    //shaders, thank you shader class :)
    Shader defaultShader("shaders/default.vert", "shaders/default.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

    // scene lights
    PointLight lights[3];
    lights[0].position = glm::vec3(0.7, 0.7, 1.0);

    lights[1].position = glm::vec3(0, 1, -1);
    lights[1].colour = glm::vec3(1, 0, 1);

    lights[2].position = glm::vec3(0, -1, -1);
    lights[2].colour = glm::vec3(0, 0, 1);

    DirectionalLight dirlight;
    dirlight.direction = glm::vec3(0.1, -1, 0.2);


    // models? yea
    Model teapot("resources/models/myTeapot/teapot.obj");
    Model lightBall("resources/models/lightball/ball.obj");
    Model dumpster("resources/models/dumpster/dump.obj");

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    VAO skyboxVAO;
    skyboxVAO.Bind();
    VBO cube(myCubeMap, sizeof(myCubeMap));
    skyboxVAO.LinkVBO(cube, 0, 3, 5, 0); // position
    skyboxVAO.LinkVBO(cube, 1, 2, 5, 3); // cubemap coords
    skyboxVAO.Unbind();
    unsigned int skyboxTexID = loadTexture("resources/textures/skybox/skybox.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // VV render loop i think VV
    while (!glfwWindowShouldClose(window))
    {
        //delta tiem
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();


        //in🅱️ut
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // catches mouse
        processInput(window); // gets input
        glfwGetWindowSize(window, &WINW, &WINH); // resizes window (if it happenes)


        //ren🅱️ering
        glClearColor(0.1, 0.1, 0.1, 1); // grey background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clearing background

        // resetting matricies
        glm::mat4 world = glm::mat4(1.0f); // aka model matrix
        glm::mat4 view = glm::mat4(1.0f); // aka camera matrix
        glm::mat4 projection = glm::mat4(1.0f); // this ones important

        // view / camera matrix
        view = camera.GetMatrix();

        // projection matrix !!!
        projection = glm::perspective(glm::radians(100.0f), (float)WINW / (float)WINH, 0.01f, 100.0f);


        // rendering a model
        defaultShader.Use(); // which shader to use?
        // setting uniforms
        defaultShader.SetMat4("view", view);
        defaultShader.SetMat4("projection", projection);
        // other uniforms
        defaultShader.SetFloat("iTime", glfwGetTime());
        // updating world matrix, and even more uniforms!
        world = glm::mat4(1.0f);
        world = glm::rotate(world, glm::radians(90.0f), glm::vec3(0, 1, 0));
        world = glm::translate(world, glm::vec3(0, -0.5, 0));
        defaultShader.SetMat4("world", world);

        // all of our funky lights
        defaultShader.SetVec3("lights[0].position", lights[0].position);
        defaultShader.SetVec3("lights[1].position", lights[1].position);
        defaultShader.SetVec3("lights[2].position", lights[2].position);

        defaultShader.SetVec3("lights[0].colour", lights[0].colour);
        defaultShader.SetVec3("lights[1].colour", lights[1].colour);
        defaultShader.SetVec3("lights[2].colour", lights[2].colour);

        defaultShader.SetVec3("dirlight.direction", dirlight.direction);
        defaultShader.SetVec3("dirlight.colour", dirlight.colour);

        dumpster.Draw(defaultShader); // awwww yeaa



        lightShader.Use(); // use lighting shader
        //uniforms, again lol
        lightShader.SetMat4("projection", projection);
        lightShader.SetMat4("view", view);
        for (int i = 0; i < 3; i++) {
            world = glm::mat4(1.0f);
            world = glm::translate(world, lights[i].position); // position in world
            world = glm::scale(world, glm::vec3(0.1f)); // scale in world
            lightShader.SetMat4("world", world);
            lightShader.SetVec3("LightColour", lights[i].colour);
            lightBall.Draw(lightShader);
        }

        // draw skybox as last, ~optimised~
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        skyboxShader.SetMat4("view", glm::mat4(glm::mat3(camera.GetMatrix())));
        skyboxShader.SetMat4("projection", projection);
        skyboxShader.SetInt("skyboxTexture", 0);
        // skybox cube
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxVAO.Unbind();
        glDepthFunc(GL_LESS); // set depth function back to default
        
        //std::cout << glGetError() << std::endl;

        //re🅱️resh
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cube.Delete();
    skyboxVAO.Delete();

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.Move('f', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.Move('b', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.Move('l', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.Move('r', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.Move('u', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.Move('d', deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.UpdateAngles(xpos - lastX, lastY - ypos);
    lastX = xpos;
    lastY = ypos;
}

unsigned int loadTexture(char const* path, int wrapping, int minFilter, int maxFilter)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}
