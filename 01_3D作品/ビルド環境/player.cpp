//=============================================================================
//
// プレイヤー関係の処理 [player.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "fade.h"
#include "player.h"
#include "hp.h"
#include "enemy.h"
#include "throwMarker.h"
#include "collision.h"
#include "meshobj.h"
#include "sound.h"
#include "score.h"
#include "time.h"
#include "shadow.h"
#include "bullet.h"
#include "bomb.h"
#include "game.h"
#include "debugproc.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER				PATH_MODEL_PLAYER "player_body_1.obj"			// 読み込むモデル名

#define	MODEL_PLAYER_HEAD			PATH_MODEL_PLAYER "player_head_1.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_LSHD			PATH_MODEL_PLAYER "player_shoulder_left_1.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RSHD			PATH_MODEL_PLAYER "player_shoulder_right_1.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LARM			PATH_MODEL_PLAYER "player_arm_left_1.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_RARM			PATH_MODEL_PLAYER "player_arm_right_1.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_LLEG			PATH_MODEL_PLAYER "player_leg_left_1.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_RLEG			PATH_MODEL_PLAYER "player_leg_right_1.obj"		// 読み込むモデル名

#define	MODEL_PLAYER_WEAPON_GUN				PATH_MODEL_WEAPON "gun001.obj"				// 読み込むモデル名
#define	MODEL_PLAYER_WEAPON_WOODENSWORD		PATH_MODEL_WEAPON "woodenSword.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_WEAPON_BOMB			PATH_MODEL_WEAPON "bomb001.obj"				// 読み込むモデル名

static const float PLAYER_START_X = -590.0f;			// スタート位置（X）
static const float PLAYER_START_Y =  12.0f;				// スタート位置（X）
static const float PLAYER_START_Z = -500.0f;			// スタート位置（X）

static const float	PLAYER_MAP_TOP		( MAP_H * 0.5 - 10.0f);
static const float	PLAYER_MAP_DOWN		(-MAP_H * 0.5 + 10.0f);
static const float	PLAYER_MAP_LEFT		(-MAP_W * 0.5 + 10.0f);
static const float	PLAYER_MAP_RIGHT	( MAP_W * 0.5 - 10.0f);

static const float	VALUE_MOVE	(1.5f);						// 移動量
#define	VALUE_ROTATE			(D3DX_PI * 0.02f)			// 回転量

static const float PLAYER_OFFSET_Y	(12.0f);				// プレイヤーの足元をあわせる

static const int PLAYER_PARTS_MAX	(10);					// プレイヤーのパーツの数
static const float FRICTION_NUM		(0.7f);					// プレイヤーの摩擦係数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void PlayerMoveWalk(void);
static void PlayerAttackGun(void);
static void PlayerMoveStay(void);
static void AttackReturnStay(void);
static void PlayerAttackWoodenSword(void);
static void PlayerAttackBomb(void);
static void HandlePlayerAttackInput(void);
static bool IsAttackAnimFinished(void);
static void CheckWoodenSwordHit(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー（本体）
static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		roty = 0.0f;

static float		g_Player_AnimTime;

static const float	g_BombPlayerScl = 0.2f;
float				g_BombSCL = g_BombPlayerScl;

static const float kGravity = GRAVITY;						// bomb_重力
static const float kLaunchSpeed = BOMB_LAUNCH_SPEED;		// bomb_初速度
static const float kLaunchElevation = BOMB_LAUNCH_UP_ANGLE;	// bomb_角度

// プレイヤーの階層アニメーションデータ
//=============================================================================
// 待機モーションテーブル
//=============================================================================
// 頭（head）
static INTERPOLATION_DATA stay_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 6.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f),60 },
};
// 左肩（shoulder_Left）
static INTERPOLATION_DATA stay_tbl_l_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// 左腕（arm_Left）
static INTERPOLATION_DATA stay_tbl_l_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// 右肩（shoulder_Right）
static INTERPOLATION_DATA stay_tbl_r_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// 右腕（arm_Right）
static INTERPOLATION_DATA stay_tbl_r_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// 左足（leg_Left）
static INTERPOLATION_DATA stay_tbl_l_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// 右足（leg_Right）
static INTERPOLATION_DATA stay_tbl_r_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};
// 銃（weapon_gun）
static INTERPOLATION_DATA stay_tbl_weapon_gun[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-2.3f, -5.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
	{ XMFLOAT3(-2.3f, -4.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
};
// 木刀（weapon_woodenSword）
static INTERPOLATION_DATA stay_tbl_weapon_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.3f, -3.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
	{ XMFLOAT3(-3.3f, -4.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
};
// 爆弾（weapon_bomb）
static INTERPOLATION_DATA stay_tbl_weapon_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.3f, -3.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(g_BombSCL, g_BombSCL, g_BombSCL), 30},
	{ XMFLOAT3(-3.3f, -4.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(g_BombSCL, g_BombSCL, g_BombSCL), 30},
};
//=============================================================================
// 攻撃（銃）モーションテーブル
//=============================================================================
// 銃の動作（weapon_gun）
static INTERPOLATION_DATA move_tbl_weapon_gun_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	//{ XMFLOAT3(2.3f, -5.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.3f, 0.3f, 0.3f), 15},
};
// 右腕を前にだす（arm_Right_move）
static INTERPOLATION_DATA move_tbl_r_arm_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// 右肩を前にだす（shoulder_Right_move）
static INTERPOLATION_DATA move_tbl_r_shoulder_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// 攻撃（木刀）モーションテーブル
//=============================================================================
// 木刀の動作（weapon_woodenSword）
static INTERPOLATION_DATA move_tbl_weapon_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,5.0f), XMFLOAT3(0.0f, XM_PI / 2, XM_PI / 2),  XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	{ XMFLOAT3(0.0f, 2.0f,5.0f), XMFLOAT3(0.0f, XM_PI / 2, 0.0f),		XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
};
// 右腕を前にだす
static INTERPOLATION_DATA move_tbl_r_arm_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, XM_PI / 2),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// 右肩を前にだす
static INTERPOLATION_DATA move_tbl_r_shoulder_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, XM_PI / 2),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// 攻撃（爆弾）モーションテーブル
//=============================================================================
// 爆弾を投げる動作（weapon_bomb）
static INTERPOLATION_DATA move_tbl_weapon_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.0f, 0.0f, 0.0f), 15},
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.0f, 0.0f, 0.0f), 15},
};
// 右腕を前にだす
static INTERPOLATION_DATA move_tbl_r_arm_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// 右肩を前にだす（shoulder_Right_move）
static INTERPOLATION_DATA move_tbl_r_shoulder_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// 歩きモーションテーブル
//=============================================================================
// 頭（head）
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 8, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 8, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 左肩（shoulder_Left）
static INTERPOLATION_DATA move_tbl_l_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, -1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, -1.3f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 右肩（shoulder_Right）
static INTERPOLATION_DATA move_tbl_r_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 1.3f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 1.3f),		  XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 左腕（arm_Left）
static INTERPOLATION_DATA move_tbl_l_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, -1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, -1.3f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 右腕（arm_Right）
static INTERPOLATION_DATA move_tbl_r_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 1.3f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 左足（leg_Left）
static INTERPOLATION_DATA move_tbl_l_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};
// 右足（leg_Right）
static INTERPOLATION_DATA move_tbl_r_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};

