//=============================================================================
//
// エネミー関係の処理 [enemy.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "game.h"
#include "enemy.h"
#include "enemyTasks.h"
#include "collision.h"
#include "meshobj.h"
#include "player.h"
#include "sound.h"
#include "shadow.h"
#include "meshfield.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデルの読み込み
#define	MODEL_ENEMY					PATH_MODEL_ENEMY "enemy_body_1.obj"

#define	MODEL_ENEMY_HEAD			PATH_MODEL_ENEMY "enemy_head_1.obj"			
#define	MODEL_ENEMY_LARM			PATH_MODEL_ENEMY "enemy_arm_left_1.obj"		
#define	MODEL_ENEMY_RARM			PATH_MODEL_ENEMY "enemy_arm_right_1.obj"	
#define	MODEL_ENEMY_LLEG			PATH_MODEL_ENEMY "enemy_leg_left_1.obj"		
#define	MODEL_ENEMY_RLEG			PATH_MODEL_ENEMY "enemy_leg_right_1.obj"	
#define	MODEL_ENEMY_WEAPON_KNIFE	PATH_MODEL_WEAPON"knife001.obj"				

static const float 	VALUE_ROTATE (XM_PI * 0.02f);		// 回転量

static const float 	ENEMY_TIME_FRAME (1.0f);			// タイムフレーム数

static const float  ENEMY_OFFSET_Y (7.0f);				// エネミーの足元をあわせる
static const int	ENEMY_WALK_RANGE (200);				// エネミーの移動距離
static const float  ENEMY_PLAYER_DISTANCE(9.0f);		// エネミーとプレイヤーの距離

static const float 	ENEMY_MAP_TOP	(MAP_H * 0.5 - 10.0f);		// マップの端（奥）
static const float 	ENEMY_MAP_DOWN	(-MAP_H * 0.5 + 10.0f);		// マップの端（手前）
static const float 	ENEMY_MAP_LEFT	(-MAP_W * 0.5 + 10.0f);		// マップの端（左）
static const float 	ENEMY_MAP_RIGHT	(MAP_W * 0.5 - 10.0f);		// マップの端（右）

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY		g_Enemy[MAX_ENEMY];							// エネミーの体（body）
ENEMY				g_EnemyParts[MAX_ENEMY][ENEMY_PARTS_MAX];
static int			g_EnemyCount;								// エネミーの数

static DISSOLVE		g_Dissolve;			// ディゾルブを行うため
BOOL g_IsSwordDebrisActive;				// プレイヤーが木刀で攻撃したことを示すフラグ

// エネミーの階層アニメーション
// 頭（head）
static INTERPOLATION_DATA move_tbl_enemy_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 8, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 8, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 左腕（arm_Left）
static INTERPOLATION_DATA move_tbl_l_enemy_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 5.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, -1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(1.5f, 5.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, -1.3f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// 右腕（arm_Right）
static INTERPOLATION_DATA move_tbl_r_enemy_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 5.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 1.3f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(-1.5f, 5.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 1.3f),		  XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};

// 左足（leg_Left）
static INTERPOLATION_DATA move_tbl_l_enemy_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};
// 右足（leg_Right）
static INTERPOLATION_DATA move_tbl_r_enemy_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};

// ナイフ（weapon_knife）_
static INTERPOLATION_DATA move_tbl_weapon_knife[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),	   XMFLOAT3(0.3f, 0.3f, 0.3f), 120},
	{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 120},
};
// モデルのパーツをまとめたテーブル
static char* MODEL_ENEMY_PARTS[ENEMY_PARTS_MAX] =
{
	MODEL_ENEMY_HEAD,
	MODEL_ENEMY_LARM,
	MODEL_ENEMY_RARM,
	MODEL_ENEMY_LLEG,
	MODEL_ENEMY_RLEG,
	MODEL_ENEMY_WEAPON_KNIFE
};
// 各モデルに対応するモーションをまとめたテーブル
static constexpr EnemyPartDesc EnemyPartDescs[] = {
	{ ENEMY_HEAD,  MODEL_ENEMY_HEAD, move_tbl_enemy_head, sizeof(move_tbl_enemy_head) / sizeof(INTERPOLATION_DATA), -1 },
	{ ENEMY_ARM_L, MODEL_ENEMY_LARM, move_tbl_l_enemy_arm, sizeof(move_tbl_l_enemy_arm) / sizeof(INTERPOLATION_DATA), ENEMY_HEAD },
	{ ENEMY_ARM_R, MODEL_ENEMY_RARM, move_tbl_r_enemy_arm, sizeof(move_tbl_r_enemy_arm) / sizeof(INTERPOLATION_DATA), ENEMY_HEAD },
	{ ENEMY_LEG_L, MODEL_ENEMY_LLEG, move_tbl_l_enemy_leg, sizeof(move_tbl_l_enemy_leg) / sizeof(INTERPOLATION_DATA), ENEMY_HEAD },
	{ ENEMY_LEG_R, MODEL_ENEMY_RLEG, move_tbl_r_enemy_leg, sizeof(move_tbl_r_enemy_leg) / sizeof(INTERPOLATION_DATA), ENEMY_HEAD },
	{ ENEMY_KNIFE, MODEL_ENEMY_WEAPON_KNIFE, move_tbl_weapon_knife, sizeof(move_tbl_weapon_knife) / sizeof(INTERPOLATION_DATA), ENEMY_ARM_L },
};
// パーツのモーションテーブルの大きさ
static constexpr size_t NumEnemyPartDescs = std::size(EnemyPartDescs);

