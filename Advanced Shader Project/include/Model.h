#pragma once

#define MODEL_H

#include <string>
#include <vector>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model
{
public:
	/*  Functions   */
	vector<Texture> v_textures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> v_meshes;
	std::vector<glm::vec3> v_vertices;
	string directory;
	bool gammaCorrection;
	Model(string path, bool gamma);
	void Draw(Shader shader);
	void Draw(Shader shader, unsigned int p_uiAmount);
	vector<Mesh> getMesh();
	void setUpInstancing();
private:
	/*  Functions   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		string typeName);
};