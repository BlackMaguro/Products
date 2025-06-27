//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 
//
//=============================================================================
#include "bullet.h"
#include "main.h"
#include "enemy.h"
#include "boss.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "fade.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(32)	// 弾の横サイズ
#define TEXTURE_HEIGHT				(32)	// 弾の縦サイズ
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/Bullet/bullet00.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use   = FALSE;			// 未使用（発射されていない弾）
		g_Bullet[i].w     = TEXTURE_WIDTH;
		g_Bullet[i].h     = TEXTURE_HEIGHT;
		g_Bullet[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 移動量を初期化
		g_Bullet[i].dir = BULLET_DIR_RIGHT;

		g_Bullet[i].atk = 10;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			XMVECTOR pos  = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
			pos += move;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// 画面外まで進んだ？
			BG* bg = GetBG();
			PLAYER* player = GetPlayer();
			if (g_Bullet[i].pos.y < (-g_Bullet[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}
			// 左端にいたときの弾の処理
			if (player->pos.x < SCREEN_WIDTH * 0.5)
			{
				if (g_Bullet[i].pos.x < (player->pos.x - (SCREEN_WIDTH + g_Bullet[i].w * 0.5)))		// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = false;
				}
				if (g_Bullet[i].pos.x > (player->pos.x + (SCREEN_WIDTH + g_Bullet[i].w * 0.5)))		// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = false;
				}
			}
			// 中心にいたときの処理
			if (player->pos.x > SCREEN_WIDTH * 0.5)
			{
				if (g_Bullet[i].pos.x < (player->pos.x - (SCREEN_WIDTH * 0.5 + g_Bullet[i].w * 0.5)))		// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = false;
				}
				if (g_Bullet[i].pos.x > (player->pos.x + (SCREEN_WIDTH * 0.5 + g_Bullet[i].w * 0.5)))		// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = false;
				}
			}

			// 当たり判定処理
			{
				ENEMY* enemy = GetEnemy();
				int bulletAtk = g_Bullet[i].atk;
				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
							// 当たった時の処理
							enemy[j].hp -= bulletAtk;
							g_Bullet[i].use = false;
							if (enemy[j].hp <= 0)
							{
								enemy[j].use = FALSE;
								AddScore(100);
							}
							// エフェクト発生
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
						}
					}
				}
				// ボス戦だった場合
				BOSS* boss = GetBoss();

				if (GetBossFlg() == TRUE)
				{
					// エネミーの数分当たり判定を行う
					for (int j = 0; j < BOSS_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (boss[j].use == TRUE)
						{
							// ボスの目のHPを保存しておく
							int bossHP_old = boss[4].hp;
							BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
								boss[j].pos, boss[j].w, boss[j].h);
							// 当たっているとき（ボスの目）
							if ((ans == TRUE) && (j == 4))
							{
								// 当たった時の処理
								// ボスの口（5）が全開のとき、全ダメージ
								if (boss[5].patternAnim == 0 || boss[5].patternAnim == 4)
								{
									PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
									AddScore(50);
									boss[j].hp -= bulletAtk;
								}
								// ボスの口（5）が半分開いているとき、微ダメージ
								else if (boss[5].patternAnim == 1 || boss[5].patternAnim == 3)
								{
									PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
									AddScore(10);
									boss[j].hp -= (int)(bulletAtk * 0.5);
								}
								// ボスの口（5）が閉じているいるとき、ノーダメージ
								else if (boss[5].patternAnim == 2)
								{
									// 場合によっては回復する（0～30の値）
									boss[j].hp += bulletAtk * GetRand(0,3);
									if (boss[j].hp > BOSS_MAX_HP) boss[j].hp = BOSS_MAX_HP;
									if (boss[j].hp > bossHP_old) PlaySound(SOUND_LABEL_SE_12_BOSSHPHEAL);
								}
								// エフェクト発生
								g_Bullet[i].use = false;
								//SetEffect(boss[j].pos.x, boss[j].pos.y, 30);
								if (boss[j].hp <= 0)
								{
									// ボスを倒した場合、クリアフラグをオン
									SetClearFlg(TRUE);
									SetFade(FADE_OUT, MODE_EPILOGUE);		// ゲームをクリアしたのでエピローグ画面へ
									//boss[j].use = FALSE;
								}
							}
						}
					}
				}
			}


			bulletCount++;
		}
	}

	//PrintDebugProc("Bullet[1] pos.x:%f,pos.y:%f,\nBullet[2] pos.x:%f,pos.y:%f,\n", g_Bullet[0].pos.x, g_Bullet[0].pos.y, g_Bullet[1].pos.x, g_Bullet[1].pos.y);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;		// バレットの表示幅
			float ph = g_Bullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(XMFLOAT3 pos, int bulletDir)
{
	PLAYER* player = GetPlayer();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット

			// バレットの飛ぶ方向の確定
			// プレイヤーが右を向いていたとき
			if (bulletDir == BULLET_DIR_RIGHT)
			{
				g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);
			}
			// プレイヤーが左を向いていたとき
			else if (bulletDir == BULLET_DIR_LEFT)
			{
				g_Bullet[i].move = XMFLOAT3(-BULLET_SPEED, 0.0f, 0.0f);
			}
			// プレイヤーが下を向いていたとき
			else if (bulletDir == BULLET_DIR_DOWN)
			{
				g_Bullet[i].move = XMFLOAT3(0.0f, BULLET_SPEED, 0.0f);
			}
			// プレイヤーが上を向いていたとき
			else if (bulletDir == BULLET_DIR_UP)
			{
				g_Bullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);
			}
			PlaySound(SOUND_LABEL_SE_03_PLAYERBULLET);
			return;							// 1発セットしたので終了する
		}
	}
}