// アニメーションテーブル
static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_enemy_head,
	move_tbl_l_enemy_arm,
	move_tbl_r_enemy_arm,
	move_tbl_l_enemy_leg,
	move_tbl_r_enemy_leg,
	move_tbl_weapon_knife,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	g_IsSwordDebrisActive = FALSE;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;					// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;			// 当たり判定の大きさ

		g_Enemy[i].deadFlg = FALSE;				// エネミーの死亡判定
		g_Enemy[i].dissolveTimer = 0.0f;		// エネミーのディゾルブ時間
		g_Enemy[i].caledTimer = 0.0f;			// エネミーのディゾルブ時間
		g_Enemy[i].moveFlg = TRUE;				// エネミーの動きを判定
		g_Enemy[i].hp = ENEMY_MAX_HP;			// エネミーのHP
		g_Enemy[i].atk = ENEMY_ATK;				// エネミーの攻撃力
		g_Enemy[i].type = ENEMY_TYPE_KNIFEMAN;	// エネミーの種類
		g_Enemy[i].state = STATE_IDLE;			// エネミーの状態
		g_Enemy[i].moveTimer = (float)(rand() % 180 + 60);		// エネミーの向きの変更時間
		g_Enemy[i].noticeFlg = FALSE;			// エネミーが気づいているか
		g_Enemy[i].prevNoticeFlg = FALSE;		// エネミーが1つ前に気づいているか
		g_Enemy[i].movePointChangeCount = 1;	// エネミーの動く先を変えるカウント数
		g_Enemy[i].isRotLock = TRUE;			// エネミーの向きをロックするフラグ

		g_EnemyCount = MAX_ENEMY;				// エネミーの数

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		g_Enemy[i].use = TRUE;			// TRUE:生きてる

		g_Enemy[i].D_Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	// ディゾルブカラー

		g_Enemy[i].parent = NULL;		// 本体（親）なのでNULLを入れる

		// ビヘイビアツリーの初期化
		SelectorNode* root = new SelectorNode();

		// 衝突したら即Chase
		SequenceNode* noticeSeq = new SequenceNode();
		noticeSeq->AddChild(new CheckNoticeTask(&g_Enemy[i]));
		noticeSeq->AddChild(new ChaseActionTask(&g_Enemy[i]));
		root->AddChild(noticeSeq);

		// 通常の「距離→視界→Chase」
		SequenceNode* chaseSequence = new SequenceNode();
		chaseSequence->AddChild(new CheckPlayerDistanceTask(&g_Enemy[i], ENEMY_VIGILANCE_LENGTH));
		chaseSequence->AddChild(new CheckPlayerInSightTask(&g_Enemy[i]));
		chaseSequence->AddChild(new ChaseActionTask(&g_Enemy[i]));
		root->AddChild(chaseSequence);

		// それ以外はランダム
		RandomActionTask* randomIdleOrWander = new RandomActionTask(&g_Enemy[i]);
		root->AddChild(randomIdleOrWander);

		g_Enemy[i].behaviorTree = root;

	}


	// 階層アニメーションの初期化
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_EnemyParts[i][j].use = FALSE;
			// 位置・回転・スケールの初期設定
			g_EnemyParts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyParts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyParts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			// 親子関係
			g_EnemyParts[i][j].parent = &g_Enemy[i];		// 体（body）

			// 階層アニメーション用のメンバー変数の初期化
			g_EnemyParts[i][j].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_EnemyParts[i][j].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_EnemyParts[i][j].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
			// パーツの読み込みはまだしていない
			g_EnemyParts[i][j].load = FALSE;
		}
	}
	// bodyにパーツをつける初期化
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		EnemyPartsInit(i);
	}
	// エネミーの位置を設定
	//g_Enemy[0].pos = XMFLOAT3(-600.0f, 7.0f, -500.0f);	// テスト用エネミー
	g_Enemy[0].pos = XMFLOAT3(-520.0f, ENEMY_OFFSET_Y, 550.0f);
	g_Enemy[1].pos = XMFLOAT3( 550.0f, ENEMY_OFFSET_Y, 520.0f);
	g_Enemy[2].pos = XMFLOAT3( 520.0f, ENEMY_OFFSET_Y,    0.0f);
	g_Enemy[3].pos = XMFLOAT3(  90.0f, ENEMY_OFFSET_Y,  300.0f);
	g_Enemy[4].pos = XMFLOAT3( 550.0f, ENEMY_OFFSET_Y, -500.0f);
	g_Enemy[5].pos = XMFLOAT3(  30.0f, ENEMY_OFFSET_Y, -540.0f);
	g_Enemy[6].pos = XMFLOAT3(   0.0f, ENEMY_OFFSET_Y, -200.0f);
	g_Enemy[7].pos = XMFLOAT3(-150.0f, ENEMY_OFFSET_Y,    0.0f);
	g_Enemy[8].pos = XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  150.0f);
	g_Enemy[9].pos = XMFLOAT3( 150.0f, ENEMY_OFFSET_Y,    0.0f);
	g_Enemy[10].pos = XMFLOAT3(530.0f, ENEMY_OFFSET_Y, 600.0f);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].movePoint = g_Enemy[i].pos;		// エネミーの移動先ポイント
	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	// 本体（body）の解放処理
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
		if (g_Enemy[i].behaviorTree) 
		{
			delete g_Enemy[i].behaviorTree;
			g_Enemy[i].behaviorTree = NULL;
		}
	}
	// パーツの解放処理
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if (g_EnemyParts[i][j].load)
			{
				UnloadModel(&g_EnemyParts[i][j].model);
				g_EnemyParts[i][j].load = FALSE;
			}
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)
		{
			// タイマー更新
			g_Enemy[i].moveTimer -= ENEMY_TIME_FRAME;

			XMFLOAT3 enemyOldPos = g_Enemy[i].pos;

			// パーツをバラバラに動かす処理
			UpdateEnemyDebris();

			// 死亡フラグが立っていたらディゾルブを開始させる
			UpdateEnemyDissolve(i);

			// 死亡してたらその後の処理はしない
			if (g_Enemy[i].deadFlg) continue;

			// 階層アニメーション
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				UpdateEnemyPartAnimation(g_EnemyParts[i][j]);
			}
			// FSMによるエネミーの制御
			//{
				//// エネミーの状態を変更
				//UpdateEnemyState(g_Enemy[i]);
				//// エネミーの状態確認
				//CheckEnemyState(i, g_Enemy[i]);
			//}

			// ビヘイビアツリーによるエネミーの制御
			if (g_Enemy[i].behaviorTree)
			{
				g_Enemy[i].behaviorTree->Tick();
			}

			// エネミーがフィールド外に出れないようにする
			if (g_Enemy[i].pos.x < ENEMY_MAP_LEFT
				|| g_Enemy[i].pos.x > ENEMY_MAP_RIGHT
				|| g_Enemy[i].pos.z < ENEMY_MAP_DOWN
				|| g_Enemy[i].pos.z > ENEMY_MAP_TOP)
			{
				g_Enemy[i].pos = enemyOldPos;
			}

			// エネミー同士を重ならない処理
			UpdateEnemyPreventOverlap(i);

			// エネミーが床に沈まないようにする
			if (!g_Enemy[i].deadFlg) g_Enemy->pos.y = ENEMY_OFFSET_Y;

			// プレイヤーが無敵状態でないなら動き出すようにする
			if (player->muteki == FALSE) g_Enemy[i].moveFlg = TRUE;

			// フィールドオブジェクトとの当たり判定
			BOOL ans = checkEnemyFieldObj(g_Enemy[i].pos);
			if (ans)
			{
				g_Enemy[i].pos = enemyOldPos;
			}
		}
	}