static char* MODEL_PARTS[PLAYER_PARTS_MAX] =
{
	MODEL_PLAYER_HEAD,
	MODEL_PLAYER_LSHD,
	MODEL_PLAYER_RSHD,
	MODEL_PLAYER_LARM,
	MODEL_PLAYER_RARM,
	MODEL_PLAYER_LLEG,
	MODEL_PLAYER_RLEG,
	MODEL_PLAYER_WEAPON_GUN,
	MODEL_PLAYER_WEAPON_WOODENSWORD,
	MODEL_PLAYER_WEAPON_BOMB,
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	// 待機モーション
	stay_tbl_head,				// 0
	stay_tbl_l_shoulder,		// 1
	stay_tbl_r_shoulder,		// 2
	stay_tbl_l_arm,				// 3
	stay_tbl_r_arm,				// 4
	stay_tbl_l_leg,				// 5
	stay_tbl_r_leg,				// 6
	stay_tbl_weapon_gun,		// 7

	move_tbl_weapon_gun_move,	// 8
	move_tbl_r_arm_move,		// 9
	move_tbl_r_shoulder_move,	// 10

	// 歩きモーション
	move_tbl_head,				// 11
	move_tbl_l_shoulder,		// 12
	move_tbl_r_shoulder,		// 13
	move_tbl_l_arm,				// 14
	move_tbl_r_arm,				// 15
	move_tbl_l_leg,				// 16
	move_tbl_r_leg,				// 17

	// 木刀
	stay_tbl_weapon_woodenSword,		// 18
	move_tbl_weapon_woodenSword,		// 19
	move_tbl_r_arm_woodenSword,			// 20
	move_tbl_r_shoulder_woodenSword,	// 21

	// 爆弾
	stay_tbl_weapon_bomb,			// 22
	move_tbl_weapon_bomb,			// 23
	move_tbl_r_arm_bomb,			// 24
	move_tbl_r_shoulder_bomb,		// 25

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player_AnimTime = 0.0f;

	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	ZeroMemory(&g_Player.material, sizeof(g_Player.material));
	g_Player.material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_Player.pos = XMFLOAT3(PLAYER_START_X, PLAYER_START_Y, PLAYER_START_Z);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.SetPlayerHP(PLAYER_MAX_HP);
	g_Player.motion = PLAYER_STAY;				// 待機モーションをセット
	g_Player.weaponType = PLAYER_WEAPON_GUN;	// 武器の種類
	g_Player.muteki = FALSE;					// 無敵状態のフラグ
	g_Player.mutekiTime = 0;					// 無敵時間
	g_Player.spd = 0.0f;						// 移動スピードクリア
	g_Player.time = 0.0f;						// プレイヤーの時間

	g_Player.showFallPoint = FALSE;
	g_Player.fallPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Player.use = TRUE;			// TRUE:生きてる
	g_Player.moveFlg = FALSE;
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	GetModelDiffuse(&g_Player.model, &g_Player.diffuse[0]);

	// キーを押した時のプレイヤーの向き
	roty = 0.0f;

	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる


	// 階層アニメーションの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// 体

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Parts[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = FALSE;

		// パーツのセットを行う
		PlayerPartsInit(i);

	}

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		ZeroMemory(&g_Parts[i].material, sizeof(g_Parts[i].material));
		g_Parts[i].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		GetModelDiffuse(&g_Parts[i].model, &g_Parts[i].diffuse[0]);
	}

	// クォータニオンの初期化
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();
	g_Player_AnimTime += 1.0f;
	XMFLOAT3 prevPos = g_Player.pos;

	// 無敵状態なら無敵時間を計測する
	UpdateInvincibility();

	// ボムを構えている状態（武器がBOMBで攻撃中でないとき）
	UpdateThrowMarkerInPlayer();

	// モーションタイプの確認、武器変更、playerの移動処理
	UpdateProcessMovementAndActionInput();

	// カメラ視点によって動かし方を変更する
	UpdateApplyMovement(prevPos);

	// playerとの各種当たり判定処理
	UpdateResolveCollisions(prevPos);
	 
	// 武器（銃と爆弾）の発射処理
	UpdateWeaponActions();

	// 階層アニメーションの処理
	UpdatePlayerPartsAnimation();

	// 姿勢制御の処理
	UpdateOrientation();

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif

