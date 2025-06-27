//=============================================================================
//
// 武器を管理するクラス [weapon.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "sound.h"
#include "score.h"
#include "meshobj.h"
#include "game.h"
#include "collision.h"
#include "meshfield.h"
#include <DirectXMath.h>
#include <vector>
//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// クラス定義
//*****************************************************************************
class WEAPON
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
	float		size;			// サイズ
	BOOL		use;			// 使用しているかどうか


};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
XMFLOAT3 HermiteInterp(const XMFLOAT3& p0, const XMFLOAT3& m0, 
					   const XMFLOAT3& p1, const XMFLOAT3& m1, float t);