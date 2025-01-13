#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/vector_angle.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "Bezier.h"

#include <iostream>

// for random float numbers
//#include <bits/stdc++.h>

// window
gps::Window myWindow;

// matrices
// model
glm::mat4 model;
glm::mat4 sceneModel;
glm::mat4 tennisBallModel;
glm::mat4 initialTennisBallModel;
glm::mat4 raindropModel;
glm::mat4 treeModel;
glm::mat4 carModel;
glm::mat4 waterModel;
// view
glm::mat4 view;
// projection
glm::mat4 projection;
// normal
glm::mat3 normalMatrix;
glm::mat3 sceneNormalMatrix;
glm::mat3 tennisBallNormalMatrix;
glm::mat3 treeNormalMatrix;
glm::mat3 carNormalMatrix;
glm::mat3 waterNormalMatrix;

// light parameters
glm::vec3 directionalLightDir = glm::vec3(0.0f, 10.0f, 10.0f);
glm::vec3 directionalLightColor;

// positional lights parameters
glm::vec3 positionalLightDir1 = glm::vec3(-1-89.7075f, 10.3366f, 21.2985f); //  red
glm::vec3 positionalLightDir2 = glm::vec3(-1-89.7215f, 10.291f, 1.01083); // orange
glm::vec3 positionalLightDir3 = glm::vec3(-1-115.338f, 10.2782f, 0.875658f); // purple
glm::vec3 positionalLightDir4 = glm::vec3(-1-115.416f, 10.3454f, 21.3536f);  // blue
glm::vec3 positionalLightColor1;
glm::vec3 positionalLightColor2;    
glm::vec3 positionalLightColor3;
glm::vec3 positionalLightColor4;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
// locatii lumina directionala
GLint directionalLightDirLoc;
GLint lightColorLoc;
// locatii lumini pozitionale
GLint positionalLightDirLoc1;
GLint positionalLightColorLoc1;
GLint positionalLightDirLoc2;
GLint positionalLightColorLoc2;
GLint positionalLightDirLoc3;
GLint positionalLightColorLoc3;
GLint positionalLightDirLoc4;
GLint positionalLightColorLoc4;

