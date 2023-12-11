#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "pyramid.h"
#include "stb_image.h"

//GLM Libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <random>
#include "cube.h"
#include "texture2d.h"
#include "sphere.h"

//define PI
#define M_PI 3.1415926535897932384626433832795

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods);
void ToggleProjectionMatrix();
glm::mat4  ResetModelView(float angle);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f)); //Default spawn location of the camera

bool usePerspective = true;
glm::mat4 projection; //Projection matrix

bool useDirectionalLight = true;
bool useFlashlight = false;

float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

// Define a wobble parameters
//float wobbleSpeed = 0.5f; // Adjust the speed of the wobble
//float wobbleAmount = 30.0f; // Adjust the amount of wobble
//float rotationSpeed = 10.0f; // Adjust the speed of rotation

//FrameRate
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My 3D Scene - Christopher Roelle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Lock mouse to window and add a callback for mouse movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    //Scroll callback
    glfwSetScrollCallback(window, scroll_callback);

    //Key callback for single action buttons
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Enable depth testing (will stay on until we disable with) glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);

    //TODO::SOME MESHES HAVE FLIPPED FACES, ENABLE BACK-FACE CULLING TO SEE THEM. NEED TO CORRECT THESE
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaderfiles/vertex.glsl", "shaderfiles/fragment.glsl"); // you can name your shader files however you like

    //Testing lighting
    //Shader sampleLitObjShader("shaderfiles/samplePointLightVertex.glsl", "shaderfiles/samplePointLightFragm.glsl");
    Shader lightCubeSampleShader("shaderfiles/lightCubeVertex.glsl", "shaderfiles/lightCubeFragm.glsl");
    Shader multiLightShader("shaderfiles/sampleMultiLightVertex.glsl", "shaderfiles/sampleMultiLightFragm.glsl");

    //Models
    // 
    // TODO:: Implement Indices to cut down on extra verts
    // 
    //                       Position                       len    wid
    Plane floorPlane = Plane(glm::vec3(0.0f, -0.01f, -0.3f), 3.0f,  8.0f);

    //Candle Jar, Wax and Wicks
    Cylinder candleJar = Cylinder(glm::vec3(0.0f,  0.0f, 0.0f), 0.5f, 0.75f, 40,   3,           false,   true); //No top, because its a candle holder
    Cylinder candle = Cylinder(glm::vec3(0.0f, 0.01f, 0.0f), 0.49f, 0.3f, 40, 1, true, false);

    Cylinder wick1 = Cylinder(glm::vec3( 0.20f, 0.3f,  0.15f), 0.05f, 0.1f, 8, 1, true, false);
    Cylinder wick2 = Cylinder(glm::vec3(-0.20f, 0.3f,  0.15f), 0.05f, 0.1f, 8, 1, true, false);
    Cylinder wick3 = Cylinder(glm::vec3(  0.0f, 0.3f, -0.20f), 0.05f, 0.1f, 8, 1, true, false);

    //Pumpkin Holder
    Sphere pumpkinHolderBase = Sphere(glm::vec3(1.5f, 0.0f, 0.5f), 0.4f, 0.2f, 30, true);//position, radLong, radLat, sides, semi
    Cylinder pumpkinHolderStem = Cylinder(glm::vec3(1.5f, 0.17f, 0.5f), 0.2f, 0.2f, 30, 3, false, false); //position, rad, height, sides, subdivs, draw top, draw btm
    Cylinder pumpkinHolderBody = Cylinder(glm::vec3(1.5f, 0.37f, 0.5f), 0.6f, 1.5f, 40, 3, false, true); //position, rad, height, sides, subdivs, draw top, draw btm

    //Pumpkin
    Sphere pumpkinBody = Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.4f, 0.3f, 15, false);
    Cylinder pumpkinStem = Cylinder(glm::vec3(0.0f, 0.28f, 0.0f), 0.045f, 0.08f, 15, 3, true, false); //position, rad, height, sides, subdivs, draw top, draw btm

    //Black Candle Jar, similar in height as the pumpkin holder.
    Cylinder blackJar = Cylinder(glm::vec3(-1.1f, 0.0f, 0.85f), 0.6f, 1.9f, 40, 3, false, true);


    //Build the wick array
    vector<float> candleWicks;
    candleWicks.insert(candleWicks.end(), wick1.Vertices.begin(), wick1.Vertices.end());
    candleWicks.insert(candleWicks.end(), wick2.Vertices.begin(), wick2.Vertices.end());
    candleWicks.insert(candleWicks.end(), wick3.Vertices.begin(), wick3.Vertices.end());

    Cube lightCube = Cube(glm::vec3(0.0f), 0.05f, 0.05f, 0.05f);


    //Texture stuff
    //Generate and store textures (Default constructor: FilePath, hasAlphaChannel), Texture2D.Texture to return the texture data
    Texture2D groundPlaneDiffuseTexture = Texture2D("textures/blackWood-diffuse.jpg", false);
    Texture2D groundPlaneSpecularTexture = Texture2D("textures/blackWood-specular.jpg", false);
    
    Texture2D ceramicDiffuseTexture = Texture2D("textures/ceramicJar-diffuse.jpg", false);
    Texture2D ceramicBlackDiffuseTexture = Texture2D("textures/ceramicJarBlack-diffuse.jpg", false);
    Texture2D ceramicSpecularTexture = Texture2D("textures/ceramicJar-specular.png", true);

    Texture2D waxDiffuseTexture = Texture2D("textures/wax-diffuse.jpg", false);
    Texture2D waxSpecularTexture = Texture2D("textures/wax-specular.jpg", false);

    Texture2D wickDiffuseTexture = Texture2D("textures/wick-diffuse.jpg", false);
    Texture2D wickSpecularTexture = Texture2D("textures/wick-specular.jpg", false);
    
    Texture2D candleLabelDiffuseTexture = Texture2D("textures/label-diffuse.png", true, false, true, true, true); //Using Overload to turn off repeat of texture
    Texture2D candleLabelSpecularTexture = Texture2D("textures/label-specular.png", true, false, true, true, true); //Using Overload to turn off repeat of texture

    Texture2D backWallDiffuseTexture = Texture2D("textures/backWall-diffuse.jpg", false);
    Texture2D backWallSpecularTexture = Texture2D("textures/backWall-specular.jpg", false);

    Texture2D silverDiffuseTexture = Texture2D("textures/silver-diffuse.jpg", false);
    Texture2D silverSpecularTexture = Texture2D("textures/silver-specular.jpg", false);

    Texture2D pumpkinDiffuseTexture = Texture2D("textures/pumpkin-diffuse.jpg", false);
    Texture2D pumpkinSpecularTexture = Texture2D("textures/pumpkin-specular.jpg", false);

    //Initial Set Camera Projection Matrix
    ToggleProjectionMatrix();

    //Adjust the intensity of the values
    float keyIntensity = 1.0f;
    float candleIntensity = 0.5f;

    glm::vec3 keyLightPosition = glm::vec3(0.0f, 3.0f, 3.0);
    glm::vec3 keyLightColor = glm::vec3(0.3f);
    glm::vec3 keyLightAttenuation = glm::vec3(1.0f, 0.09f, 0.032f);

    //Point Lights
    glm::vec3 candleLightPositions[] = {
        glm::vec3(wick1.Position.x, wick1.Position.y + wick1.Dimensions.y, wick1.Position.z), //candle light 1
        glm::vec3(wick2.Position.x, wick2.Position.y + wick2.Dimensions.y, wick2.Position.z), //candle light 2
        glm::vec3(wick3.Position.x, wick3.Position.y + wick3.Dimensions.y, wick3.Position.z)  //candle light 3
    };

    glm::vec3 candleLightColors[] = {
        //glm::vec3(1.0f, 1.0f, 1.0f) * keyIntensity,    //Key Light
        glm::vec3(1.0f, 0.0f, 0.0f) * candleIntensity, //candle light 1
        glm::vec3(0.5f, 0.5f, 0.0f) * candleIntensity, //candle light 1
        glm::vec3(1.0f, 0.5f, 0.0f) * candleIntensity  //candle light 1
    };

    //CONSTANT, LINEAR, QUADRATIC
    glm::vec3 candleLightAttenuations[]{
        glm::vec3(1.0f, 0.1f, 7.8f),
        glm::vec3(1.0f, 0.1f, 7.8f),
        glm::vec3(1.0f, 0.1f, 7.8f)
    };


    //Positions of the individual pumpkins in the container.
    glm::vec3 pumpkinPositions[]{
        glm::vec3(1.5f, 0.7f, 0.5f),
        glm::vec3(1.4f, 1.3f, 0.4f),
        glm::vec3(1.7f, 1.7f, 0.7f),
        glm::vec3(1.25f, 1.9f, 0.4f),
        glm::vec3(1.7f, 1.83f, 0.3f),
        glm::vec3(1.3f, 1.83f, 0.8f),
    };

    //Ratations of the individual pumpkins in the container.
    float pumpkinRotationAngles[]{
    0.0f,
    15.0f,
    -20.0f,
    25.0f,
    -15.0f,
    19.0f
    };

    //Scales of the individual pumpkins in the container.
    float pumpkinScales[]{
        1.0f,
        1.0f,
        0.75f,
        0.75f,
        0.80f,
        0.5f
    };

    //TODO::ADD ATTENUATION ARRAY FOR THE POINT LIGHTS

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //Delta Time stuff
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //GL_DEPTH_BUFFER_BIT to clear depth info from prev. frame.

        //Generate transforms
        glm::mat4 model = glm::mat4(1.0f); //Create the Model Matrix (for rendering in 3D) [MOVED BELOW FOR MULTI POSITION]
        float rotationAngle = 0.0f;
        glm::vec3 transformAxis = glm::vec3(0.0f);

        /*
        * =====================
        * Begin Rendering Stuff
        * =====================
        */

        glm::mat4 view = camera.GetViewMatrix();
        multiLightShader.use(); //Primary Shader

        //Set the viewer's position (the camera)
        multiLightShader.setVec3("viewPos", camera.Position);

        //Set the material
        multiLightShader.setInt("material.diffuse", 0);
        multiLightShader.setInt("material.specular", 1);
        multiLightShader.setInt("material.overlayDiffuse", 2);
        multiLightShader.setInt("material.overlaySpecular", 3);
        multiLightShader.setFloat("material.shininess", 32.0f);

        //Turn off the overlay textures, only enable when in use
        multiLightShader.setBool("material.useOverlayTexture", 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Set the Directional Light
        multiLightShader.setBool("dirLight.useDirectionalLight", useDirectionalLight);     //Toggles the calculations for directional lights
        multiLightShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f); //Direction of the light
        multiLightShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);   //Set low to not overbear
        multiLightShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);      //Light color
        multiLightShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);     //Color of the specular highlight

        //Candle Lights
        for (int i = 0; i < sizeof(candleLightPositions) / sizeof(candleLightPositions[0]); i++) {
            multiLightShader.setVec3("pointLights[" + std::to_string(i) + "].position", candleLightPositions[i]); //Light Position
            multiLightShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", candleLightColors[i] / 0.5f); //Set low to not overbear
            multiLightShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", candleLightColors[i] / 0.5f); //Light color
            multiLightShader.setVec3("pointLights[" + std::to_string(i) + "].specular", candleLightColors[i] / 0.5f); //Color of the specular highlight
            multiLightShader.setFloat("pointLights[" + std::to_string(i) + "].constant", candleLightAttenuations[i].x); //Attenuation Variables
            multiLightShader.setFloat("pointLights[" + std::to_string(i) + "].linear", candleLightAttenuations[i].y); //Attenuation Variables
            multiLightShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", candleLightAttenuations[i].z); //Attenuation Variables
        }

        //Key light
        multiLightShader.setVec3("pointLights[" +  std::to_string(3) + "].position", keyLightPosition); //Light Position
        multiLightShader.setVec3("pointLights[" +  std::to_string(3) + "].ambient", keyLightColor / 0.5f); //Set low to not overbear
        multiLightShader.setVec3("pointLights[" +  std::to_string(3) + "].diffuse", keyLightColor / 0.5f); //Light color
        multiLightShader.setVec3("pointLights[" +  std::to_string(3) + "].specular", keyLightColor / 0.5f); //Color of the specular highlight
        multiLightShader.setFloat("pointLights[" + std::to_string(3) + "].constant", keyLightAttenuation.x); //Attenuation Variables
        multiLightShader.setFloat("pointLights[" + std::to_string(3) + "].linear", keyLightAttenuation.y); //Attenuation Variables
        multiLightShader.setFloat("pointLights[" + std::to_string(3) + "].quadratic", keyLightAttenuation.z); //Attenuation Variables

        // SpotLight (Flashlight)
        multiLightShader.setBool("spotLight.useSpotLight", useFlashlight);
        multiLightShader.setVec3("spotLight.position", camera.Position); //Where the light is coming from, Flashlight, so camera
        multiLightShader.setVec3("spotLight.direction", camera.Front); //Direction, since flashlight, itll be the front of the camera
        multiLightShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f); //Set low to not overbear
        multiLightShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f); //Light color
        multiLightShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f); //Color of the specular highlight
        multiLightShader.setFloat("spotLight.constant", 1.0f); //Attenuation Variables
        multiLightShader.setFloat("spotLight.linear", 0.09f); //Attenuation Variables
        multiLightShader.setFloat("spotLight.quadratic", 0.032f); //Attenuation Variables
        multiLightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(15.5f))); //Cutoff of the brightest part of the light
        multiLightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f))); //Fades from the brightest to this angle to soften the light

        model = glm::mat4(1.0f); //Resetting the model view
        multiLightShader.setMat4("model", model);
        multiLightShader.setMat4("view", view);
        multiLightShader.setMat4("projection", projection);

        /*
        * =====================
        * Ground Plane
        * =====================
        */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundPlaneDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, groundPlaneSpecularTexture.Texture);
        floorPlane.Draw();
        
        /*
        * =====================
        * Candle Jar
        * =====================
        */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ceramicDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ceramicSpecularTexture.Texture);
        glActiveTexture(GL_TEXTURE2);
        multiLightShader.setBool("material.useOverlayTexture", 1);
        glBindTexture(GL_TEXTURE_2D, candleLabelDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, candleLabelSpecularTexture.Texture);

        //Rotate the model 90d
        model = ResetModelView(180.0f); //Necessity for a bug... Too late to correct at the moment
        multiLightShader.setMat4("model", model);
        
        candleJar.Draw();

        //Set the texture then draw the candle in the jar
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waxDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, waxSpecularTexture.Texture);

        //Turn off the overlay textures
        multiLightShader.setBool("material.useOverlayTexture", 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        candle.Draw();

        /*
        * =====================
        * PUMPKIN HOLDER
        * =====================
        */
        //Turn up the shininess, since this is metal
        multiLightShader.setFloat("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, silverDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, silverSpecularTexture.Texture);
        pumpkinHolderBase.Draw();
        pumpkinHolderStem.Draw();
        pumpkinHolderBody.Draw();

        multiLightShader.setFloat("material.shininess", 32.0f);

        /*
        * =====================
        * Pumpkins
        * =====================
        */
        for (int i = 0; i < sizeof(pumpkinPositions) / sizeof(pumpkinPositions[0]); i++)
        {
            //Reset then rotate the pumpkins so that they are haphazardly placed in the jar
            model = ResetModelView(180.0f);
            model = glm::translate(model, pumpkinPositions[i]);
            model = glm::scale(model, glm::vec3(pumpkinScales[i]));
            model = glm::rotate(model, glm::radians(pumpkinRotationAngles[i]), glm::vec3(1.0f, 0.0f, 1.0f));
            multiLightShader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pumpkinDiffuseTexture.Texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pumpkinSpecularTexture.Texture);
            pumpkinBody.Draw();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, wickDiffuseTexture.Texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, wickSpecularTexture.Texture);
            pumpkinStem.Draw();
        }

        /*
        * =====================
        * Black Jar
        * =====================
        */
        model = ResetModelView(180.0f);
        multiLightShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ceramicBlackDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ceramicSpecularTexture.Texture);
        glActiveTexture(GL_TEXTURE2);
        blackJar.Draw();

        /*
        * =====================
        * Wicks
        * =====================
        */

        //Set the texture and draw the wicks
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wickDiffuseTexture.Texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wickSpecularTexture.Texture);

        model = glm::mat4(1.0f); //Reset the model
        multiLightShader.setMat4("model", model);

        wick1.Draw();
        wick2.Draw();
        wick3.Draw();

        /*
        * =====================
        * LIGHTING
        * =====================
        */

        //Draw the light cube
        lightCubeSampleShader.use();
        lightCubeSampleShader.setMat4("projection", projection);
        lightCubeSampleShader.setMat4("view", view);

        for (int i = 0; i < sizeof(candleLightPositions) / sizeof(candleLightPositions[0]); i++) {
            model = glm::mat4(1.0f); //Reset the model
            model = glm::translate(model, candleLightPositions[i]);
            lightCubeSampleShader.setMat4("model", model);
            lightCubeSampleShader.setVec3("lightColor", candleLightColors[i]);

            lightCube.Draw();
        }

        //Draw the key light
        model = glm::mat4(1.0f); //Reset the model
        model = glm::translate(model, keyLightPosition);
        model = glm::scale(model, glm::vec3(3.0f));
        lightCubeSampleShader.setMat4("model", model);
        lightCubeSampleShader.setVec3("lightColor", keyLightColor);
        lightCube.Draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    floorPlane.DeallocateVertexArrayBuffers();

    candleJar.DeallocateVertexArrayBuffers();
    candle.DeallocateVertexArrayBuffers();

    wick1.DeallocateVertexArrayBuffers();
    wick2.DeallocateVertexArrayBuffers();
    wick3.DeallocateVertexArrayBuffers();

    pumpkinBody.DeallocateVertexArrayBuffers();
    pumpkinStem.DeallocateVertexArrayBuffers();

    pumpkinHolderBase.DeallocateVertexArrayBuffers();
    pumpkinHolderStem.DeallocateVertexArrayBuffers();
    pumpkinHolderBody.DeallocateVertexArrayBuffers();

    lightCube.DeallocateVertexArrayBuffers();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

