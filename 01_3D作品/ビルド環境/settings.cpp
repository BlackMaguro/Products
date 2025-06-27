//=============================================================================
//
// 設定画面処理 [settings.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "settings.h"
#include "input.h"
#include "fade.h"
#include "file.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO					(240)			// タイトルロゴサイズ（横）
#define TEXTURE_HEIGHT_LOGO					(120)			// タイトルロゴサイズ（縦） 
#define TEXTURE_WIDTH_LOGO_SETTINGS			(200)			// ロゴサイズ（横）
#define TEXTURE_HEIGHT_LOGO_SETTINGS		(60)			// ロゴサイズ（縦）
#define TEXTURE_LOGO_SETTINGS				(80)			// ロゴ（三角形）サイズ
#define TEXTURE_HEIGHT_LOGO_SETTINGS_X		(200)			// 各種メニューロゴのX座標（初期位置） 
#define TEXTURE_HEIGHT_LOGO_SETTINGS_Y		(200)			// 各種メニューロゴのY座標（初期位置） 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/07_SETTINGS/bg_settings.png",
	"data/TEXTURE/07_SETTINGS/settings_logo1_title.png",
	"data/TEXTURE/07_SETTINGS/settings_logo2_BGM.png",
	"data/TEXTURE/07_SETTINGS/settings_logo3_SE.png",
	"data/TEXTURE/07_SETTINGS/settings_logo4_modoru.png",
	"data/TEXTURE/07_SETTINGS/settings_logo5_triangle.png",
	"data/TEXTURE/07_SETTINGS/settings_logo6_soundMark.png",
	"data/TEXTURE/number16_32_original.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号


static BOOL						g_Load = FALSE;