// camera
gps::Camera myCamera(
    glm::vec3(-20.0f, 3.0f, 3.0f),
    glm::vec3(10.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.2f;
bool canFly = true; // true -> camera se poate deplasa si sus, jos; false -> camera se poate misca doar inainte, inapoi, stanga, dreapta 

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D scene;
gps::Model3D tennis_ball;
gps::Model3D screenQuad;
gps::Model3D raindrop_obj;
gps::Model3D scena_doar_copaci;
gps::Model3D rain_instanced;
gps::Model3D car;
gps::Model3D leftBackWheel;
gps::Model3D leftFrontWheel;
gps::Model3D rightBackWheel;
gps::Model3D rightFrontWheel;
gps::Model3D windowsCar;
gps::Model3D oneTree;
GLfloat angle;

// shaders
gps::Shader myBasicShader;

// pentru miscarea mouse-ului
float speed = 0.01; //viteza deplasare
float pitch, yaw; // pitch = rotatia fata de axa z, yaw = rotatia fata de axa y
float prevX = 400, prevY = 300;
float sensitivity = 0.01f;

bool firstMouseMove = true;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// light shaders
gps::Shader lightingShader;

// shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;

// animatie minge de tenis 
float deltaZ = 0, deltaY = 3, viteza = 0.1, gravity = 0.05f;
bool upDirection = false;

// bounding boxes
const unsigned int GROUND_LEVEL = 0;

// rain 
const unsigned int START_RAINDROP_HEIGHT = 10;
const unsigned int NR_RAINDROPS = 100;
// raindrops place (only near the camera; they move with the camera)
const int RAINDROP_X = 10;
const int RAINDROP_Z = 10; // in OpenGL, y si z sunt inversate fata de sistemul de coordonate cartezian
const unsigned int MIN_RAINDROP_SPEED = 5.0f;
const unsigned int MAX_RAINDROP_SPEED = 15.0f;
// positions array
typedef struct r {
    glm::vec3 position;
    float velocity;
}Raindrop;
std::vector<Raindrop> raindrops;
float deltaTime;
float currentTime, lastTime = 0.0f;
GLuint raindropVAO, raindropVBO;
std::vector<glm::vec2> raindropsPosOffset;

// tree rotation after camera
float treeRotationAngle;
glm::vec3 cameraUpWorldSpace;
gps::Shader treeShader;

//dir masina
float dirX, dirZ;
glm::vec3 carBlenderPosition(0.0f, -1.0f, -20.0f);
gps::CarCamera carCamera(
    carBlenderPosition + glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -100.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 carPositionAdjust = -carBlenderPosition;//+ glm::vec3(0.0f, 1.0f, 0.0f);

bool canDrive = false;
bool carDrivingViewMode = false;
bool driving = false;
float wheelsAngle;
glm::vec3 rightBackWheelPos(1.57486f, 0.249317f, 2.56864f);
glm::vec3 rightFrontWheelPos(1.63543f, 0.246326f, -2.54323f);
glm::vec3 leftBackWheelPos(-1.84524f, 0.248186f, 2.54359f);
glm::vec3 leftFrontWheelPos(-1.7716f, 0.249858f, -2.5726f);

// Sine Waves
gps::Shader basicWaveShader;

const float GRID_WIDTH = 340.0f;
const float GRID_HEIGHT = 280.0f;
const int GRID_NUM_POINTS_WIDTH = 150;
const int GRID_NUM_POINTS_HEIGHT = 150;

//VBO, EBO and VAO
GLuint gridPointsVBO;
GLuint gridTrianglesEBO;
GLuint gridVAO;

//texture
const float textureRepeatU = 2.0f; //number of times to repeat seamless texture on u axis
const float textureRepeatV = 2.0f; //number of times to repeat seamless texture on v axis
const float waterYPos = -1.2f;
GLuint gridTexture;
GLint gridTextureLoc;

//simulation time
float simTime;
GLint simTimeLoc;

//light
GLint lightDirLoc;
//GLint lightColorLoc;
glm::vec3 lightDir;
glm::vec3 lightColor;

typedef struct spot {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float innerCutOff;
    float outerCutOff;
}SpotLight;

//spot light
SpotLight spotLight;

// for fog
bool fog = false;

// light type
bool isLight = true;

// scene presentation (camera animation)
std::vector<glm::vec3> cameraPositions, cameraTargets;
bool cameraAnimation, runningPresentation;
float cameraAnimationT;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
    WindowDimensions wd;
    glfwGetFramebufferSize(myWindow.getWindow(), &wd.width, &wd.height);
    myWindow.setWindowDimensions(wd);

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)wd.width / (float)wd.height,
        0.1f, 400.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glViewport(0, 0, wd.width, wd.height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        if (canDrive || driving) {
            driving = !driving;
            if (!driving) {
                myCamera.setCameraPosition(carCamera.getCameraPosition() + glm::vec3(-6.0f, 3.0f, 0.0f));
                myCamera.setCameraTarget(carCamera.getCameraTarget());
            }
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        carDrivingViewMode = !carDrivingViewMode;
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        fog = !fog;
    }

    if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        canFly = !canFly;
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        showDepthMap = !showDepthMap;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        cameraAnimation = !cameraAnimation;
        cameraAnimationT = 0;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        isLight = !isLight;
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (firstMouseMove) {
        prevX = xpos;
        prevY = ypos;
        firstMouseMove = false;

        //Get initial camera orientation matrix
        glm::mat4 orientationMatrix = myCamera.getViewMatrix();
        yaw = -glm::degrees(atan2(-orientationMatrix[0][2], orientationMatrix[2][2])); 
        pitch = -glm::degrees(asin(orientationMatrix[1][2])); 
    }
    else {

        double offsetX = prevX - xpos;
        double offsetY = prevY - ypos; // y-coordinates range from buttom to top
        prevX = xpos;
        prevY = ypos;

        offsetX *= sensitivity; // ca sa controlam sensibil va fi mouse-ul la miscare
        offsetY *= sensitivity;

        yaw += offsetX;
        pitch += offsetY;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        myCamera.rotate(pitch, yaw);
    }
}

float euclideanDistance(glm::vec3 a, glm::vec3 b) {
    return std::sqrt(std::pow((a.x - b.x), 2) + std::pow((a.y - b.y), 2) + std::pow((a.z-b.z), 2));
}

void processMovement() {
    
	if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed, canFly);
        float distance = euclideanDistance(myCamera.getCameraPosition(), carCamera.getCameraPosition());
        if(distance < 4)
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed, canFly);
	}

	if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed, canFly);
        float distance = euclideanDistance(myCamera.getCameraPosition(), carCamera.getCameraPosition());
        if (distance < 4)
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed, canFly);
	}

	if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed, canFly);
        float distance = euclideanDistance(myCamera.getCameraPosition(), carCamera.getCameraPosition());
        if (distance < 4)
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed, canFly);
	}

	if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed, canFly);
        float distance = euclideanDistance(myCamera.getCameraPosition(), carCamera.getCameraPosition());
        if (distance < 4)
            myCamera.move(gps::MOVE_LEFT, cameraSpeed, canFly);
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    }

    if (pressedKeys[GLFW_KEY_F]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_K]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        if (driving) {
            carCamera.move(gps::MOVE_FORWARD, cameraSpeed, false);
            wheelsAngle -= 2.0f;
        }
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        if (driving) {
            carCamera.move(gps::MOVE_BACKWARD, cameraSpeed, false);
            wheelsAngle += 2.0f;
        }
    }

    if (pressedKeys[GLFW_KEY_RIGHT]) {
        if (driving) {
            carCamera.move(gps::MOVE_RIGHT, cameraSpeed, false);
        }
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        if (driving) {
            carCamera.move(gps::MOVE_LEFT, cameraSpeed, false);
        }
    }

}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	//glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClearColor(0.5, 0.5, 0.5, 1.0);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face 
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_BLEND); // pentru combinarea culorilor (obiecte transparente sau semi-transparente)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

