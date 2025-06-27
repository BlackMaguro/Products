//=============================================================================
//
// UI処理 [hp.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "fade.h"
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "sprite.h"
#include "hp.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// playerのHPテクスチャ
static const float PLAYER_HP_X				  (20.0f);		// プレイヤーHPのX座標
static const float PLAYER_HP_Y				  (450.0f);		// プレイヤーHPのX座標
static const float PLAYER_HP_WIDTH			  (300.0f);		// プレイヤーHPの幅
static const float PLAYER_HP_HEIGHT			  (40.0f);		// プレイヤーHPの縦
// エネミーの文字テクスチャ（何〇体）
static const float ENEMY_COUNT_TEXTURE_X	  (80.0f);		// エネミー数テクスチャのX座標
static const float ENEMY_COUNT_TEXTURE_Y	  (450.0f);		// エネミー数テクスチャのX座標
static const float ENEMY_COUNT_TEXTURE_WIDTH  (300.0f);		// エネミー数文字テクスチャの幅
static const float ENEMY_COUNT_TEXTURE_HEIGHT (130.0f);		// エネミー数文字テクスチャの縦
// エネミーの残り数テクスチャ
static const float ENEMY_COUNT_NUM_X		(80.0f);		// エネミー数テクスチャのX座標
static const float ENEMY_COUNT_NUM_Y		(450.0f);		// エネミー数テクスチャのX座標
static const float ENEMY_COUNT_NUM_WIDTH	(35.0f);		// エネミー数テクスチャの幅
static const float ENEMY_COUNT_NUM_HEIGHT	(80.0f);		// エネミー数テクスチャの縦
// ダメージエフェクト
static const float DAMAGE_EFFECT_WIDTH		(960.0f);			// ダメージエフェクトのフィルター
static const float DAMAGE_EFFECT_HEIGHT		(540.0f);		// ダメージエフェクトのフィルター
// Pキー
static const float PKEY_TEXTURE_X			(400.0f);	// PキーテクスチャのX座標
static const float PKEY_TEXTURE_Y			(450.0f);	// PキーテクスチャのX座標
static const float PKEY_TEXTURE_WIDTH		(200.0f);	// Pキーテクスチャの幅
static const float PKEY_TEXTURE_HEIGHT		(80.0f);	// Pキーテクスチャの縦
// 操作説明の表示
static const float OPERATION_TEXTURE_X		(10.0f);	// 操作説明テクスチャのX座標
static const float OPERATION_TEXTURE_Y		(110.0f);	// 操作説明テクスチャのX座標
static const float OPERATION_TEXTURE_WIDTH	(200.0f);	// 操作説明テクスチャの幅
static const float OPERATION_TEXTURE_HEIGHT (200.0f);	// 操作説明テクスチャの縦
// アイコンの表示関係
static const float ICON_SIZE		  (60.0f);				// アイコンの大きさ（縦横統一）
static const float ICON_GUN_X		  (20.0f);				// アイコン（銃）のX座標
static const float ICON_GUN_Y		  (370.0f);				// アイコン（銃）のY座標
static const float ICON_WOODENSWORD_X (90.0f);				// アイコン（木刀）のX座標
static const float ICON_WOODENSWORD_Y (370.0f);				// アイコン（木刀）のY座標
static const float ICON_BOMB_X		  (160.0f);				// アイコン（爆弾）のX座標
static const float ICON_BOMB_Y		  (370.0f);				// アイコン（爆弾）のY座標
static const float ICON_BG_BLACK_X	  (SCREEN_CENTER_X);	// アイコン（黒背景）のX座標
static const float ICON_BG_BLACK_Y	  (10.0f);				// アイコン（黒背景）のY座標
static const float ICON_EYE_X		  (SCREEN_CENTER_X);	// アイコン（目）のX座標
static const float ICON_EYE_Y		  (10.0f);				// アイコン（目）のY座標


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[UI_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TextureName[] = {
	"data/TEXTURE/bar_white.png",								// 0:体力ゲージ
	"data/TEXTURE/damage_UI.png",								// 1:ダメージ時の赤い背景
	"data/TEXTURE/04_INGAME/01_ICON/icon_01_gun.png",			// 2:アイコン_銃
	"data/TEXTURE/04_INGAME/02_UI/EnemyCount.png",				// 3:エネミーの残り数テクスチャ
	"data/TEXTURE/number16_32_original.png",					// 4:数字のテクスチャ
	"data/TEXTURE/04_INGAME/01_ICON/icon_00_bg_BLAKC.png",		// 5:アイコン_背景_黒
	"data/TEXTURE/04_INGAME/01_ICON/icon_02_eye.png",			// 6:アイコン_目
	"data/TEXTURE/04_INGAME/02_UI/01_PKey.png",					// 7:UI_Pキー
	"data/TEXTURE/04_INGAME/02_UI/02_Operation.png",			// 8:UI_操作説明
	"data/TEXTURE/04_INGAME/01_ICON/icon_03_woodenSword.png",	// 9:アイコン_木刀
	"data/TEXTURE/04_INGAME/01_ICON/icon_04_bomb.png",			// 10:アイコン_爆弾
};


static bool						g_Use;						// true:使っている  false:未使用
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static UI						g_UI;						// ポリゴンデータ
static int						g_TexNo;					// テクスチャ番号
static BOOL						g_DamageFlg = FALSE;		// テクスチャ番号
static float g_DamageTimeCount = 0.0f;
static float g_DamageEffectTrans = 1.0f;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHp(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < UI_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 体力の初期化
	g_Use   = true;
	g_Pos   = { 920.0f, 20.0f, 0.0f };
	g_DamageFlg = FALSE;				// ダメージフラグをオフ
	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitHp(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < UI_TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateHp(void)
{
	g_DamageTimeCount += 1.0f;
	if (g_DamageFlg == TRUE)
	{
		g_DamageEffectTrans -= 0.01f;
		if (g_DamageEffectTrans < 0.0f)
		{
			g_DamageFlg = FALSE;
			g_DamageEffectTrans = 1.0f;
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("EnemyCount :%d", g_EnemyCount);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHp(void)
{
	PLAYER* player = GetPlayer();
	float damageEffectTime = 0.0f;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ_共通
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	//=============================================================================
	// プレイヤーHP
	//=============================================================================
	{
		// プレイヤーのHPゲージ（背景の黒）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_HP_GAUGE]);
		material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = PLAYER_HP_X;			// ゲージの表示位置X
		float py = PLAYER_HP_Y;			// ゲージの表示位置Y
		float pw = PLAYER_HP_WIDTH;		// ゲージの表示幅
		float ph = PLAYER_HP_HEIGHT;	// ゲージの表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// プレイヤーのHPに応じて長さを変更（赤色のバー）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_HP_GAUGE]);
		material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャー座標を反映
		pw = pw * ((float)player->GetPlayerHP() / PLAYER_MAX_HP);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	//=============================================================================
	// アイコン
	//=============================================================================
	// アイコン（銃）表示
	{
		float pw = ICON_SIZE;		// 表示幅
		float ph = ICON_SIZE;		// 表示高さ
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_GUN]);
		
		if (player->weaponType == PLAYER_WEAPON_GUN)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = ICON_GUN_X;		// 表示位置X
		float py = ICON_GUN_Y;		// 表示位置Y


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	// アイコン（木刀）表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_WOODSWORD]);
		if (player->weaponType == PLAYER_WEAPON_WOODENSWORD)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = ICON_WOODENSWORD_X;		// 表示位置X
		float py = ICON_WOODENSWORD_Y;		// 表示位置Y
		float pw = ICON_SIZE;				// 表示幅
		float ph = ICON_SIZE;				// 表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	// アイコン（爆弾）表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_BOMB]);
		if (player->weaponType == PLAYER_WEAPON_BOMB)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = ICON_BOMB_X;		// 表示位置X
		float py = ICON_BOMB_Y;		// 表示位置Y
		float pw = ICON_SIZE;		// 表示幅
		float ph = ICON_SIZE;		// 表示高さ

		// 1枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	// アイコン_BGの背景
	if (IsAnyEnemyNotice())
	{
		// アイコン_BGの表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_BG_BLACK]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = ICON_BG_BLACK_X;	// 表示位置X
		float py = ICON_BG_BLACK_Y;	// 表示位置Y
		float pw = ICON_SIZE;		// 表示幅
		float ph = ICON_SIZE;		// 表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// アイコン_目（気づいているときは表示する）
		{
			// アイコン_目の表示
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_EYE]);
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			SetMaterial(material);

			//ゲージの位置やテクスチャ座標を反映
			float px = ICON_EYE_X;		// 表示位置X
			float py = ICON_EYE_Y;		// 表示位置Y
			float pw = ICON_SIZE;		// 表示幅
			float ph = ICON_SIZE;		// 表示高さ

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}
	//=============================================================================
	// Pキーと操作説明
	//=============================================================================
	// Pキーの表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_P_KEY]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = PKEY_TEXTURE_X;			// Pキーの表示位置X
		float py = PKEY_TEXTURE_Y;			// Pキーの表示位置Y
		float pw = PKEY_TEXTURE_WIDTH;		// Pキーの表示幅
		float ph = PKEY_TEXTURE_HEIGHT;		// Pキーの表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	// 操作説明の表示
	if(!GetPauseFlg())
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_INSTRUCTION]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャ座標を反映
		float px = OPERATION_TEXTURE_X;			// 操作説明の表示位置X
		float py = OPERATION_TEXTURE_Y;			// 操作説明の表示位置Y
		float pw = OPERATION_TEXTURE_WIDTH;		// 操作説明の表示幅
		float ph = OPERATION_TEXTURE_HEIGHT;	// 操作説明の表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	//=============================================================================
	// エネミー数（残りN体）
	//=============================================================================
	// 残りエネミー数テクスチャの表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ENEMY_COUNT]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//テクスチャの位置やテクスチャ座標を反映
		float px = SCREEN_WIDTH - ENEMY_COUNT_TEXTURE_WIDTH;	// 残りエネミー数テクスチャの表示位置X
		float py = SCREEN_HEIGHT - ENEMY_COUNT_TEXTURE_HEIGHT;	// 残りエネミー数テクスチャの表示位置Y
		float pw = ENEMY_COUNT_TEXTURE_WIDTH;					// 残りエネミー数テクスチャの横幅
		float ph = ENEMY_COUNT_TEXTURE_HEIGHT;					// 残りエネミー数テクスチャの縦幅

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	//=============================================================================
	// ダメージエフェクト
	//=============================================================================
	if (g_DamageFlg)
	{
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_DamageEffectTrans);
		SetMaterial(material);
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_DAMAGE_RED_BG]);

		//ゲージの位置やテクスチャー座標を反映
		float px = 0.0f;					// ゲージの表示位置X
		float py = 0.0f;					// ゲージの表示位置Y
		float pw = DAMAGE_EFFECT_WIDTH;		// ゲージの表示幅
		float ph = DAMAGE_EFFECT_HEIGHT;	// ゲージの表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	// エネミー数の表示（数字）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_NUMBER]);

	// 桁数分処理する
	int enemyNum = GetEnemyCount();
	int enemyNumDigit = enemyNum / 10 + 1;
	for (int i = 0; i < enemyNumDigit; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(enemyNum % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH - ENEMY_COUNT_NUM_WIDTH * i - 100.0f;	// 残りエネミー数の表示位置X
		float py = SCREEN_HEIGHT - ENEMY_COUNT_NUM_HEIGHT + 10.0f;			// 残りエネミー数の表示位置Y
		float pw = ENEMY_COUNT_NUM_WIDTH;							// 残りエネミー数の横幅
		float ph = ENEMY_COUNT_NUM_HEIGHT;							// 残りエネミー数の縦幅

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		enemyNum /= 10;
	}

}

//=============================================================================
// ダメージを受けたかの判断（アクセサ関数）
//=============================================================================
BOOL GetDamageFlg()
{
	return g_DamageFlg;
}

void SetDamageFlg(BOOL damageFlg)
{
	g_DamageFlg = damageFlg;
}




