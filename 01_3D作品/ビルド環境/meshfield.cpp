//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "enemy.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(1)				// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer					*g_IndexBuffer = NULL;	// インデックスバッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static XMFLOAT3		g_posField;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotField;								// ポリゴンの回転角

static int			g_nNumBlockXField, g_nNumBlockZField;	// ブロック数
static int			g_nNumVertexField;						// 総頂点数	
static int			g_nNumVertexIndexField;					// 総インデックス数
static int			g_nNumPolygonField;						// 総ポリゴン数
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// ブロックサイズ

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/field001.png",
};


// 波の処理

static VERTEX_3D	*g_Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		g_Center;					// 波の発生場所
static float		g_Time = 0.0f;				// 波の経過時間
static float		g_wave_frequency  = 2.0f;	// 波の周波数
static float		g_wave_correction = 0.02f;	// 波の距離補正
static float		g_wave_amplitude  = 20.0f;	// 波の振幅

static BOOL			g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// ポリゴン表示位置の中心座標を設定
	g_posField = pos;

	g_rotField = rot;

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

	g_TexNo = 0;

	// ブロック数の設定
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	// 頂点数の設定
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;


	// 頂点情報をメモリに作っておく（波の為）
	// 波の処理
	// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 波の発生場所
	g_Time = 0.0f;								// 波の経過時間(＋とーとで内側外側になる)
	g_wave_frequency = 1.0f;					// 波の周波数（上下運動の速さ）
	g_wave_correction = 0.02f;					// 波の距離補正（変えなくても良いと思う）
	g_wave_amplitude = 30.0f;					// 波の振幅(波の高さ)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
			// 波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;


			// 法線の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxZ < (g_nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
	// 波の処理
	float dt = 0.03f;

	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
			//  　波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
		}

	}
	g_Time += dt;


	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早い
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}


//*****************************************************************************
// 垂直方向にレイを飛ばす関数
//*****************************************************************************
BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	if (start.x == 0.0f)
	{
		start.x += 0.5f;
		end.x += 0.5f;
	}
	if (start.z == 0.0f)
	{
		start.z -= 0.5f;
		end.z -= 0.5f;
	}

	// 少し上から、ズドーンと下へレイを飛ばす
	start.y += 100.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなくて、座標からポリゴンを割り出している
	float fz = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	float fx = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	int sz = (int)((-start.z+fz) / g_fBlockSizeZField);
	int sx = (int)(( start.x+fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	sz = sx = 0;	// ここ態とやってる
	ez = g_nNumBlockZField-1;
	ex = g_nNumBlockXField-1;

	// 飛ばしたレイが当たっていなければ処理しない
	if ((sz < 0) || (sz > g_nNumBlockZField-1) ||
		(sx < 0) || (sx > g_nNumBlockXField-1))
	{
		*Normal = {0.0f, 1.0f, 0.0f};
		return FALSE;
	}


	// 必要数分検索を繰り返す
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

			// 三角ポリゴンだから２枚分の当たり判定
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}


	return FALSE;
}

//*****************************************************************************
// 水平方向にレイを飛ばす関数
// note：調整中
//*****************************************************************************
BOOL RayHitHorizontal(const ENEMY& enemies, XMFLOAT3* pHitPosition, XMFLOAT3* pNormal)
{
	// 開始・終了点をXZ平面でオフセット
	const float length = 30.0f;
	XMFLOAT3 start = enemies.pos;
	XMFLOAT3 dir = { cosf(enemies.rot.y), 0.0f, sinf(enemies.rot.y) };
	XMFLOAT3 end = {
		start.x + dir.x * length,
		start.y,  // Y はそのまま
		start.z + dir.z * length
	};

	// セル範囲の算出
	float halfZ = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	float halfX = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	int sz1 = int((-start.z + halfZ) / g_fBlockSizeZField);
	int sx1 = int((start.x + halfX) / g_fBlockSizeXField);
	int sz2 = int((-end.z + halfZ) / g_fBlockSizeZField);
	int sx2 = int((end.x + halfX) / g_fBlockSizeXField);
	int szMin = max(0, min(sz1, sz2));
	int sxMin = max(0, min(sx1, sx2));
	int szMax = min(g_nNumBlockZField - 1, max(sz1, sz2));
	int sxMax = min(g_nNumBlockXField - 1, max(sx1, sx2));

	// その範囲だけ三角ポリゴンをテスト
	for (int z = szMin; z <= szMax; ++z) {
		for (int x = sxMin; x <= sxMax; ++x) {
			// 元のフィールド頂点取得
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + x + 1].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x + 1].Position;


			XMFLOAT3 dummyHit, dummyNorm;
			XMFLOAT3* outHit = pHitPosition ? pHitPosition : &dummyHit;
			XMFLOAT3* outNor = pNormal ? pNormal : &dummyNorm;

			if (RayCast(p0, p2, p1, start, end, outHit, outNor)) return TRUE;
			if (RayCast(p1, p2, p3, start, end, outHit, outNor)) return TRUE;
		}
	}

	return FALSE;
}

//*****************************************************************************
// 水平方向にレイを飛ばす関数
// 戻り値: 地面に衝突したした場合 true
// note：Nullは想定していない
//*****************************************************************************
BOOL GetGroundHeightAndNormal(
	float x, float z,
	float& outHeight,
	XMFLOAT3& outNormal)
{
	// ワールド XZ をグリッド座標に変換
	float halfW = (g_nNumBlockXField * g_fBlockSizeXField) * 0.5f;
	float halfD = (g_nNumBlockZField * g_fBlockSizeZField) * 0.5f;

	// グリッドのセル番号
	int ix = static_cast<int>((x + halfW) / g_fBlockSizeXField);
	int iz = static_cast<int>((-z + halfD) / g_fBlockSizeZField);

	// 境界チェック（外に出たら平坦地面として y=0, 法線=up）
	if (ix < 0 || ix >= g_nNumBlockXField ||
		iz < 0 || iz >= g_nNumBlockZField)
	{
		outHeight = 0.0f;
		outNormal = { 0,1,0 };
		return FALSE;
	}

	// セル内のローカル uv（0..1）
	float localX = (x + halfW - ix * g_fBlockSizeXField) / g_fBlockSizeXField;
	float localZ = (-z + halfD - iz * g_fBlockSizeZField) / g_fBlockSizeZField;

	// ４つの頂点の高さを取得
	auto getY = [&](int cx, int cz) {
		return g_Vertex[cz * (g_nNumBlockXField + 1) + cx].Position.y;
		};
	float h00 = getY(ix, iz);
	float h10 = getY(ix + 1, iz);
	float h01 = getY(ix, iz + 1);
	float h11 = getY(ix + 1, iz + 1);

	// バイリニア補間
	float h0 = h00 * (1 - localX) + h10 * localX;
	float h1 = h01 * (1 - localX) + h11 * localX;
	outHeight = h0 * (1 - localZ) + h1 * localZ;

	// 法線は隣接セルをクロスで計算 or 単純に上向き
	// ここでは簡易に上向きベクトルを返す
	outNormal = { 0,1,0 };

	return TRUE;
}