void initModels() {
    teapot.LoadModel("models/teapot/teapot20segUT.obj");
    scene.LoadModel("models/Scena_2/Scena_2.obj");
    tennis_ball.LoadModel("models/Minge_putine_varfuri/Minge_tenis_final.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    raindrop_obj.LoadModel("models/Raindrop/raindrop.obj");
    scena_doar_copaci.LoadModel("models/Copaci/Locatie_copaci.obj");
    car.LoadModel("models/Masina/CarWithoutWheels/Car2.obj");
    leftBackWheel.LoadModel("models/Masina/LeftBackWheel/LeftBackWheel.obj");
    leftFrontWheel.LoadModel("models/Masina/LeftFrontWheel/LeftFrontWheel.obj");
    rightBackWheel.LoadModel("models/Masina/RightBackWheel/RightBackWheel.obj");
    rightFrontWheel.LoadModel("models/Masina/RightFrontWheel/RightFrontWheel.obj");
    windowsCar.LoadModel("models/Geamuri_masina/Geamuri_masina.obj");
    //oneTree.LoadModel("models/Un_singur_copac/Un_singur_copac.obj");
    oneTree.LoadModel("models/Un_singur_copac_un_plan/Copac_un_plan.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/rainInstanced.vert",
        "shaders/rainInstanced.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    lightingShader.loadShader("shaders/lightingShader.vert", "shaders/lightingShader.frag");
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    treeShader.loadShader("shaders/treeShader.vert", "shaders/treeShader.frag"); 
    glCheckError();
    basicWaveShader.loadShader("shaders/basicWave.vert", "shaders/basicWave.frag");

}

void initOtherUniformsRainShader(gps::Shader &shader) {
    shader.cameraPosLoc = glGetUniformLocation(shader.shaderProgram, "cameraPos");
    shader.timeLoc = glGetUniformLocation(shader.shaderProgram, "time");
}

void initUniformsRainShader(gps::Shader &shader) {
	shader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	shader.viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	shader.normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 400.0f);
	shader.projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(shader.projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
    //directionalLightDir = glm::vec3(0.0f, 10.0f, 1.0f);
    shader.directionalLightDirLoc = glGetUniformLocation(shader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(shader.directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

	//set light color
    directionalLightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	shader.dirLightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(shader.dirLightColorLoc, 1, glm::value_ptr(directionalLightColor));
    shader.shadowMapLoc = glGetUniformLocation(shader.shaderProgram, "shadowMap");
    shader.lightSpaceTrMatrixLoc = glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix");
    initOtherUniformsRainShader(shader);

}

void initLightAttrUniforms(gps::Shader& shader) {
    //set the light direction (direction towards the light)
    //directionalLightDir = glm::vec3(0.0f, 10.0f, 1.0f);
    shader.directionalLightDirLoc = glGetUniformLocation(shader.shaderProgram, "dirLightDir");
    // send light dir to shader
    glUniform3fv(shader.directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

    //set light color
    directionalLightColor = glm::vec3(1.0f, 1, 1.0f); //white light
    shader.dirLightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(shader.dirLightColorLoc, 1, glm::value_ptr(directionalLightColor));


    // send positional lights directions
    // 1
    shader.positionalLightLoc1 = glGetUniformLocation(shader.shaderProgram, "posLightDir1");
    glUniform3fv(shader.positionalLightLoc1, 1, glm::value_ptr(positionalLightDir1));
    // 2
    shader.positionalLightLoc2 = glGetUniformLocation(shader.shaderProgram, "posLightDir2");
    glUniform3fv(shader.positionalLightLoc2, 1, glm::value_ptr(positionalLightDir2));
    // 3
    shader.positionalLightLoc3 = glGetUniformLocation(shader.shaderProgram, "posLightDir3");
    glUniform3fv(shader.positionalLightLoc3, 1, glm::value_ptr(positionalLightDir3));
    // 4
    shader.positionalLightLoc4 = glGetUniformLocation(shader.shaderProgram, "posLightDir4");
    glUniform3fv(shader.positionalLightLoc4, 1, glm::value_ptr(positionalLightDir4));
    //set light color
    // 1
    positionalLightColor1 = glm::vec3(1.0f, 0.0f, 0.0f); // red
    shader.positionalLightColorLoc1 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor1");
    // send light color to shader
    glUniform3fv(shader.positionalLightColorLoc1, 1, glm::value_ptr(positionalLightColor1));
    //2
    positionalLightColor2 = glm::vec3(0.0f, 0.0f, 1.0f);//glm::vec3(0.9f, 0.6f, 0.2f);  // orange
    shader.positionalLightColorLoc2 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor2");
    // send light color to shader
    glUniform3fv(shader.positionalLightColorLoc2, 1, glm::value_ptr(positionalLightColor2));
    // 3
    positionalLightColor3 = glm::vec3(0.0f, 1.0f, 0.0f);//glm::vec3(0.7f, 0.4f, 1.0f);  //purple
    shader.positionalLightColorLoc3 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor3");
    // send light color to shader
    glUniform3fv(shader.positionalLightColorLoc3, 1, glm::value_ptr(positionalLightColor3));
    // 4
    positionalLightColor4 = glm::vec3(1.0f, 1.0f, 1.0f);//glm::vec3(0.9f, 0.6f, 0.2f);//glm::vec3(0.0f, 0.0f, 1.0f); // blue
    shader.positionalLightColorLoc4 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor4");
    // send light color to shader
    glUniform3fv(shader.positionalLightColorLoc4, 1, glm::value_ptr(positionalLightColor4));
    shader.shadowMapLoc = glGetUniformLocation(shader.shaderProgram, "shadowMap");
    shader.lightSpaceTrMatrixLoc = glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix");

    //spot light
    spotLight.position = myCamera.getCameraPosition();
    shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
    glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
    spotLight.direction = myCamera.getCameraFrontDirection();
    shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
    glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.direction));
    spotLight.color = glm::vec3(0.0f, 0.0f, 1.0f);
    shader.spotLightColorLoc = glGetUniformLocation(shader.shaderProgram, "spotLightColor");
    glUniform3fv(shader.spotLightColorLoc, 1, glm::value_ptr(spotLight.color));
    spotLight.innerCutOff = 8.5f;
    shader.spotLightInnerCutOffLoc = glGetUniformLocation(shader.shaderProgram, "spotLightInnerCutOff");
    glUniform1f(shader.spotLightInnerCutOffLoc, glm::cos(glm::radians(spotLight.innerCutOff)));
    spotLight.outerCutOff = 10.5f;
    shader.spotLightOuterCutOffLoc = glGetUniformLocation(shader.shaderProgram, "spotLightOuterCutOff");
    glUniform1f(shader.spotLightOuterCutOffLoc, glm::cos(glm::radians(spotLight.outerCutOff)));

    

}

void initLightUniformsForShader(gps::Shader &shader) {
    shader.useShaderProgram();

    // create model matrix for teapot
    //model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    shader.viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    shader.normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 400.0f);
    shader.projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(shader.projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    shader.fogLoc = glGetUniformLocation(shader.shaderProgram, "fog");
    shader.lightTypeLoc = glGetUniformLocation(shader.shaderProgram, "lightType");

    initLightAttrUniforms(shader);

}

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    mySkyBox.Load(faces);
    skyboxShader.fogLoc = glGetUniformLocation(skyboxShader.shaderProgram, "fog");
    //skyboxShader.lightTypeLoc = glGetUniformLocation(skyboxShader.shaderProgram, "lightType");
}

void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //attach texture to FBO 
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// initialize raindrops positions
float randomNumber(float min, float max) {
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}
void initRaindrops() {
    Raindrop aux_raindrop;
    for (int i = 0; i < NR_RAINDROPS; i++)
    {
        aux_raindrop.position = glm::vec3(myCamera.getCameraPosition().x + randomNumber(-RAINDROP_X, RAINDROP_X), START_RAINDROP_HEIGHT, myCamera.getCameraPosition().z + randomNumber(-RAINDROP_Z, RAINDROP_Z));
        aux_raindrop.velocity = -randomNumber(MIN_RAINDROP_SPEED, MAX_RAINDROP_SPEED);
        raindrops.push_back(aux_raindrop);
    }
}

void initRaindropsInstanced() {

    glm::vec2 aux_pos;
    for (int i = 0; i < NR_RAINDROPS; i++)
    {
        aux_pos = glm::vec2(randomNumber(-RAINDROP_X, RAINDROP_X), randomNumber(-RAINDROP_Z, RAINDROP_Z));
        raindropsPosOffset.push_back(aux_pos);
        //raindropsPosOffset.push_back(aux_pos);
    }
}

void initSineWavesVBOs() {
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    //prepare vertex data to send to shader
    static GLfloat vertexData[GRID_NUM_POINTS_WIDTH * GRID_NUM_POINTS_HEIGHT * 4];

    //for each vertex in grid
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH; j++) {

            //tex coords
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 0] = j * textureRepeatU / (float)(GRID_NUM_POINTS_WIDTH - 1);
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 1] = textureRepeatV - i * textureRepeatV / (float)(GRID_NUM_POINTS_HEIGHT - 1);
            
            //xy position indices in grid (for computing sine function)
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 2] = (float)(j);
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 3] = (float)(i);
        }
    }

    glGenBuffers(1, &gridPointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridPointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //prepare triangle indices to send to shader
    static GLuint triangleIndices[(GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3];

    //for each square/rectangle in grid (each four neighboring points)
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT - 1; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH - 1; j++) {

            // setam varfurile pentru 2 triunghiuri => de asta mergem din 6 in 6 (2*3)
            //lower triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j)] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 1] = (i + 1) * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 2] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;

            //upper triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 3] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 4] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 5] = i * GRID_NUM_POINTS_WIDTH + j + 1;
        }
    }

    glGenBuffers(1, &gridTrianglesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridTrianglesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);

    //split vertex attributes
    //tex coords
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    //grid XY indices
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void initUniformsForSineWaves(gps::Shader &shader) {
    shader.useShaderProgram();

    // create model matrix for grid
    waterModel = glm::mat4(1.0f);
    shader.modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

    // get view matrix for current camera
    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else 
        view = myCamera.getViewMatrix();
    shader.viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix grid
    waterNormalMatrix = glm::mat3(glm::inverseTranspose(view * waterModel));
    shader.normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 400.0f);
    shader.projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(shader.projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    shader.gridTextureLoc = glGetUniformLocation(shader.shaderProgram, "diffuseTexture");
    glm::vec2 gridSize{ GRID_NUM_POINTS_WIDTH, GRID_NUM_POINTS_HEIGHT };
    shader.gridSizeLoc = glGetUniformLocation(shader.shaderProgram, "gridSize");
    glUniform2fv(shader.gridSizeLoc, 1, glm::value_ptr(gridSize));
    glm::vec2 gridDimensions{ GRID_WIDTH, GRID_HEIGHT };
    shader.gridDimensionsLoc = glGetUniformLocation(shader.shaderProgram, "gridDimensions");
    glUniform2fv(shader.gridDimensionsLoc, 1, glm::value_ptr(gridDimensions));

    initLightUniformsForShader(shader);
    shader.fogLoc = glGetUniformLocation(shader.shaderProgram, "fog");

    shader.timeLoc = glGetUniformLocation(shader.shaderProgram, "time");
}