#ifdef _DEBUG
	//PrintDebugProc("EnemyCount :%d,\ng_EnemyParts[0][0].rot.y:%f,\n", g_EnemyCount, g_EnemyParts[0][0].rot.y);
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	PrintDebugProc("g_Enemy[%d].dissolveTimer:%f,\n",i, g_Enemy[i].dissolveTimer);
	//	PrintDebugProc("g_Enemy[%d].caledTimer:%f,\n", i,g_Enemy[i].caledTimer);
	//}
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	// ディゾルブの時の色
	g_Dissolve.Diffuse = g_Enemy[0].D_Diffuse;
	SetDissolveDiffuse(&g_Dissolve);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ディゾルブ有効
		SetDissolveBuffer(TRUE, g_Enemy[i].caledTimer);

		// ワールドマトリックスの組み立て（body）
		EnemyBuildWorldTransforms(g_Enemy[i], mtxWorld, mtxScl, mtxRot, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		// リムライトの設定
		SetRimLight(1);

		// モデル描画
		DrawModel(&g_Enemy[i].model);

		// 階層アニメーション
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			const ENEMY& parts = g_EnemyParts[i][j];
			if (!parts.use) continue;
			XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;

			EnemyBuildWorldTransforms(parts, mtxWorld, mtxScl, mtxRot, mtxTranslate);

			if (g_EnemyParts[i][j].parent != NULL)	// 子供だったら親と結合する
			{
				// g_Player.mtxWorldを指している
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_EnemyParts[i][j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_EnemyParts[i][j].mtxWorld, mtxWorld);

			// 使われているなら処理する
			if (g_EnemyParts[i][j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// モデル描画
			DrawModel(&g_EnemyParts[i][j].model);

		}
	}
	// ディゾルブ無効
	SetDissolveBuffer(FALSE, 0.0f);
	// リムライトの無効
	SetRimLight(0);
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// プレイヤーにパーツをセットするメソッド
// int pNum ：パーツ番号
// note：nullは許容していない
//=============================================================================
void EnemyPartsInit(int enemyIndex)
{
	// パーツの読み込み
	for (int partIndex = 0; partIndex < ENEMY_PARTS_MAX; partIndex++)
	{
		// ENEMYとして取り出す
		ENEMY& parts = g_EnemyParts[enemyIndex][partIndex];
		parts.use = TRUE;

		// ナイフ（KNIFE）だけ腕パーツを親に、それ以外は本体（body）を親に
		if (partIndex == ENEMY_KNIFE) {
			parts.parent = &g_EnemyParts[enemyIndex][ENEMY_ARM_L];
		}
		else 
		{
			parts.parent = &g_Enemy[enemyIndex];
		}

		// アニメーション用のインデックス
		parts.tblNo = partIndex;
		// 各パーツに対応するテーブルの最大数をセット
		switch (partIndex)
		{
		case ENEMY_HEAD:
			parts.tblMax = sizeof(move_tbl_enemy_head) / sizeof(INTERPOLATION_DATA);
			break;
		case ENEMY_ARM_L:
			parts.tblMax = sizeof(move_tbl_l_enemy_arm) / sizeof(INTERPOLATION_DATA);
			break;
		case ENEMY_ARM_R:
			parts.tblMax = sizeof(move_tbl_r_enemy_arm) / sizeof(INTERPOLATION_DATA);
			break;
		case ENEMY_LEG_L:
			parts.tblMax = sizeof(move_tbl_l_enemy_leg) / sizeof(INTERPOLATION_DATA);
			break;
		case ENEMY_LEG_R:
			parts.tblMax = sizeof(move_tbl_r_enemy_leg) / sizeof(INTERPOLATION_DATA);
			break;
		case ENEMY_KNIFE:
			parts.tblMax = sizeof(move_tbl_weapon_knife) / sizeof(INTERPOLATION_DATA);
			break;
		default:
				break;
		}

		parts.load = TRUE;
		// モデルの読み込み
		LoadModel(MODEL_ENEMY_PARTS[partIndex], &parts.model);
	}
}
//=============================================================================
// プレイヤーのパーツを階層アニメーションさせるメソッド
// ENEMY& part ：エネミーの構造体を渡す処理
// note：nullは許容していない
//=============================================================================
void UpdateEnemyPartAnimation(ENEMY& part)
{
	if (!part.use || part.tblMax <= 0) return;

	int nowNo = (int)part.time;				// 整数分であるテーブル番号を取り出す
	int maxNo = part.tblMax;				// 登録テーブル数を数える
	int nextNo = (nowNo + 1) % maxNo;		// 移動先テーブルの番号を求める
	INTERPOLATION_DATA const* tbl = g_MoveTblAdr[part.tblNo];	// 行動テーブルのアドレスを取得

	float nowTime = part.time - float(nowNo);

	// 各パラメータの「現在」と「次」をベクトル化
	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);
	XMVECTOR nextPos = XMLoadFloat3(&tbl[nextNo].pos);

	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);
	XMVECTOR nextRot = XMLoadFloat3(&tbl[nextNo].rot);

	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);
	XMVECTOR nextScl = XMLoadFloat3(&tbl[nextNo].scl);

	// Lerpで補間
	XMVECTOR Pos = XMVectorLerp(nowPos, nextPos, nowTime);
	XMVECTOR Rot = XMVectorLerp(nowRot, nextRot, nowTime);
	XMVECTOR Scl = XMVectorLerp(nowScl, nextScl, nowTime);

	XMStoreFloat3(&part.pos, Pos);
	XMStoreFloat3(&part.rot, Rot);
	XMStoreFloat3(&part.scl, Scl);

	// フレームレートでtimeを進める
	part.time += 1.0f / tbl[nowNo].frame;
	if (part.time >= float(maxNo)) part.time -= float(maxNo);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//=============================================================================
