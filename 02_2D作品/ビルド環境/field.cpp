//=============================================================================
//
// フィールド処理 [field.cpp]
// Author : 
//
//=============================================================================
#include "field.h"
#include "player.h"
#include "tutorial.h"
#include "settings.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(10)			// テクスチャの数
#define STAGE_MAX					(1)				// ステージの最大数

#define MAPCHIP_TEXTURE_WIDTH		(512)			// マップチップテクスチャの横幅
#define MAPCHIP_TEXTURE_HEIGHT		(704)			// マップチップテクスチャの縦幅
#define TEXTURE_PATTERN_DIVIDE_X	(8)				// テクスチャの横幅の分割数
#define TEXTURE_PATTERN_DIVIDE_Y	(11)			// テクスチャの縦幅の分割数

#define FIELD_SIZE_MAPCHIP_WIDTH			(FIELD_WIDTH/MAPCHIP_WIDTH)				// フィールドに対する横幅のチップ数（値はチップ数）
#define FIELD_SIZE_MAPCHIP_HEIGHT			(FIELD_HEIGHT/MAPCHIP_HEIGHT)			// フィールドに対する縦幅のチップ数（値はチップ数）
#define FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH	(FIELD_TUTORIAL_WIDTH/MAPCHIP_WIDTH)	// フィールドに対する横幅のチップ数（値はチップ数）
#define FIELD_SIZE_BOSS_MAPCHIP_WIDTH		(FIELD_BOSS_WIDTH/MAPCHIP_WIDTH)		// フィールドに対する横幅のチップ数（値はチップ数）

//#define TEXTURE_PATTERN_DEVIDE_HEIGHT	()				// マップチップの横の分割数
//#define TEXTURE_PATTERN_DEVIDE_WIDTH	()				// マップチップの横の分割数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int mapNum;			// マップNo.
static int g_ScreenDispX;	// スクリーンのX座標
static int g_ScreenDispY;	// スクリーンのY座標
int g_chipX;				// チップのX座標
int g_chipY;				// チップのY座標
PLAYER* player;				// プレイヤーへのアクセス用変数

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Feild/block_black.png",
	"data/TEXTURE/Feild/block_1_ash.png",
	"data/TEXTURE/Feild/block_2_woodBox.png",
	"data/TEXTURE/Feild/block_3_woodBox.png",
	"data/TEXTURE/Feild/block_4_woodBox.png",
	"data/TEXTURE/Feild/block_5_futureBox.png",
	"data/TEXTURE/Feild/block_6_future_arrowBox.png",
	"data/TEXTURE/Feild/block_7_door_future.png",
	"data/TEXTURE/Keyboard/EnterKey.png",
	"data/TEXTURE/arrow_down.png",
};

char g_Map_Tutorial[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH + 1] =
{
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,8,0,5},
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

char g_Map[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_MAPCHIP_WIDTH + 1] =
{
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,0,0,0,0,0,0,0,0,0,6,6,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,5,0,0,0,0,0,5,0,0,0,0,0,5,0,0,0,3,0,0,0,5,0,3,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,6,0,6,0,0,0,6},
	{ 3,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,3,0,0,0,4,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,4,0,0,5,0,5,0,0,3,0,3,0,0,0,3,0,0,0,5,0,0,0,0,0,5,0,5,0,0,0,7,7,0,0,0,0,0,0,6,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,9,0,6},
	{ 3,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,3,0,0,0,0,4,0,0,0,0,0,3,0,0,3,3,5,5,5,5,5,5,5,5,5,5,5,5,3,3,3,3,0,0,0,0,0,4,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,7,7,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,6},
	{ 3,3,5,3,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,0,0,0,4,0,0,3,0,3,3,0,3,3,3,5,5,5,5,5,5,5,5,5,3,5,5,3,3,3,3,3,0,0,0,0,4,0,5,5,5,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,3,3,0,0,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,0,0,0,8,0,6},
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