void setupRaindropsAttributes()
{
    rain_instanced.LoadModel("models/Raindrop/raindrop.obj");
    initRaindropsInstanced();
    rain_instanced.configureInstancedArray(raindropsPosOffset, NR_RAINDROPS);

}

glm::mat4 computeLightSpaceTrMatrix() { 
    glm::mat4 lightView = glm::lookAt(directionalLightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
    const GLfloat near_plane = -60.0f, far_plane = 200.0f; 
    glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane); 
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;  
    return lightSpaceTrMatrix; 
}

void bindShadowMap(gps::Shader shader, bool depthPass)
{
    glActiveTexture(GL_TEXTURE3); 
    glBindTexture(GL_TEXTURE_2D, depthMapTexture); 
    if(!depthPass)
        glUniform1i(shader.shadowMapLoc, 3);

    glUniformMatrix4fv(shader.lightSpaceTrMatrixLoc,
        1, 
        GL_FALSE, 
        glm::value_ptr(computeLightSpaceTrMatrix())); 

}

void renderTeapot(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    //send teapot model matrix data to shader`
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    model = glm::translate(model, glm::vec3(0, 3, 5));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    
    //send teapot normal matrix data to shader
    if (!depthPass) 
    {
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction)); 
        
        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }


    // draw teapot
    teapot.Draw(shader);
}

