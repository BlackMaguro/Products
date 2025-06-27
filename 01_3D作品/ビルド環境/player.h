//=============================================================================
//
// プレイヤー関係の処理 [player.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include "character.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int	MAX_PLAYER		(1);			// プレイヤーの数
static const int	PLAYER_MAX_HP	(100);			// プレイヤーの最大HP
static const float 	PLAYER_SIZE		(5.0f);			// 当たり判定の大きさ
static const int	MUTEKI_TIME_MAX (300);			// 無敵時間の最大数

// @brief プレイヤーのパーツを管理する
enum PlayerPartsIndex
{
	// プレイヤーのパーツ（bodyはg_Playerで設定）
	PLAYER_HEAD = 0,	// 頭（head）
	PLAYER_SHOULDER_L,	// 左肩（shoulder_Left）
	PLAYER_SHOULDER_R,	// 右肩（shoulder_Right）
	PLAYER_ARM_L,		// 左腕（arm_Left）
	PLAYER_ARM_R,		// 右腕（arm_Right）
	PLAYER_LEG_L,		// 左足（lef_Left）
	PLAYER_LEG_R,		// 右足（lef_Right）
	// 武器
	PLAYER_GUN,			// 銃（Gun）		
	PLAYER_WOODENSWORD,	// 木刀（WoodenSword）
	PLAYER_BOMB,		// 爆弾（Bomb）

	PLAYER_PARTS_COUNT
};
// @brief プレイヤーの動きテーブルを管理する
enum PlayerMoveTableIndex
{
	// 待機
	MT_STAY_HEAD = 0,       // stay_tbl_head
	MT_STAY_SHOULDER_L,     // stay_tbl_l_shoulder
	MT_STAY_SHOULDER_R,     // stay_tbl_r_shoulder
	MT_STAY_ARM_L,          // stay_tbl_l_arm
	MT_STAY_ARM_R,          // stay_tbl_r_arm
	MT_STAY_LEG_L,          // stay_tbl_l_leg
	MT_STAY_LEG_R,          // stay_tbl_r_leg
	MT_STAY_WEAPON_GUN,     // stay_tbl_weapon_gun
	// 攻撃（銃）
	MT_ATTACK_WEAPON_GUN,		// move_tbl_weapon_gun_move
	MT_ATTACK_GUN_ARM_R,        // move_tbl_r_arm_move
	MT_ATTACK_GUN_SHOULDER_R,   // move_tbl_r_shoulder_move
	// 歩きモーション
	MT_WALK_HEAD,           // move_tbl_head
	MT_WALK_SHOULDER_L,     // move_tbl_l_shoulder
	MT_WALK_SHOULDER_R,     // move_tbl_r_shoulder
	MT_WALK_ARM_L,          // move_tbl_l_arm
	MT_WALK_ARM_R,          // move_tbl_r_arm
	MT_WALK_LEG_L,          // move_tbl_l_leg
	MT_WALK_LEG_R,          // move_tbl_r_leg
	// 待機（木刀）
	MT_STAY_WEAPON_WOODENSWORD,			// stay_tbl_weapon_woodenSword
	// 攻撃（木刀）
	MT_ATTACK_WEAPON_WOODENSWORD,		// move_tbl_weapon_woodenSword_move
	MT_ATTACK_WOODENSWORD_ARM_R,        // move_tbl_r_arm_move
	MT_ATTACK_WOODENSWORD_SHOULDER_R,	// move_tbl_r_shoulder_move

	MT_STAY_WEAPON_BOMB,				// stay_tbl_weapon_bomb
	// 攻撃（爆弾）
	MT_ATTACK_WEAPON_BOMB,				// move_tbl_weapon_bomb
	MT_ATTACK_BOMB_ARM_R,				// move_tbl_r_arm_bomb
	MT_ATTACK_BOMB_SHOULDER_R,			// move_tbl_r_shoulder_bomb

	MT_MAX
};

// @brief プレイヤーの状態を管理する
enum PlayerStateIndex
{
	PLAYER_STAY = 0,				// 待機状態
	PLAYER_WALK,					// 歩行状態
	PLAYER_ATTACK_GUN,				// 攻撃アクション（銃）
	PLAYER_ATTACK_WOODENSWORD,		// 攻撃アクション（木刀）
	PLAYER_ATTACK_BOMB,				// 攻撃アクション（爆弾）
	PLAYER_MOVE_MAX
};
// @brief プレイヤーの武器を管理する
enum PlayerWeaponIndex
{
	PLAYER_WEAPON_GUN = 0,		// 銃
	PLAYER_WEAPON_WOODENSWORD,	// 木刀
	PLAYER_WEAPON_BOMB,			// 爆弾

