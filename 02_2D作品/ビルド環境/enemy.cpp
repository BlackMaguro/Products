//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "main.h"
#include "bg.h"
#include "player.h"
#include "field.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// キャラサイズ(200/2)
#define TEXTURE_HEIGHT				(64)	// 
#define TEXTURE_MAX					(6)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X		(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_BALL_DIVIDE_X	(6)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y		(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)			// アニメーションパターン数
#define ANIM_PATTERN_BALL_NUM		(TEXTURE_PATTERN_BALL_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT						(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/enemy_1_maggotsG.png",				// 0_マゴッツ_緑_通常
	"data/TEXTURE/Enemy/enemy_2_maggotsP.png",				// 1_マゴッツ_紫_通常
	"data/TEXTURE/Enemy/enemy_3_maggotsR.png",				// 2_マゴッツ_赤_通常
	"data/TEXTURE/Enemy/enemy_4_maggotsG.png",				// 3_マゴッツ_緑_丸い
	"data/TEXTURE/Enemy/enemy_5_maggotsP.png",				// 4_マゴッツ_紫_丸い
	"data/TEXTURE/Enemy/enemy_6_maggotsR.png",				// 5_マゴッツ_赤_丸い
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];		// エネミー構造体

static int		g_EnemyCount = ENEMY_MAX;

static int g_EnemyMAX = ENEMY_MAX;		// エネミーの最大数を変更するための変数