void renderField(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    sceneModel = glm::mat4(1.0f);
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(sceneModel));

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    

    if (!depthPass)
    {
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        sceneNormalMatrix = glm::mat3(glm::inverseTranspose(view * sceneModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(sceneNormalMatrix));
    } 
    scene.Draw(shader);
}

void renderTennisBall(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    if (deltaZ + viteza > 10 || deltaZ + viteza < -10) {
        viteza = -viteza;
        upDirection = false;
        gravity = -gravity;
    }

    if (deltaY - 0.5 <= 0) {
        upDirection = true;
        gravity = -gravity;
    }
    if (deltaY + 0.5 >= 3 && upDirection)
        deltaY = 3.0f;
    else 
        deltaY += gravity;
    deltaZ += viteza;
    
    tennisBallModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 17));
    tennisBallModel = glm::translate(tennisBallModel, glm::vec3(0, deltaY, deltaZ));


    bindShadowMap(shader, depthPass);

    //tennisBallModel = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(tennisBallModel));

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    
    if (!depthPass)
    {
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        tennisBallNormalMatrix = glm::mat3(glm::inverseTranspose(view * tennisBallModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(tennisBallNormalMatrix));
    }

    tennis_ball.Draw(shader);

}

void renderRain(gps::Shader shader) {
    shader.useShaderProgram();
  
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime; 
    lastTime = currentTime;
    
    for (int i = 0; i<NR_RAINDROPS; i++)
    {
        
        raindrops[i].position.y += raindrops[i].velocity / 10; //* deltaTime; // we try to simulate the real movement of a raindrop based on it's speed and the time passed from the last time the scene was rendered
        if (raindrops[i].position.y < GROUND_LEVEL) {
            raindrops[i].position.x = myCamera.getCameraPosition().x + randomNumber(-RAINDROP_X, RAINDROP_X);
            raindrops[i].position.y = START_RAINDROP_HEIGHT;
            raindrops[i].position.z = myCamera.getCameraPosition().z + randomNumber(-RAINDROP_Z, RAINDROP_Z);
        }

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        raindropModel = glm::translate(glm::mat4(1.0f), raindrops[i].position);
        glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(raindropModel));
        normalMatrix = view;
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        raindrop_obj.Draw(shader);

    }
    
}

