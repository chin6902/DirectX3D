/*============================================================================
Contents   :  [player_camera.h]
              
Author     : Chin Qing You
LastUpdate : 2026/10/27
-----------------------------------------------------------------------------

============================================================================*/
#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include <DirectXMath.h>

void PlayerCamera_Initialize(const DirectX::XMFLOAT3& target_position, float length, float angle_x, float angle_y);
void PlayerCamera_Finalize();
void PlayerCamera_Update(float delta_time);

void PlayerCamera_SetTargetPosition(const DirectX::XMFLOAT3& target_position);
void PlayerCamera_Orbit(float rotation_angle);

DirectX::XMMATRIX PlayerCamera_GetViewMatrix();
DirectX::XMMATRIX PlayerCamera_GetProjectionMatrix();

DirectX::XMVECTOR PlayerCamera_GetFrontVector();
DirectX::XMVECTOR PlayerCamera_GetRightVector();

#endif
