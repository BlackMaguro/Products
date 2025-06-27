//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ（横）
#define TEXTURE_HEIGHT_LOGO			(80)			// ロゴサイズ（縦）
#define TEXTURE_WIDTH_LOGO_MANU		(200)			// メニューの選択肢のサイズ（横）
#define TEXTURE_HEIGHT_LOGO_MANU	(60)			// メニューの選択肢のサイズ（縦）
#define TEXTURE_WIDTH_MANU_ARROW	(100)			// メニュー選択の矢印のサイズ（横） 
#define TEXTURE_HEIGHT_MANU_ARROW	(60)			// メニュー選択の矢印のサイズ（縦）

#define TEXTURE_MANU_ARROW_X		(350)			// メニュー矢印のX座標 
#define TEXTURE_MANU_ARROW_Y		(320)			// メニュー矢印のY座標（初期値） 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/01_BG/01_title.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/02_TITLE/title_softname.png",				// タイトル前にロゴをだす
	"data/TEXTURE/02_TITLE/title_menu_00_start.png",		// はじめから
	"data/TEXTURE/02_TITLE/title_menu_01_settings.png",		// せってい
	"data/TEXTURE/02_TITLE/title_menu_02_end.png",			// おわり
	"data/TEXTURE/02_TITLE/title_menu_arrow2.png",			// メニュー選択の矢印
	"data/TEXTURE/02_TITLE/title_menu_03_UserGuide.png",	// そうさせつめい
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;
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

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_00_TITLE);

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

	// クレジット表記が終わったら
	if (GetCreditFlg() != TRUE)
	{
		// サウンド関係の処理
		{
			// Enter、ゲームパッドのBかスタートが押されたときの音
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				PlaySound(SOUND_LABEL_SE_01_ENTER);
			}
			// カーソルの上下キーを押したとき
			if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_UP) ||
				IsButtonTriggered(0, BUTTON_UP) || IsButtonTriggered(0, BUTTON_DOWN))
			{
				PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
			}
		}

		// Enter、ゲームパッドのBかスタートが押されたら、ステージを切り替える
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			// はじめるを選択時
			if (menu_select == 0)
			{
				// TODO:チュートリアルに飛ばす
				SetFade(FADE_OUT, MODE_GAME);
			}
			else if (menu_select == 1)
			{
				// TODO:操作画面に飛ばす
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			// せっていを選択時
			else if (menu_select == 2)
			{
				// TODO:設定画面に飛ばす
				SetFade(FADE_OUT, MODE_SETTINGS);
			}
			// おわりを選択時
			else if (menu_select == 3)
			{
				// TODO:設定画面に飛ばす
				SetFade(FADE_OUT, MODE_END);
			}
		}

		if (flag_alpha == TRUE)
		{
			alpha -= 0.02f;
			if (alpha <= 0.0f)
			{
				alpha = 0.0f;
				flag_alpha = FALSE;
			}
		}
		else
		{
			alpha += 0.02f;
			if (alpha >= 1.0f)
			{
				alpha = 1.0f;
				flag_alpha = TRUE;
			}
		}

		// メニュー画面を選択したときに矢印を移動させる
		{
			int select = 1;
			if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN))
			{
				menu_select += select;
			}
			if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
			{
				menu_select -= select;
			}

			menu_select = menu_select % 4;

			if (menu_select < 0)
			{
				menu_select = 3;
			}

		}
	}





#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

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

	// タイトル画面の前にロゴ表示
	if (GetCreditFlg() == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_creditTrans));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	else if (GetCreditFlg() != TRUE)
	{
		// タイトルの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		//==========================================
		// メニュー処理（処理開始）
		//==========================================
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		float tx = 780.0f;		// メニューの選択肢のX座標
		float ty = 300.0f;		// メニューの選択肢のY座標
		float lw = 680.0f;		// メニューの選択肢の横幅
		float lh = 100.0f;		// メニューの選択肢の縦幅

		// 選択されていた場合、文字を赤くする
		float selectColor = 1.0f;
		// 透明度
		float transparentValue;

		float tmy = TEXTURE_MANU_ARROW_Y;		// 矢印と同じY座標（初期位置）
		// メニューの表示（はじめから）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			// メニューの座標（Y軸のみ変更）
			if (menu_select == 0)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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
		}


		// メニューの表示（操作方法）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			// メニューの座標（Y軸のみ変更）
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 1)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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
		}


		// メニューの表示（せってい）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			// メニューの座標（Y軸のみ変更）
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 2)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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

		}

		// メニューの表示（おわり）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			// メニューの座標（Y軸のみ変更）
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 3)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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

		}



		// メニューの矢印（arrow）表示
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);
			tx -= 160.0f;
			// 矢印のY座標
			float tmcy = (float)TEXTURE_MANU_ARROW_Y;
			tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
			float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW;

			if (GetMode() == MODE_TITLE)
			{
				arrowHeight *= alpha;	// 矢印（arrow）を回転させる
			}
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, tx, tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}

}





