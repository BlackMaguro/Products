//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"


// マクロ定義
//#define EFFECT_TEXTURE			_T("data/TEXTURE/bomb.png")	// 画像

#define EFFECT_NUM_PARTS 30		//エフェクト内パーティクル数
#define EFFECT_NUM_EFFECTS 10	//エフェクト最大数


//*****************************************************************************
// 構造体宣言
//*****************************************************************************

class PARTICLE	// パーティクルクラス
{
public:
	XMFLOAT3		pos;						// ポリゴンの移動量
	XMFLOAT3		move;						// 移動量
	int				PatternAnim;				// アニメーションパターンナンバー
	int				CountAnim;					// アニメーションカウント

	int				liveTime;

	bool			isFinish;
};

class EFFECT	// エフェクトクラス
{
public:
	int				use;
	int				isEnding;
	bool			isRemoveOnFinish;

	XMFLOAT3		pos;						// ポリゴンの移動量

	int				duration;
	int				elapsed;
	int				numFinish;

	int				effectCount;
	int				emitCounter;

	PARTICLE		pParticle[EFFECT_NUM_PARTS];

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);
void SetEffect(float x, float y, int duration);
