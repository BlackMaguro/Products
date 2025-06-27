//=============================================================================
//
// エネミー関係の処理 [enemy.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include "character.h"
#include "behaviorTree.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int	 MAX_ENEMY				(11);			// エネミーの数
static const int	 ENEMY_MAX_HP			(30);			// エネミーのHP
static const int	 ENEMY_ATK				(15);			// エネミーの攻撃力
static const float	 ENEMY_VIGILANCE_LENGTH	(250.0f);		// エネミーが警戒する距離
static const float	 ENEMY_NOTICE_LENGTH	(200.0f);		// エネミーが発見する距離
static const float	 ENEMY_SIZE				(5.0f);			// 当たり判定の大きさ
static const float   ENEMY_VIGILANCE_LENGTH_SQ = ENEMY_VIGILANCE_LENGTH * ENEMY_VIGILANCE_LENGTH; // 警戒する距離を二乗

// @brief エネミーの種類
enum EnemyType
{
	ENEMY_TYPE_KNIFEMAN = 0,		// ナイフで攻撃するエネミー
	ENEMY_TYPE_BULLETMAN,			// 銃で攻撃してくるエネミー（TODO：実装予定）
	
	ENEMY_TYPE_MAX,					// エネミーの最大種類
};

// @brief エネミーの状態
enum EnemyState
{
	STATE_IDLE = 0,		// 待機状態
	STATE_WANDER,		// 歩き回る
	STATE_CHASE,		// 追いかける

	STATE_MAX,			// 状態の最大数
};

// @brief エネミーのパーツ
enum EnemyPartsIndex
{
	// エネミーのパーツ（bodyはg_Playerで設定）
	ENEMY_HEAD = 0,		// 頭（head）
	ENEMY_ARM_L,		// 左腕（arm_Left）
	ENEMY_ARM_R,		// 右腕（arm_Right）
	ENEMY_LEG_L,		// 左足（lef_Left）
	ENEMY_LEG_R,		// 右足（lef_Right）
	// 武器
	ENEMY_KNIFE,		// ナイフ（Knife）		

	ENEMY_PARTS_COUNT	// パーツの最大数
};
// @brief エネミーの動きテーブル
enum EnemyMoveTableIndex
{
	// 待機
	ENEMY_MT_HEAD = 0,       // move_tbl_enemy_head
	ENEMY_MT_ARM_L,          // move_tbl_l_enemy_arm
	ENEMY_MT_ARM_R,          // move_tbl_r_enemy_arm
	ENEMY_MT_LEG_L,          // move_tbl_l_enemy_leg
	ENEMY_MT_LEG_R,          // move_tbl_r_enemy_leg
	// 攻撃（ナイフ）
	ENEMY_MT_ATTACK_WEAPON_GUN,	// move_tbl_weapon_knife

	ENEMY_MT_MAX			 // テーブルの最大数
};

//*****************************************************************************
// クラス定義
//*****************************************************************************
class ENEMY : public CHARACTER
{
public:

	// フラグ
	BOOL				deadFlg;			// 死亡するかを確認するフラグ
	BOOL				moveFlg;			// 動くか確認するフラグ
	BOOL				noticeFlg;			// 気づいているかのフラグ
	BOOL				prevNoticeFlg;		// 1つ前に気づいているかのフラグ
	BOOL				isRotLock;			// 向きを固定するためのフラグ
	BOOL				isIdleInitialized;  // Idleの初期化フラグ
	BOOL				isWanderInitialized;// Wanderの初期化フラグ

	// 描画用データ
	XMFLOAT4			D_Diffuse;			// モデルのディゾルブ
	float				dissolveTimer;		// ディゾルブの時間（消す）
	float				caledTimer;			// ディゾルブの時間（出現）
	XMFLOAT3			flyDir;				// 飛散方向ベクトル
	float				flySpeed;			// 飛散速度
	XMFLOAT3			rotVel;				// 回転速度（ラジアン/秒）

	// 動き
	XMFLOAT3			movePoint;				// 移動先のポイント
	int					movePointChangeCount;	// 向き先を変えるカウント
	float				moveTimer;				// エネミーの向き変更タイマー

	// ステータス
	int					hp;					// エネミーの体力
	int					atk;				// エネミーの攻撃力
	int					type;				// エネミーの種類（0:ナイフで攻撃する、1:銃で攻撃する人）
	int					state;				// エネミーの状態
	
