//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "skysphere.h"
#include "light.h"
#include "meshobj.h"
#include "player.h"
#include "enemy.h"
#include "hp.h"
#include "mark.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "fire.h"
#include "throwMarker.h"
#include "effect.h"
#include "bullet.h"
#include "bomb.h"
#include "score.h"
#include "time.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_PauseFlg = TRUE;	// ポーズON/OFF

static PLAYER* g_Player = GetPlayer();

static char* g_TexturName[] = {
	"data/TEXTURE/damage_UI.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// スカイスフィアの初期化
	InitSkySphere();

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);
	//InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 20, 13.0f, 13.0f);

	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// フィールドオブジェクトの設置
	InitFieldObj();

	// 炎の初期化
	InitFire();

	// エフェクトのの初期化
	InitExplosion();

	// 投擲先マーカーの初期化
	InitThrowMarker();

	// マーク（エネミー）の初期化
	InitMark();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);


	// 弾の初期化
	InitBullet();

	// 爆弾の初期化
	InitBomb();

	// スコアの初期化
	InitScore();

	// スコアの初期化
	InitTime();

	// スコアの初期化
	InitHp();

	// パーティクルの初期化
	InitParticle();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_02_INGAME);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticle();

	// UIの終了処理
	UninitHp();

	// タイムの終了処理
	UninitTime();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// 爆弾の終了処理
	UninitBomb();

	// エフェクトの終了処理
	UninitExplosion();

	// 炎の終了処理
	UninitFire();

	// 投擲先マーカーの終了処理
	UninitThrowMarker();

	// マークの終了処理
	UninitMark();

	// フィールドオブジェクトの終了処理
	UninitFieldObj();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// スカイスフィアの終了処理
	UninitSkySphere();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{

#ifdef _DEBUG
	//if (GetKeyboardTrigger(DIK_V))
	//{
	//	g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
	//	SetViewPort(g_ViewPortType_Game);
	//}

#endif

	// ゲームを一時停止する
	if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
	{
		// SE再生
		PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
		g_PauseFlg = g_PauseFlg ? FALSE : TRUE;
	}

	if(g_PauseFlg == FALSE)
		return;

	// スカイスフィア処理の更新
	UpdateSkySphere();

	// 地面処理の更新
	UpdateMeshField();


	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// フィールドオブジェクトの更新処理
	UpdateFieldObj();

	// 炎の更新処理
	UpdateFire();

	//エフェクトの更新処理
	UpdateExplosion();

	// 投擲先マーカーの更新処理
	UpdateThrowMarker();

	// 弾の更新処理
	UpdateBullet();

	// 爆弾の更新処理
	UpdateBomb();

	// パーティクルの更新処理
	UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// HPの更新処理
	UpdateHp();

	// マークの更新処理
	UpdateMark();

	// タイムの更新処理
	UpdateTime();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// シャドウマップ
	{
		SetLightProjectViewMatrix();
		SetShader(RENDER_MODE_SHADOW_MAP);
		SetRenderTargetView(RENDER_MODE_SHADOW_MAP);
		SetViewPort(TYPE_SHADOWMAP);

		// フィールドオブジェクトの描画処理
		DrawFieldObj();

		// プレイヤーの描画処理
		DrawPlayer();

		// エネミーの描画処理
		DrawEnemy();

		// 弾の描画処理
		DrawBullet();

		// 爆弾の描画処理
		DrawBomb();

	}
	// 通常マップ
	{
		SetViewPort(TYPE_FULL_SCREEN);
		SetRenderTargetView(RENDER_MODE_NORMAL);
		SetShaderResource(RENDER_MODE_SHADOW_MAP);
		SetShader(RENDER_MODE_NORMAL);
	}
	// シャドウマップ

	// スカイスフィアの描画処理
	DrawSkySphere();

	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 弾の描画処理
	DrawBullet();

	// 爆弾の描画処理
	DrawBomb();

	// エフェクトの描画処理
	DrawExplosion();

	// フィールドオブジェクトの描画処理
	DrawFieldObj();

	// 炎の描画処理
	DrawFire();

	// 投擲先マーカーの描画処理
	DrawThrowMarker();

	// 壁の描画処理
	DrawMeshWall();

	// パーティクルの描画処理
	DrawParticle();

	// マークの描画処理
	DrawMark();


	//*****************************************************************************
	// 2Dで描画するものは以下に記載する
	//*****************************************************************************
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();

	// タイムの描画処理
	DrawTime();

	// UIの処理
	DrawHp();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();			// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();		// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();		// 弾のポインターを初期化
	int enemyCount = GetEnemyCount();	// エネミーの数

	XMFLOAT3 oldPos = player->pos;

	// 敵とプレイヤーとの当たり判定
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE) continue;
		if (enemy[i].deadFlg == TRUE) continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) && player->muteki == FALSE)
		{
			enemy[i].moveFlg = FALSE;	// エネミーの動きを止める
			player->muteki = TRUE;		// 無敵時間フラグをオンにする（無敵状態）
			player->mutekiTime = 0;		// 無敵時間のリセット
			player->pos = oldPos;

			enemy[i].noticeFlg = TRUE;
			enemy[i].isRotLock = FALSE;   // 向きを自由に

			// 敵キャラクターの種類によってダメージを変更する
			if (enemy[i].type == ENEMY_TYPE_KNIFEMAN)
			{
				// SE再生
				PlaySound(SOUND_LABEL_SE_04_PLAYERFFROMDAMAGE);
				// ダメージを受けたときの処理
				SetDamageFlg(TRUE);
				player->TakePlayerDamage(ENEMY_ATK);
				
				if (player->GetPlayerHP() <= 0)
				{
					SetFade(FADE_OUT, MODE_GAMEOVER);
				}
			}
		}
	}


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == FALSE)
			continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == FALSE) continue;
			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size) && !enemy[j].deadFlg)
			{
				// 当たったから弾を未使用に戻す
				bullet[i].use = FALSE;
				// 敵キャラクターは倒される
				enemy[j].deadFlg = TRUE;
				enemy[j].dissolveTimer = 0.0f;
				enemy[j].caledTimer = 0.0f;
				// スコアを足す
				AddScore(SCORE_POINT);
			}
		}
	}

	// 生きているエネミーを計算する
	int aliveCount = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use) aliveCount++;
	}
	// エネミー数をセットする
	SetEnemyCount(aliveCount);

	// 生きているエネミーがいないなら終了
	if (aliveCount <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

#ifdef _DEBUG
	//PrintDebugProc("enemyCount :%d,\n", enemyCount);
#endif
}

//=============================================================================
// 当たり判定処理（playerとenemy）
// playerOldPos：プレイヤーのエネミーに当たる前の処理
//=============================================================================
void checkPlayerToEnemy(XMFLOAT3 playerOldPos)
{
	ENEMY* enemy = GetEnemy();			// エネミーのポインターを初期化
	PLAYER* player = GetPlayer();		// プレイヤーのポインターを初期化

	// 敵とプレイヤーとの当たり判定
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE) continue;
		if (enemy[i].deadFlg == TRUE) continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size -1.0f))
		{
			player->pos = playerOldPos;
		}
	}
}

//=============================================================================
// 中断フラグの取得（アクセサ関数）
//=============================================================================
BOOL GetPauseFlg(void)
{
	return g_PauseFlg;
}

void SetPauseFlg(BOOL pauseFlg)
{
	g_PauseFlg = pauseFlg;
}




