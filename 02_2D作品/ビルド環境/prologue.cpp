//=============================================================================
//
// プロローグ画面処理 [prologue.cpp]
// Author : 
//
//=============================================================================
#include "prologue.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(14)				// テクスチャの数

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

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_0.png",									// 0_モードに入る前の選択画面のBG（チュートリアルと同じBG）
	"data/TEXTURE/Prologue/prologue_logo_0.png",				// 1_モードに入る前の選択画面のロゴ_タイトル
	"data/TEXTURE/tutorial_logo_1_yes.png",						// 2_モードに入る前の選択画面のロゴ_はい
	"data/TEXTURE/tutorial_logo_2_no.png",						// 3_モードに入る前の選択画面のロゴ_いいえ
	"data/TEXTURE/title_menu_arrow.png",						// 4_メニュー選択の矢印
	"data/TEXTURE/Prologue/prologue_bg.png",					// 5_プロローグの背景
	"data/TEXTURE/Prologue/prologue_message_0.png",				// 6_プロローグのメッセージ0
	"data/TEXTURE/Prologue/prologue_message_1.png",				// 7_プロローグのメッセージ1
	"data/TEXTURE/Prologue/prologue_message_2.png",				// 8_プロローグのメッセージ2
	"data/TEXTURE/Prologue/prologue_message_3.png",				// 9_プロローグのメッセージ3
	"data/TEXTURE/Prologue/prologue_charaMessage_0.png",		// 10_プロローグのキャラメッセージ0
	"data/TEXTURE/Prologue/prologue_charaMessage_1.png",		// 11_プロローグのキャラメッセージ1
	"data/TEXTURE/Prologue/prologue_charaMessage_2.png",		// 12_プロローグのキャラメッセージ2
	"data/TEXTURE/Prologue/prologue_charaMessage_3.png",		// 13_プロローグのキャラメッセージ2
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号


