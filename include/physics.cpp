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
glm::vec2 Clip(glm::vec2 point1, glm::vec2 point2, glm::vec2 ref1, glm::vec2 ref2){
    bool inside1, inside2;

    if((ref2.x-ref1.x)*(point1.y-ref1.y) - (ref2.y-ref1.y)*(point1.x-ref1.x) > 0) inside1 = false;
    else inside1 = true;

    if((ref2.x-ref1.x)*(point2.y-ref1.y) - (ref2.y-ref1.y)*(point2.x-ref1.x) > 0) inside2 = false;
    else inside2 = true;

    // both points inside
    if(inside1 && inside2) return point2;

    // one inside, one outside, return intersection
    if((inside1 && !inside2) || (!inside1 && inside2)){
        float x = ((point1.x*point2.y)-(point1.y*point2.x))*(ref1.x-ref2.x) - (point1.x-point2.x)*((ref1.x*ref2.y)-(ref1.y*ref2.x));
        x /= ((point1.x-point2.x)*(ref1.y-ref2.y) - (point1.y-point2.y)*(ref1.x-ref2.x));

        float y = ((point1.x*point2.y)-(point1.y*point2.x))*(ref1.y-ref2.y) - (point1.y-point2.y)*((ref1.x*ref2.y)-(ref1.y*ref2.x));
        y /= ((point1.x-point2.x)*(ref1.y-ref2.y) - (point1.y-point2.y)*(ref1.x-ref2.x));

        return glm::vec2(x, y);
    }

    // both outside, return (0, 0) <- not best solution...
    return glm::vec2(0, 0);
}

