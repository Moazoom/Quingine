// ok so basically
// this is a phyics engne
// or hopefully will be one, someday
// USES GJK ALGORYTHM
// made by moazoom aw yea#
// lets start with 2d

/* STEPS:
1. pick random direction, find extreme support points and minskowsky difference
2. point towards origin, find two more points & difference
3. point towared line normal closest to origin, find points and diff
4. check triangle:
    if origin is within trianclge, return true
    if it isnt find where origin is continue 3 & 4. if you cant, retunrn false

helpful vid: https://www.youtube.com/watch?v=ajv46BSqcK4
*/

#include "glm/glm.hpp" // aw yea its maths time!!


// aka support function
glm::vec2 findFarthestPoint(glm::vec2 targetDir, glm::vec2* vertices, int nVertices){
    float temp;
    float biggest = 0;
    glm::vec2 farthest;
    // loops through verticies
    for(int i = 0; i < nVertices; i++){
        temp = glm::dot(glm::normalize(targetDir), *vertices); // stores the dot product
        if(biggest < temp){ // checks if this point is farther, and sets accordingly
            biggest = temp;
            farthest = *vertices;
        }
        vertices++;
    }
    return farthest; // neat
}

// deduces if a point is beyond the origin, using a direction vector
bool checkIfBeyondOrigin(glm::vec2 targetDir, glm::vec2 point){
    if(glm::dot(targetDir, glm::normalize(point)) > 0.0f){
        return true;
    }
    else return false;
}

// return perpendicular normal to iline1 
//(IN CROSS PRODUCT POSITION MATTERS remember right had rule
// in this function, if you want a positive, iline 1 should be "clockwise" to iline 2
glm::vec2 tripleProduct(glm::vec2 iLine1, glm::vec2 iLine2){
    // extending into 3d
    glm::vec3 line1 = glm::vec3(iLine1, 0);
    glm::vec3 line2 = glm::vec3(iLine2, 0);
    // first cross product
    glm::vec3 outward = glm::cross(line1, line2);
    // second cross product
    glm::vec3 normal = glm::cross(outward, line1);
    return glm::normalize(glm::vec2(normal.x, normal.y));
}

// finds normal direction on line AB towards origin
// if AB is "anticlockwise" to AO, triple product will still point in the correct direction!
glm::vec2 findNormalToOrigin(glm::vec2 A, glm::vec2 B){
    // find triple product between AB and A to origin
    glm::vec2 normal = tripleProduct(B - A, -A);

    return glm::normalize(normal);
}

// A is newest point ( this is where the magic happens!)
// return an int pointer; 1 if AB, 2 if AC, 0 if intersection
glm::vec2 checkTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C){
    // first check AB region
    glm::vec2 ABnormal = findNormalToOrigin(A, B);
    // if dot product is bigger than 0, origin is in AB
    if(glm::dot(ABnormal, glm::normalize(-A)) > 0.0f){
        return ABnormal;
    }

    // now check AC region
    glm::vec2 ACnormal = findNormalToOrigin(A, C);
    // we know how it iz
    if(glm::dot(ACnormal, glm::normalize(-A)) > 0.0f){
        return ACnormal;
    }

    // and finally, if both checks failed, then there is a intersection!
    return glm::vec2(0);
}


// our main big boy, determines intersection between two sets of verticies, nice
bool checkForIntersection(glm::vec2* vertices1, int nV1, glm::vec2* vertices2, int nV2){
    glm::vec2 targetDir;
    glm::vec2 A, B, C, temp; // our blyatful triangle
    int region; // for triangle checking

    // first, find a direction
    targetDir = glm::vec2(0, 1);

    // getting first 3 points
    C = findFarthestPoint(targetDir, vertices1, nV1) - findFarthestPoint(-targetDir, vertices2, nV2);
    targetDir = -C; // towards origin

    B = findFarthestPoint(targetDir, vertices1, nV1) - findFarthestPoint(-targetDir, vertices2, nV2);
    if(!checkIfBeyondOrigin(targetDir, B)) return false; // if B isnt beyond origin, shapes arent intersecting
    targetDir = findNormalToOrigin(C, B); // find our next direction

    A = findFarthestPoint(targetDir, vertices1, nV1) - findFarthestPoint(-targetDir, vertices2, nV2);
    if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

    // main loop basically find new direction, discard a point, find a new one and try again
    targetDir = checkTriangle(A, B, C);
    while(targetDir != glm::vec2(0)){
        // getting rid of C
        C = B;
        B = A;

        // recasting A
        A = findFarthestPoint(targetDir, vertices1, nV1) - findFarthestPoint(-targetDir, vertices2, nV2);
        if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

        // retrying new triangle
        targetDir = checkTriangle(A, B, C);
    }

    return true; // if all other checks fail
}