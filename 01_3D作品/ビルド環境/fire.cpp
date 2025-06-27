//=============================================================================
//
// 炎処理 [fire.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "fire.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(8)				// テクスチャの数

#define	FIRE_WIDTH			(30.0f)			// 頂点サイズ
#define	FIRE_HEIGHT			(60.0f)			// 頂点サイズ

#define TEXANIM_FIRE		(9)				// 炎のテクスチャアニメーションの間隔

#define	MAX_FIRE			(256)			// 炎の最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		use;			// 使用しているかどうか

} FIRE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexFire(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FIRE					g_Fire[MAX_FIRE];	// 炎のテクスチャ
static BOOL					g_bAlpaTest;		// アルファテストON/OFF

static int					g_TexNo;			// テクスチャ番号
static int					g_TexAnim;			// テクスチャアニメ用

static char *g_TextureName[] =
{
	"data/TEXTURE/04_INGAME/01_FIRE/Fire1.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire2.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire3.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire4.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire5.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire6.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire7.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire8.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFire(void)
{
	MakeVertexFire();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = g_TexAnim = 0;

	// 炎の初期化
	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		ZeroMemory(&g_Fire[nCntFire].material, sizeof(g_Fire[nCntFire].material));
		g_Fire[nCntFire].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Fire[nCntFire].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Fire[nCntFire].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Fire[nCntFire].fWidth = FIRE_WIDTH;
		g_Fire[nCntFire].fHeight = FIRE_HEIGHT;
		g_Fire[nCntFire].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	// 炎の設定
	SetFire(XMFLOAT3(-300.0f, -3.0f, 550.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(180.0f,  -3.0f, 310.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(250.0f,  -3.0f, 550.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(90.0f,   -3.0f, 300.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(250.0f,  -3.0f,-300.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(220.0f,  -3.0f,-600.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFire(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFire(void)
{

	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		if(g_Fire[nCntFire].use)
		{
			// 影の位置設定
			SetPositionShadow(g_Fire[nCntFire].nIdxShadow, XMFLOAT3(g_Fire[nCntFire].pos.x, 0.1f, g_Fire[nCntFire].pos.z));
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFire(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);
	//SetLightEnable(TRUE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_FIRE; i++)
	{
		if(g_Fire[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 正方行列（直交行列）を転置行列させて逆行列を作る
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// スケールを反映
			mtxScl = XMMatrixScaling(g_Fire[i].scl.x, g_Fire[i].scl.y, g_Fire[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Fire[i].pos.x, g_Fire[i].pos.y, g_Fire[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Fire[i].material);

			// テクスチャ設定
			int texNo = i % TEXTURE_MAX;
			{
				// テクスチャアニメ
				g_TexAnim++;
				if ((g_TexAnim % TEXANIM_FIRE) == 0)
				{
					g_TexNo = (g_TexNo + 1) % TEXTURE_MAX;
				}
				texNo = g_TexNo;
			}
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexFire(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 炎のパラメータをセット
//=============================================================================
int SetFire(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxFire = -1;

	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		if(!g_Fire[nCntFire].use)
		{
			g_Fire[nCntFire].pos = pos;
			g_Fire[nCntFire].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Fire[nCntFire].fWidth = fWidth;
			g_Fire[nCntFire].fHeight = fHeight;
			g_Fire[nCntFire].use = TRUE;

			// 影の設定
			g_Fire[nCntFire].nIdxShadow = CreateShadow(g_Fire[nCntFire].pos, 0.5f, 0.5f);

			nIdxFire = nCntFire;

			break;
		}
	}

	return nIdxFire;
}
