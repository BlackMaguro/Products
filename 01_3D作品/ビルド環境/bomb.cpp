//=============================================================================
//
// 弾発射処理 [bomb.cpp]
// Author : 白井陽大
//
//=============================================================================
#include "bomb.h"
#include "enemy.h"
#include "renderer.h"
#include "meshobj.h"
#include "effect.h"

using namespace DirectX;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BOMB		"data/MODEL/05_WEAPON/bomb001.obj"		// 読み込むモデル名
#define TEXTURE_MAX			(1)				// テクスチャの数

static const float	BOMB_WIDTH	(5.0f);			// 頂点サイズ
static const float	BOMB_HEIGHT	(5.0f);			// 頂点サイズ
static const float	BOMB_DEPTHZ	(5.0f);			// 頂点サイズ

static int g_BombNum = MAX_BOMB;							// 爆弾の数（残り）
static const float BOMB_EXPOLOSION_SIZE = 100.0f;			// 爆弾の爆発範囲
static const int EFFECT_EXIT_MAX(30);						// 爆発エフェクトが存在する時間

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBomb(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BOMB						g_Bomb[MAX_BOMB];

static char *g_TextureName[] =
{
	"data/TEXTURE/bomb000.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBomb(void)
{
	MakeVertexBomb();

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

	g_BombNum = MAX_BOMB;

	// 爆弾の初期化
	for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
	{
		LoadModel(MODEL_BOMB, &g_Bomb[nCntBomb].model);
		g_Bomb[nCntBomb].load = TRUE;

		ZeroMemory(&g_Bomb[nCntBomb].material, sizeof(g_Bomb[nCntBomb].material));
		g_Bomb[nCntBomb].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Bomb[nCntBomb].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bomb[nCntBomb].rot = XMFLOAT3(0.0f, 1.5f, 0.0f);
		g_Bomb[nCntBomb].fallPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bomb[nCntBomb].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_Bomb[nCntBomb].fWidth = BOMB_WIDTH;
		g_Bomb[nCntBomb].fHeight = BOMB_HEIGHT;
		g_Bomb[nCntBomb].fDepthZ = BOMB_DEPTHZ;
		g_Bomb[nCntBomb].size = BOMB_SIZE;	// 当たり判定の大きさ
		g_Bomb[nCntBomb].use = FALSE;

		GetModelDiffuse(&g_Bomb[nCntBomb].model, &g_Bomb[nCntBomb].diffuse[0]);

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBomb(void)
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

	for (int nCntModel = 0; nCntModel < MAX_BOMB; nCntModel++)
	{
		if (g_Bomb[nCntModel].load)
		{
			UnloadModel(&g_Bomb[nCntModel].model);
			g_Bomb[nCntModel].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBomb(void)
{
	const float gravity = GRAVITY;				// 重力
	const float deltaTime = DELTATIME;			// デルタタイム
	const float AirResistance = 0.1f;			// 空気抵抗
	const float addFallAccel = GRAVITY * 50.0f;	// 追加の落下加速
	
	for (int i = 0; i < MAX_BOMB; i++)
	{
		if (g_Bomb[i].use)
		{

			// 放物線の準備
			g_Bomb[i].timer += deltaTime;
			float time = g_Bomb[i].timer;
			float prevVy = g_Bomb[i].vel.y;		// 以前のVyベクトルを保存しておく

			g_Bomb[i].vel.y -= gravity * deltaTime;

			// 空気抵抗
			if (!g_Bomb[i].isApex)
			{
				g_Bomb[i].vel.x *= (1.0f - AirResistance * deltaTime);
				g_Bomb[i].vel.z *= (1.0f - AirResistance * deltaTime);

				if (g_Bomb[i].vel.y <= 0.0f)
				{
					g_Bomb[i].isApex = TRUE;
				}
			}
			else
			{
				// 下降中は水平のみ AirResistance、垂直は追加重力で加速
				g_Bomb[i].vel.x *= (1.0f - AirResistance * deltaTime);
				g_Bomb[i].vel.z *= (1.0f - AirResistance * deltaTime);

				// extraFallAccel を追加してさらに速く落とす
				g_Bomb[i].vel.y -= addFallAccel * deltaTime;
			}

			// 位置を更新
			g_Bomb[i].pos.x += g_Bomb[i].vel.x * deltaTime;
			g_Bomb[i].pos.y += g_Bomb[i].vel.y * deltaTime;
			g_Bomb[i].pos.z += g_Bomb[i].vel.z * deltaTime;

			// フィールドオブジェクトとの当たり判定
			// 建物との当たり判定（衝突したら爆発）
			checkBombFieldObj();
			BOOL ans = checkBombToEnemy(i);
			// enemyとの当たり判定（衝突したら爆発）
 			if (ans)
			{
				Explode(i);
			}
			// 地形判定処理
			float groundY;
			XMFLOAT3 groundN;
			if (GetGroundHeightAndNormal(g_Bomb[i].pos.x, g_Bomb[i].pos.z, groundY, groundN))
			{
				if (g_Bomb[i].pos.y <= groundY)
				{
					// 衝突したら爆発
					Explode(i);
					continue;
				}
			}

			// フィールドの外に出たら爆弾を消す処理
			if (g_Bomb[i].pos.x < MAP_LEFT
				|| g_Bomb[i].pos.x > MAP_RIGHT
				|| g_Bomb[i].pos.z < MAP_DOWN
				|| g_Bomb[i].pos.z > MAP_TOP)
			{
				Explode(i);
				continue;
			}
		}
	}

	if (g_BombNum == 0)
	{
		// 飛んでいる爆弾が残っていないかチェック
		bool anyActive = false;
		for (int i = 0; i < MAX_BOMB; i++)
		{
			if (g_Bomb[i].use)
			{
				anyActive = true;
				break;
			}
		}
		// 全部消えていたらリロード
		if (!anyActive)
		{
			g_BombNum = MAX_BOMB;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBomb(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);


	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BOMB; i++)
	{
		if (g_Bomb[i].use)
		{
			// モデル
			for (int j = 0; j < g_Bomb[i].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Bomb[i].model, j, g_Bomb[i].diffuse[j]);
			}

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bomb[i].scl.x, g_Bomb[i].scl.y, g_Bomb[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bomb[i].rot.x, g_Bomb[i].rot.y + XM_PI, g_Bomb[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bomb[i].pos.x, g_Bomb[i].pos.y, g_Bomb[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bomb[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Bomb[i].model);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBomb(void)
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

	float fWidth = BOMB_WIDTH;
	float fHeight = BOMB_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 爆弾のパラメータをセット
//=============================================================================
int SetBomb(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBomb = -1;

	for (int i = 0; i < MAX_BOMB; i++)
	{
		if (!g_Bomb[i].use)
		{
			g_Bomb[i].startPos = pos;
			g_Bomb[i].pos = pos;
			g_Bomb[i].rot = rot;
			g_Bomb[i].spd = BOMB_LAUNCH_SPEED;
			g_Bomb[i].elevation = BOMB_LAUNCH_UP_ANGLE;
			g_Bomb[i].timer = 0.0f;
			g_Bomb[i].scl = { 0.2f,0.2f,0.2f };
			g_Bomb[i].explosionRadius = BOMB_EXPOLOSION_SIZE;
			g_Bomb[i].isApex = FALSE;
			g_Bomb[i].use = TRUE;


			// 方向ベクトル dirVec の計算
			float yaw = rot.y + XM_PI;
			XMFLOAT3 forward = { sinf(yaw), 0.0f, cosf(yaw) };
			XMVECTOR dir = XMLoadFloat3(&forward) + XMVectorSet(0, sinf(g_Bomb[i].elevation), 0, 0);
			dir = XMVector3Normalize(dir);

			// 初速ベクトル
			XMFLOAT3 initialVec;
			XMStoreFloat3(&initialVec, XMVectorScale(dir, g_Bomb[i].spd));
			g_Bomb[i].vel = initialVec;

			nIdxBomb = i;
			// 爆弾の数を減らす
			g_BombNum -= 1;

			break;
		}
	}

	return nIdxBomb;
}
//=============================================================================
// 爆発処理を行う関数
// int index：爆発する爆弾
// note：nullは許容していない
//=============================================================================
void Explode(int index)
{
	BOMB& bomb = g_Bomb[index];
	ENEMY* enemy = GetEnemy();

	// エフェクトを出す
	ShowExplosion(bomb.pos, EFFECT_EXIT_MAX);
	// 爆発音を鳴らす
	PlaySound(SOUND_LABEL_SE_11_EXPLOSIONBOMB);

	// 敵への当たり判定
	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (!enemy[j].use)       continue;  // 倒された敵はスキップ

		if (CollisionBC(
			bomb.pos,                  // 爆心
			enemy[j].pos,				// 敵の位置
			bomb.explosionRadius,      // 爆発範囲半径
			enemy[j].size))				// 敵の当たり判定半径
		{
			enemy[j].deadFlg = TRUE;       // 敵を倒す
			AddScore(SCORE_POINT);                 // スコア加算
		}
	}

	// 爆弾を削除
	bomb.use = FALSE;

}

//=============================================================================
// 爆弾との当たり判定関数
// int index：爆発する爆弾
// note：nullは許容していない
//=============================================================================
BOOL checkBombToEnemy(int index)
{
	BOOL ans = FALSE;

	BOMB& bomb = g_Bomb[index];
	ENEMY* enemies = GetEnemy();

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// エネミーとの当たり判定
		if (!enemies[i].use)   continue;

		if (CollisionBC(
			g_Bomb[index].pos,          // 爆弾中心
			enemies[i].pos,         // 敵中心
			g_Bomb[index].size,         // 爆弾半径
			enemies[i].size))       // 敵半径
		{
			enemies[i].deadFlg = TRUE;
			ans = TRUE;
			return ans;
		}
	}
	return ans;
}


//=============================================================================
// アクセサ関数
//=============================================================================
int GetBombNum()
{
	return g_BombNum;
}

//=============================================================================
// 爆弾の取得
//=============================================================================
BOMB *GetBomb(void)
{
	return &(g_Bomb[0]);
}
