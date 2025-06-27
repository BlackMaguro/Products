//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "bomb.h"
#include "shadow.h"
#include "effect.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
static const int TEXTURE_MAX (16);			// テクスチャの数

static const float	EFFECT_EXPLOSION_WIDTH	(200.0f);	// 頂点サイズ
static const float	EFFECT_EXPLOSION_HEIGHT	(200.0f);	// 頂点サイズ

static const int TEXANIM_EXPLOSION	(5);	// エフェクトのテクスチャアニメーションの間隔

static const int MAX_EFFECT	(MAX_BOMB);		// エフェクトの最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexExplosion(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static EXPLOSION			g_Explosion[MAX_EFFECT];	// エフェクトのテクスチャ
static BOOL					g_bAlpaTest;		// アルファテストON/OFF

static int					g_TexNo;			// テクスチャ番号
static int					g_TexAnim;			// テクスチャアニメ用

static char* g_TextureName[] =
{
	PATH_TEXTURE_EFFECT_EXPLOSION "01.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "02.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "03.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "04.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "05.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "06.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "07.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "08.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "09.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "10.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "11.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "12.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "13.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "14.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "15.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "16.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitExplosion(void)
{
	MakeVertexExplosion();

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

	// 爆発エフェクトの初期化
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		ZeroMemory(&g_Explosion[i].material, sizeof(g_Explosion[i].material));
		g_Explosion[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Explosion[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Explosion[i].fWidth = EFFECT_EXPLOSION_WIDTH;
		g_Explosion[i].fHeight = EFFECT_EXPLOSION_HEIGHT;
		g_Explosion[i].passedFrame = 0;
		g_Explosion[i].existFrame = 0;
		g_Explosion[i].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitExplosion(void)
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
void UpdateExplosion(void)
{
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (!g_Explosion[i].use) continue;

		// 影の位置設定
		SetPositionShadow(g_Explosion[i].nIdxShadow, XMFLOAT3(g_Explosion[i].pos.x, 0.1f, g_Explosion[i].pos.z));

		if (g_Explosion[i].passedFrame++ >= g_Explosion[i].existFrame)
		{
			g_Explosion[i].use = FALSE;
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
void DrawExplosion(void)
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

	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (g_Explosion[i].use)
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
			mtxScl = XMMatrixScaling(g_Explosion[i].scl.x, g_Explosion[i].scl.y, g_Explosion[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Explosion[i].pos.x, g_Explosion[i].pos.y, g_Explosion[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Explosion[i].material);

			// テクスチャ設定
			int texNo = i % TEXTURE_MAX;
			{
				// テクスチャアニメ
				g_TexAnim++;
				if ((g_TexAnim % MAX_EFFECT) == 0)
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
HRESULT MakeVertexExplosion(void)
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

	float fWidth = EFFECT_EXPLOSION_WIDTH;
	float fHeight = EFFECT_EXPLOSION_HEIGHT;

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
// 爆発エフェクトを出力する関数
//=============================================================================
void ShowExplosion(const XMFLOAT3& pos, int durationTime)
{
	float explosionYOffset = 80.0f;
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (!g_Explosion[i].use)
		{
			g_Explosion[i].pos = { pos.x, pos.y - explosionYOffset, pos.z };
			g_Explosion[i].use = TRUE;
			g_Explosion[i].passedFrame = 0;
			g_Explosion[i].existFrame = durationTime;
			break;
		}
	}
}
//=============================================================================
// 爆発エフェクトを消す関数
//=============================================================================
void HideExplosion()
{
	for (int i = 0; i < MAX_EFFECT; i++)
		g_Explosion[i].use = FALSE;
}