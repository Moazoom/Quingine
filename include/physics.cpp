// suvatic 2d physics engine
// by moazoom
// uses GJK and EPA
// pretty proud of this
// no rotations yet tho

// F = m * a
// a = v / t
// v = d / t

#ifndef PHYSICS_H
#define PHYSICS_H

#include "glm/glm.hpp" // its maths time!!
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class physicsObject{
public:
    glm::vec2 position, velocity, resultantForce; // should reset resultant force to 0 at the start of frame
    float mass, rotation;                         // no rotations yet, but soon
    physicsObject *pNext = nullptr;
    std::vector<glm::vec2> base;     // set of base points (shape), will be transfomed through position and rotation for collisions
    std::vector<glm::vec2> collider; // transformed and rotated points will be stored here
    bool colliding = false;          // only for changing colour
    

    // adds a force, pretty simple like all good things should be
    void AddForce(glm::vec2 force){
        resultantForce += force;
    }

    // initialiser
    physicsObject(glm::vec2 iposition, float imass, float *array, int size);

private:
    // utility function to convert arr of floats to vec list
    std::vector<glm::vec2> floatToVecList(float *array, int size){
        std::vector<glm::vec2> vectorList(size);

        for (int i = 0; i < size; i++)
        {
            vectorList[i].x = *array;
            array++;
            vectorList[i].y = *array;
            array++;
            array++;
        }

        return vectorList;
    }
};

// for physics world
physicsObject *pStart = nullptr;
physicsObject *pEnd = nullptr;
int sizeOfPhysicsWorld; // assumes no objects are deleted

physicsObject::physicsObject(glm::vec2 iposition, float imass, float *array, int size){
    position = iposition;
    mass = imass;
    base = floatToVecList(array, size);
    collider = base;
    if (pStart == nullptr){ // first object in physics world
        pStart = this; // "this" points to this class nice
        pEnd = this;
        sizeOfPhysicsWorld++;
    }
    else{ // adding to physics world
        (*pEnd).pNext = this; // adding this onto end of list
        pEnd = this;          // this object is now the end of the list
        sizeOfPhysicsWorld++;
    }
}

glm::vec2 EPA(glm::vec2 A, glm::vec2 B, glm::vec2 C, physicsObject *PO1, physicsObject *PO2); // prototype for EPA algorithm
// GJK
// translates colliders by posisiton and rotation, PO needs to be passed as reference
void TranslatePO(physicsObject *PO){
    // first get our matrix transform
    glm::mat3 transform = glm::mat3(1);
    transform = glm::translate(transform, (*PO).position);
    transform = glm::rotate(transform, glm::radians((*PO).rotation));
    // now we apply this matrix tranform to all our collider points
    for (unsigned int i = 0; i < size((*PO).base); i++){
        (*PO).collider[i] = glm::vec2(transform * glm::vec3((*PO).base[i], 1));
    }
}

// support function, returns hull point given two shapes and a direction
glm::vec2 Support(physicsObject PO1, physicsObject PO2, glm::vec2 direction){
    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(direction, PO1.collider[0]);
    glm::vec2 point1 = PO1.collider[0];
    for (unsigned int i = 0; i < size(PO1.collider); i++){
        temp = glm::dot(direction, PO1.collider[i]);
        if (biggest < temp){
            biggest = temp;
            point1 = PO1.collider[i];
        }
    }

    // get largest dp of second collider in opposing direction
    temp = 0;
    biggest = glm::dot(-direction, PO2.collider[0]);
    glm::vec2 point2 = PO2.collider[0];
    for (unsigned int i = 0; i < size(PO2.collider); i++){
        temp = glm::dot(-direction, PO2.collider[i]);
        if (biggest < temp){
            biggest = temp;
            point2 = PO2.collider[i];
        }
    }

    // return the difference
    return point1 - point2;
}

// checks triangle case, returns 1 for region AB, 2 for region AC, 0 for collisison! also updates direction
int TriangleCase(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 *direction){
    // find AB normal using (AC X AB) X AB, check AB
    glm::vec2 ABnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(C - A, 0), glm::vec3(B - A, 0)), glm::vec3(B - A, 0)));
    if (glm::dot(ABnormal, -A) > 0){
        *direction = ABnormal;
        return 1;
    }

    // find AC normal using (AB X AC) X AC, check AC
    glm::vec2 ACnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(B - A, 0), glm::vec3(C - A, 0)), glm::vec3(C - A, 0)));
    if (glm::dot(ACnormal, -A) > 0){
        *direction = ACnormal;
        return 2;
    }

    // if both checks fail, origin is with simplex
    return 0;
}