// エネミーの数の取得（アクセサ関数）
//=============================================================================
int GetEnemyCount(void)
{
	return g_EnemyCount;
}

void SetEnemyCount(int enemyCount)
{
	g_EnemyCount = enemyCount;
}

//=============================================================================
// エネミーのnoticeFlgの取得（アクセサ関数）
//=============================================================================
BOOL GetEnemyNoticeFlg(ENEMY& enemy)
{
	return enemy.noticeFlg;
}

//=============================================================================
// エネミーのnoticeFlgの確認
//=============================================================================
BOOL IsAnyEnemyNotice()
{
	ENEMY* enemies = GetEnemy(); // 全エネミー取得
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// 使用中かつ気づいてる
		if (enemies[i].use && GetEnemyNoticeFlg(enemies[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
// エネミーのタイマーの更新処理
// @param &enemy：エネミー構造体
//=============================================================================
void EnemyMoveTimerUpdate(ENEMY& enemy)
{
	enemy.moveTimer = (float)(rand() % 180 + 60);
}

//=============================================================================
// エネミーの立ち止まっている状態
// @param &enemy：エネミー構造体
//=============================================================================
void EnemyIdle(ENEMY& enemy)
{
	// 初回のIdle状態突入時のみの初期化
	if (!enemy.isIdleInitialized)
	{
		EnemyMoveTimerUpdate(enemy);			// 待機時間のセット
		enemy.isIdleInitialized = TRUE;
		enemy.isRotLock = TRUE;                 // 向きを完全ロック
	}
	// タイマー終了後は初期化をリセット
	if (enemy.moveTimer <= 0.0f)
	{
		enemy.isIdleInitialized = FALSE;
		enemy.isRotLock = FALSE;
		enemy.movePointChangeCount = 1;
	}
}

//=============================================================================
// エネミーの歩行処理
// @param &enemy：エネミー構造体
//=============================================================================
void EnemyWander(ENEMY& enemy)
{

	// 徘徊状態に入った最初の1回だけmoveTimerをセット
	if (!enemy.isWanderInitialized)
	{
		EnemyMoveTimerUpdate(enemy);
		enemy.isWanderInitialized = TRUE;
		enemy.isRotLock = FALSE; // 動くのでロック解除
	}

	XMFLOAT3 oldPos = enemy.pos;
	// 移動先ポイントの決定
	float movePointX = enemy.movePoint.x;
	float movePointZ = enemy.movePoint.z;
	float speed = 0.5f; // 距離が近いほど速くなる

	// ランダムで移動先の変更
	if (enemy.movePointChangeCount == 1)
	{
		enemy.movePoint.x = (float)(GetRand(-ENEMY_WALK_RANGE, ENEMY_WALK_RANGE) + movePointX);
		enemy.movePoint.z = (float)(GetRand(-ENEMY_WALK_RANGE, ENEMY_WALK_RANGE) + movePointZ);
		// movePointChangeCountを0にしてもう一度変更できないようにする
		enemy.movePointChangeCount = 0;
	}

	// 移動先ポイントが大きすぎ、小さすぎてしまったら
	if (enemy.movePoint.x < ENEMY_MAP_LEFT)   enemy.movePoint.x = ENEMY_MAP_LEFT;
	if (enemy.movePoint.x > ENEMY_MAP_RIGHT)  enemy.movePoint.x = ENEMY_MAP_RIGHT;
	if (enemy.movePoint.z < ENEMY_MAP_DOWN)   enemy.movePoint.z = ENEMY_MAP_DOWN;
	if (enemy.movePoint.z > ENEMY_MAP_TOP)    enemy.movePoint.z = ENEMY_MAP_TOP;

	// 移動方向の決定
	float dx = enemy.movePoint.x - enemy.pos.x;
	float dz = enemy.movePoint.z - enemy.pos.z;

	// 角度を計算
	float rad = atan2f(dx, dz);			// 体の向き
	float radChase = atan2f(dz, dx);	// 追いかける方向

	// 向きがロックされていない場合だけ更新
	if (!enemy.isRotLock)
	{
		enemy.rot.y = rad;
	}
	// 移動を開始する
	enemy.pos.x += cosf(radChase) * speed;
	enemy.pos.z += sinf(radChase) * speed;


	// エネミーがフィールド外に出れないようにする
	if (enemy.pos.x < ENEMY_MAP_LEFT
		|| enemy.pos.x > ENEMY_MAP_RIGHT
		|| enemy.pos.z < ENEMY_MAP_DOWN
		|| enemy.pos.z > ENEMY_MAP_TOP)
	{
		enemy.pos = oldPos;

		if (enemy.pos.x < ENEMY_MAP_LEFT || enemy.pos.x > ENEMY_MAP_RIGHT) enemy.movePoint.x *= -1;	// 左か右端に到達したら
		if (enemy.pos.z < ENEMY_MAP_DOWN || enemy.pos.z > ENEMY_MAP_TOP) enemy.movePoint.z *= -1;	// 奥か手前に到達したら
	}

	// タイマーがゼロになったらランダムに向きを変える
	if (enemy.moveTimer <= 0.0f)
	{
		// カウント数の復活
		enemy.movePointChangeCount = 1;
		enemy.isWanderInitialized = FALSE;

	}
}

//=============================================================================
// エネミーの追いかける状態
// @param &enemy：エネミー構造体
//=============================================================================
void EnemyChase(ENEMY& enemy)
{
	PLAYER* player = GetPlayer();

	// プレイヤーとエネミーの距離を計算
	float dx = player->pos.x - enemy.pos.x;
	float dz = player->pos.z - enemy.pos.z;

	// 2次元距離を計算
	float distance = sqrtf(dx * dx + dz * dz);
	float rad = atan2f(dx, dz);
	float radChase = atan2f(dz, dx);

	// プレイヤーの方向にエネミーを向ける
	// 向きがロックされていない場合だけ更新
	if (!enemy.isRotLock)
	{
		enemy.rot.y = rad;
	}

	// 最小距離を設定してプレイヤーと重ならないようにする
	if((ENEMY_PLAYER_DISTANCE < distance) && (enemy.moveFlg == TRUE))
	{
		float speed = 1.5f + (100.0f - distance) * 0.005f; // 距離が近いほど速くなる

		// プレイヤーに向かって進む
		enemy.pos.x += cosf(radChase) * speed;
		enemy.pos.z += sinf(radChase) * speed;

	}
}
//=============================================================================
// エネミーの体をバラバラにする関数
// idx：エネミーのindex
//=============================================================================
void SpawnEnemyDebris(int idx)
{
	const float MIN_SPEED = 2.0f;
	const float MAX_SPEED = 4.0f;
	// 本体を飛ばす
	{
		ENEMY& enemies = g_Enemy[idx];
		enemies.parent = nullptr;
		// 全方向ランダムベクトル
		float u = rand() / (float)RAND_MAX;
		float v = rand() / (float)RAND_MAX;
		float theta = u * XM_2PI;
		float phi = acosf(2 * v - 1);
		enemies.flyDir = {
			sinf(phi) * cosf(theta),
			cosf(phi),
			sinf(phi) * sinf(theta)
		};
		enemies.flySpeed = MIN_SPEED + (rand() / (float)RAND_MAX) * (MAX_SPEED - MIN_SPEED);
	}

	// パーツを飛散設定
	for (int j = 0; j < ENEMY_PARTS_MAX; j++)
	{
		ENEMY& part = g_EnemyParts[idx][j];
		part.use = TRUE;
		part.parent = nullptr;
		part.pos = g_Enemy[idx].pos;
		// 全方向ランダム
		float u = rand() / (float)RAND_MAX;
		float v = rand() / (float)RAND_MAX;
		float theta = u * XM_2PI;
		float phi = acosf(2 * v - 1);
		part.flyDir = {
			sinf(phi) * cosf(theta),
			cosf(phi),
			sinf(phi) * sinf(theta)
		};
		part.flySpeed = MIN_SPEED + (rand() / (float)RAND_MAX) * (MAX_SPEED - MIN_SPEED);
	}

	ENEMY* all[ENEMY_PARTS_MAX + 1];
	all[0] = &g_Enemy[idx];

	for (int j = 0; j < ENEMY_PARTS_MAX; j++) all[j + 1] = &g_EnemyParts[idx][j];

	for (ENEMY* enemies : all)
	{
		// 全方向ランダム回転速度を−π～+π（rad/s）の範囲で実行
		enemies->rotVel.x = (rand() / (float)RAND_MAX * 2 - 1) * XM_PI;
		enemies->rotVel.y = (rand() / (float)RAND_MAX * 2 - 1) * XM_PI;
		enemies->rotVel.z = (rand() / (float)RAND_MAX * 2 - 1) * XM_PI;
	}
}
//=============================================================================
// エネミーの体をバラバラにするUpdate関数
// @param dt：デルタタイム
// note：引数へnullを渡さない
//=============================================================================
void UpdateEnemyDebris(float dt)
{
	const float MAX_ROTV = XM_PI * 0.25f;

	// 全エネミー本体（body）
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		ENEMY& enemies = g_Enemy[i];
		// deadFlgかつ親なしなら飛散中とみなす
		if (enemies.deadFlg && enemies.parent == nullptr)
		{

			// 位置更新
			enemies.pos.x += enemies.flyDir.x * enemies.flySpeed * dt;
			enemies.pos.y += enemies.flyDir.y * enemies.flySpeed * dt;
			enemies.pos.z += enemies.flyDir.z * enemies.flySpeed * dt;
			// 減速
			enemies.flySpeed *= 1.0f;
			// 回転を加える
			enemies.rot.x += ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
			enemies.rot.y += ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
			enemies.rot.z += ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
		}
	}

	// 全パーツ(bodyを除く)
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		for (int j = 0; j < ENEMY_PARTS_MAX; ++j)
		{
			ENEMY& part = g_EnemyParts[i][j];
			if (!part.use || part.parent) continue;
			
			// 位置更新
			part.pos.x += part.flyDir.x * part.flySpeed * dt;
			part.pos.y += part.flyDir.y * part.flySpeed * dt;
			part.pos.z += part.flyDir.z * part.flySpeed * dt;
			// 減速
			part.flySpeed *= 1.0f;
			// 回転を加える
			part.rotVel.x = ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
			part.rotVel.y = ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
			part.rotVel.z = ((rand() / (float)RAND_MAX) * 2 - 1) * MAX_ROTV;
		}
	}
}
//=============================================================================
// エネミーの体をバラバラにするための関数を呼び出しフラグを管理する関数
// note：引数へnullを渡さない
//=============================================================================
void UpdateEnemyDebris()
{
	// フラグが立っているときだけ物理更新＆表示管理
	if (g_IsSwordDebrisActive)
	{
		// 物理演算パーツ更新
		UpdateEnemyDebris(DELTATIME);

		// 全パーツ消滅時にフラグをリセット
		if (!IsAnyDebrisActive())
		{
			g_IsSwordDebrisActive = FALSE;
		}
	}
}
//=============================================================================
// バラバラに動いているパーツがあるか判定する関数
// note：引数へnullを渡さない
//=============================================================================
BOOL IsAnyDebrisActive()
{
	// 本体（deadFlg＆parent==nullptr）をチェック
	for (int i = 0; i < MAX_ENEMY; ++i) {
		ENEMY& enemiesBody = g_Enemy[i];
		if (enemiesBody.deadFlg && enemiesBody.parent == nullptr) 
			return TRUE;
	}
	// パーツをチェック
	for (int i = 0; i < MAX_ENEMY; ++i) {
		for (int j = 0; j < ENEMY_PARTS_MAX; ++j) {
			ENEMY& enemiesParts = g_EnemyParts[i][j];
			if (enemiesParts.use && enemiesParts.parent == nullptr) 
				return TRUE;
		}
	}
	return FALSE;
}
//=============================================================================
// ディゾルブを実行する関数
// @param idx：エネミーインデックス
// note：deadFlgがTRUEなら実行される
// note：enemyが倒されたときdeadFlgをTRUEにする
//=============================================================================
void UpdateEnemyDissolve(int idx)
{
	ENEMY& enemies = g_Enemy[idx];

	// 死亡フラグが立っていたらディゾルブを開始させる
	if (enemies.deadFlg)
	{
		enemies.dissolveTimer += 0.01f;
		enemies.caledTimer = sinf(enemies.dissolveTimer);

		if (enemies.caledTimer >= 0.6f)
		{
			// 同時に、飛散パーツも消す
			for (int j = 0; j < ENEMY_PARTS_MAX; ++j)
			{
				g_EnemyParts[idx][j].use = FALSE;
			}
			enemies.use = FALSE;
			enemies.deadFlg = FALSE;
		}
	}
}
//=============================================================================
// enemyが重ならないようにするための関数
// @param idx：エネミーインデックス
// @note：
//=============================================================================
void UpdateEnemyPreventOverlap(int idx)
{
	ENEMY& enemies = g_Enemy[idx];

	XMFLOAT3 spaceVec = { 0.0f,0.0f,0.0f };
	int neighborCount = 0;
	

	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (idx == j || !g_Enemy[j].use) continue;

		if (CollisionBC(enemies.pos, g_Enemy[j].pos, ENEMY_SIZE, ENEMY_SIZE))
		{
			// エネミー[idx]と[j]の位置の差ベクトルを計算
			XMFLOAT3 diff;
			diff.x = enemies.pos.x - g_Enemy[j].pos.x;
			diff.y = enemies.pos.y - g_Enemy[j].pos.y;
			diff.z = enemies.pos.z - g_Enemy[j].pos.z;

			// 正規化（ゼロ長さの場合は加算しない）
			float length = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			if (length > 0.001f)
			{
				diff.x /= length;
				diff.y /= length;
				diff.z /= length;

				// 重なっている度合いに応じた重みを掛ける
				spaceVec.x += diff.x;
				spaceVec.y += diff.y;
				spaceVec.z += diff.z;
				neighborCount++;
			}
		}
	}
	// 近接するエネミーがいるなら平均ベクトルを適用して離す
	if (neighborCount > 0)
	{
		spaceVec.x /= neighborCount;
		spaceVec.y /= neighborCount;
		spaceVec.z /= neighborCount;

		// 離れる距離
		float separateStrength = 1.0f;
		enemies.pos.x += spaceVec.x * separateStrength;
		enemies.pos.y += spaceVec.y * separateStrength;
		enemies.pos.z += spaceVec.z * separateStrength;
	}
}
//=============================================================================
// enemyの本体（body）を描画する際のワールド行列を組み立てる関数
// @param mtxWorld：合成後のワールド行列
// @param mtxScl：スケール行列
// @param mtxRot：オイラー回転行列
// @param mtxTranslate：平行移動行列
// note：パーツは別の関数を使って組み立てる
//=============================================================================
void EnemyBuildWorldTransforms(const ENEMY& enemy,
	XMMATRIX& mtxWorld,
	XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,
	XMMATRIX& mtxTranslate
)
{
	// 初期化
	mtxWorld = XMMatrixIdentity();
	// スケール
	mtxScl = XMMatrixScaling(enemy.scl.x, enemy.scl.y, enemy.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// 回転（オイラーからラジアン）
	mtxRot = XMMatrixRotationRollPitchYaw(enemy.rot.x, enemy.rot.y, enemy.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// 平行移動
	mtxTranslate = XMMatrixTranslation(enemy.pos.x, enemy.pos.y, enemy.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
}

//=============================================================================
// エネミーの状態更新（FSMによるエネミーの制御）
// ENEMY &enemy：エネミー構造体
//=============================================================================
//void UpdateEnemyState(ENEMY &enemy)
//{
//	PLAYER* player = GetPlayer();
//	// 1つ前のエネミーが気づいているかのフラグを保存
//	enemy.prevNoticeFlg = enemy.noticeFlg;
//
//	// プレイヤーとエネミーの距離を計算
//	float dx = player->pos.x - enemy.pos.x;
//	float dz = player->pos.z - enemy.pos.z;
//	float distance = sqrtf(dx * dx + dz * dz);
//
//	// 発見した場合
//	if (distance < ENEMY_NOTICE_LENGTH)
//	{
//		// 追いかける処理に変更
//		enemy.noticeFlg = TRUE;
//		enemy.state = STATE_CHASE;
//	}
//	else
//	{
//		// 見失ったら他の状態に遷移する
//		enemy.noticeFlg = FALSE;
//		enemy.state = STATE_WANDER;
//	}
//}

//=============================================================================
// エネミーの状態確認（FSMによるエネミーの制御）
// int enemyIndex：エネミーの番号
//=============================================================================
//void CheckEnemyState(int enemyIndex, ENEMY& enemy)
//{
//	switch (g_Enemy[enemyIndex].state)
//	{
//		case STATE_IDLE:			// 止まる
//			EnemyIdle(enemy);
//			break;
//
//		case STATE_WANDER:			// 歩き回る
//			EnemyWander(enemy);
//			break;
//
//		case STATE_CHASE:			// 追いかける
//			//EnemyChase(enemy);
//			EnemyWander(enemy);		// デバッグ用
//			break;
//
//		default:
//			break;
//	}
//}
