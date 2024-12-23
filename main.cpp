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

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 directionalLightDir;
glm::vec3 directionalLightColor;

// positional lights parameters
glm::vec3 positionalLightDir1 = glm::vec3(8.0f, -7.0f, 7.0f); //  red
glm::vec3 positionalLightDir2 = glm::vec3(8.0f, 7.0f, 7.0f); // orange
glm::vec3 positionalLightDir3 = glm::vec3(8.0f, -7.0f, -7.0f); // purple
glm::vec3 positionalLightDir4 = glm::vec3(8.0f, 7.0f, -7.0f);  // blue
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
    glm::vec3(0.0f, 1.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.05f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D scene;
gps::Model3D tennis_ball;
GLfloat angle;

// shaders
gps::Shader myBasicShader;

// pentru miscarea mouse-ului
float speed = 0.01; //viteza deplasare
float pitch, yaw;
float prevX = 400, prevY = 300;
float sensitivity = 0.01f;

bool firstMouseMove = true;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// light shaders
gps::Shader lightingShader;

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
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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

        printf("Mouse: %lf, %lf\n", yaw, pitch);
        myCamera.rotate(pitch, yaw);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_L]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    }

    if (pressedKeys[GLFW_KEY_F]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/teapot/teapot20segUT.obj");
    scene.LoadModel("models/Scena_Copac_2/Scena_2.obj");
    tennis_ball.LoadModel("models/Minge_tenis_obj/Minge_tenis_final.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    lightingShader.loadShader("shaders/lightingShader.vert", "shaders/lightingShader.frag");
}

void initUniforms(gps::Shader myBasicShader) {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 400.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
    directionalLightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    directionalLightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

	//set light color
    directionalLightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(directionalLightColor));

}

void initLightUniformsForShader(gps::Shader shader) {
    shader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 400.0f);
    projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    directionalLightDir = glm::vec3(0.0f, 10.0f, 10.0f);
    directionalLightDirLoc = glGetUniformLocation(shader.shaderProgram, "dirLightDir");
    // send light dir to shader
    glUniform3fv(directionalLightDirLoc, 1, glm::value_ptr(directionalLightDir));

    //set light color
    directionalLightColor = glm::vec3(1.0f, 1, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(directionalLightColor));


    // send positional lights directions
    // 1
    positionalLightDirLoc1 = glGetUniformLocation(shader.shaderProgram, "posLightDir1");
    glUniform3fv(positionalLightDirLoc1, 1, glm::value_ptr(positionalLightDir1));
    // 2
    positionalLightDirLoc2 = glGetUniformLocation(shader.shaderProgram, "posLightDir2");
    glUniform3fv(positionalLightDirLoc2, 1, glm::value_ptr(positionalLightDir2));
    // 3
    positionalLightDirLoc3 = glGetUniformLocation(shader.shaderProgram, "posLightDir3");
    glUniform3fv(positionalLightDirLoc3, 1, glm::value_ptr(positionalLightDir3));
    // 4
    positionalLightDirLoc4 = glGetUniformLocation(shader.shaderProgram, "posLightDir4");
    glUniform3fv(positionalLightDirLoc4, 1, glm::value_ptr(positionalLightDir4));
    //set light color
    // 1
    positionalLightColor1 = glm::vec3(1.0f, 0.0f, 0.0f); // red
    positionalLightColorLoc1 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor1"); 
    // send light color to shader
    glUniform3fv(positionalLightColorLoc1, 1, glm::value_ptr(positionalLightColor1));
    //2
    positionalLightColor2 = glm::vec3(0.0f, 0.0f, 1.0f);//glm::vec3(0.9f, 0.6f, 0.2f);  // orange
    positionalLightColorLoc2 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor2"); 
    // send light color to shader
    glUniform3fv(positionalLightColorLoc2, 1, glm::value_ptr(positionalLightColor2));
    // 3
    positionalLightColor3 = glm::vec3(1.0f, 0.0f, 0.0f);//glm::vec3(0.7f, 0.4f, 1.0f);  //purple
    positionalLightColorLoc3 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor3");   
    // send light color to shader
    glUniform3fv(positionalLightColorLoc3, 1, glm::value_ptr(positionalLightColor3));
    // 4
    positionalLightColor4 = glm::vec3(0.0f, 0.0f, 1.0f);//glm::vec3(0.9f, 0.6f, 0.2f);//glm::vec3(0.0f, 0.0f, 1.0f); // blue
    positionalLightColorLoc4 = glGetUniformLocation(shader.shaderProgram, "positionalLightColor4");
    // send light color to shader
    glUniform3fv(positionalLightColorLoc4, 1, glm::value_ptr(positionalLightColor4));
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
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}

void renderField(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    scene.Draw(shader);
}

void renderTennisBall(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    tennis_ball.Draw(shader);

}

void renderSkybox(gps::Shader shader) {
    shader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene

	// render the teapot
	renderTeapot(myBasicShader);
    renderField(lightingShader); 
    renderTennisBall(lightingShader);
    renderSkybox(skyboxShader);

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
	initModels();
	initShaders();
	initUniforms(myBasicShader);
    initLightUniformsForShader(lightingShader);
    initSkybox();
    setWindowCallbacks();

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
