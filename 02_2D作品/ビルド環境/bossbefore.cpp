//=============================================================================
//
// ボス前の会話画面処理 [bossbefore.cpp]
// Author : 
//
//=============================================================================
#include "bossbefore.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(20)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO				(480)			// ロゴサイズ（幅）
#define TEXTURE_HEIGHT_LOGO				(100)			// ロゴサイズ（縦） 
#define TEXTURE_WIDTH_LOGO_MANU			(200)			// ロゴサイズ（幅）
#define TEXTURE_HEIGHT_LOGO_MANU		(60)			// ロゴサイズ（縦） 
#define TEXTURE_WIDTH_MANU_ARROW		(60)			// メニュー矢印のサイズ（幅） 
#define TEXTURE_HEIGHT_MANU_ARROW		(60)			// メニュー矢印のサイズ（縦）

#define TEXTURE_MANU_ARROW_X			(350)			// メニュー矢印のX座標 
#define TEXTURE_MANU_ARROW_Y			(320)			// メニュー矢印のY座標（初期値）


#define TEXTURE_MESSAGE_WIDTH			(600)			// メッセージの横幅 
#define TEXTURE_MESSAGE_HEIGHT			(150)			// メッセージの縦幅
#define TEXTURE_MESSAGE_PB_X			(450)			// メッセージ（バディとプレイヤー）のX座標の表示値 
#define TEXTURE_MESSAGE_MAN_X			(510)			// メッセージ（男）のX座標の表示値 
#define TEXTURE_MESSAGE_Y1				(100)			// メッセージのY座標の表示値 
#define TEXTURE_MESSAGE_Y2				(270)			// メッセージのY座標の表示値 
#define TEXTURE_MESSAGE_Y3				(440)			// メッセージのY座標の表示値 

#define TEXTURE_BOSS_Y					(220)			// メッセージのY座標の表示値 
#define TEXTURE_BOSS_MESSAGE_Y			(450)			// ボスのメッセージのY座標の表示値 
#define TEXTURE_BOSS_WIDTH				(760)			// ボスの横幅
#define TEXTURE_BOSS_HEIGHT				(450)			// ボスの高さ

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
	"data/TEXTURE/BossBefore/bossBefore_logo.png",				// 1_モードに入る前の選択画面のロゴ_タイトル
	"data/TEXTURE/tutorial_logo_1_yes.png",						// 2_モードに入る前の選択画面のロゴ_はい
	"data/TEXTURE/tutorial_logo_2_no.png",						// 3_モードに入る前の選択画面のロゴ_いいえ
	"data/TEXTURE/title_menu_arrow.png",						// 4_メニュー選択の矢印
	"data/TEXTURE/BG/bg_boss1.png",								// 5_ボス前の会話の背景
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_0.png",	// 6_ボス前の会話のメッセージ0
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_1.png",	// 7_ボス前の会話のメッセージ1
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_2.png",	// 8_ボス前の会話のメッセージ2
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_3.png",	// 9_ボス前の会話のメッセージ3
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_4.png",	// 10_ボス前の会話のメッセージ4
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_5.png",	// 11_ボス前の会話のメッセージ5
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_6.png",	// 12_ボス前の会話のメッセージ6
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_7.png",	// 13_ボス前の会話のメッセージ7
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_8.png",	// 14_ボス前の会話のメッセージ8
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_9.png",	// 15_ボス前の会話のメッセージ9
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_10.png",	// 16_ボス前の会話のメッセージ10
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_11.png",	// 17_ボス前の会話のメッセージ11（ボスの咆哮）
	"data/TEXTURE/BossBefore/bossBefore_boss.png",				// 18_ボス前の会話のメッセージ12（ボスの姿）
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号


