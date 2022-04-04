#ifndef CAMERA_H
#define CAMERA_H

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//fps style camera class
class Camera{
    public:
        float yaw; // right and left
        float pitch; // up and down
        float speed = 2.5; // multiply by deltaTime
        float mSens = 0.08; // 0.08 is nice

        glm::vec3 pos; // the position, duh
        glm::vec3 front; // cameras z axis, points into the camera
        glm::vec3 up; // cameras positive y axis
        glm::vec3 right; // cameras positive x axis

        //constructor !!
        Camera(glm::vec3 iPos, float iYaw, float iPitch);

        //updates angles from input
        void UpdateAngles(double offsetX, double offsetY);

        // get view matrix
        glm::mat4 GetMatrix(){
            return glm::lookAt(pos, pos + front, up);
        }

        void Move(char dir, float deltaTime);

    private:
        const glm::vec3 worldUp = glm::vec3(0, 1, 0); // hardcoded up vector

        // this one updates our camera
        void UpdateCamera();
};

// takes yaw and pitch as degree angles
Camera::Camera(glm::vec3 iPos, float iYaw, float iPitch){
    yaw = glm::radians(iYaw);
    pitch = glm::radians(iPitch);
    pos = iPos;
    UpdateCamera();
}

void Camera::UpdateCamera(){
    // front vector from rotations
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // up and right vectors relative to camera
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::UpdateAngles(double offsetX, double offsetY){
    yaw += offsetX * mSens;
    pitch += offsetY * mSens;

    // clamping pitch
    if(pitch > 89.99) pitch = 89.99;
    if(pitch < -89.99) pitch = -89.99;

    UpdateCamera();
}

//moves camera
void Camera::Move(char dir, float deltaTime){
    switch(dir){
        case 'u': pos.y += speed * deltaTime; break;

        case 'd': pos.y -= speed * deltaTime; break;

        case 'r': pos += right * speed * deltaTime; break;

        case 'l': pos -= right * speed * deltaTime; break;

        case 'f': pos += front * speed * deltaTime; break;

        case 'b': pos -= front * speed * deltaTime; break;

        default: std::cout << "bruh invalid dir for camera";
    }
    UpdateCamera();
}

#endif