#ifdef _DEBUG
	// デバッグ表示
	//PrintDebugProc("Player X:%f Y:%f Z:%f N:%f\nroty:%f\n,g_Player_AnimTime：%f\n,g_Player.weaponType：%d\n", 
	//				g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y, roty, g_Player_AnimTime, g_Player.weaponType);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	//=============================================================================
	// モデルの描画
	//=============================================================================
	{
		// 無敵時間中なら点滅させる
		DrawApplyInvincibilityToModel(g_Player.model, g_Player.diffuse, g_Player.model.SubsetNum);

		// ワールドマトリックスの組み立て（ボディ）
		BuildWorldTransforms(g_Player, mtxWorld, mtxScl, mtxRot, quatMatrix, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

		// リムライトの設定
		SetRimLight(0);		// オフ

		// モデル描画
		DrawModel(&g_Player.model);

		//=============================================================================
		// パーツの描画
		//=============================================================================
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// 無敵状態なら色を変更する
			DrawApplyInvincibilityToModel(g_Parts[i].model, g_Parts[i].diffuse, g_Parts[i].model.SubsetNum);

			// ワールドマトリックスの組み立て（パーツ）
			BuildPartsWorldTransforms(g_Parts[i], mtxWorld, mtxScl, mtxRot, quatMatrix, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
			{
				// g_Player.mtxWorldを指している
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// 使われているなら処理する
			if (g_Parts[i].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// 武器の描画を分ける
			if(!ShowDrawParts(i)) continue;

			// モデル描画
			DrawModel(&g_Parts[i].model);
		}

		SetRimLight(0);
	}
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// プレイヤーの向き情報を取得（アクセサ関数）
//=============================================================================
float GetPlayerRoty(void)
{
	return roty;
}
void SetPlayerRoty(float newRoty)
{
	roty = newRoty;
}

//=============================================================================
// プレイヤーのHPを取得する（アクセサ関数）
//=============================================================================
int PLAYER::GetPlayerHP(void) const
{
	return g_Player.hp;
}
void PLAYER::SetPlayerHP(int hp)
{
	g_Player.hp = hp;
}

//=============================================================================
// プレイヤーにダメージを与えるメソッド
// @param damage：ダメージ量
// note：playerにダメージを与えるときはこのメソッドを必ず呼ぶ
// note： 最大値以上の値の場合、PLAYER_MAX_HPを格納する
//=============================================================================
void PLAYER::TakePlayerDamage(int damage)
{
	if (damage >= PLAYER_MAX_HP) damage = PLAYER_MAX_HP;
	SetPlayerHP(g_Player.hp - damage);
}

//=============================================================================
// 武器のアニメーションを管理する
//=============================================================================
namespace
{
	struct WeaponAnimDef
	{
		int		partIndex;		// パーツインデックス
		int		attackTblNo;	// 攻撃テーブルNo.
		float	attackDuration;	// 攻撃時間
		int		stayTblNo;		// 待機テーブルNo.
		int		stayTblMax;		// 待機テーブルの最大値
	};

	static const WeaponAnimDef g_WeaponDefs[PLAYER_WEAPON_MAX] =
	{
		// 銃
		{
			PLAYER_GUN,
			MT_ATTACK_WEAPON_GUN,
			30.0f,
			MT_STAY_WEAPON_GUN,
			sizeof(stay_tbl_weapon_gun) / sizeof(INTERPOLATION_DATA)
		},
		// 木刀
		{
			PLAYER_WOODENSWORD,
			MT_ATTACK_WEAPON_WOODENSWORD,
			30.0f,
			MT_STAY_WEAPON_WOODENSWORD,
			sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA)
		},
		// 爆弾
		{
			PLAYER_BOMB,
			MT_ATTACK_WEAPON_BOMB,
			30.0f,
			MT_STAY_WEAPON_BOMB,
			sizeof(stay_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA)
		},
	};
}

//=============================================================================
// プレイヤーにパーツをセットするメソッド
// @param pNum ：パーツ番号
// note：nullは許容していない
//=============================================================================
void PlayerPartsInit(int pNum)
{
	// パーツの読み込み
	for (int pNum = 0; pNum < PLAYER_PARTS_MAX; pNum++)
	{
		g_Parts[pNum].use = TRUE;
		g_Parts[pNum].load = TRUE;
		g_Parts[pNum].parent = &g_Player;	// 親をセット
		g_Parts[pNum].tblNo = pNum;			// 再生するアニメデータの先頭アドレスをセット

		switch (pNum)
		{
		case PLAYER_HEAD:// 頭（HEAD）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_SHOULDER_L:// 左肩（shoulder_Left）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_SHOULDER_R:// 右肩（shoulder_Right）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_ARM_L:// 左腕（arm_Left）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_ARM_R:// 右腕（arm_Right）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_LEG_L:// 左足（leg_Left）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_LEG_R:// 右足（right_Left）
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_GUN:// 銃（Gun）
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;		// 親をセット
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_gun) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_WOODENSWORD:// 木刀（woodenSword）
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;
			g_Parts[pNum].tblNo = MT_STAY_WEAPON_WOODENSWORD;
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		case PLAYER_BOMB:// 爆弾（bomb）
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;
			g_Parts[pNum].tblNo = MT_STAY_WEAPON_BOMB;
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
			break;
		default:
			break;
		}
		LoadModel(MODEL_PARTS[pNum], &g_Parts[pNum].model);
	}
}

//=============================================================================
// プレイヤーの攻撃モーションを管理するメソッド
//=============================================================================
static void HandlePlayerAttackInput(void)
{
	// 攻撃トリガーは共通
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
	{
		switch (g_Player.weaponType)
		{
		case PLAYER_WEAPON_GUN:
			PlayerAttackGun();
			break;
		case PLAYER_WEAPON_WOODENSWORD:
			PlaySound(SOUND_LABEL_SE_09_WOODENSWORD);
			PlayerAttackWoodenSword();
			CheckWoodenSwordHit();
			break;
		case PLAYER_WEAPON_BOMB:
			PlayerAttackBomb();
			break;
		default:
			// TODO：他の武器が増えたらここに追加
			break;
		}
	}
}

//=============================================================================
// プレイヤーの待機モーションを実行するメソッド
//=============================================================================
static void PlayerMoveStay(void)
{
	g_Player.motion = PLAYER_STAY;
	// 頭（head）
	g_Parts[PLAYER_HEAD].tblNo = MT_STAY_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_HEAD].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 左肩（shoulder_Left）
	g_Parts[PLAYER_SHOULDER_L].tblNo = MT_STAY_SHOULDER_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_L].tblMax = sizeof(move_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 左腕（arm_Left）
	g_Parts[PLAYER_ARM_L].tblNo = MT_STAY_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_L].tblMax = sizeof(move_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 右腕（arm_Right）
	g_Parts[PLAYER_ARM_R].tblNo = MT_STAY_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右肩（shoulder_Right）
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_STAY_SHOULDER_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 左足（lef_Left）
	g_Parts[PLAYER_LEG_L].tblNo = MT_STAY_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_LEG_L].tblMax = sizeof(move_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右足（lef_Right）
	g_Parts[PLAYER_LEG_R].tblNo = MT_STAY_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_LEG_R].tblMax = sizeof(move_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
}

