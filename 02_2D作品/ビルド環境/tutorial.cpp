//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "tutorial.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(22)				// テクスチャの数

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
	"data/TEXTURE/bg_0.png",							// 0_モードに入る前の選択画面のBG
	"data/TEXTURE/Tutorial/tutorial_logo_0.png",		// 1_モードに入る前の選択画面のロゴ_タイトル
	"data/TEXTURE/tutorial_logo_1_yes.png",				// 2_モードに入る前の選択画面のロゴ_はい
	"data/TEXTURE/tutorial_logo_2_no.png",				// 3_モードに入る前の選択画面のロゴ_いいえ
	"data/TEXTURE/title_menu_arrow.png",				// 4_メニュー選択の矢印
	"data/TEXTURE/bg_tutorial.png",						// 5_チュートリアル画面のBG
	"data/TEXTURE/Tutorial/tutorial_message_1.png",		// 6_チュートリアル画面のメッセージ1
	"data/TEXTURE/Tutorial/tutorial_message_2.png",		// 7_チュートリアル画面のメッセージ2
	"data/TEXTURE/Tutorial/tutorial1_Keyboard1.png",	// 8_チュートリアル画面のキーボード表示（カーソルキー）
	"data/TEXTURE/arrow_down.png",						// 9_チュートリアル画面の矢印表示
	"data/TEXTURE/Tutorial/tutorial_message_3.png",		// 10_チュートリアル画面のメッセージ3
	"data/TEXTURE/Tutorial/tutorial1_Keyboard2.png",	// 11_チュートリアル画面のキーボード表示（J）
	"data/TEXTURE/Tutorial/tutorial_message_4.png",		// 12_チュートリアル画面のメッセージ4
	"data/TEXTURE/Tutorial/tutorial_message_5.png",		// 13_チュートリアル画面のメッセージ5
	"data/TEXTURE/Tutorial/tutorial1_Keyboard3.png",	// 14_チュートリアル画面のキーボード表示（SPACE）
	"data/TEXTURE/Tutorial/tutorial_message_6.png",		// 15_チュートリアル画面のメッセージ6
	"data/TEXTURE/Tutorial/tutorial_message_7.png",		// 16_チュートリアル画面のメッセージ7
	"data/TEXTURE/Tutorial/tutorial1_Keyboard4.png",	// 17_チュートリアル画面のキーボード表示（H）
	"data/TEXTURE/Tutorial/tutorial_message_8.png",		// 18_チュートリアル画面のメッセージ8
	"data/TEXTURE/Tutorial/tutorial_message_9.png",		// 19_チュートリアル画面のメッセージ9
	"data/TEXTURE/Tutorial/tutorial_message_10.png",	// 20_チュートリアル画面のメッセージ10
	"data/TEXTURE/Tutorial/tutorial_message_11.png",	// 21_チュートリアル画面のメッセージ11
};

static BOOL						g_PointToPlayer;			// ポイントに到達したかを確認するフラグ
static BOOL						g_Point0;					// チュートリアルを行ったのかを確認するフラグ（初期表示）
static BOOL						g_Point1;					// チュートリアルを行ったのかを確認するフラグ（ジャンプ）
static BOOL						g_Point2;					// チュートリアルを行ったのかを確認するフラグ（エネミーを倒す）
static BOOL						g_Point3;					// チュートリアルを行ったのかを確認するフラグ（回復）
static BOOL						g_Point4;					// チュートリアルを行ったのかを確認するフラグ（扉でEnter）
static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号


static BOOL						g_Load = FALSE;
static BOOL						g_TutorialFlg = FALSE;		// チュートリアルを実行するかのフラグ
int g_tutorial_menu_select;									// モードに入る前の選択肢用変数
int g_tutorialMessageCount;									// チュートリアル画面のメッセージウィンドウ数

static TUTORIAL	g_Tutorial;
float g_tutorialTimeCount;									// チュートリアル画面のタイム変数
static XMFLOAT3 g_PlayerInitPos;							// チュートリアル画面のプレイヤーの初期位置

