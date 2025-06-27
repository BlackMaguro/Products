//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "title.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(10)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO				(480)			// ロゴサイズ（幅）
#define TEXTURE_HEIGHT_LOGO				(100)			// ロゴサイズ（縦） 
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
FILE* fp = fopen("savedata.bin", "rb");								// セーブデータの有無

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title/title_bg.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/title/title_01.png",					// タイトル
	"data/TEXTURE/title_menu_00_hajimekara.png",		// はじめから
	"data/TEXTURE/title_menu_01_tudukikara.png",		// つづきから
	"data/TEXTURE/title_menu_02_settings.png",			// せってい
	"data/TEXTURE/title_menu_03_owari.png",				// おわり
	"data/TEXTURE/title_menu_arrow.png",				// メニュー選択の矢印
	"data/TEXTURE/title/title_02.png",					// タイトル（クリア後）
	"data/TEXTURE/title/title_softname.png",			// タイトル前に会社ロゴをだす
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static float g_creditTrans = 1.0f;
static int menu_select;
float g_titleTimeCount;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	menu_select = 0;	// メニューセレクト変数
	g_titleTimeCount = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	g_titleTimeCount += 1.0f;
	if (g_titleTimeCount > 180.0f)
	{
		g_creditTrans -= 0.01f;
		if (g_creditTrans < 0.0f)
		{
			SetCreditFlg(FALSE);
			g_creditTrans = 0.0f;
		}
	}


	if (GetCreditFlg() != TRUE)
	{
		// サウンド関係の処理
		{
			// エンターキーが押されたとき
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				PlaySound(SOUND_LABEL_SE_01_ENTER);
			}
			// カーソルの上下キーを押したとき
			if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_UP))
			{
				PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			}
		}

		// 「はじめから」を選択
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 0)
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_PROLOGUE);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 0)
		{

			SetFade(FADE_OUT, MODE_PROLOGUE);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 0)
		{
			SetFade(FADE_OUT, MODE_PROLOGUE);
		}

		// セーブデータをロードする
		// 「つづきから」を選択
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 1 && fp != NULL)
		{// Enter押したら、ステージを切り替える
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 1 && fp != NULL)
		{
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 1 && fp != NULL)
		{
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}

		// 「せってい」を選択
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 2)
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_SETTINGS);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 2)
		{
			SetFade(FADE_OUT, MODE_SETTINGS);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 2)
		{
			SetFade(FADE_OUT, MODE_SETTINGS);
		}

		// 「おわり」を選択
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 3)
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_END);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 3)
		{
			SetFade(FADE_OUT, MODE_END);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 3)
		{
			SetFade(FADE_OUT, MODE_END);
		}


		// メニュー画面を選択したときに矢印を移動させる
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			// SE：カーソル上下を押したとき
			PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			menu_select += select;
			// セーブデータがなかったら矢印を移動させない
			if ((menu_select == 1) && (fp == NULL))
			{
				menu_select = 2;
			}
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			// SE：カーソル上下を押したとき
			PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			menu_select -= select;
			// セーブデータがなかったら矢印を移動させない
			if ((menu_select == 1) && (fp == NULL))
			{
				menu_select = 0;
			}
		}

		menu_select = menu_select % 4;

		if (menu_select < 0)
		{
			menu_select = 3;
		}

#ifdef _DEBUG	// デバッグ情報を表示する
		//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
		PrintDebugProc("g_creditTrans:%f.\n", g_creditTrans);
#endif
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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
	float sinValue = (sin(g_titleTimeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる
	// タイトル画面の前の会社名（仮）表示
	if(GetCreditFlg() == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_creditTrans));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	else if(GetCreditFlg() != TRUE)
	{
		// タイトルの背景（BG）を描画
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
		if (GetClearFlg() == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);
		}
		else
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		}
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		float tx = SCREEN_WIDTH / 2;
		float ty = 100;
		float lw = 680.0f;		// ロゴの横幅
		float lh = 100.0f;		// ロゴの縦幅

		SetSpriteColor(g_VertexBuffer, tx, ty, lw, lh, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//==========================================
		// メニュー処理（処理開始）
		//==========================================
		// 選択されていた場合、文字を赤くする
		float selectColor = 1.0f;
		// 透明度
		float transparentValue;

		// メニューの表示（はじめから）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// メニューの座標（Y軸のみ変更）
		float tmy = TEXTURE_MANU_ARROW_Y;		// 矢印と同じY座標（初期位置）
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

		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// メニューの表示（つづきから）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// メニューの座標（Y軸のみ変更）
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
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

		// セーブデータが無かったら矢印の位置をとばす
		if (fp == NULL)
		{
			SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));
		}

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// メニューの表示（せってい）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// メニューの座標（Y軸のみ変更）
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
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
		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// メニューの表示（おわり）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// メニューの座標（Y軸のみ変更）
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
		if (menu_select == 3)
		{
			selectColor = 0.0f;
			transparentValue = sinValue + 0.3f;
		}
		else
		{
			selectColor = 1.0f;
			transparentValue = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		//==========================================
		// メニュー処理（処理終了）
		//==========================================

		// メニューの矢印（arrow）表示（赤）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		// 矢印のY座標
		float tmcy = (float) TEXTURE_MANU_ARROW_Y;
		tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW * sinValue;	// 矢印（arrow）を回転させる
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, TEXTURE_MANU_ARROW_X, tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}





