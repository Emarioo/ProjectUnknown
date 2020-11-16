#pragma once

#include "World/Dimension.h"
#include <unordered_map>

#include "AnimData.h"
#include "MeshData.h"
#include "CollData.h"

/*
Handling data
*/



// Dimension
void AddDimension(std::string s, Dimension dim);
Dimension* GetDimension();
void SetDimension(std::string);

// Animation
void AddAnim(std::string name, std::string path);
/*
returns nullptr if name doesn't exist
*/
AnimData* GetAnim(std::string name);

// Mesh
void AddMesh(std::string name, std::string path);
MeshData* GetMesh(std::string name);
glm::vec3 GetMeshPos(std::string name);
glm::vec3 GetMeshRot(std::string name);

// Collider
void AddColl(std::string name, std::string path);
CollData* GetColl(std::string name);