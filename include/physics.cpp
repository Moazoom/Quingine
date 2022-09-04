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

class physicsObject
{
public:
    glm::vec2 position, velocity, resultantForce; // should reset resultant force to 0 at the start of frame
    float mass, rotation;                         // no rotations yet, but soon
    physicsObject *pNext = nullptr;
    std::vector<glm::vec2> base;     // set of base points (shape), will be transfomed through position and rotation for collisions
    std::vector<glm::vec2> collider; // transformed and rotated points will be stored here
    bool colliding = false;          // straightforward for once lol

    void AddForce(glm::vec2 force)
    {
        resultantForce += force;
    }

    physicsObject(glm::vec2 iposition, float imass, float *array, int size);

private:
    std::vector<glm::vec2> floatToVecList(float *array, int size)
    {
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

    std::vector<glm::vec2> convertArrToList(glm::vec2 *array, int size)
    {
        std::vector<glm::vec2> vector(size);
        for (int i = 0; i < size; i++)
        {
            vector[i] = *array;
            array++;
        }
        return vector;
    }

    // utility function for generating "hitboxes" vector arrays from float arrays, expects 2 same sized arrays
    int fillVecArray(float *floatArray, glm::vec2 *vecArray, int size)
    {
        // for each set of 3 floats, theres 1 vec
        for (int i = 0; i < size; i++)
        {
            (*vecArray).x = *floatArray;
            floatArray++;
            (*vecArray).y = *floatArray;
            floatArray++;
            floatArray++; // skip z, back to x
            vecArray++;   // next vector
        }
        return 0;
    }
};

// for physics world
physicsObject *pStart = nullptr;
physicsObject *pEnd = nullptr;
int sizeOfPhysicsWorld; // assumes no objects are deleted
glm::vec2 EPA(glm::vec2 A, glm::vec2 B, glm::vec2 C, physicsObject *PO1, physicsObject *PO2);

physicsObject::physicsObject(glm::vec2 iposition, float imass, float *array, int size)
{
    position = iposition;
    mass = imass;
    base = floatToVecList(array, size);
    collider = base;
    if (pStart == nullptr)
    {                  // first object in physics world
        pStart = this; // "this" points to this class nice
        pEnd = this;
        sizeOfPhysicsWorld++;
    }
    else
    {                         // adding to physics world
        (*pEnd).pNext = this; // adding this onto end of list
        pEnd = this;          // this object is now the end of the list
        sizeOfPhysicsWorld++;
    }
}

// GJK

// translates colliders by posisiton and rotation, PO needs to be passed as reference
void TranslatePO(physicsObject *PO)
{
    // first get our matrix transform
    glm::mat3 transform = glm::mat3(1);
    transform = glm::translate(transform, (*PO).position);
    transform = glm::rotate(transform, glm::radians((*PO).rotation));
    // now we apply this matrix tranform to all our collider points
    for (unsigned int i = 0; i < size((*PO).base); i++)
    {
        (*PO).collider[i] = glm::vec2(transform * glm::vec3((*PO).base[i], 1)); // huh
    }
}

// support function, returns hull point given two shapes and a direction
glm::vec2 Support(physicsObject PO1, physicsObject PO2, glm::vec2 direction)
{
    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(direction, PO1.collider[0]); // huh?
    glm::vec2 point1 = PO1.collider[0];
    for (unsigned int i = 0; i < size(PO1.collider); i++)
    {
        temp = glm::dot(direction, PO1.collider[i]);
        if (biggest < temp)
        {
            biggest = temp;
            point1 = PO1.collider[i];
        }
    }

    glm::vec2 point2 = PO2.collider[0];
    // get largest dp of second collider in opposing direction
    temp = 0;
    biggest = glm::dot(-direction, PO2.collider[0]);
    for (unsigned int i = 0; i < size(PO2.collider); i++)
    {
        temp = glm::dot(-direction, PO2.collider[i]);
        if (biggest < temp)
        {
            biggest = temp;
            point2 = PO2.collider[i];
        }
    }

    // return the difference
    return point1 - point2;
}

// checks triangle case, returns 1 for region AB, 2 for region AC, 0 for collisison! also updates direction
int TriangleCase(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 *direction)
{
    // find AB normal using (AC X AB) X AB, check AB
    glm::vec2 ABnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(C - A, 0), glm::vec3(B - A, 0)), glm::vec3(B - A, 0)));
    if (glm::dot(ABnormal, -A) > 0)
    {
        *direction = ABnormal;
        return 1;
    }

    // find AC normal using (AB X AC) X AC, check AC
    glm::vec2 ACnormal = glm::vec2(glm::cross(glm::cross(glm::vec3(B - A, 0), glm::vec3(C - A, 0)), glm::vec3(C - A, 0)));
    if (glm::dot(ACnormal, -A) > 0)
    {
        *direction = ACnormal;
        return 2;
    }

    // if both checks fail, origin is with simplex
    return 0;
}

