#include <cmath>
#include <complex>
#include <iostream>
#include <ostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "SHADER.h"
#include "TEXTURE.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CAMERA.h"

static float aspect = 800.f / 600.f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (float)width / height;
    if (width == 0 || height == 0) {
        aspect = 800.f/600.f;
    }
}

static Camera* _CAM;
static double lastXpos, lastYpos;
static bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) return;

    const float sensitivity = 0.1f;
    if (firstMouse) {
        firstMouse = false;
        lastXpos = xpos;
        lastYpos = ypos;
        return;
    }
    double xOffset = xpos - lastXpos;
    double yOffset = lastYpos - ypos;

    _CAM->yaw += xOffset * sensitivity;
    _CAM->pitch += yOffset * sensitivity;
    _CAM->updateDirection();

    if (_CAM->pitch > 89.9f) _CAM->pitch = 89.89f;
    if (_CAM->pitch < -89.9f) _CAM->pitch = -89.9f;

    lastXpos = xpos;
    lastYpos = ypos;
}

static float deltaTime;
static GLFWmonitor* monitor;
static bool fullscreen = false;
//static glm::vec3 velocity(0.0f, 0.0f, 0.0f);

void processInput(GLFWwindow *window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const float cameraSpeed = 10.f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (fullscreen == true) {
            glfwSetWindowMonitor(window, NULL, 100, 100, 800, 600, GLFW_DONT_CARE);
            fullscreen = !fullscreen;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPos(window, 0, 0);
            firstMouse = true;
        }
    }

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            _CAM->cameraPos += cameraSpeed * glm::vec3(cos(glm::radians(_CAM->yaw)), 0, sin(glm::radians(_CAM->yaw)));
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            _CAM->cameraPos -= cameraSpeed * glm::vec3(cos(glm::radians(_CAM->yaw)), 0, sin(glm::radians(_CAM->yaw)));
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            _CAM->cameraPos -= cameraSpeed * glm::normalize(glm::cross(_CAM->cameraFront, _CAM->cameraUp));
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            _CAM->cameraPos += cameraSpeed * glm::normalize(glm::cross(_CAM->cameraFront, _CAM->cameraUp));
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            _CAM->cameraPos += cameraSpeed * _CAM->cameraUp;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            _CAM->cameraPos -= cameraSpeed * _CAM->cameraUp;
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
            switch (fullscreen) {
                case false:
                    glfwSetWindowMonitor(window, monitor, 0, 0, 10, 10, GLFW_DONT_CARE); //lol me neither
                    fullscreen = !fullscreen;
                case true:
                    break;
            }
        }
        /*if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && _CAM->cameraPos.y <= -1.f) {
            velocity.y += (9.8 + 4.43);
        }*/

    //_CAM->cameraPos.y = 0.0f;

    _CAM->updateDirection();
}