void renderRainInstanced(gps::Shader shader) {
    shader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    raindropModel = glm::identity<glm::mat4>();
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(raindropModel));
    normalMatrix = view;
    glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(shader.cameraPosLoc, 1, glm::value_ptr(myCamera.getCameraPosition()));
    glUniform1f(shader.timeLoc, deltaTime);
    rain_instanced.DrawRain(shader, NR_RAINDROPS);
}

void renderSkybox(gps::Shader shader) {
    shader.useShaderProgram();
    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1i(shader.fogLoc, fog);
    //glUniform1i(shader.lightTypeLoc, isLight);
    mySkyBox.Draw(skyboxShader, view, projection);
}

void renderTrees(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    
    sceneModel = glm::mat4(1.0f);
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(sceneModel));

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    
    if (!depthPass)
    {
        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        sceneNormalMatrix = glm::mat3(glm::inverseTranspose(view * treeModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(sceneNormalMatrix));

    }

    glDisable(GL_CULL_FACE); 
    scena_doar_copaci.Draw(shader);
    glEnable(GL_CULL_FACE);
}

void renderCar(gps::Shader shader, bool depthPass, gps::Model3D &model) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    carModel = glm::translate(glm::mat4(1.0f), carBlenderPosition); 
    carModel = glm::translate(carModel, (carCamera.getCameraPosition() + carPositionAdjust));
    carModel = glm::rotate(carModel, glm::pi<float>() - carCamera.carYAngle, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));

    

    if (!depthPass)
    {
        if (driving && !carDrivingViewMode)
            view = carCamera.getViewMatrix();
        else
            view = myCamera.getViewMatrix();
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(carNormalMatrix));
    }

    glDisable(GL_CULL_FACE); 
    model.Draw(shader);
    glEnable(GL_CULL_FACE); 
}

void renderOneTree(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    treeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-75, -0.5, -20));
    treeModel = glm::scale(treeModel, glm::vec3(1.5f, 2.0f, 1.0f));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(treeModel));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    if (!depthPass)
        glUniform1i(shader.shadowMapLoc, 3);

    glUniformMatrix4fv(shader.lightSpaceTrMatrixLoc,
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));

    

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
    glm::mat4 aux = view * treeModel;
    glm::vec3 xMod = aux[0];
    glm::vec3 yMod = aux[1];

    glm::vec3 cr = glm::cross(glm::vec3(1, 0, 0), yMod);

    glm::vec3 yFinal = yMod;
    glm::vec3 zFinal = cr;
    glm::vec3 xFinal = glm::cross(yFinal, zFinal);


    aux[0] = glm::vec4(xFinal, 0);
    aux[1] = glm::vec4(yFinal, 0);
    aux[2] = glm::vec4(zFinal, 0);


    if (!depthPass)
    {
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(aux));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        treeNormalMatrix = glm::mat3(glm::inverseTranspose(aux * treeModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(treeNormalMatrix));

    }

    glDisable(GL_CULL_FACE);
    oneTree.Draw(shader);
    glEnable(GL_CULL_FACE);
}

