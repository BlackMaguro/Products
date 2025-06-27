//=============================================================================
//
// フィールドオブジェクトモデル処理 [enemy.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int MAX_FIELD_OBJ_BUILDING(25);	// フィールドオブジェクトの数（ビル）
static const int MAX_FIELD_OBJ_STREETLIGHT(36);	// フィールドオブジェクトの数（街灯）
static const int MAX_FIELD_OBJ(MAX_FIELD_OBJ_BUILDING + MAX_FIELD_OBJ_STREETLIGHT); // フィールドオブジェクトの総数

static const int  BUILDING_INTERVAL			(200);			// フィールドオブジェクトの数（ビル）
static const int  MAX_BUILDING_HEIGHT_NUM	(4);			// フィールドオブジェクトの数（ビル）
static const int  MAX_BUILDING_WIDTH_NUM	(6);			// フィールドオブジェクトの数（ビル）

static const int  MAX_STREETLIGHT_HEIGHT_NUM	(3);		// フィールドオブジェクトの数（ライト）
static const int  MAX_STREETLIGHT_WIDTH_NUM		(3);		// フィールドオブジェクトの数（ライト）

static const float	FIELD_OBJ_SIZE_BUILDING		(90.0f);	// 当たり判定の大きさ（ビル）
static const float	FIELD_OBJ_SIZE_STREETLIGHT	(10.0f);	// 当たり判定の大きさ（街灯）

//*****************************************************************************
// 列挙型定義
//*****************************************************************************
// @brief オブジェクトの種類
enum OBJ_TYPE
{
	OBJ_TYPE_BUILDING = 0,
	OBJ_TYPE_STREETLIGHT,

	MAX_OBJ_TYPE,
};

//*****************************************************************************
// クラス定義
//*****************************************************************************
class FIELDOBJ
{
public:
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	OBJ_TYPE			type;				// オブジェクトの種類
	XMFLOAT3			collisionSize;		// 当たり判定の大きさ

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFieldObj(void);
void UninitFieldObj(void);
void UpdateFieldObj(void);
void DrawFieldObj(void);

const std::vector<FIELDOBJ>& GetFieldObjBuildings(void);
const std::vector<FIELDOBJ>& GetFieldObjLights(void);
void checkPlayerFieldObj(const XMFLOAT3& playerOldPos);
BOOL checkEnemyFieldObj(const XMFLOAT3& enemyOldPos);
void checkBulletFieldObj(void);
void checkBombFieldObj(void);