//=============================================================================
//
// ボス処理 [boss.h]
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
#define BOSS_MAX		(6)		// ボスのMax人数
#define BOSS_MAX_HP		(300)	// ボスのMaxHP
#define BOSS_MAX_ATK	(50)	// ボスのMax攻撃力


//*****************************************************************************
// 構造体定義
//*****************************************************************************

class BOSS
{
public:
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度

	float		time;			// 線形補間用
	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	int			hp;				// BOSSのHP
	int			atk;			// BOSSの攻撃力
	BOOL		moveFlg;		// BOSSが動いてよいかのフラグ
	int			patternAtk;		// BOSSが攻撃パターンナンバー

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(void);

BOSS* GetBoss(void);
BOOL GetBossAtkFlg(void);
void SetBossAtkFlg(BOOL bossAtkFlg);

int GetBossCount(void);
int GetBossPatternAtk(void);