// GJK algorithm for detecting collisions
bool GJK(physicsObject *PO1, physicsObject *PO2, glm::vec2 *resultant)
{
    // translate our colliders by position + rotation
    TranslatePO(PO1);
    TranslatePO(PO2);

    // find first simplex point
    glm::vec2 direction = glm::vec2(1, 1); // where to look, random direction for now
    glm::vec2 C, B, A;                     // simplex triangle, from oldest to newest point
    C = Support(*PO1, *PO2, direction);
    // find next direction to search in
    direction = -C; // towareds origin

    // find next simplex point
    B = Support(*PO1, *PO2, direction);
    // loop line case
    while (glm::dot(direction, B) < 0)
    {
        C = B;
        B = Support(*PO1, *PO2, direction);
        if (B == C)
            return false; // sanity check making sure point passes origin
    }
    // find next direction (normal to line towards origin) using triple product (AB X AO) X AB
    direction = glm::vec2(glm::cross(glm::cross(glm::vec3(B - C, 0), glm::vec3(-C, 0)), glm::vec3(B - C, 0))); // might break

    // complete simplex triangle
    A = Support(*PO2, *PO2, direction);
    // loop the triangle case, chase better triangles!
    int region = 3;
    while (region != 0)
    {
        // lose the furthest point
        if (region == 1)
            C = A;
        if (region == 2)
            B = A;

        // recasting A
        A = Support(*PO1, *PO2, direction);
        if (glm::dot(direction, A) <= 0)
            return false; // sanity check
        if ((C == A) || (B == A))
            return false; // sanity check

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
glm::vec2 findNormalToOrigin(glm::vec2 A, glm::vec2 B)
{
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

glm::vec2 EPA(glm::vec2 A, glm::vec2 B, glm::vec2 C, physicsObject *PO1, physicsObject *PO2)
{
    std::vector<glm::vec2> polytope = {A, B, C}; // hopefully corrent handedness

    int index = 0;
    float minDistance = INFINITE;
    glm::vec2 minNormal;

    while (minDistance == INFINITE)
    {
        for (unsigned int i = 0; i < polytope.size(); i++)
        {
            int j = (i + 1) % polytope.size(); // the next point, loops round to 0

            // get da normal
            glm::vec2 normal = -findNormalToOrigin(polytope[i], polytope[j]);

            float distance = glm::dot(polytope[i], normal);

            if (distance < 0)
            {
                distance *= -1;
                normal *= -1;
            }

            if (distance < minDistance)
            {
                index = j;
                minDistance = distance;
                minNormal = normal;
            }
        }

        // use support to find closest point to normal
        glm::vec2 support = Support(*PO1, *PO2, minNormal);
        float sDistance = dot(minNormal, support);

        // normal is not on the edge
        if (abs(sDistance - minDistance) > 0.00001)
        {
            minDistance = INFINITE;
            polytope.insert(polytope.begin() + index, support); // add new point to polytope
        }
    }

    glm::vec2 result = minNormal * (float)(minDistance + 0.01);
    // std::cout << "resultant vector: " << result.x << " , " << result.y << std::endl;
    return result;
}

// takes our physics objects, loops through, applies forces, and updates positions / rotations
void UpdatePhysics(float deltaTime)
{
    // first update all velocities / positions
    physicsObject *pCurrent = pStart; // first physics object
    glm::vec2 acceleration, friction;

    for (int i = 0; i < sizeOfPhysicsWorld; i++)
    {                                  // iterates through list
        (*pCurrent).colliding = false; // yeah

        // adding friction to the objects
        friction = glm::vec2(-1) * glm::normalize((*pCurrent).velocity) * glm::vec2(2) * glm::vec2((*pCurrent).mass);
        if ((*pCurrent).velocity != glm::vec2(0))
        {
            // (*pCurrent).AddForce(friction);
        }

        acceleration = (*pCurrent).resultantForce / (*pCurrent).mass;
        (*pCurrent).velocity += acceleration * deltaTime;
        (*pCurrent).position += (*pCurrent).velocity * deltaTime; // nice

        (*pCurrent).resultantForce = glm::vec2(0, 0); // resetting in preperation for next frame
        pCurrent = (*pCurrent).pNext;                 // next physics object ggwp
    }

    // then check for collisions, also haha pP
    physicsObject *pPO1 = pStart; // first object
    physicsObject *pPO2 = pStart; // second object
    glm::vec2 resultant;
    // loop first object
    while (pPO1 != pEnd)
    {
        // loop second object
        while (pPO2 != pEnd)
        {
            pPO2 = (*pPO2).pNext;
            resultant = glm::vec2(0);
            GJK(pPO1, pPO2, &resultant);
            // if collide
            if (resultant != glm::vec2(0))
            {
                float mTotal = (*pPO1).mass + (*pPO2).mass;
                float angle = 0; // angle for our transformation matrix

                angle = glm::dot(glm::vec2(0, 1), glm::normalize(resultant));
                angle = acos(angle);
                // std::cout << "angle is " << angle / 3.141 * 57.295 << std::endl;

                (*pPO1).position += -((*pPO2).mass / mTotal) * resultant; // half of the offset to this
                (*pPO2).position += ((*pPO1).mass / mTotal) * resultant;  // and half to the other

                // after offsetting, add forces to each object
                // in elastic collision, both objects exxentially swap kinetic energies
                // Ek = 0.5 * mass * length(velocity) * length(velocity)
                // also P0 = P1 ie momentum before is momentum after
                // use absolute velocities
                /*
                glm::vec2 vel1 = (*pPO2).velocity - (*pPO1).velocity; // sus
                glm::vec2 vel2 = (*pPO1).velocity - (*pPO2).velocity;

                // use velocities to find forces
                vel1 /= deltaTime; // getting acceleration
                vel2 /= deltaTime;

                glm::vec2 force1 = glm::vec2(((*pPO1).velocity.x / deltaTime) * (*pPO1).mass, 0);
                glm::vec2 force2 = glm::vec2(((*pPO2).velocity.x / deltaTime) * (*pPO2).mass, 0);

                // all collisisons are fully elastic, so transfer ALL energy over from one object to another
                (*pPO1).AddForce(force2 * 1.0f);
                (*pPO1).AddForce(-force1);
                (*pPO2).AddForce(force1 * 1.0f);
                (*pPO2).AddForce(-force2);
                */

                // finding impulse using crazy black box equasion
                float impulse;
                glm ::vec2 normal = glm::normalize(resultant);
                glm::vec2 Vab = (*pPO1).velocity - (*pPO2).velocity;
                float elasticity = 1;

                impulse = glm::dot(-(1+elasticity) * Vab, normal);
                impulse /= glm::dot(normal, normal * ((1/(*pPO1).mass) + (1/(*pPO2).mass)));

                (*pPO1).velocity += (impulse * normal) / (*pPO1).mass;
                (*pPO2).velocity -= (impulse * normal) / (*pPO2).mass;
            }
        }
        // update active objects to check
        pPO1 = (*pPO1).pNext;
        pPO2 = pPO1;
    }
    // then update colliding objects
}

#endif