void renderSineWaves(gps::Shader shader, bool depthPass) {


    // select active shader program
    shader.useShaderProgram();
    glUniform1i(shader.fogLoc, fog);
    glUniform1i(shader.lightTypeLoc, isLight);
    //bindShadowMap(shader, depthPass);

    //send grid model matrix data to shader
    waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 0.0f, 75.0f));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(waterModel));

        //update view matrix
        if (driving && !carDrivingViewMode)
            view = carCamera.getViewMatrix();
        else
            view = myCamera.getViewMatrix();
        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));


        // compute normal matrix for grid
        waterNormalMatrix = glm::mat3(glm::inverseTranspose(view * waterModel));

        //send grid normal matrix data to shader
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(waterNormalMatrix));

        // lumina directionala
        glUniform3fv(shader.directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

        //send texture to shader
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, gridTexture);
        glUniform1i(shader.gridTextureLoc, 4);

        //send sim time
        glUniform1f(shader.timeLoc, simTime);

        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));


        glBindVertexArray(gridVAO);
    // draw grid
    glDrawElements(GL_TRIANGLES, (GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3, GL_UNSIGNED_INT, 0);

    //initLightAttrUniforms(shader);

}

void renderOneWheel(gps::Shader shader, bool depthPass, gps::Model3D& model) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    carModel = glm::translate(glm::mat4(1.0f), carBlenderPosition);
    carModel = glm::translate(carModel, (carCamera.getCameraPosition() + carPositionAdjust));
    carModel = glm::rotate(carModel, glm::pi<float>() - carCamera.carYAngle, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();
   

    if (!depthPass)
    {
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightDirectionLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(carNormalMatrix));
        glUniform3fv(shader.directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

    }

    glDisable(GL_CULL_FACE);
    model.Draw(shader);
    glEnable(GL_CULL_FACE);
}


void renderCarWheels(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    bindShadowMap(shader, depthPass);

    carModel = glm::translate(glm::mat4(1.0f), carBlenderPosition);
    carModel = glm::translate(carModel, (carCamera.getCameraPosition() + carPositionAdjust));
    carModel = glm::rotate(carModel, glm::pi<float>() - carCamera.carYAngle, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));

    if (driving && !carDrivingViewMode)
        view = carCamera.getViewMatrix();
    else
        view = myCamera.getViewMatrix();

    if (!depthPass)
    {
        // spot light 
        spotLight.position = myCamera.getCameraPosition();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightPos");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.position));
        spotLight.direction = myCamera.getCameraFrontDirection();
        shader.spotLightPosLoc = glGetUniformLocation(shader.shaderProgram, "spotLightDirection");
        glUniform3fv(shader.spotLightPosLoc, 1, glm::value_ptr(spotLight.direction));

        glUniformMatrix4fv(shader.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(shader.fogLoc, fog);
        glUniform1i(shader.lightTypeLoc, isLight);
        carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
        glUniformMatrix3fv(shader.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(carNormalMatrix));
        glUniform3fv(shader.directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));
        // left back wheel
        glDisable(GL_CULL_FACE);
        glm::mat4 aux_model;
        aux_model = carModel;
        aux_model = glm::translate(carModel, leftBackWheelPos);
        aux_model = glm::rotate(aux_model, glm::radians(wheelsAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        aux_model = glm::translate(aux_model, -leftBackWheelPos);
        glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(aux_model));
        leftBackWheel.Draw(shader);

        // left front wheel
        aux_model = carModel;
        aux_model = glm::translate(carModel, leftFrontWheelPos);
        aux_model = glm::rotate(aux_model, glm::radians(wheelsAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        aux_model = glm::translate(aux_model, -leftFrontWheelPos);
        glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(aux_model));
        leftFrontWheel.Draw(shader);

        // right back wheel
        aux_model = carModel;
        aux_model = glm::translate(carModel, rightBackWheelPos);
        aux_model = glm::rotate(aux_model, glm::radians(wheelsAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        aux_model = glm::translate(aux_model, -rightBackWheelPos);
        glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(aux_model));
        rightBackWheel.Draw(shader);

        // right front wheel
        aux_model = carModel;
        aux_model = glm::translate(carModel, rightFrontWheelPos);
        aux_model = glm::rotate(aux_model, glm::radians(wheelsAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        aux_model = glm::translate(aux_model, -rightFrontWheelPos);
        glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(aux_model));
       


        rightFrontWheel.Draw(shader);


        glEnable(GL_CULL_FACE);


    }
    
    
}

void drawObjects(gps::Shader shader, bool depthPass)
{
    glCheckError();
    //renderTeapot(depthPass ? shader : lightingShader, depthPass);  
    renderField(depthPass ? shader : lightingShader, depthPass); 
    renderTennisBall(depthPass ? shader : lightingShader, depthPass);  
    renderTrees(depthPass ? shader : lightingShader, depthPass);   
    renderCar(depthPass ? shader : lightingShader, depthPass, car);   
    renderCarWheels(depthPass ? shader : lightingShader, depthPass);
    renderCar(depthPass ? shader : lightingShader, depthPass, windowsCar); 

    if (!depthPass)
    {
        renderOneTree(depthPass ? shader : treeShader, depthPass);
        renderSineWaves(basicWaveShader, depthPass);
        renderSkybox(skyboxShader); 
        renderRainInstanced(myBasicShader); 
    }
}

GLuint initTexture(const char* file_name) {

    int x, y, n;
    int force_channels = 4;
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // NPOT check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(
            stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
        );
    }

    int width_in_bytes = x * 4;
    unsigned char* top = NULL;
    unsigned char* bottom = NULL;
    unsigned char temp = 0;
    int half_height = y / 2;

    for (int row = 0; row < half_height; row++) {
        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, //GL_SRGB,//GL_RGBA,
        x,
        y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // rasterizarea in harta de adancime
    depthMapShader.useShaderProgram(); 
    depthMapShader.modelLoc = glGetUniformLocation(depthMapShader.shaderProgram, "model");
    depthMapShader.lightSpaceTrMatrixLoc = glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix");
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); 
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO); 
    glClear(GL_DEPTH_BUFFER_BIT); 

    drawObjects(depthMapShader, true); 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // rasterizarea cu umbre a scenei

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawObjects(depthMapShader, false);
    }

}

