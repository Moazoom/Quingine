// ok so basically
// this is a phyics engne
// or hopefully will be one, someday
// USES GJK ALGORYTHM
// made by moazoom aw yea#
// lets start with 2d

// helpful vid: https://www.youtube.com/watch?v=ajv46BSqcK4
// and thyis one for EPA https://www.youtube.com/watch?v=0XQ2FSz3EK8


#include "glm/glm.hpp" // aw yea its maths time!!
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <iostream>
#include <vector> // huh?
#include <array>

glm::vec2 MEPA(std::vector<glm::vec2> polytope, glm::vec2* iColliders1, int size1, glm::vec2* iColliders2, int size2);


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

// utility func for translating the colliders using a matrix, the inputted colliders should be the "base" colliders
int translateColliders(glm::vec2* Colliders, int size, glm::vec2 position, float rotation){
    glm::mat3 transform = glm::mat3(1.0f);
    transform = glm::translate(transform, position); // translation
    transform = glm::rotate(transform, glm::radians(rotation)); // rotation
    // apply matrix tranform to each element in a loop
    for(int i = 0; i < size; i++){
        *Colliders = transform * glm::vec3(*Colliders, 1);
        Colliders++;
    }
    return 0;
}

// aka support function
glm::vec2 findFurthestPoint(glm::vec2 targetDir, glm::vec2* vertices, int nVertices){
    float temp = 0;
    float biggest = glm::dot(targetDir, *vertices); // INIT THIS 
    glm::vec2 farthest = *vertices; // and this too
    // loops through verticies
    for(int i = 0; i < nVertices; i++){
        temp = glm::dot(targetDir, *vertices); // stores the dot product
        if(biggest < temp){ // checks if this point is further, and sets accordingly
            biggest = temp;
            farthest = *vertices;
        }
        vertices++; // next vertex
    }

    return farthest; // neat
}

// deduces if a point is beyond the origin, using a direction vector
bool checkIfBeyondOrigin(glm::vec2 targetDir, glm::vec2 point){
    if(glm::dot(glm::normalize(targetDir), glm::normalize(point)) >= 0.0f){
        return true;
    }
    else return false;
}

// finds normal direction on line AB towards origin
glm::vec2 findNormalToOrigin(glm::vec2 A, glm::vec2 B){
    // extending into 3d
    glm::vec3 line1 = glm::vec3(B-A, 0);
    glm::vec3 line2 = glm::vec3(-A, 0);
    // offsetting a small amount incase the two vectors line up perfectly (rare)
    if(glm::abs(glm::normalize(line1)) == glm::abs(glm::normalize(line2))){
        line1.x += 0.0001;
        line1.y += 0.0001;
    }
    // triple product
    glm::vec3 normal = glm::cross(glm::cross(line1, line2), line1);

    return glm::vec2(normal);
}

// return an int pointer; 1 if AB, 2 if AC, 0 if intersection
glm::vec2 checkTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, int* region){
    // first check AB region  (ACXAB)XAB
    glm::vec2 ABnormal = glm::cross(glm::cross(glm::vec3(C.x-A.x, C.y-A.y, 0), glm::vec3(B.x-A.x, B.y-A.y, 0)), glm::vec3(B.x-A.x, B.y-A.y, 0));
    // if dot product is bigger than 0, origin is in AB
    if(glm::dot(ABnormal, glm::normalize(-A)) > 0.0f){
        *region = 1;
        return ABnormal;
    }
    // now check AC region (ABXAC)XAC
    glm::vec2 ACnormal = glm::cross(glm::cross(glm::vec3(B.x-A.x, B.y-A.y, 0), glm::vec3(C.x-A.x, C.y-A.y, 0)), glm::vec3(C.x-A.x, C.y-A.y, 0));;
    // we know how it iz
    if(glm::dot(ACnormal, glm::normalize(-A)) > 0.0f){
        *region = 2;
        return ACnormal;
    }
    // and finally, if both checks failed, then there is a intersection!
    *region = 0;
    return glm::vec2(0);
}