int main() {
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW3. fml" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "title", NULL, NULL);
    monitor = glfwGetPrimaryMonitor();
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    ///////////////
    Shader defaultShader("../resources/shaders/vertex.vs", "../resources/shaders/fragment.fs");
    defaultShader.use();

    Shader lightShader("../resources/shaders/lightVertex.vs", "../resources/shaders/lightFragment.fs");

    Texture defaultTexture("../resources/textures/defaultTexture.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Texture awesome("../resources/textures/awesomeface.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Texture defaultSpecular("../resources/textures/defaultSpecular.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    defaultTexture.use(0);
    defaultSpecular.use(1);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    float vertices[] = {
        // positions          // normals           // texture coords
        ///back face
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,   // top left
        ///front face
        0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    // top left
        ///left face
        -0.5f,  0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        -0.5f, -0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,    // top left
        ///right face
        0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,    // top left
        ///bottom
        0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,   // top left
        ///top
        0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f   // top left
    };
    unsigned int indicies[] = {
        //back
        0, 1, 3,
        1, 2, 3,
        //front
        4, 5, 7,
        5, 6, 7,
        //left
        8, 9, 11,
        9, 10, 11,
        //right
        12, 13, 15,
        13, 14, 15,
        //bottom
        16, 17, 19,
        17, 18, 19,
        //top
        20, 21, 23,
        21, 22, 23,
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_READ);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int sVAO;
    glGenVertexArrays(1, &sVAO);
    glBindVertexArray(sVAO);

    float skyVertices[] = {
        // positions          // normals           // texture coords
        ///back face
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   100.0f, 100.0f,   // top right
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   100.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 100.0f,   // top left
        ///front face
        0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   100.0f, 100.0f,   // top right
        0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   100.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 100.0f,    // top left
        ///left face
        -0.5f,  0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,   100.0f, 100.0f,   // top right
        -0.5f, -0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,   100.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   000.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 100.0f,    // top left
        ///right face
        0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   100.0f, 100.0f,   // top right
        0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   100.0f, 0.0f,   // bottom right
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 100.0f,    // top left
        ///bottom
        0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,   100.0f, 100.0f,   // top right
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   100.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 100.0f,   // top left
        ///top
        0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   100.0f, 100.0f,   // top right
        0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   100.0f, 0.0f,   // bottom right
       -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 100.0f   // top left
    };
    unsigned int skyIndicies[] = {
        //back
        0, 1, 3,
        1, 2, 3,
        //front
        4, 5, 7,
        5, 6, 7,
        //left
        8, 9, 11,
        9, 10, 11,
        //right
        12, 13, 15,
        13, 14, 15,
        //bottom
        16, 17, 19,
        17, 18, 19,
        //top
        20, 21, 23,
        21, 22, 23,
    };
    unsigned int sVBO;
    glGenBuffers(1, &sVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    unsigned int sEBO;
    glGenBuffers(1, &sEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyIndicies), skyIndicies, GL_STATIC_READ);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);

    static Camera camera;
    camera.cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
    _CAM = &camera;

    glfwSwapInterval(1);
    while(!glfwWindowShouldClose(window))
    {
        float lastFrame;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        /*
        if (camera.cameraPos.y != -1) velocity.y -= 9.8f;


        std::cout << velocity.y << std::endl;

        camera.cameraPos += velocity * deltaTime;
        if (camera.cameraPos.y  <= -1) {velocity.y = 0.0f; camera.cameraPos.y = -1;}
        */
        processInput(window);

        glClearColor(.0f, .0f, .0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        awesome.use();

        //moves object to world space
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        //projects coordinates to NDCs
        glm::mat4 projection;
        //projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 99.9f);
        //projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 10.0f);
        camera.pitch = -45.0f;
        camera.yaw = 45.0f;
        camera.updateDirection();

        defaultShader.use();
        defaultShader.setMat4fv("modelMatrix", model);
        defaultShader.setMat4fv("viewMatrix", camera.getView());
        defaultShader.setMat4fv("projectionMatrix", projection);

        defaultShader.setVec3("viewPos", camera.cameraPos);
        defaultShader.setInt("material.diffuse", 0);
        defaultShader.setInt("material.specular", 1);
        defaultShader.setFloat("material.shininess", 32.0f);
        //defaultShader.setVec3f("dirLight.ambient",  0.1f, 0.1f, 0.1f);
        //defaultShader.setVec3f("dirLight.diffuse",  0.9f, 0.9f, 0.9f); // darken diffuse light a bit
        //defaultShader.setVec3f("dirLight.specular", 1.0f, 1.0f, 1.0f);
        
        //defaultShader.setVec3f("light.position", 1.2f, 2.0f, 2.0f);
        //defaultShader.setVec4f("light.descriptor", 1.2f, 2.0f, 2.0f, 1.f);
        //defaultShader.setFloat("light.constant",  1.0f);
        //defaultShader.setFloat("light.linear",    0.09f);
        //defaultShader.setFloat("light.quadratic", 0.032f);
        //defaultShader.setVec4("light.descriptor",  glm::vec4(camera.cameraPos, 1.0f));
        //defaultShader.setVec3("light.direction", camera.cameraFront);
        //defaultShader.setFloat("light.cutOff",   glm::cos(glm::radians(12.5f)));
        //defaultShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        //defaultShader.setVec3f("dirLight.direction", 0.0f, -1.0f, 0.0f);
        //defaultShader.setVec3f("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        //defaultShader.setVec3f("dirLight.diffuse", 0.0f, 0.4f, 0.0f);
        //defaultShader.setVec3f("dirLight.specular", 1.0f, 1.0f, 1.0f);

        defaultShader.setVec3f("pointLights[0].position", 1.2f, 2.0f, 2.0f);
        defaultShader.setFloat("pointLights[0].constant",  1.0f);
        defaultShader.setFloat("pointLights[0].linear",    0.09f);
        defaultShader.setFloat("pointLights[0].quadratic", 0.032f);
        defaultShader.setVec3f("pointLights[0].ambient", 0.1f, 0.1f, 0.1f);
        defaultShader.setVec3f("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        defaultShader.setVec3f("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

        defaultShader.setVec3f("pointLights[1].position", -1.2f, 2.0f, -2.0f);
        defaultShader.setFloat("pointLights[1].constant",  1.0f);
        defaultShader.setFloat("pointLights[1].linear",    0.09f);
        defaultShader.setFloat("pointLights[1].quadratic", 0.032f);
        defaultShader.setVec3f("pointLights[1].ambient", 0.3f, 0.2f, 0.0f);
        defaultShader.setVec3f("pointLights[1].diffuse", 0.8f, 0.7f, 0.0f);
        defaultShader.setVec3f("pointLights[1].specular", 1.0f, 1.0f, 0.0f);

        defaultShader.setVec3f("pointLights[2].position", 0.0f, -2.0f, 0.0f);
        defaultShader.setFloat("pointLights[2].constant",  1.0f);
        defaultShader.setFloat("pointLights[2].linear",    0.09f);
        defaultShader.setFloat("pointLights[2].quadratic", 0.032f);
        defaultShader.setVec3f("pointLights[2].ambient", 0.3f, 0.0f, 0.0f);
        defaultShader.setVec3f("pointLights[2].diffuse", 0.8f, 0.0f, 0.0f);
        defaultShader.setVec3f("pointLights[2].specular", 1.0f, 0.0f, 0.0f);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.2f, 2.0f, 2.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        lightShader.setMat4fv("modelMatrix", model);
        lightShader.setMat4fv("viewMatrix", camera.getView());
        lightShader.setMat4fv("projectionMatrix", projection);
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.2f, 2.0f, -2.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        lightShader.setMat4fv("modelMatrix", model);
        lightShader.setMat4fv("viewMatrix", camera.getView());
        lightShader.setMat4fv("projectionMatrix", projection);
        lightShader.setVec3("lightColor", glm::vec3(0.8f, 0.7f, 0.0f));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        lightShader.setMat4fv("modelMatrix", model);
        lightShader.setMat4fv("viewMatrix", camera.getView());
        lightShader.setMat4fv("projectionMatrix", projection);
        lightShader.setVec3("lightColor", glm::vec3(1.f, 0.f, 0.0f));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        defaultTexture.use(0);
        glBindVertexArray(sVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);
        defaultShader.use();
        defaultShader.setVec3f("pointLights[0].ambient", 0.1f, 0.1f, 0.1f);
        defaultShader.setVec3f("pointLights[0].diffuse", .5f, 0.5f, 0.5f);
        defaultShader.setFloat("pointLights[0].linear", .0f);
        defaultShader.setFloat("pointLights[0].quadratic", 0.f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 47.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.f));
        defaultShader.setMat4fv("modelMatrix", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}