	PLAYER_WEAPON_MAX			// 武器の最大数
};

//*****************************************************************************
// クラス定義
//*****************************************************************************
class PLAYER : public CHARACTER
{
private:
	int					hp;				// プレイヤーのHP
public:
	// ステータス
	int					motion;			// プレイヤーのモーション
	int					weaponType;		// 武器の種類

	// 動き
	float				dir;			// プレイヤーの向き
	float				jumpY;			// ジャンプの高さ
	float				jumpYMax;		// ジャンプの最大値
	int					jumpCnt;		// ジャンプ中のカウント

	// フラグ
	BOOL				jump;			// ジャンプフラグ
	BOOL				muteki;			// 無敵のフラグ
	BOOL				showFallPoint;	// 投げる準備中か
	BOOL				moveFlg;		// 移動フラグ

	// その他
	int					mutekiTime;		// 無敵の時間
	XMFLOAT3			fallPoint;		// 落下地点
	PLAYER				*parent;		// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	// HP用のインターフェース
	int GetPlayerHP(void) const;			// HPを取得する
	void SetPlayerHP(int hp);				// HPを取得する

	// @brief プレイヤーにダメージを与える。
	// @param[in] damage ダメージ量（0 以上）。呼び出し後、HPが0未満になる場合は0を格納する。
	void TakePlayerDamage(int damage);		// ダメージを与えるメソッド
public:

	// @brief 投擲爆弾の着地点を予測する
	// @return 予測落下地点のワールド座標
	XMFLOAT3 PredictBombLanding() const;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// @brief プレイヤーのリソースを初期化する。
// @return S_OK  成功 ／ E_FAIL 失敗
HRESULT InitPlayer(void);

// @brief プレイヤーのリソースを解放する。
void UninitPlayer(void);

// @brief 毎フレームプレイヤーの更新処理
void UpdatePlayer(void);

// @brief プレイヤーの描画処理
void DrawPlayer(void);

// @brief プレイヤーインスタンスへのポインタを取得する。
// @return 有効な PLAYER*（NULL は返さない）
PLAYER* GetPlayer(void);

// @brief 無敵時間の経過処理を行い、フラグを管理する
void UpdateInvincibility(void);

// @brief 爆弾の投擲マーカーの表示／非表示を更新する。
void UpdateThrowMarkerInPlayer(void);

// @brief 移動＆アクション入力をまとめて処理する。
void UpdateProcessMovementAndActionInput(void);

// @brief カメラの視点変更と更新を行う
// @param prevPos 前の座標
void UpdateApplyMovement(const XMFLOAT3& prevPos);

// @brief 衝突判定と地形へのY座標補正をまとめて行う。
// @param prevPos 前の座標
void UpdateResolveCollisions(const XMFLOAT3& prevPos);

// @brief 武器発射・投擲処理をまとめて行う
void UpdateWeaponActions(void);

// @brief 1パーツ分の階層アニメーションを補間で更新する。
// @param tbl  補間データへのポインタ
// @param part 対象のパーツ（g_Parts）
void UpdateSinglePart(INTERPOLATION_DATA const* tbl, PLAYER& part);

// @brief g_Parts全体の階層アニメーションを更新する。
void UpdatePlayerPartsAnimation(void);

// @brief プレイヤーの姿勢を地面法線からslerpで更新する。
void UpdateOrientation(void);

// @brief 任意モデルに無敵点滅エフェクトを適用する。
// @param model       描画モデル情報
// @param diffuseCols サブセットごとの通常色配列
// @param subsetNum   サブセット数
void DrawApplyInvincibilityToModel(DX11_MODEL& model, const XMFLOAT4 diffuseCols[], int subsetNum);

// @brief フィールド外に出たらoldPosに位置を戻す。
// @param oldPos 前の座標
void checkPlayerOverField(const XMFLOAT3& oldPos);

// @brief プレイヤーのパーツ配列を初期化する。
// @param pNum パーツ数（PLAYER_PARTS_MAX 以下）
void PlayerPartsInit(int pNum);

// @brief プレイヤーのY軸の回転角を取得する。
// @return ラジアン
float GetPlayerRoty(void);

// @brief プレイヤーのY軸の回転角を設定する。
// @param roty ラジアン
void SetPlayerRoty(float roty);

// @brief パーツが描画可能かチェックする。
// @param partsIndex パーツインデックス
// @return TRUE: 描画する／FALSE: スキップ
BOOL ShowDrawParts(int partsIndex);

// @brief PLAYERのワールド行列を作る。
void BuildWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot, XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate);

// @brief パーツ用のワールド行列を作る（XM_PIのオフセットなし）
void BuildPartsWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot, XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate);