// グリーンマゴッツ（玉）
static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率					時間
	{ XMFLOAT3(2688.0f, 320.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(3392.0f, 320.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};

static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(4352.0f,448.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(5632.0f,448.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.1f, 1.1f, 1.0f),	60 },
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	if (GetMode() == MODE_TUTORIAL)
	{
		g_EnemyMAX = 1;
	}
	else if(GetMode() == MODE_GAME)
	{
		g_EnemyMAX = ENEMY_MAX;
	}
	// エネミー構造体の初期化
	g_EnemyCount = 0;
	for (int i = 0; i < g_EnemyMAX; i++)
	{
		g_EnemyCount++;
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = 0;
		g_Enemy[i].hp = ENEMY_G_MAX_HP;					// エネミーのHPをセット
		g_Enemy[i].atk = ENEMY_G_MAX_ATK;				// エネミーの攻撃力をセット
		g_Enemy[i].type = ENEMY_TYPE_G;					// エネミーの種類をセット
		g_Enemy[i].ballFlg = FALSE;						// エネミーの玉の状態

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].colPos = 0.0f;							// 衝突したときの座標用
		g_Enemy[i].colDir = EDGE_LEFT;						// 衝突したときの向き

		g_Enemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// エネミーの種類によって値を変更
		// パープルマゴッツの場合（紫のウジ虫）
		if (i == 4 || i == 6 || i == 7 || i == 11 || i == 12 || i == 15)
		{
			g_Enemy[i].hp = ENEMY_P_MAX_HP;
			g_Enemy[i].atk = ENEMY_P_MAX_ATK;
			g_Enemy[i].type = ENEMY_TYPE_P;

		}
		// レッドマゴッツの場合（赤のウジ虫）
		if (i == 8 || i == 11 || i == 13 || i == 14 || i == 16 || i == 17)
		{
			g_Enemy[i].hp = ENEMY_R_MAX_HP;
			g_Enemy[i].atk = ENEMY_R_MAX_ATK;
			g_Enemy[i].type = ENEMY_TYPE_R;
		}
		// エネミーが玉の状態かを判断する
		if (i == 5 || i == 12 || i == 13 || i == 15 || i == 16 || i == 17)
		{
			g_Enemy[i].ballFlg = TRUE;							// エネミーの玉の状態
			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// 移動量
		}

	}

	g_Enemy[0].pos = XMFLOAT3(1000.0f, 448.0f, 0.0f);	// エネミー1（グリーン）
	// チュートリアル用エネミー（グリーンマゴット）
	if (GetMode() == MODE_TUTORIAL) 
	{
		g_Enemy[0].pos = XMFLOAT3(1200.0f, 448.0f, 0.0f);
	}

	g_Enemy[1].pos = XMFLOAT3(960.0f, 256.0f, 0.0f);	// エネミー2（グリーン）
	g_Enemy[2].pos = XMFLOAT3(1600.0f, 440.0f, 0.0f);	// エネミー3（グリーン）
	g_Enemy[3].pos = XMFLOAT3(1472.0f, 256.0f, 0.0f);	// エネミー4（グリーン）
	g_Enemy[4].pos = XMFLOAT3(2176.0f, 384.0f, 0.0f);	// エネミー5（パープル）
	g_Enemy[5].pos = XMFLOAT3(2816.0f, 320.0f, 0.0f);	// エネミー6（グリーン：玉）
	g_Enemy[6].pos = XMFLOAT3(3072.0f, 320.0f, 0.0f);	// エネミー7（パープル）
	g_Enemy[7].pos = XMFLOAT3(3456.0f, 128.0f, 0.0f);	// エネミー8（パープル、右、高いところ）
	g_Enemy[8].pos = XMFLOAT3(4160.0f, 384.0f, 0.0f);	// エネミー9（レッド、強いことを示す）
	g_Enemy[9].pos = XMFLOAT3(4352.0f, 448.0f, 0.0f);	// エネミー10（グリーン）
	// 木の箱をジャンプする箇所のエネミー
	g_Enemy[10].pos = XMFLOAT3(4672.0f, 448.0f, 0.0f);	// エネミー11（パープル：右）
	g_Enemy[11].pos = XMFLOAT3(4900.0f, 448.0f, 0.0f);	// エネミー12（レッド）
	g_Enemy[12].pos = XMFLOAT3(5300.0f, 448.0f, 0.0f);	// エネミー13（パープル：玉）
	g_Enemy[13].pos = XMFLOAT3(5500.0f, 448.0f, 0.0f);	// エネミー14（レッド：玉）
	// 未来風ステージのエネミー
	g_Enemy[14].pos = XMFLOAT3(6300.0f, 320.0f, 0.0f);	// エネミー15（レッド）
	g_Enemy[15].pos = XMFLOAT3(7200.0f, 128.0f, 0.0f);	// エネミー16（パープル：玉）
	g_Enemy[16].pos = XMFLOAT3(6700.0f, 448.0f, 0.0f);	// エネミー17（レッド：玉：線形補間）
	g_Enemy[17].pos = XMFLOAT3(7552.0f, 256.0f, 0.0f);	// エネミー18（レッド：玉）


	// 5番だけ線形補間で動かしてみる
	g_Enemy[5].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[5].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[5].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 13番だけ線形補間で動かしてみる
	g_Enemy[13].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[13].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[13].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{

	if (g_Load == FALSE) return;
	g_EnemyCount = 0;			// 生きてるエネミーの数

	for (int i = 0; i < g_EnemyMAX; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Enemy[i].use == TRUE)
		{
			g_EnemyCount++;		// 生きてた敵の数
			
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// アニメーション
			g_Enemy[i].countAnim += 1.0f;
			// 通常の動き
			if (g_Enemy[i].ballFlg != TRUE)
			{
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}
			}
			// 玉の時の動き
			else if(g_Enemy[i].ballFlg == TRUE)
			{
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_BALL_NUM;
				}
			}

			// 重力を設定
			float gravity = 4.0f;
			float vertical = 0.0f;			// 縦の移動量
			float horizontal = -g_Enemy[i].move.x;		// 横の移動量

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得
				
				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換
				
				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している
				
				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している
				
				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}
			// 移動処理（簡易版）
			// 移動できる場所かのチェック（重力）
			if (CollisionEnemyToMapchip(XMFLOAT3(0.0f, gravity, 0.0f), i) == FALSE)
			{
				// 移動量を足す
				g_Enemy[i].pos.y += gravity;
			}
			else
			{
				// 衝突前に戻す
				g_Enemy[i].pos.y = g_Enemy[i].colPos;
			}
			// 移動できる場所かのチェック（横方向）
			if (CollisionEnemyToMapchip(XMFLOAT3(horizontal, 0.0f, 0.0f), i) == FALSE)
			{
				// 移動量を足す
				g_Enemy[i].pos.x += horizontal * 0.1f;
			}
			else
			{
				// 移動方向を反転させる
				g_Enemy[i].move.x *= (-1.0f);

				// 衝突前に戻す
				g_Enemy[i].pos.x = g_Enemy[i].colPos;
			}
			// 移動できる場所かのチェック（縦方向）
			if (CollisionEnemyToMapchip(XMFLOAT3(0.0f, vertical, 0.0f), i) == FALSE)
			{
				// 移動量を足す
				g_Enemy[i].pos.y += vertical;
			}
			else
			{
				// 衝突前に戻す
				g_Enemy[i].pos.y = g_Enemy[i].colPos;
			}


			// 移動が終わったらエネミーとの当たり判定
			{
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < g_EnemyMAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w, g_Enemy[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE && (player[0].muteki == FALSE))
						{
							player->hp -= 10;
						}
					}
				}
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc(
		"Enemy[0] pos.x:%f,pos.y:%f,\nEnemy[1] pos.x:%f,pos.y:%f,\nEnemy[5] pos.x:%f,pos.y:%f,\nEnemy[5] HP:%d,\n",
		g_Enemy[0].pos.x, g_Enemy[0].pos.y,
		g_Enemy[1].pos.x, g_Enemy[1].pos.y,
		g_Enemy[5].pos.x, g_Enemy[5].pos.y,
		g_Enemy[5].hp
		);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < g_EnemyMAX; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Enemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Enemy[i].w;					// エネミーの表示幅
			float ph = g_Enemy[i].h;					// エネミーの表示高さ
			float tw = 1.0f;							// テクスチャの幅
			float th = 1.0f;							// テクスチャの高さ
			float tx = 0.0f;							// テクスチャの左上X座標
			float ty = 0.0f;							// テクスチャの左上Y座標

			// エネミー（通常）
			if (g_Enemy[i].ballFlg != TRUE)
			{
				tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// 緑のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_G)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// 紫のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_P)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// 紫のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_R)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}
			// エネミー（玉）
			else if (g_Enemy[i].ballFlg == TRUE)
			{
				tw = 1.0f / TEXTURE_PATTERN_BALL_DIVIDE_X;	// テクスチャの幅
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_BALL_DIVIDE_X) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_BALL_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// 緑のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_G)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// 紫のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_P)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// 赤のウジ虫
				if (g_Enemy[i].type == ENEMY_TYPE_R)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}


