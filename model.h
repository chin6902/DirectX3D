/*============================================================================
Contents   :  [model.h]
              
Author     : Chin Qing You
LastUpdate : 2026/10/20
-----------------------------------------------------------------------------

============================================================================*/
#ifndef MODEL_H
#define MODEL_H

#include <unordered_map>

#include <d3d11.h>
#include <DirectXMath.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "collision.h"
#pragma comment (lib, "assimp/assimp-vc143-mt.lib")


struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer = nullptr;
	ID3D11Buffer** IndexBuffer = nullptr;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	CollisionAABB AABB{};
};

MODEL* ModelLoad(const char* FileName, float scale = 1.0f, bool bBlender = false);
void ModelRelease(MODEL* model);

void ModelDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld);


#endif