//=============================================================================
// プレイヤーの歩行モーションを実行するメソッド
//=============================================================================
static void PlayerMoveWalk(void)
{
	// 動きのアニメーションに変更
	g_Player.motion = PLAYER_WALK;
	// 頭（head）
	g_Parts[PLAYER_HEAD].tblNo = MT_WALK_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_HEAD].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 左肩（shoulder_Left）
	g_Parts[PLAYER_SHOULDER_L].tblNo = MT_WALK_SHOULDER_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_L].tblMax = sizeof(move_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右肩（shoulder_Right）
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_WALK_SHOULDER_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 左腕（arm_Left）
	g_Parts[PLAYER_ARM_L].tblNo = MT_WALK_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_L].tblMax = sizeof(move_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右腕（arm_Right）
	g_Parts[PLAYER_ARM_R].tblNo = MT_WALK_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 左足（lef_Left）
	g_Parts[PLAYER_LEG_L].tblNo = MT_WALK_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_LEG_L].tblMax = sizeof(move_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右足（lef_Right）
	g_Parts[PLAYER_LEG_R].tblNo = MT_WALK_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_LEG_R].tblMax = sizeof(move_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
}

//=============================================================================
// プレイヤーの攻撃モーション（銃）を実行するメソッド
//=============================================================================
static void PlayerAttackGun(void)
{
	// 銃を撃つのモーションに変更
	g_Player.motion = PLAYER_ATTACK_GUN;
	// 銃を撃つモーションに変更
	g_Player_AnimTime = 0.0f;

	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_GUN_SHOULDER_R;										// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_move) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右腕の動作
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_GUN_ARM_R;													// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_move) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	// 銃の動作
	g_Parts[PLAYER_GUN].tblNo = MT_ATTACK_WEAPON_GUN;													// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_GUN].tblMax = sizeof(move_tbl_weapon_gun_move) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
}
//=============================================================================
// プレイヤーの攻撃モーション（木刀）を実行するメソッド
//=============================================================================
static void PlayerAttackWoodenSword(void)
{
	// 木刀で切るモーションに変更
	g_Player.motion = PLAYER_ATTACK_WOODENSWORD;
	// 木刀で切るモーションに変更
	g_Player_AnimTime = 0.0f;
	// 右肩の動作
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_WOODENSWORD_SHOULDER_R;										// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_woodenSword) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右腕の動作
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_WOODENSWORD_ARM_R;												// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_woodenSword) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	// 木刀の動作
	g_Parts[PLAYER_WOODENSWORD].tblNo = MT_ATTACK_WEAPON_WOODENSWORD;											// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_WOODENSWORD].tblMax = sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
}
//=============================================================================
// 攻撃モーション（爆弾）を実行するメソッド
//=============================================================================
static void PlayerAttackBomb(void)
{
	// 爆弾を投げるモーションに変更
	g_Player.motion = PLAYER_ATTACK_BOMB;
	g_Player_AnimTime = 0.0f;

	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_BOMB_SHOULDER_R;											// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_bomb) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	// 右腕の動作
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_BOMB_ARM_R;														// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_bomb) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	// 爆弾の動作
	g_Parts[PLAYER_BOMB].tblNo = MT_ATTACK_WEAPON_BOMB;													// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PLAYER_BOMB].tblMax = sizeof(move_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
}
//=============================================================================
// プレイヤーの攻撃モーションから待機モーションに戻すメソッド
//=============================================================================
static void AttackReturnStay(void)
{
	// 右肩と右腕は共通
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_STAY_SHOULDER_R;
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(stay_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);
	g_Parts[PLAYER_ARM_R].tblNo = MT_STAY_ARM_R;
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(stay_tbl_r_arm) / sizeof(INTERPOLATION_DATA);

	// 武器の種類を取得する
	const WeaponAnimDef& def = g_WeaponDefs[g_Player.weaponType];

	g_Parts[def.partIndex].tblNo = def.stayTblNo;
	g_Parts[def.partIndex].tblMax = def.stayTblMax;

	// プレイヤーの状態を待機に戻す
	g_Player.motion = PLAYER_STAY;
}

