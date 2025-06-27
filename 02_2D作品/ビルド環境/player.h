//=============================================================================
//
// プレイヤー処理 [player.h]
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
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(16)	// 16分身

enum
{
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,
	CHAR_DIR_RIGHT,
	CHAR_DIR_UP,


	CHAR_DIR_MAX
};
// 辺の情報
enum
{
	EDGE_LEFT,
	EDGE_RIGHT,
	EDGE_TOP,
	EDGE_BOTTOM,

	EDGE_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

class PLAYER
{
public:

	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号

	int			hp;				// プレイヤーのHP
	
	int			dir;			// 向き（0:上 1:右 2:下 3:左）
	BOOL		moving;			// 移動中フラグ
	BOOL		tutorialMove;   // チュートリアル中の移動フラグ


	BOOL		dash;			// ダッシュ中フラグ
	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	float		jumpYMax;		// 
	XMINT2		vector;			// 移動量ベクトル
	float		colPos;			// 衝突したときの座標
	int			colDir;			// 衝突したときの向き（0:左 1:右 2:上 3:下）

	BOOL		muteki;			// 無敵のフラグ
	int			mutekiTime;		// 無敵の時間
	int			healItem;		// 回復するためのアイテム

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

int GetPlayerCount(void);
BOOL CollisionPlayerToMapchip(XMFLOAT3 vector, int i);
BOOL GetBossFlg(void);
void SetBossFlg(BOOL flg);
void SetCollisionPos(int x, int y, int i, int colDir);