// プレイヤーのポイント位置
int playerPointPos;
// チュートリアルポイントの位置
float g_tutorialPoint0;
float g_tutorialPoint1;
float g_tutorialPoint2;
float g_tutorialPoint3;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
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

	PLAYER* player = GetPlayer();
	// 変数の初期化
	g_PointToPlayer = FALSE;
	g_Point0 = TRUE;
	g_Point1 = TRUE;
	g_Point2 = TRUE;
	g_Point3 = TRUE;
	g_Point4 = TRUE;
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;
	playerPointPos = 0;
	g_PlayerInitPos = player->pos;		// プレイヤーの初期値を取得しておく
	g_tutorialPoint0 = g_PlayerInitPos.x;
	g_tutorialPoint1 = 576.0f;
	g_tutorialPoint2 = 768.0f;
	g_tutorialPoint3 = 1600.0f;
	g_tutorial_menu_select = 0;
	g_tutorialTimeCount = 0;
	g_tutorialMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
{
	// クリアフラグをオフにする
	SetClearFlg(FALSE);

	g_tutorialTimeCount += 1.0f;

	// チュートリアル中にEnterをおしたとき
	if (GetTutorialFlg() == TRUE)
	{
		// Enterキーが押されたとき（チュートリアル中のEnter音）
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}

	// プレイヤーの場所の確認
	PLAYER* player = GetPlayer();
	// エネミーの確認
	ENEMY* enemy = GetEnemy();
	CheckPlayerToChipNum(player->pos.y, player->pos.x);
	// チュートリアルポイントに来たかどうかの確認
	CheckPlayerTutorialPoint(player->pos.x);

	// チュートリアルポイント 0（メッセージ1、2（値は0と1））
	//if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (player->pos.x == g_PlayerInitPos.x))
	if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (g_PointToPlayer == TRUE))
	//if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (g_tutorialMessageCount <= 2))
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントが挙げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 2)
			{
				g_tutorialMessageCount = 2;
			}
		}

		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 2)
		{
			player->tutorialMove = TRUE;
			g_Point0 = FALSE;
		}

	}
	 //チュートリアルポイント1（g_tutorialMessageCountは2）
	if ((g_Point1 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint1))
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントが挙げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 3)
			{
				g_tutorialMessageCount = 3;
			}
		}
		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 3)
		{
		 	player->tutorialMove = TRUE;
			g_Point1 = FALSE;
		}
	}
	//チュートリアルポイント2（g_tutorialMessageCountは3）
	if ((g_Point2 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint2))
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントを上げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 5)
			{
				g_tutorialMessageCount = 5;
			}
		}
		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 5)
		{
			player->tutorialMove = TRUE;
			g_Point2 = FALSE;
		}
	}
	//チュートリアルポイント3（g_tutorialMessageCountは5）
	if ((g_Point3 == TRUE) && (enemy->use == FALSE))
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントが挙げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 7)
			{
				g_tutorialMessageCount = 7;
			}
		}
		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 7)
		{
			g_Point3 = FALSE;
		}
	}
	// 回復ボタンを押したとき
	if ((g_tutorialMessageCount == 7) && GetKeyboardTrigger(DIK_H))
	{
		PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
		player->hp += 100;
		player->healItem -= 1;
		if (player->hp > 100) player->hp = 100;
		player->tutorialMove = TRUE;
		playerPointPos = 3;
	}
	// 回復ボタンを押した後、message8が出ているとき
	if ((g_tutorialMessageCount == 7) && (playerPointPos == 3))
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントを上げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 8)
			{
				g_tutorialMessageCount = 8;
			}
		}
		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 8)
		{
			player->tutorialMove = TRUE;
		}
	}
	//チュートリアルポイント4。変数は3なので注意（g_tutorialMessageCountは8）
	if ((g_Point4 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint3) && enemy->use == FALSE)
	{
		// プレイヤーの動きを止める
		player->tutorialMove = FALSE;

		// Enterキーが押されたらカウントが挙げる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 11)
			{
				g_tutorialMessageCount = 11;
			}
		}
		// チュートリアルポイントの説明が終わったら
		if (g_tutorialMessageCount == 11)
		{
			player->tutorialMove = TRUE;
			g_Point4 = FALSE;
		}
	}


	// チュートリアルを実施するかの処理（フラグがTRUEになったら消える）
	if(GetTutorialFlg() == FALSE)
	{
		// サウンドの処理
		{
			// Enterキーが押されたとき（チュートリアル前のEnter音）
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
			if (GetKeyboardTrigger(DIK_RETURN) && g_tutorial_menu_select == 0)
			{
				// チュートリアルフラグをオンにする
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_tutorial_menu_select == 0)
			{
				// チュートリアルフラグをオンにする
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_tutorial_menu_select == 0)
			{
				// チュートリアルフラグをオンにする
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
		}

		// 選択肢が「いいえ」のとき
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_tutorial_menu_select == 1)
			{
				// Enter押したら、ステージを切り替える
				SetFade(FADE_OUT, MODE_GAME);
			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_tutorial_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_tutorial_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
		}

		// メニュー画面を選択したときに矢印を移動させる
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_tutorial_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_tutorial_menu_select -= select;
		}

		g_tutorial_menu_select = g_tutorial_menu_select % 2;

		if (g_tutorial_menu_select < 0)
		{
			g_tutorial_menu_select = 1;
		}
	}




