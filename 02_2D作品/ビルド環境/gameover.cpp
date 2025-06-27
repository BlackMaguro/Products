//=============================================================================
//
// ゲームオーバー画面処理 [gameover.cpp]
// Author : 
//
//=============================================================================
#include "gameover.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO				(480)			// ロゴサイズ（幅）
#define TEXTURE_HEIGHT_LOGO				(200)			// ロゴサイズ（縦） 
#define TEXTURE_WIDTH_LOGO_MANU			(200)			// ロゴサイズ（幅）
#define TEXTURE_HEIGHT_LOGO_MANU		(60)			// ロゴサイズ（縦） 
#define TEXTURE_WIDTH_MANU_ARROW		(60)			// メニュー矢印のサイズ（幅） 
#define TEXTURE_HEIGHT_MANU_ARROW		(60)			// メニュー矢印のサイズ（縦） 
#define TEXTURE_MANU_ARROW_X			(350)			// メニュー矢印のX座標 
#define TEXTURE_MANU_ARROW_Y			(320)			// メニュー矢印のY座標（初期値） 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/GameOver/GameOver_bg.png",				// ゲームオーバー画面の背景
	"data/TEXTURE/GameOver/GameOver_title.png",				// ゲームオーバーの文字
	"data/TEXTURE/GameOver/GameOver_menu_00_retry.png",		// タイトルへ
	"data/TEXTURE/GameOver/GameOver_menu_01_toTitle.png",	// リトライ
	"data/TEXTURE/GameOver/GameOver_menu_02_owari.png",		// おわり
	"data/TEXTURE/GameOver/GameOver_menu_arrow.png",		// メニュー選択の矢印

};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static int menu_select;		// メニューセレクト変数
float g_timeCount;
float g_titleTrans;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameOver(void)
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
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	menu_select = 0;
	g_timeCount = 0.0f;
	g_titleTrans = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameOver(void)
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
void UpdateGameOver(void)
{
	// GameOverの文字を透明にする時の話
	if(g_titleTrans != 1.0f) g_titleTrans += 1.0f / 300.0f;

	g_timeCount += 1.0f;
	// 「リトライ」を選択
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 0)
	{// Enter押したら、ステージを切り替える
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 0)
	{
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 0)
	{
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}

	// 「タイトルへ」を選択
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 1)
	{// Enter押したら、ステージを切り替える
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 1)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 1)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}

	// 「おわり」を選択
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 2)
	{// Enter押したら、ステージを切り替える
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 2)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 2)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}


	// メニュー画面を選択したときに矢印を移動させる
	int select = 1;
	if (GetKeyboardTrigger(DIK_DOWN)) menu_select += select;
	if (GetKeyboardTrigger(DIK_UP)) menu_select -= select;

	menu_select = menu_select % 3;

	if (menu_select < 0)
	{
		menu_select = 2;
	}
#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	//PrintDebugProc("volume:", menu_select);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameOver(void)
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

	// sin値の取得
	float sinValue = (sin(g_timeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる

	// タイトルの背景(BG)を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルロゴ_タイトル表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	float lx = TEXTURE_WIDTH / 2;		// 画面の中央に配置
	float ly = 100;
	float lw = TEXTURE_WIDTH_LOGO;
	float lh = TEXTURE_HEIGHT_LOGO;
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	SetSpriteColor(g_VertexBuffer, lx, ly, lw, lh, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_titleTrans));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	//==========================================
	// メニュー処理（処理開始）
	//==========================================
	// 選択されていた場合、文字を赤くする
	float selectColor = 1.0f;
	// 透明度
	float transparentValue;

	// メニューの表示（リトライ）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	// メニューの座標（Y軸のみ変更）
	float lmy = TEXTURE_MANU_ARROW_Y;		// 矢印と同じY座標（初期位置）
	float lmw = TEXTURE_WIDTH_LOGO_MANU;
	float lmh = TEXTURE_HEIGHT_LOGO_MANU;

	if (menu_select == 0)
	{
		selectColor = 0.0f;
		transparentValue = sinValue + 0.3f;
	}
	else
	{
		selectColor = 1.0f;
		transparentValue = 1.0f;
	}

	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// メニューの表示（タイトルへ）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	// メニューの座標（Y軸のみ変更）
	lmy += TEXTURE_HEIGHT_LOGO_MANU;
	if (menu_select == 1)
	{
		selectColor = 0.0f;
		transparentValue = sinValue + 0.3f;
	}
	else
	{
		selectColor = 1.0f;
		transparentValue = 1.0f;
	}
	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// メニューの表示（おわり）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	// メニューの座標（Y軸のみ変更）
	lmy += TEXTURE_HEIGHT_LOGO_MANU;
	if (menu_select == 2)
	{
		selectColor = 0.0f;
		transparentValue = sinValue + 0.3f;
	}
	else
	{
		selectColor = 1.0f;
		transparentValue = 1.0f;
	}

	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	//==========================================
	// メニュー処理（処理終了）
	//==========================================

	// メニューの矢印（arrow）表示（赤）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
	// 矢印のY座標
	int tmcy = TEXTURE_MANU_ARROW_Y;
	tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
	float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW * sinValue;	// 矢印（arrow）を回転させる
	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, TEXTURE_MANU_ARROW_X, (float)tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}





