//=============================================================================
//
// フィールド処理 [field.h]
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
#define FIELD_WIDTH					(8000)			// 通常ステージ
#define FIELD_TUTORIAL_WIDTH		(2000)			// 通常ステージ
#define FIELD_BOSS_WIDTH			(960)			// ボスステージ
#define FIELD_HEIGHT				(640)			// フィールドの縦幅
#define MAPCHIP_WIDTH				(64)			// マップチップの横幅
#define MAPCHIP_HEIGHT				(64)			// マップチップの縦幅

#define SCREEN_SIZE_MAPCHIP_WIDTH			(SCREEN_WIDTH/MAPCHIP_WIDTH)	// スクリーンに対する横幅のチップ数（値はチップ数）


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class FIELD
{
public:
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号
	int			fieldNo;	// 使用しているフィールドの番号

	float		scrl;		// スクロール
	XMFLOAT3	old_pos;	// １フレ前のポリゴンの座標
	BOOL	goalFlg;		// ゴールにいるかのフラグ

};

enum
{
	FIELD_LABEL_NONE,
	FIELD_LABEL_BLOCK,

	FIELD_LABEL_MAX,

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);
void CheckPlayerToChipNum(float y, float x);
char GetField(int y, int x);
int CheckField(int y, int x);

FIELD* GetField(void);