static BOOL						g_Load = FALSE;
static BOOL						g_BossBeforeFlg = FALSE;	// ボス前の会話を実行するかのフラグ
static BOOL						g_BossVoiceFlg = FALSE;		// ボスが咆哮したかのフラグ
int g_BossBefore_menu_select;									// モードに入る前の選択肢用変数
float g_bossBeforeTimeCount;									// ボス前の会話画面のタイム変数
int g_bossBeforeMessageCount;									// ボス前の会話画面のメッセージを透明にする変数
float g_bossBeforeMessageTrans = 1.0f;
float g_bossBeforeBossTrans = 0.0f;
float g_bossBossVoice = 0.0f;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBossBefore(void)
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
	g_BossBefore_menu_select = 0;
	g_bossBeforeTimeCount = 0;
	g_bossBeforeMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBossBefore(void)
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
void UpdateBossBefore(void)
{
	g_bossBeforeTimeCount += 1.0f;
	g_bossBossVoice += 0.01f;

	if (g_bossBossVoice > 1.0f)
	{
		g_bossBossVoice = 1.0f;
	}

	// 徐々に見えてくる
	if (g_bossBeforeMessageCount >= 11)
	{
		g_bossBeforeBossTrans += 0.01f;
	}

	// 徐々に透明になる
	if (GetBossBeforeFlg() == TRUE)
	{
		g_bossBeforeMessageTrans -= 0.005f;
		if (g_bossBeforeMessageTrans < 0.0f)
		{
			g_bossBeforeMessageTrans = 0.0f;
		}
	}

	// ボス前の会話中にEnterをおしたとき
	if (GetBossBeforeFlg() == TRUE)
	{
		// エンターキーが押されたとき（ボス前の会話中のEnter音）
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}
	// ボスの咆哮
	if ((!g_BossVoiceFlg) && g_bossBeforeMessageCount == 11)
	{
		PlaySound(SOUND_LABEL_SE_16_BOSSVOICE);
		g_BossVoiceFlg = TRUE;
	}

	if ((GetBossBeforeFlg() == TRUE) && GetKeyboardTrigger(DIK_RETURN))
	{
		// ボス前の会話のメッセージカウントを上げる
		// カウントを上げさせない
		if (g_bossBeforeMessageTrans == 0.0f)
		{
			g_bossBeforeMessageCount++;
		}
	}

	// ボス前の会話の最後まで行ったらモードをボスへ
	if (GetKeyboardTrigger(DIK_RETURN) && g_bossBeforeMessageCount >= 12)
	{		
		// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_BOSS);
	}

	// ボス前の会話を実施するかの処理（フラグがTRUEになったら消える）
	if (GetBossBeforeFlg() == FALSE)
	{
		// サウンドの処理
		{
			// エンターキーが押されたとき（ボス前の会話の前のEnter音）
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
			if (GetKeyboardTrigger(DIK_RETURN) && g_BossBefore_menu_select == 0)
			{
				// ボス前の会話フラグをオンにする
				SetBossBeforeFlg(TRUE);
				// ボス前の会話の音を設定
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_BossBefore_menu_select == 0)
			{
				// ボス前の会話フラグをオンにする
				SetBossBeforeFlg(TRUE);
				// ボス前の会話の音を設定
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_BossBefore_menu_select == 0)
			{
				// ボス前の会話フラグをオンにする
				SetBossBeforeFlg(TRUE);
				// ボス前の会話の音を設定
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
		}

		// 選択肢が「いいえ」のとき
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_BossBefore_menu_select == 1)
			{
				// Enter押したら、ステージを切り替える
				SetFade(FADE_OUT, MODE_BOSS);
			}
			// ゲームパッドで入力処理
			else if (IsButtonTriggered(0, BUTTON_START) && g_BossBefore_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_BOSS);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_BossBefore_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_BOSS);
			}
		}

		// メニュー画面を選択したときに矢印を移動させる
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_BossBefore_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_BossBefore_menu_select -= select;
		}

		g_BossBefore_menu_select = g_BossBefore_menu_select % 2;

		if (g_BossBefore_menu_select < 0)
		{
			g_BossBefore_menu_select = 1;
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("g_bossBeforeMessageCount:%d,\n", g_bossBeforeMessageCount);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBossBefore(void)
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
	float sinValue = (sin(g_bossBeforeTimeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる

	// チュートリアルをするかの確認画面
	if (g_BossBeforeFlg == FALSE)
	{
		// ボス前の会話前の選択肢画面のBG表示
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
		float pw = 800.0f;		// ロゴの横幅
		float ph = 130.0f;		// ロゴの縦幅

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// ボス前の会話前の選択肢画面のタイトル
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
		//  ボス前の会話前の選択肢画面のメニュー「はい」
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_BossBefore_menu_select == 0)
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


		//  ボス前の会話前の選択肢画面のメニュー「いいえ」
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_BossBefore_menu_select == 1)
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

		pmy += g_BossBefore_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// 矢印（arrow）を回転させる

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
	// ボス前の会話の場面開始
	else if(g_BossBeforeFlg != FALSE)
	{
		//==========================================
		// ボス前の会話の場面の処理
		//==========================================
		// ボス前の会話の背景を描画（ずっと表示している）
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// ボス前の会話の黒い背景を描画（最初だけ表示。時間ともに消える）
		if (g_bossBeforeMessageCount == 0)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_bossBeforeMessageTrans));
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		//==========================================
		// メッセージ処理
		//==========================================
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float px = SCREEN_WIDTH / 2;
			float py = TEXTURE_MESSAGE_Y1;
			float pw = TEXTURE_MESSAGE_WIDTH;		// メッセージの横幅
			float ph = TEXTURE_MESSAGE_HEIGHT;		// メッセージの縦幅

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// ボス前の会話メッセージ0（バディ「とうちゃく」）
			if (g_bossBeforeMessageCount >= 0 && g_bossBeforeMessageCount <= 1 && g_bossBeforeMessageTrans == 0.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ1（プレイヤー「....」）
			if (g_bossBeforeMessageCount == 1)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ2（男「おせーよ」）
			if (g_bossBeforeMessageCount >= 2 && g_bossBeforeMessageCount <= 4)
			{
				px = TEXTURE_MESSAGE_MAN_X;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ3（プレイヤー「？？？」）
			if (g_bossBeforeMessageCount >= 3 && g_bossBeforeMessageCount <= 4)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ3（男「クスリ打たんかったん？」）
			if (g_bossBeforeMessageCount == 4)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y3;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ4（バディ「落としちゃった」）
			if (g_bossBeforeMessageCount >= 5 && g_bossBeforeMessageCount <= 7)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ5（男「犬でも...死ね」）
			if (g_bossBeforeMessageCount >= 6 && g_bossBeforeMessageCount <= 7)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// TODO：音鳴らす（Updateでやるべき？）

			}
			// ボス前の会話キャラメッセージ7（バディ「え？」）
			if (g_bossBeforeMessageCount == 7)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y3;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ8（プレイヤー「...!!!」）
			if (g_bossBeforeMessageCount >= 8 && g_bossBeforeMessageCount <= 9)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// TODO：音鳴らす（Updateでやるべき？）

			}
			// ボス前の会話キャラメッセージ9（男「チィ！！やっぱり...灰にしてやる!」）
			if (g_bossBeforeMessageCount == 9)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話キャラメッセージ10（男「こい！最高傑作！」）
			if (g_bossBeforeMessageCount == 10)
			{
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話のボス（正弦波で揺らすやつ）
			if (g_bossBeforeMessageCount >= 11)
			{
				px = SCREEN_WIDTH * 0.5;
				py = TEXTURE_BOSS_Y;
				pw = TEXTURE_BOSS_WIDTH;
				ph = TEXTURE_BOSS_HEIGHT;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_bossBeforeBossTrans));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			// ボス前の会話ボスメッセージ11（ボス「グォオオオ・・・___・・・（ここ実はモールス信号にしている）」）
			if (g_bossBeforeMessageCount >= 11)
			{
				py = TEXTURE_BOSS_MESSAGE_Y;
				pw = TEXTURE_MESSAGE_WIDTH;			// メッセージの横幅
				ph = TEXTURE_MESSAGE_HEIGHT;		// メッセージの縦幅

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}

//=============================================================================
// ボス前の会話が必要かどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetBossBeforeFlg(void)
{
	return g_BossBeforeFlg;
}

void SetBossBeforeFlg(BOOL bossBeforeFlg)
{
	g_BossBeforeFlg = bossBeforeFlg;
}