// 生きてるエネミーの数
int GetEnemyCount(void)
{
	return g_EnemyCount;
}

//=============================================================================
// エネミーとマップチップ（壁）との当たり判定
//=============================================================================
BOOL CollisionEnemyToMapchip(XMFLOAT3 move, int i)
{
	// 各頂点の情報
	int leftX = (int)((g_Enemy[i].pos.x + move.x) / MAPCHIP_WIDTH);
	int rightX = (int)((g_Enemy[i].pos.x + g_Enemy[i].w + move.x - 1.0f) / MAPCHIP_WIDTH);
	int topY = (int)((g_Enemy[i].pos.y + move.y) / MAPCHIP_HEIGHT);
	int bottomY = (int)((g_Enemy[i].pos.y + g_Enemy[i].h + move.y - 1.0f) / MAPCHIP_HEIGHT);

	XMINT2 edgeList[EDGE_MAX][2]
	{
		// 左辺
		{
			XMINT2(leftX,topY),
			XMINT2(leftX,bottomY)
		},
		// 右辺
		{
			XMINT2(rightX,topY),
			XMINT2(rightX,bottomY)
		},
		// 上辺
		{
			XMINT2(leftX,topY),
			XMINT2(rightX,topY)
		},
		// 下辺
		{
			XMINT2(leftX,bottomY),
			XMINT2(rightX,bottomY)
		},
	};

	// 使用する辺を決める
	BOOL isCheckEdge[EDGE_MAX]
	{
		move.x < 0 ? TRUE : FALSE,		// プレイヤーは左向き。当たる辺は右辺
		move.x > 0 ? TRUE : FALSE,		// プレイヤーは右向き。当たる辺は左辺
		move.y < 0 ? TRUE : FALSE,		// プレイヤーは上向き。当たる辺は下辺
		move.y > 0 ? TRUE : FALSE,		// プレイヤーは下向き。当たる辺は上辺
	};

	for (int j = 0; j < (int)EDGE_MAX; j++)
	{
		if (isCheckEdge[j] == FALSE) continue;

		for (int y = edgeList[j][0].y; y <= edgeList[j][1].y; y++)
		{
			for (int x = edgeList[j][0].x; x <= edgeList[j][1].x; x++)
			{
				int ans = CheckField(y, x);
				switch (ans)
				{
				case 1:
					g_Enemy[i].colDir = j;
					SetCollisionEnemyPos(x, y, i, j);
					return TRUE;
				default:
					break;
				}
			}
		}
	}
	return FALSE;
}

// 接触した箇所の情報の取得
void SetCollisionEnemyPos(int x, int y, int i, int colDir)
{
	XMFLOAT3 mapchipPos = XMFLOAT3((float)x * MAPCHIP_WIDTH, (float)y * MAPCHIP_HEIGHT, 0.0f);

	switch (colDir)
	{
		// エネミーの左側
	case EDGE_LEFT:
		g_Enemy[i].colPos = mapchipPos.x + MAPCHIP_WIDTH;
		break;
		// エネミーの右側
	case EDGE_RIGHT:
		g_Enemy[i].colPos = mapchipPos.x - g_Enemy[i].w;
		break;
		// エネミーの上側
	case EDGE_TOP:
		g_Enemy[i].colPos = mapchipPos.y + MAPCHIP_HEIGHT;
		break;
		// エネミーの下側
	case EDGE_BOTTOM:
		g_Enemy[i].colPos = mapchipPos.y - g_Enemy[i].h;
		break;

	default:
		break;
	}
}