static BOOL						g_Load = FALSE;
static BOOL						g_PrologueFlg = FALSE;		// プロローグを実行するかのフラグ
int g_prologue_menu_select;									// モードに入る前の選択肢用変数
float g_prologueTimeCount;									// プロローグ画面のタイム変数
int g_prologueMessageCount;									// プロローグ画面のメッセージを透明にする変数
float g_prologueMessageTrans = 1.0f;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPrologue(void)
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
	g_prologue_menu_select = 0;
	g_prologueTimeCount = 0;
	g_prologueMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPrologue(void)
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
void UpdatePrologue(void)
{
	// クリアフラグをオフにする
	SetClearFlg(FALSE);

	g_prologueTimeCount += 1.0f;

	// プロローグ中にEnterをおしたとき
	if (GetPrologueFlg() == TRUE)
	{
		// エンターキーが押されたとき（プロローグ中のEnter音）
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}


	if (g_prologueMessageCount >= 4)
	{
		g_prologueMessageTrans -= 0.01f;
		if (g_prologueMessageTrans < 0.0f)
		{
			g_prologueMessageTrans = 0.0f;
		}
	}

	if ((GetPrologueFlg() == TRUE) && GetKeyboardTrigger(DIK_RETURN))
	{
		// プロローグのメッセージカウントを上げる
		if ((g_prologueMessageCount == 4) && (g_prologueMessageTrans != 0.0f))
		{
			g_prologueMessageCount = 4;
		}
		g_prologueMessageCount++;

	}

	// プロローグの最後まで行ったらモードをチュートリアルへ
	if (GetKeyboardTrigger(DIK_RETURN) && g_prologueMessageCount >= 8)
	{		
		// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}

	// プロローグを実施するかの処理（フラグがTRUEになったら消える）
	if (GetPrologueFlg() == FALSE)
	{
		// サウンドの処理
		{
			// エンターキーが押されたとき（プロローグ前のEnter音）
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

		// 選択肢が「はい」のとき
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_prologue_menu_select == 0)
			{
				// プロローグフラグをオンにする
				SetPrologueFlg(TRUE);
				// プロローグの音を設定
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_prologue_menu_select == 0)
			{
				// プロローグフラグをオンにする
				SetPrologueFlg(TRUE);
				// プロローグの音を設定
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_prologue_menu_select == 0)
			{
				// プロローグフラグをオンにする
				SetPrologueFlg(TRUE);
				// プロローグの音を設定
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
		}

		// 選択肢が「いいえ」のとき
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_prologue_menu_select == 1)
			{
				// Enter押したら、ステージを切り替える
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_prologue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_prologue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
		}

		// メニュー画面を選択したときに矢印を移動させる
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_prologue_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_prologue_menu_select -= select;
		}

		g_prologue_menu_select = g_prologue_menu_select % 2;

		if (g_prologue_menu_select < 0)
		{
			g_prologue_menu_select = 1;
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("g_prologueMessageCount:%d,\n", g_prologueMessageCount);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPrologue(void)
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
	float sinValue = (sin(g_prologueTimeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる

	// チュートリアルをするかの確認画面
	if (g_PrologueFlg == FALSE)
	{
		// プロローグ前の選択肢画面のBG表示
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		float px = SCREEN_WIDTH / 2;
		float py = 100.0f;
		float pw = 700.0f;		// ロゴの横幅
		float ph = 130.0f;		// ロゴの縦幅

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// プロローグ前の選択肢画面のタイトル
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}


		// 選択されていた場合、文字を赤くする
		float selectColor = 1.0f;
		// 透明度
		float menuTransValue = 1.0f;
		//  プロローグ前の選択肢画面のメニュー「はい」
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_prologue_menu_select == 0)
			{
				selectColor = 0.0f;
				menuTransValue = sinValue + 0.3f;
			}
			else
			{
				selectColor = 1.0f;
				menuTransValue = 1.0f;
			}

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, selectColor, selectColor, menuTransValue));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}


		//  プロローグ前の選択肢画面のメニュー「いいえ」
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_prologue_menu_select == 1)
			{
				selectColor = 0.0f;
				menuTransValue = sinValue + 0.3f;
			}
			else
			{
				selectColor = 1.0f;
				menuTransValue = 1.0f;
			}

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, selectColor, selectColor, menuTransValue));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// メニューの矢印（arrow）表示（赤）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		// 矢印のY座標
		float pmx = TEXTURE_MANU_ARROW_X;
		float pmy = TEXTURE_MANU_ARROW_Y;
		float pmw = TEXTURE_WIDTH_MANU_ARROW;
		float pmh = TEXTURE_HEIGHT_MANU_ARROW;

		pmy += g_prologue_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// 矢印（arrow）を回転させる

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	// プロローグの開始
	else if(g_PrologueFlg != FALSE)
	{
		// プロローグの最初を描画（ずっと表示している）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		//==========================================
		// メッセージ処理（処理開始）
		//==========================================
		{

			// プロローグメッセージ0（......のやつ）
			if (g_prologueMessageCount == 0)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグメッセージ1（....きて..のやつ）
			if (g_prologueMessageCount == 1)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグメッセージ2（....起きて..のやつ）
			if (g_prologueMessageCount == 2)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグメッセージ3（ねぇ..起きて!のやつ）
			if (g_prologueMessageCount >= 3 && g_prologueMessageCount < 5)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_prologueMessageTrans));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

		//==========================================
		// メッセージ処理（処理開始）
		//==========================================
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float px = SCREEN_WIDTH / 2;
			float py = 100.0f;
			float pw = 600.0f;		// メッセージの横幅
			float ph = 150.0f;		// メッセージの縦幅

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// プロローグキャラメッセージ0（バディ）
			if (g_prologueMessageCount >= 4 && g_prologueMessageCount <= 5 && g_prologueMessageTrans == 0.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグキャラメッセージ1（プレイヤー）
			if (g_prologueMessageCount == 5)
			{
				py = 400.0f;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグキャラメッセージ2（バディ）
			if (g_prologueMessageCount >= 6 && g_prologueMessageCount <= 7)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// プロローグキャラメッセージ3（プレイヤー）
			if (g_prologueMessageCount == 7)
			{
				py = 400.0f;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}

//=============================================================================
// プロローグが必要かどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetPrologueFlg(void)
{
	return g_PrologueFlg;
}

void SetPrologueFlg(BOOL prologueFlg)
{
	g_PrologueFlg = prologueFlg;
}