//=============================================================================
// 武器の種類によって描画を分ける
// @param partsIndex：パーツのインデックスの値
// note：nullは想定していない
//=============================================================================
BOOL ShowDrawParts(int partsIndex)
{
	switch (partsIndex)
	{
	case PLAYER_GUN:
		return g_Player.weaponType == PLAYER_WEAPON_GUN;
	case PLAYER_WOODENSWORD:
		return g_Player.weaponType == PLAYER_WEAPON_WOODENSWORD;
	case PLAYER_BOMB:
		// 爆弾がゼロだったら描画しない
		if (GetBombNum() <= 0) return FALSE;
		return g_Player.weaponType == PLAYER_WEAPON_BOMB;
	default:
		// 武器以外のパーツは常に描画する
		return TRUE;
	}
}

//=============================================================================
// アニメーションが完了しているか判断する
// note：nullは想定していない
//=============================================================================
static bool IsAttackAnimFinished(void)
{
	const WeaponAnimDef& def = g_WeaponDefs[g_Player.weaponType];

	return (g_Parts[def.partIndex].tblNo == def.attackTblNo) && (g_Player_AnimTime > def.attackDuration);
}

//=============================================================================
// 木刀での当たり判定
//=============================================================================
static void CheckWoodenSwordHit(void)
{
	// 攻撃アニメ中のみ判定
	if (g_Player.motion != PLAYER_ATTACK_WOODENSWORD) return;

	ENEMY *enemy = GetEnemy();

	// プレイヤー位置＋向きベクトル * リーチ で当たり判定点を作る
	float rad = g_Player.rot.y;  // y軸回転角
	XMFLOAT3 forward = { sinf(rad), 0.0f, cosf(rad) };
	XMFLOAT3 forward180 = { -forward.x, 0.0f, -forward.z };
	const float reach = 20.0f;    // 刀の届く距離
	const float swordRadius = 10.0f; // 刀エリアの半径

	XMFLOAT3 hitPos = 
	{
		g_Player.pos.x + forward180.x * reach,
		g_Player.pos.y + 1.0f,
		g_Player.pos.z + forward180.z * reach
	};

	// BCで判定
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		if (!enemy[i].use) continue;

		if (CollisionBC(hitPos, enemy[i].pos, swordRadius, ENEMY_SIZE))
		{
			// 木刀で攻撃したフラグ
			g_IsSwordDebrisActive = TRUE;
			// エネミーをバラバラにする
			SpawnEnemyDebris(i);
			// 当たった場合
			// 敵キャラクターは倒される
			enemy[i].deadFlg = TRUE;
			// スコアを足す
			AddScore(SCORE_POINT);
		}
	}
}

