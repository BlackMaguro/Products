//=============================================================================
//
// ライト処理 [light.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
BOOL GetFogEnable(void);

LIGHT *GetLightData(int index);

// シャドウ（ここから）
void SetLightProjectViewMatrix(void);
// シャドウ（ここまで）
XMFLOAT3 Normalize(XMFLOAT3 v);
//void UpdateLightMatrix(void);
//XMMATRIX GetLightViewMatrix();
//XMMATRIX GetLightProjectionMatrix();

