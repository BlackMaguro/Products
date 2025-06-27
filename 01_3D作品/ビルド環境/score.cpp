//=============================================================================
//
// スコア処理 [score.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const float TEXTURE_X	  (125.0f);		// スコアテクスチャのX座標
static const float TEXTURE_Y	  (75.0f);		// スコアテクスチャのY座標
static const float TEXTURE_WIDTH  (20.0f);		// テクスチャサイズ（幅）
static const float TEXTURE_HEIGHT (45.0f);		// テスクチャサイズ（高さ）

static const float ICON_DOLL_X		(0.0f);			// アイコン（ドル）のX座標
static const float ICON_DOLL_Y		(50.0f);		// アイコン（ドル）のY座標
static const float ICON_DOLL_WIDTH	(60.0f);		// アイコン（ドル）のサイズ（幅）
static const float ICON_DOLL_HEIGHT	(50.0f);		// アイコン（ドル）のサイズ（高さ）

static const float TEXTURE_SCORE_PLUS_X		 (170.0f);	// プラスマークテクスチャのX座標
static const float TEXTURE_SCORE_PLUS_Y		 (75.0f);	// プラスマークテクスチャのY座標
static const float TEXTURE_SCORE_PLUS_WIDTH	 (30.0f);	// プラスマークテクスチャのサイズ（幅）
static const float TEXTURE_SCORE_PLUS_HEIGHT (30.0f);	// プラスマークテクスチャのサイズ（高さ）

static const float  TEXTURE_GAINSCORE_X		(240.0f);	// スコアテクスチャのX座標
static const float  TEXTURE_GAINSCORE_Y		(75.0f);	// スコアテクスチャのY座標

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
namespace
{
	enum 
	{
		TEXTURE_SCORE_NUMBER = 0,		// スコアの数字
		TEXTURE_SCORE_DOLL,				// スコアのドルマーク
		TEXTURE_SCORE_DOLL_PLUS,		// 加算されるスコア
		
		TEXTURE_SCORE_MAX				// スコアテクスチャの最大数
	};
}
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_SCORE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_SCORE_MAX] = {
	"data/TEXTURE/number16_32_original.png",			// 数字のテクスチャ
	PATH_TEXTURE_INGAME "01_ICON/icon_00_doll.png",		// ドルのアイコン
	PATH_TEXTURE_INGAME "02_UI/doll_score_plus.png",		// 加算スコア
};


static BOOL				g_Use;							// TRUE:使っている  FALSE:未使用
static float			g_w, g_h;						// 幅と高さ
static XMFLOAT3			g_Pos;							// ポリゴンの座標

static XMFLOAT3			g_GainScorePos;					// ポリゴンの座標
static int				g_CurrentScore;					// 現在のスコア
static int				g_GainValue;					// 今回獲得したスコア
static int				g_DisplayedScore;				// 画面に表示中のスコア
static const int		SCORE_STEP = 1;				    // 1フレームあたり増加する点数

static BOOL				g_ShowGainScoreFlg = FALSE;		// ポップアップ表示中フラグ
static float			g_TransTime = 1.0f;				// 透明になるまでの時間

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_SCORE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
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


	// スコアの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { TEXTURE_X, TEXTURE_Y, 0.0f };
	g_GainScorePos = { TEXTURE_GAINSCORE_X, TEXTURE_GAINSCORE_Y, 0.0f };
	g_CurrentScore = 0;
	g_DisplayedScore = 0;
	g_GainValue = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_SCORE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{
	float transTimeDec = 0.01f;
	// スコアが最大値以下ならフレーム単位で増やす
	if (g_DisplayedScore < g_CurrentScore)
	{
		int restNum = g_CurrentScore - g_DisplayedScore;
		int step = (restNum < SCORE_STEP) ? restNum : SCORE_STEP;
		g_DisplayedScore += step;
		g_TransTime -= transTimeDec;
		if(SCORE_MAX < g_DisplayedScore) g_DisplayedScore = g_CurrentScore;
	}
	if(g_TransTime < 0.0f)
	{
		g_TransTime = 0.0f;
		g_ShowGainScoreFlg = FALSE;
	}
#ifdef _DEBUG	// デバッグ情報を表示する

	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
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
	material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_NUMBER]);

	// 桁数分処理する
	int number = g_DisplayedScore;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// スコアの表示位置X
		float py = g_Pos.y;			// スコアの表示位置Y
		float pw = g_w;				// スコアの表示幅
		float ph = g_h;				// スコアの表示高さ

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
		number /= 10;
	}
	
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標
	// ドル（＄）のアイコン表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_DOLL]);
		material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャー座標を反映
		float px = ICON_DOLL_X;			// 表示位置X
		float py = ICON_DOLL_Y;			// 表示位置Y
		float pw = ICON_DOLL_WIDTH;		// 表示幅
		float ph = ICON_DOLL_HEIGHT;	// 表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	// 加算スコアの表示
	if(g_ShowGainScoreFlg)
	{
		// プラスマークテクスチャ
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_DOLL_PLUS]);
		material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
		SetMaterial(material);

		//ゲージの位置やテクスチャー座標を反映
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			TEXTURE_SCORE_PLUS_X, TEXTURE_SCORE_PLUS_Y, 
			TEXTURE_SCORE_PLUS_WIDTH, TEXTURE_SCORE_PLUS_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f,1.0f,1.0f, g_TransTime));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 獲得した金額の描画
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_NUMBER]);

		int value = g_GainValue;
		for (int i = 0; i < SCORE_GAIN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(value % 10);

			// スコアの位置やテクスチャー座標を反映
			float px = g_GainScorePos.x - g_w * i;	// スコアの表示位置X
			float py = g_GainScorePos.y;			// スコアの表示位置Y
			float pw = g_w;				// スコアの表示幅
			float ph = g_h;				// スコアの表示高さ

			float tw = 1.0f / 10;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_TransTime));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			// 次の桁へ
			value /= 10;
		}
	}

}


//=============================================================================
// スコアを加算する
// @param add :追加する点数
// note ：　引数はマイナスも受け取れるが表示上はプラスになってしまう
//=============================================================================
void AddScore(int add)
{
	PlaySound(SOUND_LABEL_SE_07_GETMONEY);

	// まだポップアップ開始直後（残り時間が 0.9以上）なら累積
	if (g_ShowGainScoreFlg && g_TransTime > 0.9f)
	{
		g_GainValue += add;
	}
	else
	{
		g_GainValue = add;
		g_ShowGainScoreFlg = TRUE;
	}
	g_TransTime = 1.0f;

	// 実スコアは常に加算
	g_CurrentScore += add;
	if (g_CurrentScore > SCORE_MAX)
		g_CurrentScore = SCORE_MAX;

}

int GetScore(void)
{
	return g_CurrentScore;
}

