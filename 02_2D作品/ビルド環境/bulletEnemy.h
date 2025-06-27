//=============================================================================
//
// エネミーバレット処理 [bulletEnemy.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_ENEMY_MAX		(8)		// エネミーバレットのMax数
#define BULLET_ENEMY_SPEED		(5.0f)			// バレットの移動スピード

// 弾の方向
enum
{
	BULLET_ENEMY_DIR_DOWN,
	BULLET_ENEMY_DIR_LEFT,
	BULLET_ENEMY_DIR_RIGHT,
	BULLET_ENEMY_DIR_UP,

	BULLET_ENEMY_DIR_MAX
};
// バレット構造体
class BULLETENEMY
{
public:
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	int					dir;				// 向き（0:上 1:右 2:下 3:左）
	int					atk;				// 攻撃力

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBulletEnemy(void);
void UninitBulletEnemy(void);
void UpdateBulletEnemy(void);
void DrawBulletEnemy(void);

BULLETENEMY *GetBulletEnemy(void);
void SetBulletEnemy(XMFLOAT3 pos, int bossPatternAtk);
BOOL GetEnemyBulletAllLostFlg(void);
void SetEnemyBulletAllLostFlg(BOOL enemyBulletAllLostFlg);
