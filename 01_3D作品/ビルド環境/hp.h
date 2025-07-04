//=============================================================================
//
// 体力処理 [hp.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 列挙型
//*****************************************************************************
enum UITextureID
{
    UI_HP_GAUGE = 0,            // 体力ゲージ
    UI_DAMAGE_RED_BG,           // ダメージ時の赤い背景
    UI_ICON_GUN,                // アイコン：銃
    UI_ENEMY_COUNT,             // エネミーの残り数テクスチャ
    UI_NUMBER,                  // 数字のテクスチャ
    UI_ICON_BG_BLACK,           // アイコン：背景（黒）
    UI_ICON_EYE,                // アイコン：目
    UI_P_KEY,                   // UI：Pキー
    UI_INSTRUCTION,             // UI：操作説明
    UI_ICON_WOODSWORD,          // アイコン：木刀
    UI_ICON_BOMB,               // アイコン：爆弾

    UI_TEXTURE_MAX              // UIの最大数
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class UI
{
public:
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitHp(void);
void UninitHp(void);
void UpdateHp(void);
void DrawHp(void);

BOOL GetDamageFlg(void);
void SetDamageFlg(BOOL damageFlg);