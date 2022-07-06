// a rewrite of suvatic2d.h
// it was getting too messy
// ggwp this is pretty cool
// or will be hopefully

// helpful vid: https://www.youtube.com/watch?v=ajv46BSqcK4
// and thyis one for EPA https://www.youtube.com/watch?v=0XQ2FSz3EK8
#ifndef SUVATIC_H
#define SUVATIC_H
#include "glm/glm.hpp" // aw yea its maths time!!
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <vector> // huh?
#include "physics.cpp"

// translates colliders by posisiton and rotation
void TranslatePO(physicsObject PO){
    // first get our matrix transform
    glm::mat3 tranform = glm::mat3(1);
    transform = glm::translate(transform, PO.position);
    transform = glm::rotate(transform, glm::radians(PO.rotation));
    // now we apply this matrix tranform to all our collider points
    for(int i = 0; i < size(PO.collider), i++){
        PO.collider[i] = tranform * glm::vec3(PO.collider, 1); // huh?
    }
}

// support function, returns hull point given two shapes and a direction
glm::vec2 Support(physicsObject PO1, physicsObject PO2, glm::vec2 direction){
    // get the largest dot product of first collider
    float temp = 0;
    float biggest = glm::dot(direction, PO1.collider[0]); // huh?
    glm::vec2 point1, point2;
    for(int i = 0; i < size(PO1.collider); i++){
        temp = glm::dot(direction, PO1.collider[i]);
        if(biggest < temp){
            biggest = temp;
            point1 = PO1.collider[i];
        }
    }

    // get largest dp of second collider in opposing direction
    temp = 0;
    biggest = glm::dot(-direction, PO2.collider[0]); // huh?
    for(int i = 0; i < size(PO2.collider); i++){
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
    TranslatePO(PO1);
    TranslatePO(PO2);

    // find first simplex point
    glm::vec2 direction = glm::vec2(1, 1); // where to look
    glm::vec2 C, B, A; // simplex triangle, from oldest to newest point
    C = Support(PO1, PO2, direction);

    // find next direction to search in
    direction = -C; // towareds origin

    // find next simplex point
    glm::vec2 B = Support(PO1, PO2, direction);
    if(glm::dot(direction, B) < 0) return false; // sanity check making sure point passes origin 

    // find next direction (normal to line towards origin) using tiple product
    // (AB X AO) X AB
    direction = glm::vec2(glm::cross(glm::cross(glm::vec3(B-C, 0), glm::vec3(0-C, 0)), glm::vec3(B-C, 0))); // might break

    // complete simplex triangle
    A = Support(PO1, PO2, direction);
    if(glm::dot(direction, A) < 0) return false; // sanity check

    // loop the triangle case, chase better triangles!
    int region = TriangleCase(A, B, C, &direction);
    while(region != 0){
        // lose the furthest point
        if(region == 1) C = A;
        if(region == 2) B = A;

        // recasting A
        A = Support(PO1, PO2, direction);
        if(glm::dot(direction, A) < 0) return false; // sanity check

        // checking new triangle
        region = TriangleCase(A, B, C, &direction);
    }

    // if loop breaks, there was a collision!
    return true;
}

#endif