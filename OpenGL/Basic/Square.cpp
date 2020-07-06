#include <glad/glad.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

static const char* vertexShaderSource = R"glsl(
                                        #version 330 core
                                        layout (location = 0) in vec3 pos;
                                        layout (location = 1) in vec3 col;
                                        out vec3 color;
                                        void main()
                                        {
                                           color = col;
                                           gl_Position = vec4(pos, 1.0);
                                        }
                                        )glsl";

static const char* fragmentShaderSource = R"glsl(
                                        #version 330 core
                                        in vec3 color;
                                        out vec4 FragColor;
                                        void main()
                                        {
                                           FragColor = vec4(color, 1.0f);
                                        }
                                        )glsl";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // 加载所有的函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;
    
    // ==================== Shader
    
    // 引用创建的顶点着色器的对象的ID
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    // 编译的着色器对象 源码字符串数量 顶点着色器源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    
    // 生成片段着色器
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // 编译片段着色器
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    
    // 创建链接所有着色器的着色器程序对象
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    
    // 将之前编译的着色器附加到程序对象上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    // 链接着色器
    glLinkProgram(shaderProgram);
    
    // 现在每个着色器都会使用这个程序对象
    glUseProgram(shaderProgram);
    
    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // ==================== Vertex
    
    float vertices[] = {
        0.5f, 0.5f, .0f,
        0.5f, -.5f, .0f,
        -.5f, -.5f, .0f,
        -.5f, 0.5f, .0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    
    unsigned int EBO, VAO, PointsVBO, ColorsVBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &PointsVBO);
    glGenBuffers(1, &ColorsVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, PointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, ColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(.8f, .8f, .8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &PointsVBO);
    glDeleteBuffers(1, &ColorsVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    
    return 0;
}