	// アドレス
	BTNode				*behaviorTree;		// ビヘイビアツリーのアドレス
	ENEMY				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

};
// @brief エネミーパーツのアニメーションの構造体
struct EnemyPartDesc 
{
	int							partIndex;			// ENEMY_HEAD, ENEMY_ARM_Lなど
	const char					*modelId;			// MODEL_ENEMY_HEAD, MODEL_ENEMY_LARMなど
	INTERPOLATION_DATA const	*animTbl;			// アニメーションテーブル
	size_t						animTblSize;		// sizeof(...) / sizeof(...)
	int							parentPart;			// 親パーツがあればENEMY_ARM_Lなど
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static const int	ENEMY_PARTS_MAX(6);					// エネミーのパーツの数
extern ENEMY g_EnemyParts[MAX_ENEMY][ENEMY_PARTS_MAX];	// プレイヤーのパーツ用
extern BOOL	 g_IsSwordDebrisActive;						// プレイヤーが木刀で攻撃したことを示すフラグ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
/**
 * @brief エネミーのリソースを初期化する。
 * @note InitGame() の中で一度だけ呼び出す。
 * @return S_OK 成功 ／ E_FAIL 初期化失敗
 */
HRESULT InitEnemy(void);

/**
 * @brief エネミーのリソースを解放する。
 * @note UninitGame() の中で一度だけ呼び出す。
 */
void UninitEnemy(void);

/**
 * @brief 毎フレーム、全エネミーを更新する。
 * @note UpdateGame() の中でDrawEnemyの前に呼び出す。
 */
void UpdateEnemy(void);

/**
 * @brief 毎フレーム、全エネミーを描画する。
 * @note DrawGame() の中でUpdateEnemyの後に呼び出す。
 */
void DrawEnemy(void);

/**
 * @brief 現在アクティブなエネミーの数を取得する。
 * @return 0以上のエネミー数
 */
int GetEnemyCount(void);

/**
 * @brief アクティブなエネミー数を設定する。
 * @param[in] enemyCount 0以上のエネミー数
 */
void SetEnemyCount(int enemyCount);

/**
 * @brief エネミー配列へのポインタを取得する。
 * @return 有効な ENEMY*配列へのポインタ
 */
ENEMY* GetEnemy(void);

/**
 * @brief エネミー本体とパーツを初期化する。
 * @param[in] eNum 初期化するエネミー数（MAX_ENEMY以下）
 */
void EnemyPartsInit(int eNum);

/**
 * @brief エネミーをアイドル状態にする処理
 * @param[in,out] enemy 処理対象のエネミー
 */
void EnemyIdle(ENEMY& enemy);

/**
 * @brief エネミーをランダムに徘徊させる処理
 * @param[in,out] enemy 処理対象のエネミー
 */
void EnemyWander(ENEMY &enemy);

/**
 * @brief プレイヤーを追跡する処理
 * @param[in,out] enemy 処理対象のエネミー
 */
void EnemyChase(ENEMY& enemy);

/**
 * @brief エネミー死亡時にモデルをバラバラにする処理
 * @param[in] idx 生成対象となるエネミーのインデックス
 */
void SpawnEnemyDebris(int idx);

/**
 * @brief 毎フレーム、全デブリの物理／描画を更新する。
 * @param[in] dt フレーム間隔（秒）
 */
void UpdateEnemyDebris(float dt);

/**
 * @brief 各パーツの物理更新と表示フラグのリセットをまとめて行う
 *
 *  既存のUpdateEnemyDebris(float)を呼び出し、
 *  全パーツが消滅したら g_IsSwordDebrisActiveをFALSEに戻す
 */
void UpdateEnemyDebris(void);

/**
 * @brief 死亡フラグが立っているエネミーのディゾルブを開始させ、
 *        所定の閾値を超えたら本体・パーツを非表示にする
 * @note UpdateEnemy() の中で毎フレーム呼び出す
 */
void UpdateEnemyDissolve(int idx);

/**
 * @brief enemy同士が重ならないにする処理
 * @note UpdateEnemy() の中で毎フレーム呼び出す
 */
void UpdateEnemyPreventOverlap(int idx);

/**
 * @brief パーツ階層アニメーションを補間で更新する。
 * @param[in,out] part 処理対象のパーツ（ENEMY配列の各要素）
 */
void UpdateEnemyPartAnimation(ENEMY& part);

/**
 * @brief エネミーごとの移動タイマーを更新する処理
 * @param[in,out] enemy 処理対象のエネミー
 */
void EnemyMoveTimerUpdate(ENEMY& enemy);

/**
 * @brief 残存中のパーツがあるかを判定する。
 * @return TRUE: 1つ以上アクティブなパーツあり
 * @return FALSE: すべて消滅
 */
BOOL IsAnyDebrisActive(void);

/**
 * @brief 指定エネミーのnoticeFlgを取得する。
 * @param[in] enemy 判定対象のエネミー
 * @return TRUE: プレイヤーを発見中
 * @return FALSE: 発見していない
 */
BOOL GetEnemyNoticeFlg(ENEMY& enemy);

/**
 * @brief 全エネミーの noticeFlg が一つでもTRUEかをチェックする。
 * @return TRUE: いずれかのエネミーが発見状態
 * @return FALSE: 全エネミーが未発見状態
 */
BOOL IsAnyEnemyNotice();

// @brief ENEMYのワールド行列を作る。
void EnemyBuildWorldTransforms(const ENEMY& enemy,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,	XMMATRIX& mtxTranslate);


// FSMでのエネミー制御（今は未使用）
//void CheckEnemyState(int enemyIndex, ENEMY& enemy);
//void UpdateEnemyState(ENEMY& enemy);