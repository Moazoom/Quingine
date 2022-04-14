// ok so basically
// this is a phyics engne
// or hopefully will be one, someday
// USES GJK ALGORYTHM
// made by moazoom aw yea#
// lets start with 2d

// helpful vid: https://www.youtube.com/watch?v=ajv46BSqcK4


#include "glm/glm.hpp" // aw yea its maths time!!
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <iostream>
#include <vector> // huh?


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
bool checkForIntersection(float* vertices1, int size1, glm::vec2 position1, float rotation1, float* vertices2, int size2, glm::vec2 position2, float rotation2){
    // generate vector arrays
    glm::vec2 colliders1[size1];
    fillVecArray(vertices1, &colliders1[0], size1);
    translateColliders(&colliders1[0], size1, position1, rotation1);

    glm::vec2 colliders2[size2];
    fillVecArray(&vertices2[0], &colliders2[0], size2);
    translateColliders(&colliders2[0], size2, position2, rotation2);

    // array to store points
    std::vector<glm::vec2> points;

    // triangle business
    glm::vec2 targetDir;
    glm::vec2 A, B, C; // our blyatful triangle
    int region, num;

    // first, find a direction
    targetDir = glm::vec2(1, 1);
    // getting first 3 points
    C = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    points.push_back(C);
    targetDir = -C; // towards origin

    B = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    points.push_back(B);
    targetDir = findNormalToOrigin(C, B); // find our next direction
    if(!checkIfBeyondOrigin(B - C, B)) return false; // if B isnt beyond origin, shapes arent intersecting

    A = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
    points.push_back(A);
    if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

    // main loop basically find new direction, discard a point, find a new one and try again
    targetDir = checkTriangle(A, B, C, &region);
    num = 0;
    // function checkTriangle returns (0, 0) if there is an intersection
    while(targetDir != glm::vec2(0.0f)){
        num++;

        // if region is AB getting rid of C
        if(region == 1){
            C = B;
            B = A;
        }

        // if region is AC getting rid of B
        if(region == 2){
            B = C;
            C = A;
        }

        // recasting A
        A = findFurthestPoint(targetDir, colliders1, size1) - findFurthestPoint(-targetDir, colliders2, size2);
        points.push_back(A);
        if(!checkIfBeyondOrigin(targetDir, A)) return false; // we know how it iz

        // retrying new triangle
        region = 0;
        targetDir = checkTriangle(A, B, C, &region);
    }

    //std::cout << num << "--";
    return true; // if all other checks fail
}


void findFurthestEdge(glm::vec2 direction, std::vector<glm::vec2> points, glm::vec2 (*edge)[2]){
    float biggest = -INFINITE;
    float temp;
    int num = 0;
    for(unsigned int i = 0; i < points.size(); i++){
        temp = glm::dot(direction, points[i]);
        // this should happen twice..
        if(biggest <= temp){
            biggest = temp;
            (*edge)[1] = (*edge)[0];
            (*edge)[0] = points[i];
            num++;
        }
    }
    if(num < 2) std::cout << "farthest edge fail rip" << std::endl;
}

// return vector array of edge pairs
std::vector<glm::vec2[2]> getEdges(std::vector<glm::vec2> points){
    glm::vec2 direction = {0, 1};
    glm::vec2 edgeTemp[2];
    float angleForEdges = 0.0f;
    float amountToAdd = 360 / points.size();
    std::vector<glm::vec2[2]> edges; // stores our edges
    // loop through each point to find corresponding edges
    for(unsigned int i = 0; i <  points.size(); i++){
        direction = glm::rotate(direction, angleForEdges);
        findFurthestEdge(direction, points, &edgeTemp);
        edges.push_back(edgeTemp);
        angleForEdges += amountToAdd;
    }
    return edges;
}


// checks if our edges vector has a point
bool checkEdges(std::vector<glm::vec2[2]> edges, glm::vec2 point){
    for(unsigned int i = 0; i < edges.size(); i++){
        if(edges[i][0] == point) return true;
        if(edges[i][1] == point) return true;
    }
    return false;
}

bool checkPoints(std::vector<glm::vec2> points, glm::vec2 point){
    for(unsigned int i = 0; i < points.size(); i++){
        if(points[i] == point) return true;
    }
    return false;
}

// using MEPA!!
void collide(std::vector<glm::vec2> points, glm::vec2* iColliders1, int size1, glm::vec2* iColliders2, int size2){
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

    // first find the edges ezpz
    std::vector<glm::vec2[2]> edges = getEdges(points); // stores our edges
    // check all points were used
    for(unsigned int i = 0; i < points.size(); i++){
        if(!checkEdges(edges, points[i])) std::cout << "missing point in edges!" << std::endl;
    }

    glm::vec2 point;
    glm::vec2 closestEdge[2];


    // our main loop that checks if weve reached the outer edge
    while(!checkPoints(points, point)){
        // add point to points
        points.push_back(point);
        
        // refresh edges
        edges = getEdges(points);
        // check all points were used
        for(unsigned int i = 0; i < points.size(); i++){
            if(!checkEdges(edges, points[i])) std::cout << "missing point in edges!" << std::endl;
        }

        // find closest point
        float closest = 0;
        glm::vec2 closestVertex;
        for(unsigned int i = 0; i < points.size(); i++){
            if(closest >= glm::length(points[i])){
                closest = glm::length(points[i]);
                closestVertex = points[i];
            }
        }

        // find closest edge with the point
        closest = 0;
        closestEdge[0] = closestVertex;
        for(unsigned int i = 0; i < edges.size(); i++){
            if((edges[i][0] == closestVertex) && (closest >= glm::length(edges[i][1]))){
                closest = glm::length(edges[i][1]);
                closestEdge[1] = edges[i][1];
            }
            if((edges[i][1] == closestVertex) && (closest >= glm::length(edges[i][0]))){
                closest = glm::length(edges[i][0]);
                closestEdge[1] = edges[i][0];
            }
        }

        // get closest direction
        glm::vec2 direction = closestEdge[0] + closestEdge[1]; // close nuff?

        // find farthest point in dir
        point = findFurthestPoint(direction, colliders1, size1)  - findFurthestPoint(-direction, colliders2, size2);
    }

    // find closest point on outer edge once we get there, using closestEdge
    // finding normal outwards to closest edge
    glm::vec2 normal = glm::normalize(-findNormalToOrigin(closestEdge[0], closestEdge[1]));
    // get angle to transform
    float angle = acos(glm::dot(normal, glm::vec2(0, 1)));
    // tranform any point on line by angle
    closestEdge[0] = glm::rotate(closestEdge[0], -angle);
    // y of this is what we need!
    glm::vec2 offset = {0, closestEdge[0].y};
    // rotate out offset vector back
    offset = glm::rotate(offset, angle);

    // printing for now
    std::cout << "offset vector: " << offset.x << " , " << offset.y << std::endl;
}

