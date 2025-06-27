//=============================================================================
//
// 物を投げるときのマーカー処理 [throwMarker.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "shadow.h"
#include "throwMarker.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int TEXTURE_MAX (5);			// テクスチャの数

static const float	THROWMARKER_WIDTH	(30.0f);	// 頂点サイズ
static const float	THROWMARKER_HEIGHT	(60.0f);	// 頂点サイズ

static const int TEXANIM_THROWMAKER	(5);	// 投擲先のマーカーのテクスチャアニメーションの間隔

static const int MAX_THROWMAKER	(3);		// 投擲先のマーカーの最大数

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

} THROWMARKER;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexThrowMarker(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static THROWMARKER			g_ThrowMarker[MAX_THROWMAKER];	// 投擲先のマーカーのテクスチャ
static BOOL					g_bAlpaTest;		// アルファテストON/OFF

static int					g_TexNo;			// テクスチャ番号
static int					g_TexAnim;			// テクスチャアニメ用

static char* g_TextureName[] =
{
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_1.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_2.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_3.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_4.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_5.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitThrowMarker(void)
{
	MakeVertexThrowMarker();

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

	// 投擲先のマーカーの初期化
	for (int nCntThrowMarker = 0; nCntThrowMarker < MAX_THROWMAKER; nCntThrowMarker++)
	{
		ZeroMemory(&g_ThrowMarker[nCntThrowMarker].material, sizeof(g_ThrowMarker[nCntThrowMarker].material));
		g_ThrowMarker[nCntThrowMarker].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ThrowMarker[nCntThrowMarker].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ThrowMarker[nCntThrowMarker].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ThrowMarker[nCntThrowMarker].fWidth = THROWMARKER_WIDTH;
		g_ThrowMarker[nCntThrowMarker].fHeight = THROWMARKER_HEIGHT;
		g_ThrowMarker[nCntThrowMarker].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitThrowMarker(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateThrowMarker(void)
{

	for (int nCntThrowMarker = 0; nCntThrowMarker < MAX_THROWMAKER; nCntThrowMarker++)
	{
		if (g_ThrowMarker[nCntThrowMarker].use)
		{
			// 影の位置設定
			SetPositionShadow(g_ThrowMarker[nCntThrowMarker].nIdxShadow, XMFLOAT3(g_ThrowMarker[nCntThrowMarker].pos.x, 0.1f, g_ThrowMarker[nCntThrowMarker].pos.z));
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawThrowMarker(void)
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
	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_THROWMAKER; i++)
	{
		if (g_ThrowMarker[i].use)
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
			mtxScl = XMMatrixScaling(g_ThrowMarker[i].scl.x, g_ThrowMarker[i].scl.y, g_ThrowMarker[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_ThrowMarker[i].pos.x, g_ThrowMarker[i].pos.y, g_ThrowMarker[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_ThrowMarker[i].material);

			// テクスチャ設定
			int texNo = i % TEXTURE_MAX;
			{
				// テクスチャアニメ
				g_TexAnim++;
				if ((g_TexAnim % MAX_THROWMAKER) == 0)
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
HRESULT MakeVertexThrowMarker(void)
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
// 投擲先マーカーを出力する関数
//=============================================================================
void ShowThrowMarker(const XMFLOAT3& pos)
{
	// まず全消し
	for (int i = 0; i < MAX_THROWMAKER; i++)
		g_ThrowMarker[i].use = FALSE;
	// 先頭だけ使う
	g_ThrowMarker[0].pos = pos;
	g_ThrowMarker[0].use = TRUE;
}
//=============================================================================
// 投擲先マーカーを消す関数
//=============================================================================
void HideThrowMarker()
{
	for (int i = 0; i < MAX_THROWMAKER; i++)
		g_ThrowMarker[i].use = FALSE;
}