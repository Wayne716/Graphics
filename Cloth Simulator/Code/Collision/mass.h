#pragma once#include "glm/glm.hpp"#include "glm/gtc/matrix_transform.hpp"#include "glm/gtc/type_ptr.hpp"class SharedEdge;struct Mass {    Mass(glm::vec3 pos, bool pin) : position(pos), startPosition(pos), lastPosition(pos), pinned(pin) {};    glm::vec3 normal();    glm::vec3 velocity(float deltaTime) {        return (position - lastPosition) / deltaTime;    }public:    bool pinned;    glm::vec3 position;    glm::vec3 startPosition;    glm::vec3 lastPosition;    glm::vec3 forces;    SharedEdge* sharedEdge;};