/*============================================================================
Contents   :  [game_object.h]

Author     : Chin Qing You
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <DirectXMath.h>

static constexpr int OBJECT_MAX = 128;
static constexpr int OBJECT_NAME_MAX = 32;

struct GameObject
{
	bool                active;
	char                name[OBJECT_NAME_MAX];
	DirectX::XMFLOAT3   position;
	DirectX::XMFLOAT3   rotation;   // radians
	DirectX::XMFLOAT3   scale;
};

void GameObject_Initialize();

// Returns the slot index, or -1 when the pool is full
int  GameObject_Spawn(const char* name, const DirectX::XMFLOAT3& position);
void GameObject_Destroy(int index);

void GameObject_Draw();

// nullptr when the index is out of range or the slot is inactive.
// The panel edits the object through this pointer.
GameObject* GameObject_Get(int index);

DirectX::XMMATRIX GameObject_GetWorldMatrix(const GameObject& object);

#ifdef _DEBUG
void GameObject_DrawDebugUI();
#endif

#endif