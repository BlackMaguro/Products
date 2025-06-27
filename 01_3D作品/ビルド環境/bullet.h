//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET		(6)			// 弾最大数（リボルバー想定）
#define	BULLET_SIZE		(5.0f)		// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class BULLET
{
public:
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	XMFLOAT4	diffuse[MODEL_MAX_MATERIAL];	// モデルの色


	BOOL				load;
	DX11_MODEL			model;	// モデル情報
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		fDepthZ;		// 奥行き
	float		size;			// 奥行き
	BOOL		use;			// 使用しているかどうか


};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BULLET *GetBullet(void);

