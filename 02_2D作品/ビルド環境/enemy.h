//=============================================================================
//
// エネミー処理 [enemy.h]
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
#define ENEMY_MAX		(18)		// エネミーのMax人数

#define ENEMY_G_MAX_HP	(30)	// エネミーのHP1
#define ENEMY_P_MAX_HP	(50)	// エネミーのHP2
#define ENEMY_R_MAX_HP	(100)	// エネミーのHP3

#define ENEMY_G_MAX_ATK	(10)	// エネミーの攻撃力1
#define ENEMY_P_MAX_ATK	(20)	// エネミーの攻撃力2
#define ENEMY_R_MAX_ATK	(30)	// エネミーの攻撃力3

enum 
{
	ENEMY_TYPE_G,		// エネミータイプ（緑）
	ENEMY_TYPE_P,		// エネミータイプ（紫）
	ENEMY_TYPE_R,		// エネミータイプ（赤）
	ENEMY_TYPE_GB,		// エネミータイプ（緑玉）
	ENEMY_TYPE_PB,		// エネミータイプ（紫玉）
	ENEMY_TYPE_RB,		// エネミータイプ（赤玉）
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

class ENEMY
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
	float		colPos;			// 衝突したときの座標
	int			colDir;			// 衝突したときの向き（0:左 1:右 2:上 3:下）

	float		time;			// 線形補間用
	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	int			hp;				// エネミーのHP
	int			atk;			// エネミーの攻撃力
	int			type;			// エネミーの種類（0:緑、1:紫、2:赤、3:緑玉、4:紫玉、5:赤玉）
	BOOL		ballFlg;		// エネミーが玉の状態か

	//INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	//int					tbl_size;			// 登録したテーブルのレコード総数
	//float				move_time;			// 実行時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

int GetEnemyCount(void);
BOOL CollisionEnemyToMapchip(XMFLOAT3 vector, int i);
void SetCollisionEnemyPos(int x, int y, int i, int colDir);



