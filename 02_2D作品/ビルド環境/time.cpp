//=============================================================================
//
// タイム処理 [time.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "time.h"
#include "sprite.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// キャラサイズ
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数
#define TIME_INITE					(300)	// タイムの初期値（開発中は999）
#define FRAME_RATE					(60)	// フレーム数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/number16_32_original.png",
};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Time;						// タイムの秒数
static int						g_TimeLittleFlg;			// 残りタイムが少ないときのフラグ
static int						g_TimeLittleSoundFlg;		// 残りタイムが少ないときに音をならすフラグ
static float					g_LittleSoundTime;			// 残りタイムが少ないときに音をならすフラグ
static int						g_TimeCount;				// 60フレームから1秒を取得する

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTime(void)
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


	// タイムの初期化
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 930.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Time = TIME_INITE;
	g_TimeCount = 0;		// タイムカウントを0にする
	g_TimeLittleFlg = 0;
	g_LittleSoundTime = 0.0f;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTime(void)
{
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTime(void)
{
	g_TimeCount += 1;
	if (g_TimeLittleFlg == 1)
	{
		g_LittleSoundTime += 0.16f;
		if (g_LittleSoundTime > 1.0f)
		{
			g_LittleSoundTime = 1.0f;
		}
	}
	if (g_TimeCount == FRAME_RATE)
	{
		g_Time -= 1;
		g_TimeCount = 0;
	}
	// タイムが残り少なくなったときフラグを立てる。音も鳴らす
	if (g_Time <= 60)
	{
		g_TimeLittleFlg = 1;
	}
	if (g_TimeLittleFlg == 1 && g_LittleSoundTime < 1.0f)
	{
		PlaySound(SOUND_LABEL_SE_10_TIMELOW);
	}
	// タイムが0になったとき
	if (g_Time < 0)
	{
		// ゲームオーバーへ
		SetMode(MODE_GAMEOVER);
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTime(void)
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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数分処理する
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// タイムの位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// タイムの表示位置X
		float py = g_Pos.y;			// タイムの表示位置Y
		float pw = g_w;				// タイムの表示幅
		float ph = g_h;				// タイムの表示高さ

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		float redColor = 1.0f;		// 文字の色を変更する変数


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		if (g_TimeLittleFlg == 1) redColor = 0.0f;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, redColor, redColor, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


//=============================================================================
// タイムを加算する
// 引数:add :追加する時間。マイナスも可能
//=============================================================================
void AddTime(int add)
{
	g_Time += add;
	if (g_Time > TIME_MAX)
	{
		g_Time = TIME_MAX;
	}

}


int GetTime(void)
{
	return g_Time;
}


void SetTime(int time)
{
	g_Time = time;
}


