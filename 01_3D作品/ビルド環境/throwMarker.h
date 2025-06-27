//=============================================================================
//
// 物を投げるときのマーカー処理 [throwMarker.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitThrowMarker(void);
void UninitThrowMarker(void);
void UpdateThrowMarker(void);
void DrawThrowMarker(void);

void ShowThrowMarker(const XMFLOAT3& pos);
void HideThrowMarker(void);