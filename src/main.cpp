#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "camera.h"
#include "helpers/RootDir.h"

// #include "snake.h"

#include <iostream>
#include <vector>

// #define SKELETON

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

int init();
void updateProjView(const Shader& shader);

// settings
GLFWwindow* window;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
#ifdef SKELETON
Camera camera(glm::vec3(0.0f, 10.0f, 15.0f));
#else
Camera camera(glm::vec3(0.0f, 0.0f, -3.0f));
#endif
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting

Light light = {
    glm::vec3(5.0f, 5.0f, 5.0f),
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f,
    0.022f,
    0.0019f
};

#ifdef SNAKE_H
Snake snake;
#endif

glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
glm::mat4 view = camera.GetViewMatrix();
glm::mat4 model = glm::mat4(1.0f);
float s = 0.0f;
long long int n = 0;

int main()
{
    if (!init())
        return -1;    
    Shader lightingShader("/res/shaders/textures.vs", "./res/shaders/textures.fs");
    Shader lightCubeShader("/res/shaders/lighting.vs", "/res/shaders/lighting.fs");
    Shader coloredShader("/res/shaders/colored.vs", "/res/shaders/colored.fs");
    Shader raytracingShader("/res/shaders/raytracing.vs", "/res/shaders/raytracing.fs");

    Mesh cubeMesh = {
        // positions          // normals           // texture coords
        {        
            {{-0.5f, -0.5f, -0.5f},  {0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f},  {0.0f,  0.0f, -1.0f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f},  {0.0f, 1.0f}},

            {{-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f, 1.0f},   {0.0f, 0.0f}},
            {{0.5f, -0.5f,  0.5f},  {0.0f,  0.0f, 1.0f},   {1.0f, 0.0f}},
            {{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f, 1.0f},   {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f},  {0.0f,  0.0f, 1.0f},   {0.0f, 1.0f}},

            {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},

            {{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
            {{0.5f, -0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},

            {{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},  {1.0f, 1.0f}},
            {{0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},  {0.0f, 0.0f}},

            {{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
            {{0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 1.0f}},
            {{0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 0.0f}},
        },
        {0, 1, 2, 2, 3, 0, 
        4, 5, 6, 6, 7, 4, 
        8, 9, 10, 10, 11, 8, 
        12, 13, 14, 14, 15, 12, 
        16, 17, 18, 18, 19, 16, 
        20, 21, 22, 22, 23, 20}
    };

    Mesh planeMesh = {
        {
            { {-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
            { {0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },
            { {0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
            { {-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        },
        { 0, 1, 2, 2, 3, 0 }
    };

    Mesh screenMesh = {
        {
            { {-1.0f, -1.0f, 0.0f} },
            { { 1.0f, -1.0f, 0.0f} },
            { { 1.0f,  1.0f, 0.0f} },
            { {-1.0f,  1.0f, 0.0f} },
        },
        { 0, 1, 2, 2, 3, 0 }
    };

    Texture diffuse, specular;
    diffuse.load("/res/container2.png");
    specular.load("/res/container2_specular.png");

    #ifdef SKELETON
    glm::vec2 skeleton[] = {
        {-1.5f, 0.0f},
        {1.5f, 0.0f},
        {-1.5f/2, 3.0f/2},
        {1.5f/2, 3.0f/2},
        {0.0f, 3.0f},
        {0.0f, 4.5f},
        {-2.5f, 6.0f},
        {0.0f, 6.0f},
        {2.5f, 6.0f},
        {0.0f, 7.5f}
    };
    #endif

    #ifdef SNAKE_H
    snake.init();
    #endif

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        s = s - (s - deltaTime) / (++n);
        cout << 1.0f / s << endl;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        #if 0
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();



        //Cubes with textures
        model = glm::mat4(1.0f);
        #ifdef SKELETON
        light.position = glm::vec3(cos(100.0f * currentFrame), skeleton[9].y + 1.0f, sin(100.0f * currentFrame) + currentFrame);
        // const int s_i = (currentFrame / 0.25f - int(currentFrame / 0.25f / 10) * 10);
        // light.position = glm::vec3(skeleton[s_i], currentFrame + 1.0f);
        #endif

        updateProjView(lightingShader);
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        lightingShader.setFloat("material.shininess", 64.0f);

        lightingShader.setLight("light", light);

        lightingShader.setVec3("viewPos",  camera.Position);


        // bind diffuse map
        diffuse.bind(0);
        specular.bind(1);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f));
        lightingShader.setMat4("model", model);

        planeMesh.Draw();


        #ifdef SKELETON
        float left = currentFrame + sin(currentFrame);
        float right = currentFrame - sin(currentFrame);
        float mid = currentFrame;
        model = glm::mat4(1.0f);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[0], left))); cubeMesh.Draw(); 
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[1], right))); cubeMesh.Draw();
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[2], (left + mid) / 2.0f))); cubeMesh.Draw(); 
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[3], (right + mid) / 2.0f))); cubeMesh.Draw();
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[4], mid))); cubeMesh.Draw();

        model = glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[5], mid));
        lightingShader.setMat4("model", glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f))); cubeMesh.Draw();


        model = glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[6].x + cos(currentFrame)/2.0f, skeleton[6].y, mid));
        lightingShader.setMat4("model", model); cubeMesh.Draw();


        model = glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[7], mid));
        lightingShader.setMat4("model", glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f))); cubeMesh.Draw();


        model = glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[8].x - cos(currentFrame)/2.0f, skeleton[6].y, mid));
        lightingShader.setMat4("model", model); cubeMesh.Draw();


        lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(skeleton[9], mid))); cubeMesh.Draw();
        #endif
        
        #ifdef SNAKE_H
        if (!snake.isOvered()){
            snake.draw_blocks(cubeMesh, lightingShader);


            updateProjView(coloredShader);
            coloredShader.setLight("light", light);
            coloredShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
            coloredShader.setFloat("material.shininess", 32.0f);

            coloredShader.setVec3("viewPos",  camera.Position);


            glm::vec3 rgb = {0.5f, 1.0f, 0.31f};
            coloredShader.setVec3("material.ambient", rgb);
            coloredShader.setVec3("material.diffuse", rgb);
            snake.draw_snake(cubeMesh, lightingShader);


            rgb = {1.0f, 0.5f, 0.31f};
            coloredShader.setVec3("material.ambient", rgb);
            coloredShader.setVec3("material.diffuse", rgb);
            
            snake.draw_food(cubeMesh, coloredShader);
        }
        #endif


        //Light cube
        updateProjView(lightCubeShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        #endif

        raytracingShader.use();
        screenMesh.Draw();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    #ifdef SNAKE_H
    if (!snake.isOvered()){
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            snake.update(S_FORWARD);
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            snake.update(S_BACKWARD);
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
            snake.update(S_LEFT);
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            snake.update(S_RIGHT);
    }
    #endif
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int init() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pepes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, camera.scroll_callback);
    // glfwSetKeyCallback(window, key_callback);


    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return true;
}

void updateProjView(const Shader& shader){
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}