#include <iostream>#include <cmath>#include <random>#include <vector>#include "glm/glm.hpp"#include "glm/gtc/matrix_transform.hpp"#include "glm/gtc/type_ptr.hpp"#include "mass.h"#include "spring.h"#include "mesh.h"#include "cloth.h"#include <iostream>Cloth::Cloth(float w, float h, int pw, int ph, float t) {    this->width = w;    this->height = h;    this->pointsWidth = pw;    this->pointsHeight = ph;    this->thickness = t;}Cloth::~Cloth() {    masses.clear();    springs.clear();    delete mesh;}void Cloth::simulate(float fps, float steps, ClothParameters* cp, std::vector<glm::vec3> ext, Sphere* s0) {    float deltaTime = 1.0f / fps / steps;    // mass of a single point    float m = width * height * cp->density / pointsWidth / pointsHeight;        glm::vec3 force;    for (glm::vec3 &a : ext)        force += a;    force *= m;        for (Mass &mass : masses)        mass.forces = force;    // + glm::vec3(glm::sin(mass.position.x * mass.position.y * deltaTime * air), glm::cos(mass.position.z * deltaTime), glm::sin(glm::cos(5 * mass.position.x * mass.position.y * mass.position.z))) * 3.0f;        for (Spring &s : springs) {        if ((s.spring_type == STRUCTURAL && !cp->structual) ||            (s.spring_type == SHEARING && !cp->shearing) ||            (s.spring_type == BENDING && !cp->bending))            continue;        else {            glm::vec3 dir = s.mb->position - s.ma->position;            float strength = cp->ks * (length(dir) - s.rest_length);            if (s.spring_type == BENDING)                strength *= 0.2;            force = dir * strength / length(dir);            s.ma->forces += force;            s.mb->forces -= force;        }    }        // verlet integration    for (Mass &mass : masses) {        if (mass.pinned) continue;        float f = (1 - cp->damping) / 100.0f;        glm::vec3 v = (mass.position - mass.lastPosition) * f;        mass.lastPosition = mass.position;        mass.position += v + mass.forces / m * deltaTime * deltaTime;    }        for (Mass &mass : masses) {        s0->collideCloth(mass);    }        buildBoxes();    for (Mass &mass : masses)        selfCollide(mass, 12.0f);}float Cloth::hashPosition(glm::vec3 pos) {    float w = 3 * width / pointsWidth;    float h = 3 * height / pointsHeight;    float t = max(w, h);    glm::vec3 box((pos.x - fmod(pos.x, w))/w, (pos.y - fmod(pos.y, h))/h, (pos.z - fmod(pos.z, t))/t);    return box.x + box.y * pointsWidth + box.z * pointsWidth * pointsHeight;}void Cloth::buildBoxes() {    for (const auto& p: map)        delete p.second;    map.clear();        for (Mass& mass: masses) {        float box = hashPosition(mass.position);        if (map[box] == nullptr)            map[box] = new vector<Mass*>;        map[box]->emplace_back(&mass);    }}void Cloth::selfCollide(Mass& mass, float steps) {    glm::vec3 correction(0.0f);    int count = 0;    for (Mass* m: *map[hashPosition(mass.position)]) {        if (m->position == mass.position) continue;        float dist = length(mass.position - m->position);        if (dist < 2 * thickness) {            correction += ((mass.position - m->position) / dist * (2 * thickness - dist));            ++count;        }    }    if (count) {        mass.position += correction * 10.0f;    }}// fill up <masses> and <springs>void Cloth::buildGrid() {    float w = width / pointsWidth;    float h = height / pointsHeight;    for (int i=0; i<pointsHeight; ++i) {        for (int j=0; j<pointsWidth; ++j) {            glm::vec3 pos;            if (orientation == HORIZONTAL) {                pos = glm::vec3(w * j, 1, h * i);            }            else {                float offset = (float)rand() / 500.0 * RAND_MAX - 1.0 / 1000.0;                pos = glm::vec3(w * j, h * i, offset);            }            masses.emplace_back(Mass(pos,false));        }    }    for (int i=0; i<pinnedPoints.size(); ++i) {        std::vector<int> xy = pinnedPoints[i];        int index = xy[0] + xy[1] * pointsWidth;        masses[index].pinned = true;    }    for (int j=0; j<masses.size(); ++j) {        int x = j % pointsWidth;        int y = (j - x) / pointsWidth;        if (x > 0)            springs.emplace_back(Spring(&masses[j], &masses[x-1+y*pointsWidth], STRUCTURAL));        if (y < pointsHeight-1)            springs.emplace_back(Spring(&masses[j], &masses[x+(y+1)*pointsWidth], STRUCTURAL));        if (x > 0 && y < pointsHeight-1)            springs.emplace_back(Spring(&masses[j], &masses[x-1+(y+1)*pointsWidth], SHEARING));        if (x < pointsWidth-1 && y < pointsHeight-1)            springs.emplace_back(Spring(&masses[j], &masses[x+1+(y+1)*pointsWidth], SHEARING));        if (x > 1)            springs.emplace_back(Spring(&masses[j], &masses[x-2+y*pointsWidth], BENDING));        if (y < pointsHeight-2)            springs.emplace_back(Spring(&masses[j], &masses[x+(y+2)*pointsWidth], BENDING));    }}void Cloth::buildMesh() {    if (masses.size() == 0) return;    Mesh* mesh = new Mesh();    std::vector<Triangle*> triangles;        for (int i=0; i<pointsHeight-1; ++i) {        for (int j=0; j<pointsWidth-1; ++j) {            Mass* mass = &masses[i * pointsWidth + j];            // two triangles: up-left & down-right            // set points in counter-clock-wise            triangles.push_back(new Triangle(mass, mass + pointsWidth, mass + 1));            triangles.push_back(new Triangle(mass + 1, mass + pointsWidth, mass + pointsWidth + 1));        }    }    for (int i=0; i<triangles.size(); ++i) {        Triangle* triangle = triangles[i];        SharedEdge* se1 = new SharedEdge();        SharedEdge* se2 = new SharedEdge();        SharedEdge* se3 = new SharedEdge();        Edge* e1 = new Edge();        Edge* e2 = new Edge();        Edge* e3 = new Edge();                // edge head        triangle->sharedEdge = se1;                triangle->m1->sharedEdge = se1;        triangle->m2->sharedEdge = se2;        triangle->m3->sharedEdge = se3;                se1->edge1 = e1;        se1->next = se2;        se1->mass = triangle->m1;        se1->triangle = triangle;                se2->edge1 = e2;        se2->next = se3;        se2->mass = triangle->m2;        se2->triangle = triangle;                se3->edge1 = e3;        se3->next = se1;        se3->mass = triangle->m3;        se3->triangle = triangle;    }        // set shared edges        int trianglesHeight = (pointsHeight - 1) * 2;    int trianglesWidth = (pointsWidth - 1) * 2;    bool topLeft = true;    for (int i=0; i<triangles.size(); ++i) {        Triangle* t = triangles[i];        if (topLeft) {            // left            if (i % trianglesWidth != 0)                t->m1->sharedEdge->edge2 = triangles[i-1]->m3->sharedEdge;            else                t->m1->sharedEdge->edge2 = nullptr;                        // up            if (i >= trianglesWidth)                t->m3->sharedEdge->edge2 = triangles[i-trianglesWidth+1]->m2->sharedEdge;            else                t->m3->sharedEdge->edge2 = nullptr;                        // slide            t->m2->sharedEdge->edge2 = triangles[i+1]->m1->sharedEdge;        }        else {            // right            if (i % trianglesWidth != trianglesWidth - 1)                t->m3->sharedEdge->edge2 = triangles[i+1]->m1->sharedEdge;            else                t->m3->sharedEdge->edge2 = nullptr;                        // down            if (i + trianglesWidth - 1 < trianglesWidth * trianglesHeight / 2.0)                t->m2->sharedEdge->edge2 = triangles[i + trianglesWidth - 1]->m3->sharedEdge;            else                t->m2->sharedEdge->edge2 = nullptr;                        // slide            t->m1->sharedEdge->edge2 = triangles[i-1]->m2->sharedEdge;        }        topLeft = !topLeft;    }    mesh->triangles = triangles;    this->mesh = mesh;}