char g_Map_BOSS[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_BOSS_MAPCHIP_WIDTH + 1] =
{
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

static FIELD	g_Field;

float g_TimeCount;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitField(void)
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
	g_Field.w     = FIELD_WIDTH;
	if (GetMode() == MODE_TUTORIAL) g_Field.w = FIELD_TUTORIAL_WIDTH;
	if (GetBossFlg() == TRUE) g_Field.w = FIELD_BOSS_WIDTH;
	g_Field.h     = FIELD_HEIGHT;
	g_Field.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.texNo = 0;
	g_Field.fieldNo = GetMode();
	g_Field.goalFlg = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
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
void UpdateField(void)
{
	g_TimeCount++;
	//g_Field.old_pos = g_Field.pos;	// １フレ前の情報を保存
	g_ScreenDispX = (int)((int)g_Field.pos.x % MAPCHIP_WIDTH + g_Field.pos.x - (int)g_Field.pos.x);
	g_ScreenDispY = (int)((int)g_Field.pos.y % MAPCHIP_HEIGHT + g_Field.pos.y - (int)g_Field.pos.y);

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Field pos.x:%f,pos.y:%f,\n", g_Field.pos.x, g_Field.pos.y);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
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

	player = GetPlayer();
	// sin値の取得
	float sinValue = (sin(g_TimeCount * 0.1f) + 1) / 2;	// 0.1fを掛けて緩やかに変化させる
	// マップチップの横幅（チップ数）
	int fieldWeightChipNum = FIELD_SIZE_MAPCHIP_WIDTH;

	// チュートリアルならば描画を変更
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if (g_Field.fieldNo == MODE_TUTORIAL)
	{
		fieldWeightChipNum = FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH;
	}

	// ボス戦であれば横幅の描画を変える
	if (GetBossFlg() == TRUE)
	{
		fieldWeightChipNum = FIELD_BOSS_WIDTH / MAPCHIP_WIDTH;
	}
	// マップチップの描画
	for (int y = 0; y < FIELD_SIZE_MAPCHIP_HEIGHT + 2; y++)
	{
		for (int x = 0; x < fieldWeightChipNum + 2; x++)
		{
			// フィールドの番号を取得
			int chipNum = g_Map[y][x];
			if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
			{
				chipNum = g_Map_Tutorial[y][x];
			}
			if (GetBossFlg() == TRUE)
			{
				chipNum = g_Map_BOSS[y][x];
			}
			if (chipNum == 0) continue;

			float fx = (float)((0.0f - g_Field.pos.x) + (x * MAPCHIP_WIDTH) - (player->w * 0.5));
			float fy = (float)((0.0f - g_Field.pos.y) + (float)(y * MAPCHIP_HEIGHT) - (player->h * 0.5));
			float fw = (float)MAPCHIP_WIDTH;
			float fh = (float)MAPCHIP_HEIGHT;

			float tw = 1.0f;												// テクスチャの横幅
			float th = 1.0f;												// テクスチャの縦幅
			float tx = 0.0f;												// テクスチャの左上X座標
			float ty = 0.0f;												// テクスチャの左上Y座標

			// 1の時の描画処理
			if (chipNum == 1)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			if (chipNum == 2)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			if (chipNum == 3)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// 矢印付きの木箱（通過可能）
			if (chipNum == 4)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// 壊れた木箱（通過不可）
			if (chipNum == 5)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// 近未来のブロック（通行不可）
			if (chipNum == 6)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
			// 近未来の矢印付きブロック（通行可）
			if (chipNum == 7)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// 近未来の扉（通行可）
			if (chipNum == 8)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// Enterキー
			if (chipNum == 9 && g_Field.goalFlg == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				fw = 80.0f;
				fh = 100.0f;

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
			}
			// 矢印キー（チュートリアル）
			if (chipNum == 10)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);
				
				// TODO：カウント数（会話がどこまで進んだか）によってfxの位置を変化させる。もしくは数字を増やすかな。
				fy += 10 * sinValue;

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
			}


			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

// 指定した座標に書かれているfieldデータを取得する
char GetField(int y, int x)
{
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if (g_Field.fieldNo == MODE_TUTORIAL)
	{
		return g_Map_Tutorial[y][x];
	}
	if (GetBossFlg() == TRUE) return g_Map_BOSS[y][x];
	return g_Map[y][x];
}

void SetField(void)
{

}

//// 指定した座標はOKな場所かを確認する
int CheckField(int y, int x)
{
	int ans = 0;	// とりあえずOKをセットしておく

	// NGな場所？
	char data = g_Map[y][x];
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if ((g_Field.fieldNo == MODE_TUTORIAL))
	{
		data = g_Map_Tutorial[y][x] ;
	}
	if (GetBossFlg() == TRUE) data = g_Map_BOSS[y][x];

	switch (data)
	{
	case 1:
		ans = 1;	// その場所はNGだったので1をセットする
		break;
	case 2:
		ans = 1;	// その場所はNGだったので1をセットする
		break;
	case 3:
		ans = 1;	// その場所はNGだったので1をセットする
		break;
	case 5:
		ans = 1;	// その場所はNGだったので1をセットする
		break;
	case 6:
		ans = 1;	// その場所はNGだったので1をセットする
		break;
	case 8:
		ans = 2;	// その場所は扉だったので2をセットする
		break;
	case 10:
		ans = 3;	// その場所はチュートリアルポイントだったので3をセットする
		break;
	default:
		//ans = 1;	// その場所はNGだったので1をセットする
		break;
	}
	return ans;		// 結果を返す
}


//=============================================================================
// Field構造体の先頭アドレスを取得
//=============================================================================
FIELD* GetField(void)
{
	return &g_Field;
}

//=============================================================================
// プレイヤーがマップチップ上のどこにいるかの判定（イベントとか、、、）
//=============================================================================
void CheckPlayerToChipNum(float y, float x)
{
	PLAYER* player = GetPlayer();
	int playerX = (int)((x + player->w * 0.5) / MAPCHIP_WIDTH);
	int playerY = (int)(y / MAPCHIP_HEIGHT);

	// フィールドの何処にいるのかを確認する（中心点）
	int ans = CheckField(playerY, playerX);

	switch (ans)
	{
	// 未来風の扉にいるのであれば、
	case 2:
		g_Field.goalFlg = TRUE;
		break;
	// チュートリアルポイント1にいるのであれば、
	case 3:
		SetPLayerToPoint(TRUE);
		break;
	default:
		g_Field.goalFlg = FALSE;
		SetPLayerToPoint(FALSE);
		break;
	}
}