void initScenePresentation() {
    cameraPositions.push_back(glm::vec3(93.4104f, 5.59219f, -11.7013f));
    cameraPositions.push_back(glm::vec3(68.17f, 4.16532f, -59.7414f));
    cameraPositions.push_back(glm::vec3(15.5062f, 4.50301f, -55.5183f));
    cameraPositions.push_back(glm::vec3(-37.1633f, 4.99623f, -41.6824f));
    cameraPositions.push_back(glm::vec3(-60.6109f, 3.68978f, +2.37258f));
    cameraPositions.push_back(glm::vec3(-23.6122f, 3.11611f, +32.1608f));
    cameraPositions.push_back(glm::vec3(-33.5871f, 4.56704f, +152.858f));
    
    cameraTargets.push_back(glm::vec3(93.7707f, 4.09839f, -47.9606f));
    cameraTargets.push_back(glm::vec3(24.9304f, 4.84612f, -76.2596f));
    cameraTargets.push_back(glm::vec3(-20.6801f, 2.24363f, -64.2955f));
    cameraTargets.push_back(glm::vec3(-77.8485f, 6.70789f, 13.7462f));
    cameraTargets.push_back(glm::vec3(-58.2658f, 4.65272f, 33.2536f));
    cameraTargets.push_back(glm::vec3(-19.9138f, 3.64029f, 82.5839));
    cameraTargets.push_back(glm::vec3(7.63788f, 3.94673f, -29.4615f));
    
}

void scenePresentation()
{
    glm::vec3 point;

    cameraAnimationT += 0.001;

    if (cameraAnimationT <= 1)
    {
        // for camera positions
        point = gps::getBezierPoint(cameraPositions, cameraAnimationT, cameraPositions.size());
        myCamera.setCameraPosition(point);

        //for camera target
        point = gps::getBezierPoint(cameraTargets, cameraAnimationT, cameraPositions.size());
        myCamera.setCameraTarget(point);
    }

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initSineWavesVBOs();
    gridTexture = initTexture("textures/water_texture.jpg");
	initModels();
	initShaders();

    initUniformsForSineWaves(basicWaveShader); 
	initUniformsRainShader(myBasicShader);
    initLightUniformsForShader(lightingShader);
    initLightUniformsForShader(treeShader);
    initSkybox();
    initRaindrops();
    initFBO();
    setupRaindropsAttributes();
    initScenePresentation();
    setWindowCallbacks();

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        float distance = euclideanDistance(myCamera.getCameraPosition(), carCamera.getCameraPosition());
        if (distance < 10.0f) {
            canDrive = true;
        }
        else
            canDrive = false;

        if (cameraAnimation)
        {
            scenePresentation();
        }

	    renderScene();
        deltaTime += 1;
        simTime += 0.007f;

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
