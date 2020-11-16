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
void AddDimension();
Dimension* GetDimension();
void SetDimension(std::string);

// Animation
void AddAnimation(std::string name, std::string path);
/*
returns nullptr if name doesn't exist
*/
AnimationData* GetAnimation(std::string name);

// Mesh
void AddMesh(std::string name, std::string path);
MeshData* GetMesh(std::string name);
glm::vec3 GetMeshPos(std::string name);
glm::vec3 GetMeshRot(std::string name);

// Collider
void AddCollider(std::string name, std::string path);
CollData* GetCollider(std::string name);