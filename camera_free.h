/*============================================================================
Contents   :  [camera_free.h]
              
Author     : Chin Qing You
LastUpdate : 2026/09/18
-----------------------------------------------------------------------------

============================================================================*/
#ifndef CAMERA_FREE_H
#define CAMERA_FREE_H

#include <DirectXMath.h>

void CameraFree_Initialize(const DirectX::XMFLOAT3& position, float angle_x, float angle_y);
void CameraFree_Finalize();
void CameraFree_Update(float delta_time);

void CameraFree_DrawDebugUI();

// camera_free.h
DirectX::XMMATRIX CameraFree_GetViewMatrix();
DirectX::XMMATRIX CameraFree_GetProjectionMatrix();

#endif