// algorythm for finding point(s) of intersect: uses Sutherland-Hodgman Clipping
// takes clockwise handedness, be careful!
std::vector<glm::vec2> FindCollisionManifold(physicsObject *PO1, physicsObject *PO2, glm::vec2 normal){
    // 1. first find best points

    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(normal, PO1->collider[0]);
    int index1 = 0;
    int index2 = 0;
    glm::vec2 point1 = PO1->collider[0];
    for (unsigned int i = 0; i < PO1->collider.size(); i++){
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
    for (unsigned int i = 0; i < PO2->collider.size(); i++){
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
    bool plus1 = true;
    int index = index1 - 1;
    if(index < 0) index += PO1->collider.size();
    glm::vec2 face1A, face1B; // B is clockwise to A
    std::cout << "got this far.. " << index << " " << index1;
    face1A = PO1->collider[index];
    std::cout << " what abt this?" << std::endl;
    face1B = PO1->collider[index1];
    biggest = glm::dot(glm::vec2(-(face1B.y - face1A.y), face1B.x - face1A.x), normal);

    index = index1 + 1;
    if(index > PO1->collider.size()) index = 0;
    face1A = PO1->collider[index1];
    face1B = PO1->collider[index];

    if(glm::dot(glm::vec2(-(face1B.y - face1A.y), face1B.x - face1A.x), normal) < biggest){
        index = index1 - 1;
        if(index1 < 0) index += PO1->collider.size();
        face1A = PO1->collider[index];
        face1B = PO1->collider[index1];
        plus1 = false;
    }

    // face from PO2
    bool plus2 = true;
    index = index2 - 1;
    if(index < 0) index += PO2->collider.size();
    glm::vec2 face2A, face2B; // B is clockwise to A
    face2A = PO2->collider[index];
    face2B = PO2->collider[index2];
    biggest = glm::dot(glm::vec2(-(face2B.y - face2A.y), face2B.x - face2A.x), -normal);

    index = index2 + 1;
    if(index > (PO2->collider.size())) index = 0;
    face2A = PO2->collider[index2];
    face2B = PO2->collider[index];

    if(glm::dot(glm::vec2(-(face2B.y - face2A.y), face2B.x - face2A.x), -normal) < biggest){
        index = index2 - 1;
        if(index < 0) index += PO2->collider.size();
        face2A = PO2->collider[index];
        face2B = PO2->collider[index2];
        plus2 = false;
    }

    // 3. then sort faces, also getting adjacent ref faces for clipping
    glm::vec2 incA, incB, refA, refB, ref0, ref2; // again, B is clockwise to A; 0 and 2 are used for clipping
    if(glm::dot(glm::vec2(-(face1B.y - face1A.y), face1B.x - face1A.x), normal) > glm::dot(glm::vec2(-(face2B.y - face2A.y), face2B.x - face2A.x), -normal)){
        normal /= 2.0f;
        refA = face1A;
        refB = face1B;
        incA = face2A;
        incB = face2B;
        // getting adjacent faces for clipping
        if(plus1){
            // then face1A is at index1, and face1B is at index1 + 1 nice
            index = index1 - 1;
            if(index < 0) index += PO1->collider.size();
            ref0 = PO1->collider[index];

            index = index1 + 2;
            if(index > PO1->collider.size()) index -= PO1->collider.size();
            ref2 = PO1->collider[index];
        }
        else{
            // if were here, then face1B is at index1
            index = index1 - 2;
            if(index < 0) index += PO1->collider.size();
            ref0 = PO1->collider[index];

            index = index1 + 1;
            if(index > PO1->collider.size()) index -= PO1->collider.size();
            ref2 = PO1->collider[index];
        }
    }
    else{
        normal /= -2.0f;
        refA = face2A;
        refB = face2B;
        incA = face1A;
        incB = face2B;
        // getting adjacent faces for clipping
        if(plus2){
            // then face2A is at index2, and face2B is at index2 + 1 nice
            index = index2 - 1;
            if(index < 0) index += PO2->collider.size();
            ref0 = PO2->collider[index];

            index = index2 + 2;
            if(index > PO2->collider.size()) index -= PO2->collider.size();
            ref2 = PO2->collider[index];
        }
        else{
            // if were here, then face2B is at index2
            index = index2 - 2;
            if(index < 0) index += PO2->collider.size();
            ref0 = PO2->collider[index];

            index = index2 + 1;
            if(index > PO2->collider.size()) index -= PO2->collider.size();
            ref2 = PO2->collider[index];
        }
    }

    // 4. then clip!

    // only need 3 function calls
    std::vector<glm::vec2> list = {glm::vec2(0)};
    // to da anticlockwise
    list.push_back(Clip(incA, incB, ref0, refA) + normal);
    // to da clockwise
    list.push_back(Clip(incA, incB, refB, ref2) + normal);
    // final ref face, itself!
    list.push_back(Clip(incA, incB, refA, refB) + normal);

    // need to offset points to find true intersection point

    return list; // ggez?
}

// points for debugging
std::vector<glm::vec2> points = {glm::vec2(0)};

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
                // intersection finder??
                points = FindCollisionManifold(pPO1, pPO2, resultant);

                // raw position solver
                // (*pPO1).position -= resultant / 2.0f; // half of the offset to this
                // (*pPO2).position += resultant / 2.0f;  // and half to the other

                // impulse solver using crazy black box equasion
                // quite elegant if i do say so myself
                float impulse;
                glm ::vec2 normal = glm::normalize(resultant);
                glm::vec2 Vab = (*pPO1).velocity - (*pPO2).velocity;
                float elasticity = 1;

                impulse = glm::dot(-(1 + elasticity) * Vab, normal);
                impulse /= glm::dot(normal, normal * ((1 / (*pPO1).mass) + (1 / (*pPO2).mass)));

                // (*pPO1).velocity += (impulse * normal) / (*pPO1).mass;
                // (*pPO2).velocity -= (impulse * normal) / (*pPO2).mass;
            }
        }
        // update to next active objects to check
        pPO1 = (*pPO1).pNext;
        pPO2 = pPO1;
    }
}

#endif