#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("g_tutorialMessageCount:%d,\nplayerPointPos:%d,\n", g_tutorialMessageCount, playerPointPos);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
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
	float sinValue = (sin(g_tutorialTimeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる

	PLAYER* player = GetPlayer();
	ENEMY* enemy = GetEnemy();
	BG* bg = GetBG();

	// チュートリアルをするかの確認画面
	if (g_TutorialFlg == FALSE)
	{
		// チュートリアル前の選択肢画面のBG表示
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

		// チュートリアル前の選択肢画面のタイトル
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
		//  チュートリアル前の選択肢画面のメニュー「はい」
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_tutorial_menu_select == 0)
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


		//  チュートリアル前の選択肢画面のメニュー「いいえ」
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_tutorial_menu_select == 1)
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

		pmy += g_tutorial_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// 矢印（arrow）を回転させる

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}
	// チュートリアルの開始
	else if(g_TutorialFlg != FALSE)
	{
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		float px = SCREEN_WIDTH / 2;
		float py = 100.0f;
		float pw = 600.0f;		// ロゴの横幅
		float ph = 150.0f;		// ロゴの縦幅

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		//==========================================
		// メッセージ処理（処理開始）
		//==========================================
		// チュートリアルメッセージ1
		{
			// チュートリアルポイント0メッセージ1
			if (g_tutorialMessageCount == 0)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント0メッセージ2
			if (g_tutorialMessageCount == 1)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント1メッセージ
			if (g_tutorialMessageCount == 2 && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント2メッセージ
			if (g_tutorialMessageCount == 3 && (playerPointPos == 2))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}
			// チュートリアルポイント2メッセージ
			if (g_tutorialMessageCount == 4 && (playerPointPos == 2))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント3メッセージ1
			if (g_tutorialMessageCount == 5 && (enemy->use == FALSE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント3メッセージ2
			if (g_tutorialMessageCount == 6 && (enemy->use == FALSE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント4メッセージ1（message_8）
			if (g_tutorialMessageCount == 7 && (playerPointPos == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント5メッセージ1（message_9）
			if (g_tutorialMessageCount == 8 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[19]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント5メッセージ2（message_10）
			if (g_tutorialMessageCount == 9 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// チュートリアルポイント5メッセージ3（message_11）
			if (g_tutorialMessageCount == 10 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[21]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}

		//==========================================
		// キーボードの表示
		//==========================================
		{
			// 移動とダッシュの表示（チュートリアル0）
			if ((g_tutorialMessageCount == 2) && (g_Point0 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= 64.0f))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				//float px = 0 - bg->pos.x + g_PlayerInitPos.x;
				float px = 0 - bg->pos.x + 200.0f;
				float py = 0 - bg->pos.y + 230.0f;
				float pw = 150.0f;		// 画像の横幅
				float ph = 200.0f;		// 画像の縦幅

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ジャンプ（J）の表示（チュートリアル1）
			//if ((g_tutorialMessageCount == 2) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= g_tutorialPoint2))
			if ((g_tutorialMessageCount == 3) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				float px = 0 - bg->pos.x + 490.0f;
				float py = 0 - bg->pos.y + 340.0f;
				float pw = 100.0f;		// 画像の横幅
				float ph = 100.0f;		// 画像の縦幅

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// こうげき（SPACE）の表示（チュートリアル2）
			if ((g_tutorialMessageCount == 5) && (g_Point2 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 2) && (enemy->use == TRUE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

				float px = 0 - bg->pos.x + 1000.0f;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = 100.0f;		// 画像の横幅
				float ph = 100.0f;		// 画像の縦幅

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// かいふく（H）の表示（チュートリアル3）
			if ((g_tutorialMessageCount == 7) && (g_Point3 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 2) && (enemy->use == FALSE) && (player->healItem == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

				float px = 0 - bg->pos.x + 1100.0f;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = 100.0f;		// 画像の横幅
				float ph = 100.0f;		// 画像の縦幅

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
		//==========================================
		// 矢印の表示
		//==========================================
		{
			// ポイント1のところの表示
			if ((g_tutorialMessageCount == 2) && (g_Point0 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= 64.0f))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint1;
				float py = 0 - bg->pos.y + 448.0f;
				float pw = MAPCHIP_WIDTH;		// 画像の横幅
				float ph = MAPCHIP_HEIGHT;		// 画像の縦幅

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ポイント2のところの表示
			if ((g_tutorialMessageCount == 3) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint2;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = MAPCHIP_WIDTH;		// 画像の横幅
				float ph = MAPCHIP_HEIGHT;		// 画像の縦幅

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ポイント3のところの表示
			if ((g_tutorialMessageCount == 8) && (g_Point3 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint3;
				float py = 0 - bg->pos.y + 448.0f;
				float pw = MAPCHIP_WIDTH;		// 画像の横幅
				float ph = MAPCHIP_HEIGHT;		// 画像の縦幅

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}

//=============================================================================
// Tutorial構造体の先頭アドレスを取得
//=============================================================================
TUTORIAL* GetTutorial(void)
{
	return &g_Tutorial;
}

//=============================================================================
// チュートリアルポイントに来たかどうかの判断をする
//=============================================================================
void CheckPlayerTutorialPoint(float playerPosX)
{
	// チェックポイント0（初期位置）
	if (playerPosX == g_tutorialPoint0 && (g_Point0 == TRUE))
	{
		playerPointPos = 0;
		SetPLayerToPoint(TRUE);
	}
	else if(playerPosX == g_tutorialPoint1 && (g_Point1 == TRUE))
	{
		playerPointPos = 1;
		SetPLayerToPoint(TRUE);
	}
	else if (playerPosX == g_tutorialPoint2 && (g_Point2 == TRUE))
	{
		playerPointPos = 2;
		SetPLayerToPoint(TRUE);
	}
	else if (playerPosX == g_tutorialPoint3 && (g_Point4 == TRUE) && g_tutorialMessageCount >= 8)
	{
		playerPointPos = 4;		// 3は前の段階（回復）で使用済
		SetPLayerToPoint(TRUE);
	}
	else
	{
		SetPLayerToPoint(FALSE);
	}
}

//=============================================================================
// チュートリアルが必要かどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetTutorialFlg(void)
{
	return g_TutorialFlg;
}

void SetTutorialFlg(BOOL tutorialFlg)
{
	g_TutorialFlg = tutorialFlg;
}

//=============================================================================
// プレイヤーがチュートリアルポイントにいるかどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetPLayerToPoint(void)
{
	return g_PointToPlayer;
}

void SetPLayerToPoint(BOOL PointToPlayer)
{
	g_PointToPlayer = PointToPlayer;
}

//=============================================================================
// チュートリアル中のプレイヤーの動きを制限する（アクセサ関数）
//=============================================================================
int GetTutorialMessageCount(void)
{
	return g_tutorialMessageCount;
}




