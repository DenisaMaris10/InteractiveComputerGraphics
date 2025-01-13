//
//  Shader.hpp
//  Lab3
//
//  Created by CGIS on 05/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#if defined (__APPLE__)
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>


namespace gps {
    
    class Shader {

    public:
        GLuint shaderProgram;
        GLint modelLoc = -1, viewLoc = -1, projectionLoc = -1, normalMatrixLoc = -1, directionalLightDirLoc = -1, positionalLightLoc1 = -1, positionalLightLoc2 = -1, positionalLightLoc3 = -1, positionalLightLoc4 = -1;
        GLint dirLightColorLoc = -1, positionalLightColorLoc1 = -1, positionalLightColorLoc2 = -1, positionalLightColorLoc3 = -1, positionalLightColorLoc4 = -1;
        GLint cameraPosLoc = -1, timeLoc = -1, gridSizeLoc = -1, gridDimensionsLoc = -1, gridTextureLoc = -1;
        GLint shadowMapLoc = -1, lightSpaceTrMatrixLoc = -1;
        GLint fogLoc = -1;
        GLint spotLightPosLoc = -1, spotLightColorLoc = -1, spotLightInnerCutOffLoc = -1, spotLightOuterCutOffLoc = -1, spotLightDirectionLoc = -1, lightTypeLoc = -1;
        void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void useShaderProgram();

    
    private:
        std::string readShaderFile(std::string fileName);
        void shaderCompileLog(GLuint shaderId);
        void shaderLinkLog(GLuint shaderProgramId);
    };
    
}

#endif /* Shader_hpp */