//=============================================================================
// 落下位置の計算
// note：nullは想定していない
//=============================================================================
XMFLOAT3 PLAYER::PredictBombLanding() const
{
	// プレイヤーの現在位置・向き
	XMFLOAT3 player = g_Player.pos;
	float yaw = g_Player.rot.y + XM_PI;
	float elev = kLaunchElevation;

	// 水平ベクトル
	XMVECTOR dirH = XMVectorSet(sinf(yaw), 0, cosf(yaw), 0);
	dirH = XMVector3Normalize(dirH);
	XMFLOAT3 fwd; XMStoreFloat3(&fwd, dirH);

	// 初速ベクトル分解
	float vx = fwd.x * kLaunchSpeed;
	float vz = fwd.z * kLaunchSpeed;
	float vy = sinf(elev) * kLaunchSpeed;

	// 地面高さを固定 10.0f として計算
	float y0 = player.y;
	float yGround = 10.0f;

	// 判別式
	float disc = vy * vy + 2.0f * kGravity * (y0 - yGround);
	if (disc < 0.0f) 
	{
		return { player.x, yGround, player.z };
	}
	// 地面到達時間
	float T = (vy + sqrtf(disc)) / kGravity;

	// 着地点の XZ
	XMFLOAT3 land = 
	{
		player.x + vx * T,
		yGround,
		player.z + vz * T
	};

	constexpr float kLerpFactor = 0.5f;

	land.x = player.x + (land.x - player.x) * kLerpFactor;
	land.z = player.z + (land.z - player.z) * kLerpFactor;

	return land;
}
//=============================================================================
// 無敵時間の管理を行う関数
// note：5秒（300フレーム）経過したらフラグをFALSE
//=============================================================================
void UpdateInvincibility()
{
	if (g_Player.muteki)
	{
		g_Player.mutekiTime += 1;
		// 5秒経過したら無敵フラグをFALSEにする
		if (g_Player.mutekiTime > MUTEKI_TIME_MAX)
		{
			g_Player.muteki = FALSE;
			g_Player.mutekiTime = 0;
		}
	}
}
//=============================================================================
// 投擲マーカーの更新を行う関数
//=============================================================================
void UpdateThrowMarkerInPlayer()
{
	if (g_Player.weaponType == PLAYER_WEAPON_BOMB
		&& g_Player.motion != PLAYER_ATTACK_BOMB)
	{
		g_Player.showFallPoint = TRUE;
		XMFLOAT3 point = g_Player.PredictBombLanding();
		ShowThrowMarker(point);
	}
	else
	{
		HideThrowMarker();
		g_Player.showFallPoint = FALSE;
	}
}
//=============================================================================
// モーションの確認と制御と武器変更を行う関数
//=============================================================================
void UpdateProcessMovementAndActionInput()
{
	// 摩擦係数をかけておく
	g_Player.spd *= FRICTION_NUM;

	// 入力チェック
	const BOOL moveLeft	 = GetKeyboardPress(DIK_LEFT)  || IsButtonPressed(0, BUTTON_LEFT);
	const BOOL moveRight = GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT);
	const BOOL moveUp	 = GetKeyboardPress(DIK_UP)	   || IsButtonPressed(0, BUTTON_UP);
	const BOOL moveDown  = GetKeyboardPress(DIK_DOWN)  || IsButtonPressed(0, BUTTON_DOWN);

	// 向きと移動フラグの計算
	if (moveLeft || moveRight || moveUp || moveDown)
	{
		g_Player.moveFlg = TRUE;
		g_Player.spd = VALUE_MOVE;

		// 8方向の向きを設定
		if (moveLeft && moveUp)			roty = XM_PI * 0.75f;
		else if (moveLeft && moveDown)	roty = XM_PI * 0.25f;
		else if (moveRight && moveUp)	roty = -XM_PI * 0.75f;
		else if (moveRight && moveDown)	roty = -XM_PI * 0.25f;
		else if (moveLeft)				roty = XM_PI * 0.5f;
		else if (moveRight)				roty = -XM_PI * 0.5f;
		else if (moveUp)				roty = XM_PI;
		else							roty = 0.0f;
	}
	else
	{
		g_Player.moveFlg = FALSE;
	}

	// 歩き・待機・攻撃の切り替え
	if (g_Player.moveFlg)
	{
		PlayerMoveWalk();
	}
	else
	{
		HandlePlayerAttackInput();
		if (g_Player.motion	   != PLAYER_ATTACK_GUN
			&& g_Player.motion != PLAYER_ATTACK_WOODENSWORD
			&& g_Player.motion != PLAYER_ATTACK_BOMB)
		{
			PlayerMoveStay();
		}
	}

	// 攻撃完了で待機に戻す
	if (IsAttackAnimFinished())
	{
		AttackReturnStay();
	}

	// 武器切替
	if (g_Player.motion != PLAYER_ATTACK_GUN
		&& g_Player.motion != PLAYER_ATTACK_WOODENSWORD
		&& g_Player.motion != PLAYER_ATTACK_BOMB)
	{
		if (GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_08_CHANGEWEAPON);
			g_Player.weaponType = ((g_Player.weaponType + 1) % PLAYER_WEAPON_MAX);
		}
	}
}
//=============================================================================
// カメラの視点変更と更新を行う関数
//=============================================================================
void UpdateApplyMovement(const XMFLOAT3& prevPos)
{
	// カメラ視点を取得
	CAMERA* cam = GetCamera();

	if (!GetFirstPersonFlg())
	{
		// 3人称視点：roty にカメラ回転を足して向きを決定
		g_Player.rot.y = roty + cam->rot.y;

		// 押した方向に移動
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}
	else
	{
		// 1人称視点：常にカメラ方向
		g_Player.rot.y = cam->rot.y;

		g_Player.pos.x += sinf(roty + cam->rot.y) * g_Player.spd;
		g_Player.pos.z += cosf(roty + cam->rot.y) * g_Player.spd;
	}
}
//=============================================================================
// プレイヤーがフィールド外に出ないようにする関数
// @param prevPos：プレイヤーの前座標
// note：現在はこの実装で行う。今後はRayCastで判定させる（TODO）
//=============================================================================
void checkPlayerOverField(const XMFLOAT3& prevPos)
{
	// マップ外に出たら元の位置に戻す
	if (g_Player.pos.x < PLAYER_MAP_LEFT
		|| g_Player.pos.x > PLAYER_MAP_RIGHT
		|| g_Player.pos.z < PLAYER_MAP_DOWN
		|| g_Player.pos.z > PLAYER_MAP_TOP)
	{
		g_Player.pos = prevPos;
	}
}
//=============================================================================
// プレイヤーとの当たり判定を管理する関数
// @param prevPos：プレイヤーの前座標
// note：Nullは許容していない
//=============================================================================
void UpdateResolveCollisions(const XMFLOAT3& prevPos)
{
	// フィールドの外に出ないようにする処理
	checkPlayerOverField(prevPos);

	// フィールドオブジェクトとの当たり判定
	checkPlayerFieldObj(prevPos);

	// エネミーとあたった場合、すり抜けないように戻す
	checkPlayerToEnemy(prevPos);

	// レイキャストして足元の高さを求める
	XMFLOAT3 hitPosition;		// 交点
	XMFLOAT3 normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	BOOL ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	if (ans)
	{
		g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
		g_Player.UpVector = normal;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		g_Player.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
}
//=============================================================================
// プレイヤー武器の発射・投擲処理を管理する関数
// note：Nullは許容していない
//=============================================================================
void UpdateWeaponActions()
{
	// 共通の発射トリガー判定
	bool trigger = (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
		&& g_Player.motion != PLAYER_WALK;

	// 銃の発射
	if (g_Player.weaponType == PLAYER_WEAPON_GUN && trigger)
	{
		PlaySound(SOUND_LABEL_SE_02_GUNSHOT);

		XMFLOAT3 bulletPos = g_Player.pos;
		bulletPos.y += 1.2f;
		// roty に応じたオフセット
		if (roty > 1.47f && roty < 2.0f) bulletPos.x -= 8.0f;
		if (roty < 0.0f)                  bulletPos.x += 8.0f;

		SetBullet(bulletPos, g_Player.rot);
	}
	// 爆弾の投擲
	else if (g_Player.weaponType == PLAYER_WEAPON_BOMB && GetBombNum() > 0 && trigger)
	{
		PlaySound(SOUND_LABEL_SE_10_THROWBOMB);

		XMFLOAT3 bombPos = g_Player.pos;
		bombPos.y += 1.2f;
		if (roty < 0.0f) bombPos.x += 8.0f;

		SetBomb(bombPos, g_Player.rot);
	}
}
//=============================================================================
// プレイヤーのパーツを制御する関数
//  @param tbl：アニメーションテーブル
//  @param part：playerの構造体
//=============================================================================
void UpdateSinglePart(INTERPOLATION_DATA const* tbl, PLAYER& part)
{
	if (!part.use || part.tblMax <= 0)
		return;

	int   nowNo = static_cast<int>(part.time);
	int   maxNo = part.tblMax;
	int   nextNo = (nowNo + 1) % maxNo;
	float frac = part.time - float(nowNo);  // 0.0～1.0 の補間係数

	// 現在の状態をロードする
	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);
	XMVECTOR nextPos = XMLoadFloat3(&tbl[nextNo].pos);
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);

	// 次の状態をロードする
	XMVECTOR nextRot = XMLoadFloat3(&tbl[nextNo].rot);
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);
	XMVECTOR nextScl = XMLoadFloat3(&tbl[nextNo].scl);

	// Lerp で一発補間
	XMVECTOR interpPos = XMVectorLerp(nowPos, nextPos, frac);
	XMVECTOR interpRot = XMVectorLerp(nowRot, nextRot, frac);
	XMVECTOR interpScl = XMVectorLerp(nowScl, nextScl, frac);

	// 結果を格納する
	XMStoreFloat3(&part.pos, interpPos);
	XMStoreFloat3(&part.rot, interpRot);
	XMStoreFloat3(&part.scl, interpScl);

	// time を次フレーム分だけ進める
	part.time += 1.0f / tbl[nowNo].frame;
	if (part.time >= float(maxNo))
		part.time -= float(maxNo);
}

