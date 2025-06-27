//=============================================================================
//
// チュートリアル画面処理 [tutorial.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


class TUTORIAL
{
public:
	//BOOL point;		// チュートリアルのポイントに到達したかのフラグ
	//XMFLOAT3	pos;		// ポリゴンの座標
	//float		w, h;		// 幅と高さ
	//int			texNo;		// 使用しているテクスチャ番号
	//float		scrl;		// スクロール
	//XMFLOAT3	old_pos;	// １フレ前のポリゴンの座標
	//float		scrl2;		// スクロール

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);

BOOL GetPLayerToPoint(void);
void SetPLayerToPoint(BOOL PointToPlayer);
void CheckPlayerTutorialPoint(float playerPosX);
int GetTutorialMessageCount(void);
TUTORIAL* GetTutorial(void);