// GJK algorithm for detecting collisions
bool GJK(physicsObject *PO1, physicsObject *PO2, glm::vec2 *resultant){
    // translate our colliders by position + rotation
    TranslatePO(PO1);
    TranslatePO(PO2);

    // find first simplex point
    glm::vec2 direction = glm::vec2(1, 1); // where to look, random direction for now
    glm::vec2 C, B, A; // simplex triangle, from oldest to newest point
    C = Support(*PO1, *PO2, direction);
    // find next direction to search in
    direction = -C; // towareds origin

    // find next simplex point
    B = Support(*PO1, *PO2, direction);
    // loop line case
    while (glm::dot(direction, B) < 0){
        C = B;
        B = Support(*PO1, *PO2, direction);
        if (B == C) return false; // sanity check making sure point passes origin
    }
    // find next direction (normal to line towards origin) using triple product (AB X AO) X AB
    direction = glm::vec2(glm::cross(glm::cross(glm::vec3(B - C, 0), glm::vec3(-C, 0)), glm::vec3(B - C, 0))); // phew! didnt break :)

    // complete simplex triangle
    A = Support(*PO2, *PO2, direction);
    // loop the triangle case, chase better triangles!
    int region = 3;
    while (region != 0){
        // lose the furthest point
        if (region == 1)
            C = A;
        if (region == 2)
            B = A;

        // recasting A
        A = Support(*PO1, *PO2, direction);
        if ((glm::dot(direction, A) <= 0) || ((C == A) || (B == A))) return false; // sanity check

        // checking new triangle
        region = TriangleCase(A, B, C, &direction);
    }

    //(*PO1).colliding = true;
    //(*PO2).colliding = true;

    // if loop breaks, there was a collision!
    *resultant = EPA(A, B, C, PO1, PO2);
    return true;
}

// finds normal direction on line AB towards origin
glm::vec2 findNormalToOrigin(glm::vec2 A, glm::vec2 B){
    // extending into 3d
    glm::vec3 line1 = glm::vec3(B - A, 0);
    glm::vec3 line2 = glm::vec3(-A, 0);
    // offsetting a small amount incase the two vectors line up perfectly (rare)
    if (glm::abs(glm::normalize(line1)) == glm::abs(glm::normalize(line2)))
    {
        line1.x += 0.0001;
        line1.y += 0.0001;
    }
    // triple product
    glm::vec3 normal = glm::normalize(glm::cross(glm::cross(line1, line2), line1));

    return glm::vec2(normal);
}

// EPA algorythm!! very nice
// normal returned goes from po1 to po2
glm::vec2 EPA(glm::vec2 A, glm::vec2 B, glm::vec2 C, physicsObject *PO1, physicsObject *PO2){
    std::vector<glm::vec2> polytope = {A, B, C}; // hopefully correct handedness

    int index = 0;
    float minDistance = INFINITE;
    glm::vec2 minNormal;

    while (minDistance == INFINITE){
        for (unsigned int i = 0; i < polytope.size(); i++){
            int j = (i + 1) % polytope.size(); // the next point, loops round to 0

            // get da normal
            glm::vec2 normal = -findNormalToOrigin(polytope[i], polytope[j]);

            float distance = glm::dot(polytope[i], normal);

            if (distance < 0){
                distance *= -1;
                normal *= -1;
            }

            if (distance < minDistance){
                index = j;
                minDistance = distance;
                minNormal = normal;
            }
        }

        // use support to find closest point to normal
        glm::vec2 support = Support(*PO1, *PO2, minNormal);
        float sDistance = dot(minNormal, support);

        // normal is not on the edge
        if (abs(sDistance - minDistance) > 0.00001){
            minDistance = INFINITE;
            polytope.insert(polytope.begin() + index, support); // add new point to polytope
        }
    }

    glm::vec2 result = minNormal * (float)(minDistance + 0.01);
    return result;
}

// clipping algorythm for utility, returns a point
// uses clockwise handedness for ref line
glm::vec2 Clip(glm::vec2 point1, glm::vec2 point2, glm::vec2 ref){
    bool inside1, inside2;
    if()
    // both points inside

    // one inside, one outside, return intersection

    // both outside, return (0, 0)
}

