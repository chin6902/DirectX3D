/*============================================================================
Contents   :  [mesh_field.h]

Author     : Chin Qing You
LastUpdate : 2026/10/20
-----------------------------------------------------------------------------

============================================================================*/
#ifndef MESH_FIELD_H
#define MESH_FIELD_H

void MeshField_Initialize(float grid_width, float grid_depth, int h_count, int v_count);
void MeshField_Finalize();
void MeshField_Draw();

#endif