// our main big boy, determines intersection between two sets of verticies, nice
bool checkForIntersection(float* vertices1, int size1, glm::vec2 position1, float rotation1, float* vertices2, int size2, glm::vec2 position2, float rotation2, glm::vec2 *offset){
    // generate vector arrays
    glm::vec2 colliders1[size1];
    fillVecArray(vertices1, &colliders1[0], size1);
    translateColliders(&colliders1[0], size1, position1, rotation1);

    glm::vec2 colliders2[size2];
    fillVecArray(&vertices2[0], &colliders2[0], size2);
    translateColliders(&colliders2[0], size2, position2, rotation2);

    // array to store points
    std::vector<glm::vec2> simplex = {glm::vec2(0), glm::vec2(0), glm::vec2(0)};

    // triangle business
    glm::vec2 targetDir;
    glm::vec2 A, B, C; // our blyatful triangle
    int region, num;

    // first, find a direction
    targetDir = glm::vec2(1, 1);
    // getting first 3 points
    C = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    targetDir = -C; // towards origin

    B = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    targetDir = findNormalToOrigin(C, B); // find our next direction
    if(!checkIfBeyondOrigin(B - C, B)) return false; // if B isnt beyond origin, shapes arent intersecting

    A = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

    // main loop basically find new direction, discard a point, find a new one and try again
    targetDir = checkTriangle(A, B, C, &region);
    num = 0;
    // function checkTriangle returns (0, 0) if there is an intersection
    while(targetDir != glm::vec2(0.0f)){
        num++;

        // if region is AB getting rid of C
        if(region == 1){
            // C = B;
            // B = A;
            C = A; // to preserver handedness
        }

        // if region is AC getting rid of B
        if(region == 2){
            // B = C;
            // C = A;
            B = A; // to preserve handedness
        }

        // recasting A
        A = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
        if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

        // retrying new triangle
        region = 0;
        targetDir = checkTriangle(A, B, C, &region);
    }
    // getting simplex
    simplex[0] = C;
    simplex[1] = B;
    simplex[2] = A;

    //std::cout << num << "--";
    *offset = MEPA(simplex, colliders1, size1, colliders2, size2);
    return true; // if all other checks fail
}


glm::vec2 MEPA(std::vector<glm::vec2> polytope, glm::vec2* iColliders1, int size1, glm::vec2* iColliders2, int size2){
    // getting colliders arrays here from pointers
    glm::vec2 colliders1[size1];
    for(int i = 0; i < size1; i++){
        colliders1[i] = *iColliders1;
        iColliders1++;
    }

    glm::vec2 colliders2[size2];
    for(int i = 0; i < size1; i++){
        colliders2[i] = *iColliders2;
        iColliders2++;
    }

    int minIndex = 0;
    float minDistance = INFINITE;
    glm::vec2 minNormal;

    while(minDistance == INFINITE){
        for(unsigned int i = 0; i < polytope.size(); i++){
            int j = (i + 1) % polytope.size();

            glm::vec2 normal = - findNormalToOrigin(polytope[i], polytope[j]);
            float distance = glm::dot(polytope[i], normal);

            // flipping for handedness
            if(distance < 0) distance *= -1;

            if(minDistance > distance){
                minIndex = j;
                minDistance = distance;
                minNormal = normal;
            }
        }

        glm::vec2 support = findFurthestPoint(minNormal, colliders1, size1) - findFurthestPoint(-minNormal, colliders2, size2);
        float sDistance = glm::dot(minNormal, support);

        if(abs(sDistance - minDistance) > 0.00001f){
            minDistance = INFINITE;
            polytope.insert(polytope.begin() + minIndex, support);
        }
    }

    glm::vec2 result = minNormal * (float)(minDistance + 0.1f);
    std::cout << "result vector: " << result.x << " , " << result.y << std::endl;
    return result;
}
/* 
void Mepa(points, shape1, shape2){
    int minDistance = Infinite
    vec2 minNormal

    while(minDistance == Infinite){
        for(i < amount of polytope points){
            get an edge

            find outward normal

            distance = dot(normal, one point on edge)

            if(distance < minDist){
                minDist = distance
                minNormal = normal
            }
        }

        vec2 support = suport function taking 2 hapes and our minNormal
        float sDistance = dot(minNormal, support)

        if(abs(sDistance - MinDistance) > 0.001){
            minDistance = Infinite;
            and add support to polytope
        }
    }
    return minNormal * mibnDistance
}


*/