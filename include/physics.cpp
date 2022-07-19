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
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <iostream>
#include <vector>

class physicsObject{
    public:
        glm::vec2 position, velocity, resultantForce; // should reset resultant force to 0 at the start of frame
        float mass, rotation; // no rotations yet, but soon
        physicsObject* pNext = nullptr;
        std::vector<glm::vec2> base; // set of base points (shape), will be transfomed through position and rotation for collisions
        std::vector<glm::vec2> collider; // transformed and rotated points will be stored here
        bool colliding = false; // straightforward for once lol

        void AddForce(glm::vec2 force){
            resultantForce += force;
        }

        physicsObject(glm::vec2 iposition, float imass, float* array, int size);

    private:

        std::vector<glm::vec2> floatToVecList(float* array, int size){
            std::vector<glm::vec2> vectorList(size);

            for(int i = 0; i < size; i++){
                vectorList[i].x = *array;
                array++;
                vectorList[i].y = *array;
                array++;
                array++;
            }

            return vectorList;
        }
        
        
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

// for physics world
physicsObject* pStart = nullptr;
physicsObject* pEnd = nullptr;
int sizeOfPhysicsWorld; // assumes no objects are deleted

physicsObject::physicsObject(glm::vec2 iposition, float imass, float* array, int size){
    position = iposition;
    mass = imass;
    base = floatToVecList(array, size);
    collider = base;
    if(pStart == nullptr){ // first object in physics world
        pStart = this; // "this" points to this class nice
        pEnd = this;
        sizeOfPhysicsWorld++;
    }
    else{ // adding to physics world
        (*pEnd).pNext = this; // adding this onto end of list
        pEnd = this; // this object is now the end of the list
        sizeOfPhysicsWorld++;
    }
}


// GJK

// translates colliders by posisiton and rotation, PO needs to be passed as reference
void TranslatePO(physicsObject* PO){
    // first get our matrix transform
    glm::mat3 transform = glm::mat3(1);
    transform = glm::translate(transform, (*PO).position);
    transform = glm::rotate(transform, glm::radians((*PO).rotation));
    // now we apply this matrix tranform to all our collider points
    for(unsigned int i = 0; i < size((*PO).base); i++){
        (*PO).collider[i] = glm::vec2(transform * glm::vec3((*PO).base[i], 1)); // huh
    }
}

// support function, returns hull point given two shapes and a direction
glm::vec2 Support(physicsObject PO1, physicsObject PO2, glm::vec2 direction){
    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(direction, PO1.collider[0]); // huh?
    glm::vec2 point1 = PO1.collider[0];
    for(unsigned int i = 0; i < size(PO1.collider); i++){
        temp = glm::dot(direction, PO1.collider[i]);
        if(biggest < temp){
            biggest = temp;
            point1 = PO1.collider[i];
        }
    }

    glm::vec2 point2 = PO2.collider[0];
    // get largest dp of second collider in opposing direction
    temp = 0;
    biggest = glm::dot(-direction, PO2.collider[0]);
    for(unsigned int i = 0; i < size(PO2.collider); i++){
        temp = glm::dot(-direction, PO2.collider[i]);
        if(biggest < temp){
            biggest = temp;
            point2 = PO2.collider[i];
        }
    }

    //return the difference
    return point1 - point2;
}

// checks triangle case, returns 1 for region AB, 2 for region AC, 0 for collisison! also updates direction
int TriangleCase(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2* direction){
    // find AB normal using (AC X AB) X AB, check AB
    glm::vec2 ABnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(C-A, 0), glm::vec3(B-A, 0)), glm::vec3(B-A, 0)));
    if(glm::dot(ABnormal, -A) > 0){
        *direction = ABnormal;
        return 1;
    }

    //find AC normal using (AB X AC) X AC, check AC
    glm::vec2 ACnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(B-A, 0), glm::vec3(C-A, 0)), glm::vec3(C-A, 0)));
    if(glm::dot(ACnormal, -A) > 0){
        *direction = ACnormal;
        return 2;
    }

    // if both checks fail, origin is with simplex
    return 0;
}

// GJK algorithm for detecting collisions
bool GJK(physicsObject PO1, physicsObject PO2){
    // translate our colliders by position + rotation
    TranslatePO(&PO1);
    TranslatePO(&PO2);

    // find first simplex point
    glm::vec2 direction = glm::vec2(1, 1); // where to look
    glm::vec2 C, B, A; // simplex triangle, from oldest to newest point
    C = Support(PO1, PO2, direction);
    // find next direction to search in
    direction = -C; // towareds origin

    // find next simplex point
    B = Support(PO1, PO2, direction);
    // loop line case
    while (glm::dot(direction, B) < 0){
        C = B;
        B = Support(PO1, PO2, direction);
        if (B == C) return false; // sanity check making sure point passes origin 
    }
    // find next direction (normal to line towards origin) using tiple product (AB X AO) X AB
    direction = glm::vec2(glm::cross(glm::cross(glm::vec3(B-C, 0), glm::vec3(-C, 0)), glm::vec3(B-C, 0))); // might break

    // complete simplex triangle
    A = Support(PO2, PO2, direction);
    // loop the triangle case, chase better triangles!
    int region = 3;
    while(region != 0){
        // lose the furthest point
        if(region == 1) C = A;
        if(region == 2) B = A;

        // recasting A
        A = Support(PO1, PO2, direction);
        if(glm::dot(direction, A) <= 0) return false; // sanity check
        if((C == A) || (B == A)) return false; // sanity check

        // checking new triangle
        region = TriangleCase(A, B, C, &direction);
    }

    // if loop breaks, there was a collision!
    return true;
}




// takes our physics objects, loops through, applies forces, and updates positions / rotations
void UpdatePhysics(float deltaTime){
    // first update all velocities / positions
    physicsObject* pCurrent = pStart; // first physics object
    glm::vec2 acceleration;
    
    for(int i = 0; i < sizeOfPhysicsWorld; i++){ // iterates through list
        acceleration = (*pCurrent).resultantForce / (*pCurrent).mass;
        (*pCurrent).velocity += acceleration * deltaTime;
        (*pCurrent).position += (*pCurrent).velocity * deltaTime;// nice

        (*pCurrent).resultantForce = glm::vec2(0, 0); // resetting in preperation for next frame
        pCurrent = (*pCurrent).pNext; // next physics object ggwp
    }
    
    // then check for collisions, also haha pP
    physicsObject* pPO1 = pStart; // first object
    physicsObject* pPO2 = pStart; // second object
    // loop first object
    while(pPO1 != pEnd){
        // loop second object
        while(pPO2 != pEnd){
            pPO2 = (*pPO2).pNext;

            (*pPO1).colliding = false;
            (*pPO2).colliding = false;

            if(GJK(*pPO1, *pPO2)){
                (*pPO1).colliding = true;
                (*pPO2).colliding = true;
            }
        }
        // update active objects to check
        pPO1 = (*pPO1).pNext;
        pPO2 = pPO1;
    }
    // then update colliding objects
}


#endif