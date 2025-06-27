//=============================================================================
//
// BG処理 [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"
#include "main.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_TUTORIAL_WIDTH		(2000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_BOSS_WIDTH			(960)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(640)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/tutorial_bg.png",			// 0_チュートリアルステージのBG
	"data/TEXTURE/BG/bg_sky.png",				// 1_チュートリアルステージの空
	"data/TEXTURE/BG/bg_stage1.png",			// 2_ステージ1のBG
	"data/TEXTURE/BG/bg_boss1.png",				// 3_ボスステージのBG
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_BG;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
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


	// 変数の初期化
	g_BG.w     = TEXTURE_WIDTH;
	if (GetBossFlg() == TRUE) g_BG.w = TEXTURE_BOSS_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;
	g_BG.bgNo = GetMode();	// 使用するBGを表示する

	g_BG.scrl  = 0.0f;		// TEXスクロール
	g_BG.scrl2 = 0.0f;		// TEXスクロール

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// １フレ前の情報を保存

	g_BG.scrl -= 0.0f;		// 0.005f;		// スクロール


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("BG pos.x:%f,pos.y:%f,\ng_BG.texNo:%d\ng_BG.bgNo:%d,\ng_BG.w:%f,", g_BG.pos.x, g_BG.pos.y, g_BG.texNo, g_BG.bgNo,g_BG.w);
#endif

	// チュートリアルを描画
	if (g_BG.bgNo == MODE_TUTORIAL)
	{
		g_BG.w = TEXTURE_TUTORIAL_WIDTH;
		//g_BG.h = TEXTURE_HEIGHT;
		// 空を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_TUTORIAL_WIDTH);
			g_BG.scrl += tx * 0.001f;
			g_BG.scrl += 0.001f;

			SetSpriteLTColor(g_VertexBuffer,
				0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_HEIGHT,
				g_BG.scrl, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// チュートリアルの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}

	// stage1を描画
	if ((g_BG.bgNo == MODE_GAME) && GetBossFlg() != TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
			
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// ボス戦の背景を描画
	if (g_BG.bgNo == MODE_BOSS)
	{
		g_BG.w = TEXTURE_BOSS_WIDTH;
		//g_BG.h = TEXTURE_HEIGHT;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}





