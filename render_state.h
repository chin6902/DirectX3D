/*============================================================================
Contents   :  [render_state.h]

Author     : Chin Qing You
LastUpdate : 2026/11/17
-----------------------------------------------------------------------------

============================================================================*/
#ifndef RENDER_STATE_H
#define RENDER_STATE_H

enum DepthMode
{
	kDepth_Off,        // no test, no write: 2D sprites, debug lines on top
	kDepth_On,         // test + write: normal 3D objects
	kDepth_ReadOnly,   // test, no write: transparent things, particles
	kDepth_Max,
};

enum BlendMode
{
	kBlend_Opaque,     // no blending: solid 3D objects
	kBlend_Alpha,      // src.rgb * src.a + dst.rgb * (1 - src.a)
	kBlend_Additive,   // src.rgb * src.a + dst.rgb : glow, fire, light effects
	kBlend_Max,
};

enum CullMode
{
	kCull_None,        // no culling: double-sided, 2D sprites
	kCull_Back,        // back-face culling: default for 3D objects
	kCull_Front,       // front-face culling: for special effects
	kCull_Max,
};

bool RenderState_Initialize();
void RenderState_Finalize();

void RenderState_SetDepth(DepthMode mode);
void RenderState_SetBlend(BlendMode mode);
void RenderState_SetCull(CullMode mode);

void RenderState_Set3D();   // depth on, cull back, opaque
void RenderState_Set2D();   // depth off, cull none, alpha blend

#endif // RENDER_STATE_H