//Sets the model back to 1.0 and rotates it 180 to deal with a bug I have.... Bandaid due to time constraints
glm::mat4 ResetModelView(float angle) {
    glm::mat4 model = glm::mat4(1.0f);
    float rotationAngle = glm::radians(angle);
    glm::vec3 transformAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    model = glm::rotate(model, rotationAngle, transformAxis);

    return model;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    //For now, move up and down with Left Control and space (World relative, not screen)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    //Reset Speed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
        camera.ResetMoveSpeed();

}

//Key callback for single input keys (things that should only be done once and not every frame)
void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

    //Toggle Camera Projection Matrix
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        usePerspective = !usePerspective;
        ToggleProjectionMatrix();
    }

    //Light controls
    //Toggle Directional Light
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
        useDirectionalLight = !useDirectionalLight;
    //Toggle Flashlight
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        useFlashlight = !useFlashlight;
}

//Callback for the mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Should initially be true, then set the mouse last values to current position
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; //Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset, usePerspective);
}

//Callback for the scroll wheel
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {

    bool scrollMod = false;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        scrollMod = true;

    camera.ProcessMouseScroll(yOffset, scrollMod);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//Swaps between Perspective and Orthographic Matrices
void ToggleProjectionMatrix() {

    std::cout << "PROJECTION CHANGED::";

    //Perspective
    if (usePerspective) {
        std::cout << "PERSPECTIVE" << std::endl;
        projection = glm::perspective(glm::radians(camera.CurrentFOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
    }
    else {
        std::cout << "ORTHOGRAPHIC" << std::endl;
        float aspect = (float)(SCR_WIDTH / SCR_HEIGHT);
        projection = glm::ortho(-5.0f, 5.0f, -4.0f, 4.0f, -0.1f, 100.0f); //Orthographic
    }

}