#ifndef Model3D_hpp
#define Model3D_hpp

#include "Mesh.hpp"

#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>

namespace gps {

    class Model3D {

    public:
        ~Model3D();

		void LoadModel(std::string fileName);

		void LoadModel(std::string fileName, std::string basePath);

		void Draw(gps::Shader shaderProgram);

		// for simulating rain
		void configureInstancedArray(std::vector<glm::vec2> positions, int nrInstances);
		void DrawRain(gps::Shader shaderProgram,int nrInstances);

    private:
		// Component meshes - group of objects
        std::vector<gps::Mesh> meshes;
		// Associated textures
        std::vector<gps::Texture> loadedTextures;

		// Does the parsing of the .obj file and fills in the data structure
		void ReadOBJ(std::string fileName, std::string basePath);

		// Retrieves a texture associated with the object - by its name and type
		gps::Texture LoadTexture(std::string path, std::string type);

		// Reads the pixel data from an image file and loads it into the video memory
		GLuint ReadTextureFromFile(const char* file_name);

		GLuint vbo_instanced;
    };
}

#endif /* Model3D_hpp */