static int g_settingsSelect;					// 設定画面の大項目を選択するための変数
static int g_settingsSelectSide;				// 設定画面の音量や音を鳴らす選択をするための変数
static int g_settingsVolumeNumFront;			// 設定画面の見た目上のボリューム値
static int g_settingSelectBGMFlag;
static float g_volume;							// サウンドの音量情報

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSettings(void)
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

	g_settingsSelect = 0;
	g_settingsSelectSide = 2;			// 設定画面の横方向のセレクト変数
	g_settingSelectBGMFlag = 0;

	// サウンドの音量を設定（初期値は1.0f）
	int ans = CheckSettingsFile();
	if (ans == 1)
	{
		// 設定情報あり
		g_volume = GetVolume();
		g_settingsVolumeNumFront = GetVolumeNum();
	}
	else
	{
		// 設定情報なし
		g_volume = SOUND_VOLUME_MAX;							// 設定画面の実際の音量変数（初期値）
		g_settingsVolumeNumFront = SETTINGS_VOLUME_FRONT_MAX;	// 設定画面の見た目上の音量変数（初期値）
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSettings(void)
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
void UpdateSettings(void)
{
	// サウンド関係の処理
	{
		// エンターキーが押されたとき
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_01_ENTER);
		}
		// カーソルの左右キーを押したとき（ここだけ左右にする）
		if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_RIGHT))
		{
			PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
		}
	}
	// もどるにカーソルがあり、Eneterを押下したとき
	if (GetKeyboardTrigger(DIK_RETURN) && (g_settingsSelect == 1))
	{
		// 設定情報の保存
		SaveSettings();
		// ステージを切り替える
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START) && (g_settingsSelect == 1))
	{
		SaveSettings();
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B && (g_settingsSelect == 1)))
	{
		SaveSettings();
		SetFade(FADE_OUT, MODE_TITLE);
	}


	// 設定画面の各項目に移動したときに赤色を移動させる
	int select = 1;
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_settingsSelect += select;
	}
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_settingsSelect -= select;
	}

	// 「BGM」でEnterを押下したとき文字を赤くするためのフラグ
	if ((g_settingsSelect == 0) && GetKeyboardTrigger(DIK_RETURN))
	{
		g_settingSelectBGMFlag = 1;
	}
	else if (g_settingsSelect != 0)
	{
		g_settingSelectBGMFlag = 0;
		g_settingsSelectSide = 2;		// 初期値に戻す
	}

	// 設定画面の各項目で選択したときに数字を変化させる
	// 音量を選択時
	if (g_settingSelectBGMFlag == 1)
	{
		if (GetKeyboardTrigger(DIK_LEFT))
		{
			g_settingsSelectSide -= select;
		}
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			g_settingsSelectSide += select;
		}
		if ((g_settingsSelectSide == 0) && GetKeyboardTrigger(DIK_RETURN))
		{
			g_settingsVolumeNumFront -= select;
		}
		if ((g_settingsSelectSide == 1) && GetKeyboardTrigger(DIK_RETURN))
		{
			g_settingsVolumeNumFront += select;
		}
		// スピーカーマークを押下したとき（音を鳴らす）
		if ((g_settingsSelectSide == 2) && GetKeyboardTrigger(DIK_RETURN))
		{
			StopSound();
			PlaySound(SOUND_LABEL_BGM_05_SETTINGS);
		}
	}

	g_settingsSelect %= 2;
	g_settingsSelectSide %= 3;
	g_settingsVolumeNumFront %= 9;
	// 0を下回ったら
	if (g_settingsSelect < 0) g_settingsSelect = 1;
	if (g_settingsSelectSide < 0) g_settingsSelectSide = 2;
	if (g_settingsVolumeNumFront < 0) g_settingsVolumeNumFront = SETTINGS_VOLUME_FRONT_MAX;
	// volumeの設定値の変更
	g_volume = (float)0.125f * g_settingsVolumeNumFront;


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("volume:%f\n",g_volume);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSettings(void)
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

	// 設定画面の背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 設定画面_タイトル表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	float tx = TEXTURE_WIDTH / 2;
	float ty = 70;

	SetSpriteColor(g_VertexBuffer, tx, ty, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	//==========================================
	// 設定画面のメニュー処理（処理開始）
	//==========================================
	// 選択されていた場合、文字を赤くする
	float selectColor = 1.0f;
	// 設定画面_ロゴ_BGM表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	float tsx = TEXTURE_HEIGHT_LOGO_SETTINGS_X;
	float tsy = TEXTURE_HEIGHT_LOGO_SETTINGS_Y;
	if ((g_settingsSelect == 0))
	{
		selectColor = 0.0f;
	}
	else
	{
		selectColor = 1.0f;
	}
	SetSpriteColor(g_VertexBuffer, tsx, tsy, TEXTURE_WIDTH_LOGO_SETTINGS, TEXTURE_HEIGHT_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, selectColor, selectColor, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
	
	//==========================================
	// 設定画面のメニュー処理（処理完了）
	//==========================================


	// 設定画面_ロゴ_もどる表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	// X座標はタイトルと同じ。Y座標のみ変更
	tsy = 450;
	if (g_settingsSelect == 1)
	{
		selectColor = 0.0f;
	}
	else
	{
		selectColor = 1.0f;
	}

	SetSpriteColor(g_VertexBuffer, tx, tsy, TEXTURE_WIDTH_LOGO_SETTINGS, TEXTURE_HEIGHT_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, selectColor, selectColor, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	//==========================================
	// 設定画面の音量の三角形の処理（処理開始）
	//==========================================
	{
		// ポリゴンのテクスチャカラー
		// 通常時は灰色にしておく（非活性化）
		float selectColorVolumeR = 0.5f;
		float selectColorVolumeG = 0.5f;
		float selectColorVolumeB = 0.5f;
		float selectColorVolumeT = 0.5f;

		// 設定画面_ロゴ_BGM_TriangleLeft表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// X座標のみ変更
		float bgmTriangleX = 500;
		float bgmTriangleY = TEXTURE_HEIGHT_LOGO_SETTINGS_Y;


		// 活性化させる。
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 0)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColorRotation(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT), 3.14f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 設定画面_ロゴ_BGM_TriangleRight表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// X座標のみ変更
		bgmTriangleX = 700;
		// 活性化させる。かつ右が選択されている状態
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 1)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 設定画面_ロゴ_BGM_soundMark表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// X座標のみ変更
		bgmTriangleX = 800;
		// 活性化させる。かつ音マークが選択されている状態
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 2)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 設定画面_BGM_ボリュームナンバー表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		int number = g_settingsVolumeNumFront;
		float num = (float)(number % 10);

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = num * tw;		// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		// X座標のみ変更
		float bgmNumberX = 600;
		if ((g_settingsSelect == 0) && (g_settingSelectBGMFlag == 1))
		{
			selectColorVolumeR = 1.0f;
			selectColorVolumeG = 1.0f;
			selectColorVolumeB = 1.0f;
			//selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmNumberX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, tx, ty, tw, th,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}

//=============================================================================
// 音量情報のGet、Set
//=============================================================================
// 実際の音量値の取得
float GetVolume(void) 
{
	return g_volume;
}

// XAoudio2のメソッド(SetVolume)と被らないようにする
void SetSoundVolume(float volume) 
{
	g_volume = volume;
}

// 設定画面に表示用の音量値の取得
int GetVolumeNum(void)
{
	return g_settingsVolumeNumFront;
}

// XAoudio2のメソッド(SetVolume)と被らないようにする
void SetVolumeNum(int volumeNum)
{
	g_settingsVolumeNumFront = volumeNum;
}

// 設定情報を改ざんされていたときの処理
float CheckVolume(float volume) 
{
	if (volume > SOUND_VOLUME_MAX)	// 最大値より大きいとき
	{
		volume = 1.0f;
	}
	else if (volume < 0)	// 0より小さいとき
	{
		volume = 0.0f;
	}
	return volume;
}




