//=============================================================================
//
// バレット処理 [bulletEnemy.cpp]
// Author : 
//
//=============================================================================
#include "bulletEnemy.h"
#include "main.h"
#include "enemy.h"
#include "boss.h"
#include "player.h"
#include "collision.h"
#include "sound.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "fade.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// 弾の横サイズ
#define TEXTURE_HEIGHT				(84)	// 弾の縦サイズ
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値
#define COOLDOWNTIME				(30)	// クールダウンの値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/Bullet/bullet_enemy_01.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BOOL g_enemyBulletAllLostFlg = TRUE;		// 発射した弾がすべて消えたかどうかの確認（最初は何もないからTRUE）
static BULLETENEMY	g_BulletEnemy[BULLET_ENEMY_MAX];	// バレット構造体
float enemyBulletCooldown = (float) COOLDOWNTIME;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBulletEnemy(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		g_BulletEnemy[i].use   = FALSE;			// 未使用（発射されていない弾）
		g_BulletEnemy[i].w     = TEXTURE_WIDTH;
		g_BulletEnemy[i].h     = TEXTURE_HEIGHT;
		g_BulletEnemy[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_BulletEnemy[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BulletEnemy[i].texNo = 0;

		g_BulletEnemy[i].countAnim = 0;
		g_BulletEnemy[i].patternAnim = 0;

		g_BulletEnemy[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 移動量を初期化
		g_BulletEnemy[i].dir = BULLET_ENEMY_DIR_DOWN;

		g_BulletEnemy[i].atk = (int)(BOSS_MAX_ATK * 0.5);			// ボスの最大攻撃力の半分
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBulletEnemy(void)
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBulletEnemy(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// 処理したバレットの数
	//int bulletLostCount = 0;			// 消えたバレットの数
	//BOOL activeBulletFlg = FALSE;		// バレットがあるかないか
	// クールダウンタイム
	enemyBulletCooldown -= 1.0f;
	if (enemyBulletCooldown < 0) enemyBulletCooldown = (float)COOLDOWNTIME;

	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		if (g_BulletEnemy[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			//activeBulletFlg = TRUE;
			//g_enemyBulletAllLostFlg = FALSE;
			// アニメーション  
			g_BulletEnemy[i].countAnim++;
			if ((g_BulletEnemy[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_BulletEnemy[i].patternAnim = (g_BulletEnemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			XMVECTOR pos  = XMLoadFloat3(&g_BulletEnemy[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_BulletEnemy[i].move);
			pos += move;
			XMStoreFloat3(&g_BulletEnemy[i].pos, pos);

			// 画面外まで進んだ？
			BG* bg = GetBG();
			BOSS* boss = GetBoss();
			if (g_BulletEnemy[i].pos.y < (-g_BulletEnemy[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_BulletEnemy[i].use = false;
			}
			if (g_BulletEnemy[i].pos.y > (bg->h + g_BulletEnemy[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_BulletEnemy[i].use = false;
			}
			// 中心にいたときの処理
			if (g_BulletEnemy[i].pos.x < (boss->pos.x - (SCREEN_WIDTH * 0.5 + g_BulletEnemy[i].w * 0.5)))		// 弾の大きさを考慮して画面外か判定している
			{
				g_BulletEnemy[i].use = false;
			}
			if (g_BulletEnemy[i].pos.x > (boss->pos.x + (SCREEN_WIDTH * 0.5 + g_BulletEnemy[i].w * 0.5)))		// 弾の大きさを考慮して画面外か判定している
			{
				g_BulletEnemy[i].use = false;
			}

			PLAYER* player = GetPlayer();
			// 当たり判定処理
			{
				// ボス戦だった場合
				if (GetBossFlg() == TRUE)
				{
					// プレイヤーの数分当たり判定を行う
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (player[j].use == TRUE)
						{

							BOOL ans = CollisionBB(g_BulletEnemy[i].pos, g_BulletEnemy[i].w, g_BulletEnemy[i].h,
								player[j].pos, player[j].w, player[j].h);
							// 当たっているとき（弾）
							if ((ans == TRUE) && (player[0].muteki == FALSE))
							{
								// TODO：音を追加予定
								//if (boss[j].hp > bossHP_old) PlaySound();
								if ((ans == TRUE) && (player[j].muteki == FALSE))
								{
									// 当たった時の処理
									PlaySound(SOUND_LABEL_SE_05_PLAYERDAMAGE);
									player[j].hp -= (int)(BOSS_MAX_ATK * 0.5);
									player[j].muteki = TRUE;
									player[j].mutekiTime = 0;		// 無敵時間のリセット
								}
								g_BulletEnemy[i].use = false;
								// エフェクト発生
								//SetEffect(boss[j].pos.x, boss[j].pos.y, 30);
								if (player[j].hp <= 0)
								{
									player[j].use = FALSE;
									SetMode(MODE_GAMEOVER);
								}
							}
						}
					}
				}
			}

			bulletCount++;
		}
	}

	//PrintDebugProc("bulletCount:%d,\nbulletLostCount:%d,\n", bulletCount, bulletLostCount);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBulletEnemy(void)
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

	BG* bg = GetBG();
	BOSS* boss = GetBoss();

	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		if (g_BulletEnemy[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_BulletEnemy[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_BulletEnemy[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_BulletEnemy[i].w;		// バレットの表示幅
			float ph = g_BulletEnemy[i].h;		// バレットの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// パターン2のとき炎を大きくする
			if (boss[4].patternAtk == 1 || boss[4].patternAtk == 3)
			{
				pw = (float)(g_BulletEnemy[i].w * 1.5);		// バレットの表示幅
				ph = (float)(g_BulletEnemy[i].h * 1.5);		// バレットの表示高さ
			}

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_BulletEnemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLETENEMY * GetBulletEnemy(void)
{
	return &g_BulletEnemy[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBulletEnemy(XMFLOAT3 pos,int bossPatternAtk)
{
	//PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();
	XMFLOAT3 atkPos = pos;
	int enemyBulletMAX = BULLET_ENEMY_MAX;
	// 弾の上限を設定
	if (bossPatternAtk == 0 || bossPatternAtk == 1) enemyBulletMAX = 1;

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < enemyBulletMAX; i++)
	{
		if (enemyBulletCooldown == 0.0f)
		{
			// ランダム値の取得
			int random = GetRand(1, 9);
			if (g_BulletEnemy[i].use == FALSE)		// 未使用状態のバレットを見つける
			{
				g_BulletEnemy[i].use = TRUE;			// 使用状態へ変更する
				g_BulletEnemy[i].pos = atkPos;			// 座標をセット

				// ボスの攻撃パターンによって弾の動きを変化させる
				// パターン1：火を吐く（小）
				if (bossPatternAtk == 0)
				{
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// パターン2：火を吐く（大）
				else if (bossPatternAtk == 1)
				{
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// パターン3：火を降らす
				else if (bossPatternAtk == 2)
				{
					// 高さと出現位置の変更
					g_BulletEnemy[i].pos.y -= 200.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// パターン3：火を降らす（大）
				else if (bossPatternAtk == 3)
				{
					// 高さと出現位置の変更
					g_BulletEnemy[i].pos.y -= 200.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				else if (bossPatternAtk == 4)
				{
					// 高さと出現位置の変更
					g_BulletEnemy[i].pos.y -= 300.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, 8.0f , 0.0f);
				}
				PlaySound(SOUND_LABEL_SE_04_BOSSFIRE);
				return;							// 1発セットしたので終了する
			}
		}
	}
}

//=============================================================================
// ボスが発射した弾が全て消えたかどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetEnemyBulletAllLostFlg(void)
{
	return g_enemyBulletAllLostFlg;
}

void SetEnemyBulletAllLostFlg(BOOL enemyBulletAllLostFlg)
{
	g_enemyBulletAllLostFlg = enemyBulletAllLostFlg;
}

