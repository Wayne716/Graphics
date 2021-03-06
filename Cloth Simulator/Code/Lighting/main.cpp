#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "mass.h"
#include "spring.h"
#include "mesh.h"
#include "cloth.h"
#include <iostream>
#include <vector>
#include <random>
using namespace std;

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastFrame = 0.0f, deltaTime = 0.0f;
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
bool gammaCorrection = true;

float exposure = 1.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}



void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(SPACE, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (exposure > 0.0f)
            exposure -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        exposure += 0.01f;
    }
}

unsigned int loadTexture(const char* path, bool gam) {
    unsigned int tex;
    glGenTextures(1, &tex);
    
    int width, height, colorChannel;
    unsigned char* data = stbi_load(path, &width, &height, &colorChannel, 0);
    if (data) {
        GLenum sourceFormat;
        GLenum internalFormat;
        if (colorChannel == 3) {
            internalFormat = gam ? GL_SRGB : GL_RGB;
            sourceFormat = GL_RGB;
        }
        else {
            internalFormat = gam ? GL_SRGB_ALPHA : GL_RGBA;
            sourceFormat = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, tex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, data);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else {
        std::cout << "FAILED LOADING IMAGE" << std::endl;
        stbi_image_free(data);
    }
    return tex;
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    
    int width, height, colorChannel;
    for (unsigned int i=0; i<faces.size(); ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &colorChannel, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "FAILED LOADING CUBE IMAGES" << std::endl;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return tex;
}


int main()
{
    // ==================== Init ==================== //
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    
    if (window == NULL)
        glfwTerminate();
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // ==================== Parameters ==================== //
    
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    int fps = 30;
    int steps = 20;
    
    float damping = 0.2f;
    float density = 150.0;
    float ks = 5000.0;
    bool structural = true;
    bool shearing = true;
    bool bending = true;
    
    ClothParameters* cp = new ClothParameters(structural, shearing, bending, damping, density, ks);
    
    float width = 1.5;
    float height = 1.0;
    int pointsWidth = 30;
    int pointsHeight = 20;
    float thickness = 0.0095;
    CLOTH_ORIENTATION orientation = HORIZONTAL;
    std::vector<std::vector<int>> pin {{0, 0}, {0, 19}};
    
    Cloth* cloth = new Cloth(width, height, pointsWidth, pointsHeight, thickness);
    cloth->orientation = orientation;
    cloth->pinnedPoints = pin;
    
    Shader shader("shader.vs", "shader.fs");

    shader.use();
    shader.setInt("texMap", 0);
    shader.setVec3("lightPos", glm::vec3(0.6f, 2.4f, -0.5f));
    
    cloth->buildGrid();
    cloth->buildMesh();
    
    unsigned int clothVAO, clothVBO;
    glGenVertexArrays(1, &clothVAO);
    glBindVertexArray(clothVAO);
    glGenBuffers(1, &clothVBO);
    glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    
    unsigned int flagTexture = loadTexture("texture/flag.png", false);
    bool texInit = false;
    
    int tris = (int)cloth->mesh->triangles.size();
    int dataSize = tris * 3 * 8;
    float v[dataSize];
    
    
    // ==================== Render ==================== //

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        
        vector<glm::vec3> ext = {glm::vec3(0.0, -9.8, 0.0)};
        for (int i=0; i<steps; ++i)
            cloth->simulate(fps, steps, cp, ext);
        
        int j = 0;
        if (!texInit) {
            for (int i=0; i<tris; ++i) {
                Triangle* t = cloth->mesh->triangles[i];
                v[j+6] = t->m1->position.x/1.5;
                v[j+7] = t->m1->position.z;
                v[j+14] = t->m2->position.x/1.5;
                v[j+15] = t->m2->position.z;
                v[j+22] = t->m3->position.x/1.5;
                v[j+23] = t->m3->position.z;
                j += 24;
            }
            texInit = true;
        }
        
        j = 0;
        for (int i=0; i<tris; ++i) {
            
            Triangle* t = cloth->mesh->triangles[i];
            
            glm::vec3 pos = t->m1->position;
            glm::vec3 nor = t->m1->normal();
            v[j++] = pos.x;
            v[j++] = pos.y;
            v[j++] = pos.z;
            v[j++] = nor.x;
            v[j++] = nor.y;
            v[j++] = nor.z;
            j += 2;
            
            pos = t->m2->position;
            nor = t->m2->normal();
            v[j++] = pos.x;
            v[j++] = pos.y;
            v[j++] = pos.z;
            v[j++] = nor.x;
            v[j++] = nor.y;
            v[j++] = nor.z;
            j += 2;
            
            pos = t->m3->position;
            nor = t->m3->normal();
            v[j++] = pos.x;
            v[j++] = pos.y;
            v[j++] = pos.z;
            v[j++] = nor.x;
            v[j++] = nor.y;
            v[j++] = nor.z;
            j += 2;
        }
        
        
        glBindVertexArray(clothVAO);
        glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
        glBufferData(GL_ARRAY_BUFFER, dataSize * sizeof(float), &v, GL_DYNAMIC_DRAW);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flagTexture);
        glDrawArrays(GL_TRIANGLES, 0, tris * 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
