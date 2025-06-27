//=============================================================================
//
// 弾発射処理 [bomb.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include "weapon.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int	MAX_BOMB	= 3;		// 爆弾最大数
static const float	GRAVITY		= 9.8f;		// 重力
static const float	BOMB_SIZE	= 5.0f;		// 当たり判定の大きさ
static const float	BOMB_LAUNCH_UP_ANGLE = XM_PI / 10;		// 爆弾を投げる角度
static const float	BOMB_LAUNCH_SPEED = 100.0f;				// 爆弾の初速度

//*****************************************************************************
// クラス定義
//*****************************************************************************
class BOMB : public WEAPON
{
public:
	XMFLOAT3	vel;				// 速度ベクトル
	XMFLOAT3    startPos;			// 発射時の座標
	XMFLOAT3    fallPoint;			// 落下地点
	float       timer;				// 経過時間（秒）
	float       elevation;			// 仰角（ラジアン）
	float       explosionRadius;	// 爆発する半径
	BOOL		isApex;				// 頂点に到達したか
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBomb(void);
void UninitBomb(void);
void UpdateBomb(void);
void DrawBomb(void);

int SetBomb(XMFLOAT3 pos, XMFLOAT3 rot);
int GetBombNum(void);
void Explode(int index);
BOOL checkBombToEnemy(int index);
BOMB *GetBomb(void);

