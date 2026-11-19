/*============================================================================
Contents   :  [shader_manager.h]

Author     : Chin Qing You
LastUpdate : 2026/11/19
-----------------------------------------------------------------------------

============================================================================*/
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

enum ShaderKind
{
	kShader_3D,            
	kShader_Primitive3D,  
	kShader_Billboard,    
	kShader_2D,           
	kShader_Max,
};

bool ShaderManager_Initialize();
void ShaderManager_Finalize();
void ShaderManager_Bind(ShaderKind kind);   // VS + input layout + PS

#endif 