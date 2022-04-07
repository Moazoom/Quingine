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


// SUPPORT FUNCTION
// take al points ( as vectors in model space!), find dot products of them and our target direction
// largest dot product is farthest away :)

// aka support function
glm::vec2 findFarthestPoint(glm::vec2 targetDir, glm::vec2* vertices, int nVertices){
    float temp;
    float biggest = 0;
    glm::vec2 farthest;
    // loops through verticies
    for(int i = 0; i < nVertices; i++){
        temp = glm::dot(targetDir, *vertices); // stores the dot product
        if(biggest < temp){ // checks if this point is farther, and sets accordingly
            biggest = temp;
            farthest = *vertices;
        }
    }
    return farthest; // neat
}



// SANITY CHECK BEYOND ORIGIN
// do a dot product between our direction & furthest point coorrds as a vector from origin (normalised)
// if dot product > 0, angle between vectors is < 90 therefore it is beyond the origin

// deduces if a point is beyond the origin, using a direction vector
bool checkIfBeyondOrigin(glm::vec2 targetDir, glm::vec2 point){
    if(glm::dot(targetDir, glm::normalize(point)) > 0.0f){
        return true;
    }
    else return false;
}




// VECTOR TRIPLE PRODUCT ie finding next dir for triangle
// also used above

// finds normal direction on line AB towards origin
glm::vec2 findNormalToOrigin(glm::vec2 A, glm::vec2 B){
    // converting points to 3D
    glm::vec3 nA = glm::vec3(A.x, A.y, 0);
    glm::vec3 nB = glm::vec3(B.x, B.y, 0);

    // find cross product between AB and A to origin
    glm::vec3 outward = glm::cross(nB - nA, glm::vec3(0) - nA);
    
    // cross product of "outward" vector to find perpendicular vector to origin!
    glm::vec3 originDir = glm::cross(nB - nA, outward);
    return originDir;
}


// CHECKING IF SIMPLEX DIFFERENCE TRIANGLE HAS ORIGIN
// need to check 2 regions, again check the vid its helpful
// lots of dot product shi ill leave this for later
// if checks for only 2 possible outside regions fail, origin is within triangle!
