//=============================================================================
//
// ���C�g���� [light.h]
// Author : ����z��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
BOOL GetFogEnable(void);

LIGHT *GetLightData(int index);

// �V���h�E�i��������j
void SetLightProjectViewMatrix(void);
// �V���h�E�i�����܂Łj
XMFLOAT3 Normalize(XMFLOAT3 v);
//void UpdateLightMatrix(void);
//XMMATRIX GetLightViewMatrix();
//XMMATRIX GetLightProjectionMatrix();

