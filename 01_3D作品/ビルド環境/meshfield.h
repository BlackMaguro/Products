//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "enemy.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);
BOOL RayHitHorizontal(const ENEMY& enemies, XMFLOAT3* pHitPosition, XMFLOAT3* pNormal);
BOOL GetGroundHeightAndNormal(float x, float z, float& outHeight, XMFLOAT3& outNormal);