// algorythm for finding point(s) of intersect: uses Sutherland-Hodgman Clipping
// takes clockwise handedness, be careful!
std::vector<glm::vec2> FindCollisionManifold(physicsObject *PO1, physicsObject *PO2, glm::vec2 normal){
    // 1. first find best points

    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(normal, PO1->collider[0]);
    int index1, index2 = 0;
    glm::vec2 point1 = PO1->collider[0];
    for (unsigned int i = 0; i < size(PO1->collider); i++){
        temp = glm::dot(normal, PO1->collider[i]);
        if (biggest < temp){
            biggest = temp;
            point1 = PO1->collider[i];
            index1 = i;
        }
    }

    // now second collider
    temp = 0;
    biggest = glm::dot(-normal, PO2->collider[0]);
    glm::vec2 point2 = PO2->collider[0];
    for (unsigned int i = 0; i < size(PO2->collider); i++){
        temp = glm::dot(-normal, PO2->collider[i]);
        if (biggest < temp){
            biggest = temp;
            point2 = PO2->collider[i];
            index2 = i;
        }
    }

    // 2. then best faces: flip x and y, negate x and thats the normal <- could break
    //TODO: fix this shi!!!

    // face from PO1
    int index = index1 - 1;
    if(index1 == 0) index = sizeof(PO1->collider) / sizeof(PO1->collider[0]);
    glm::vec2 face1 = PO1->collider[index1] - PO1->collider[index];
    biggest = glm::dot(glm::vec2(-face1.y, face1.x), normal);

    index = index1 + 1;
    if(index1 == (sizeof(PO1->collider) / sizeof(PO1->collider[0]))) index = 0;
    face1 = PO1->collider[index1] - PO1->collider[index];

    if(glm::dot(glm::vec2(-face1.y, face1.x), normal) < biggest){
        index = index1 - 1;
        if(index1 == 0) index = sizeof(PO1->collider) / sizeof(PO1->collider[0]);
        face1 = PO1->collider[index1] - PO1->collider[index];
    }

    // face from PO2
    index = index2 - 1;
    if(index2 == 0) index = sizeof(PO2->collider) / sizeof(PO2->collider[0]);
    glm::vec2 face2 = PO2->collider[index2] - PO2->collider[index];
    biggest = glm::dot(glm::vec2(-face1.y, face1.x), -normal);

    index = index2 + 1;
    if(index2 == (sizeof(PO2->collider) / sizeof(PO2->collider[0]))) index = 0;
    face2 = PO2->collider[index2] - PO2->collider[index];

    if(glm::dot(glm::vec2(-face1.y, face1.x), -normal) < biggest){
        index = index2 - 1;
        if(index2 == 0) index = sizeof(PO2->collider) / sizeof(PO2->collider[0]);
        face2 = PO2->collider[index2] - PO2->collider[index];
    }

    // 3. then sort faces
    glm::vec2 inc, ref;
    if(glm::dot(glm::vec2(-face1.y, face1.x), normal) > glm::dot(glm::vec2(-face1.y, face1.x), -normal)){
        ref = face1;
        inc = face2;
    }
    else{
        ref = face2;
        inc = face1;
    }

    // 4. then clip!
}

// points for debugging
std::vector<glm::vec2> points = {};

// takes our physics objects, loops through, applies forces, and updates positions / rotations
void UpdatePhysics(float deltaTime){
    // first update all velocities / positions
    physicsObject *pCurrent = pStart; // first physics object
    glm::vec2 acceleration;

    for (int i = 0; i < sizeOfPhysicsWorld; i++){ // iterates through list
        (*pCurrent).colliding = false; // yeah

        // adding friction to the objects
        // glm::vec2 friction = glm::vec2(-1) * glm::normalize((*pCurrent).velocity) * glm::vec2(2) * glm::vec2((*pCurrent).mass);
        if ((*pCurrent).velocity != glm::vec2(0)){
            // (*pCurrent).AddForce(friction);
        }

        acceleration = (*pCurrent).resultantForce / (*pCurrent).mass;
        (*pCurrent).velocity += acceleration * deltaTime;
        (*pCurrent).position += (*pCurrent).velocity * deltaTime; // nice

        (*pCurrent).resultantForce = glm::vec2(0, 0); // resetting in preperation for next frame
        pCurrent = (*pCurrent).pNext; // next physics object ggwp
    }

    // then check for collisions, also haha pP
    physicsObject *pPO1 = pStart; // first object
    physicsObject *pPO2 = pStart; // second object
    glm::vec2 resultant;
    // loop first object
    while (pPO1 != pEnd){
        // loop second object
        while (pPO2 != pEnd){
            pPO2 = (*pPO2).pNext;
            resultant = glm::vec2(0);
            GJK(pPO1, pPO2, &resultant); // big physics engine call!!
            // if collide
            if (resultant != glm::vec2(0)){
                // raw position solver
                (*pPO1).position -= resultant / 2.0f; // half of the offset to this
                (*pPO2).position += resultant / 2.0f;  // and half to the other

                // impulse solver using crazy black box equasion
                // quite elegant if i do say so myself
                float impulse;
                glm ::vec2 normal = glm::normalize(resultant);
                glm::vec2 Vab = (*pPO1).velocity - (*pPO2).velocity;
                float elasticity = 1;

                impulse = glm::dot(-(1 + elasticity) * Vab, normal);
                impulse /= glm::dot(normal, normal * ((1 / (*pPO1).mass) + (1 / (*pPO2).mass)));

                (*pPO1).velocity += (impulse * normal) / (*pPO1).mass;
                (*pPO2).velocity -= (impulse * normal) / (*pPO2).mass;
            }
        }
        // update to next active objects to check
        pPO1 = (*pPO1).pNext;
        pPO2 = pPO1;
    }
}

#endif