//=============================================================================
// プレイヤーのパーツを全て更新する関数
//=============================================================================
void UpdatePlayerPartsAnimation()
{
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// tblNoを使って正しいテーブルを取り出す
		int tableIndex = g_Parts[i].tblNo;
		UpdateSinglePart(g_MoveTblAdr[tableIndex], g_Parts[i]);
	}
}
//=============================================================================
// プレイヤーの姿勢制御を行う関数
// note：フィールドを動かす時に使用する
//=============================================================================
void UpdateOrientation()
{
	// UpdateResolveCollisions で UpVector にセット済みの法線を使う
	XMVECTOR upRef = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR surface = XMLoadFloat3(&g_Player.UpVector);

	// 法線と上向きベクトルの外積 → 回転軸
	XMVECTOR axis = XMVector3Cross(upRef, surface);
	float    len = XMVectorGetX(XMVector3Length(axis));

	// len が 0（法線が上向きそのもの）なら軸をupRefにフォールバック
	if (len < 1e-6f) {
		axis = upRef;
	}
	else {
		axis = XMVector3Normalize(axis);
	}

	// 回転角は asin(len) で取得
	float angle = asinf(len);

	// 目標姿勢クォータニオン
	XMVECTOR targetQuat = XMQuaternionRotationNormal(axis, angle);

	// 現在のクォータニオンと slerp で補間
	XMVECTOR currentQuat = XMLoadFloat4(&g_Player.Quaternion);
	XMVECTOR blended = XMQuaternionSlerp(currentQuat, targetQuat, 0.05f);

	XMStoreFloat4(&g_Player.Quaternion, blended);
}
//=============================================================================
// プレイヤーの無敵状態を描画する関数
// DX11_MODEL& model：対象のモデル
// XMFLOAT4 diffuseCols：モデルのカラー
// int subsetNum：点滅時のカラー
// note：
//=============================================================================
void DrawApplyInvincibilityToModel(DX11_MODEL& model,const XMFLOAT4 diffuseCols[], int subsetNum)
{
	// 点滅する感覚
	const int blinkInterval = 1;
	//bool flashOn = (g_Player.muteki && (g_Player.mutekiTime % 2) == 1);
	bool flashOn = (g_Player.muteki && ((g_Player.mutekiTime / blinkInterval) % 2) == 1);

	for (int i = 0; i < subsetNum; i++)
	{
		SetModelDiffuse(&model, i, flashOn ? XMFLOAT4{ 0.7f,0.0f,0.0f,1.0f }
		: diffuseCols[i]);
	}
}
//=============================================================================
// プレイヤーの本体（body）を描画する際のワールド行列を組み立てる関数
// @param mtxWorld：合成後のワールド行列
// @param mtxScl：スケール行列
// @param mtxRot：オイラー回転行列
// @param quatMatrix：クォータニオン回転行列
// @param mtxTranslate：平行移動行列
// note：パーツは別の関数を使って組み立てる
//=============================================================================
void BuildWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld,
	XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,
	XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate
)
{
	// 初期化
	mtxWorld = XMMatrixIdentity();
	// スケール
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// 回転（オイラーからラジアン）
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// クォータニオン
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	// 平行移動
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
}
//=============================================================================
// プレイヤーのパーツを描画する際のワールド行列を組み立てる関数
// @param mtxWorld：合成後のワールド行列
// @param mtxScl：スケール行列
// @param mtxRot：オイラー回転行列
// @param quatMatrix：クォータニオン回転行列
// @param mtxTranslate：平行移動行列
// note：パーツの時にはplayer.rot.yにXM_PIを足さない
//=============================================================================
void BuildPartsWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld,
	XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,
	XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate
)
{
	// 初期化
	mtxWorld = XMMatrixIdentity();
	// スケール
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// 回転（オイラーからラジアン）
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// クォータニオン
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	// 平行移動
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
}