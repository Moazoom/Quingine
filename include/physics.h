// suvatic only handles collisions for now
// its time for real physics babey
// moazooms stuff yea
// only for rigid bodies (for now :))))

// F = m * a
// a = v / t
// v = d / t

// conserve energy? good plan mebe

#ifndef PHYSICS_H
#define PHYSICS_H

#include "glm/glm.hpp" // its maths time!!
#include <vector>

// like factorial but for adding
int Addtorial(int num){
    int temp = num;
    while(num != 0){
        temp += num - 1;
    }
    return temp;
}


// start and end of our physics world
//physicsObject* pStart = nullptr;
//physicsObject* pEnd = nullptr;
int lengthOfPhysicsWorld = 0;

class physicsObject{
    public:
        glm::vec2 position, velocity, resultantForce; // should reset resultant force to 0 at the start of frame
        float mass, rotation; // no rotations yet, but soon
        physicsObject* pNext = nullptr;
        std::vector<glm::vec2> collider; // set of base points, will be transfomed through position and rotation for collisions

        void AddForce(glm::vec2 force){
            resultantForce += force;
        }

        physicsObject(glm::vec2 iposition, float imass, float* array, int size);

    private:

        std::vector<glm::vec2> convertArrToList(glm::vec2* array, int size){
            std::vector<glm::vec2> vector(size);
            for(int i = 0; i < size; i++){
                vector[i] = *array;
                array++;
            }
            return vector;
        }

        // utility function for generating "hitboxes" vector arrays from float arrays, expects 2 same sized arrays
        int fillVecArray(float* floatArray, glm::vec2* vecArray, int size){
            // for each set of 3 floats, theres 1 vec
            for(int i = 0; i < size; i++){
                (*vecArray).x = *floatArray;
                floatArray++;
                (*vecArray).y = *floatArray;
                floatArray++; floatArray++; // skip z, back to x 
                vecArray++; // next vector
            }
            return 0;
        }
};

physicsObject* pStart = nullptr;
physicsObject* pEnd = nullptr;

physicsObject::physicsObject(glm::vec2 iposition, float imass, float* array, int size){
    position = iposition;
    mass = imass;
    glm::vec2 vecArray[size];
    fillVecArray(array, &vecArray[0], size);
    collider = convertArrToList(vecArray, size); // converting arrays of vec2s to vector lists
    if(pStart == nullptr){ // first object in physics world
        pStart = this; // "this" points to this class nice
        pEnd = this;
        lengthOfPhysicsWorld++;
    }
    else{ // adding to physics world
        (*pEnd).pNext = this; // adding this onto end of list
        pEnd = this; // this object is now the end of the list
        lengthOfPhysicsWorld++;
    }
}

// takes our physics objects, loops through, applies forces, and updates positions / rotations
void UpdatePhysics(float deltaTime){
    // first update all velocities / positions
    physicsObject* pCurrent = pStart; // first physics object
    glm::vec2 acceleration;
    for(int i = 0; i < 1; i++){ // iterates through list
        acceleration = (*pCurrent).resultantForce / (*pCurrent).mass;
        (*pCurrent).velocity += acceleration * deltaTime;
        (*pCurrent).position += (*pCurrent).velocity * deltaTime;// nice

        (*pCurrent).resultantForce = glm::vec2(0, 0); // resetting in preperation for next frame
        pCurrent++; // next physics object ggwp
    }
    
    // then check for collisions

    // then update colliding objects
}


#endif