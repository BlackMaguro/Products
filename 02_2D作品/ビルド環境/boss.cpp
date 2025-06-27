//=============================================================================
//
// ボス処理 [boss.cpp]
// Author : 
//
//=============================================================================
#include "boss.h"
#include "bulletEnemy.h"
#include "bg.h"
#include "player.h"
#include "field.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(760)	// ボスキャラサイズ_体_横
#define TEXTURE_HEIGHT				(450)	// ボスキャラサイズ_体_縦
#define TEXTURE_MAX					(8)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)						// アニメーションパターン数（ボスの口）
#define ANIM_EYE_PATTERN_NUM			(TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X*TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y)	// アニメーションパターン数（ボスの目）
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値
#define ANIM_WAIT_EYE				(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Boss/boss_6_back.png",		// ボスの背中
	"data/TEXTURE/Boss/boss_5_body.png",		// ボスの体
	"data/TEXTURE/Boss/boss_4_face.png",		// ボスの頭
	"data/TEXTURE/Boss/boss_3_tongue.png",		// ボスの舌
	"data/TEXTURE/Boss/boss_2_eye.png",			// ボスの目
	"data/TEXTURE/Boss/boss_1_mouth.png",		// ボスの口
	"data/TEXTURE/bar_white.png",				// ボスのHP
	"data/TEXTURE/BOSS/boss_HPBar.png",			// ボスのHPバー
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static BOSS	g_Boss[BOSS_MAX];		// ボス構造体
BOOL g_bossAtkFlg = FALSE;			// ボスが攻撃するかのフラグ

static int		g_BossCount = BOSS_MAX;
float g_bossTimeCount = 0.0f;
int	  g_bossTime = 0;
float bossX = (float)SCREEN_CENTER_X;	// ボスの中心座標
float bossBodyY = 320.0f;
int g_bossAtkTime = 0;
int atkStartTime = 0;
float sinTime = 0.0f;


// ボスの体の動き
static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率					時間
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.1f, 1.1f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.9f, 0.9f, 1.0f),	60 },
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(3000.0f, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoss(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// ボス構造体の初期化
	g_BossCount = 0;
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_BossCount++;
		g_Boss[i].use = TRUE;
		g_Boss[i].pos = XMFLOAT3(bossX, 320.0f, 0.0f);	// 中心点から表示
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		//g_Boss[i].w = TEXTURE_WIDTH;
		g_Boss[i].w = 0.0f;
		g_Boss[i].h = 0.0f;
		g_Boss[i].texNo = 0;

		g_Boss[i].countAnim = 0;
		g_Boss[i].patternAnim = 0;

		g_Boss[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Boss[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Boss[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Boss[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Boss[i].hp = BOSS_MAX_HP;				// ボスのHPをセット
		g_Boss[i].atk = (int)(BOSS_MAX_ATK * 0.5);		// ボスの最大攻撃力の半分をセット
		g_Boss[i].moveFlg = TRUE;				// ボスが動いているかのフラグ
		g_Boss[i].patternAtk = 0;				// ボスの攻撃パターン
	}
	// ボスの背中
	g_Boss[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Boss[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Boss[0].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Boss[0].w = 760.0f;
	g_Boss[0].h = 450.0f;

	// ボスの体
	g_Boss[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Boss[1].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Boss[1].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Boss[1].w = 760.0f;
	g_Boss[1].h = 450.0f;

	// ボスの頭
	g_Boss[2].w = 198.0f;
	g_Boss[2].h = 306.0f;
	g_Boss[2].pos = XMFLOAT3(bossX, 310.0f, 0.0f);

	// ボスの舌
	g_Boss[3].w = 64.0f;
	g_Boss[3].h = 54.0f;
	g_Boss[3].pos = XMFLOAT3(bossX, 385.0f, 0.0f);

	// ボスの目
	g_Boss[4].w = 64.0f;
	g_Boss[4].h = 64.0f;
	g_Boss[4].pos = XMFLOAT3(bossX, 360.0f, 0.0f);

	//// ボスの口
	g_Boss[5].w = 110.0f;
	g_Boss[5].h = 165.0f;
	g_Boss[5].pos = XMFLOAT3(bossX, 360.0f, 0.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void)
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
void UpdateBoss(void)
{
	if (g_Load == FALSE) return;
	g_BossCount = 0;			// 生きてるボスの数
	g_bossTimeCount += 1.0f;

	if (g_bossTimeCount == 60.0f)
	{
		g_bossTime += 1;
		g_bossAtkTime += 1;
		g_bossTimeCount = 0.0f;
	}
	sinTime += 0.016f;
	float sinValue = sin(sinTime);
	for (int i = 0; i < BOSS_MAX; i++)
	{
		// 生きてるボスだけ処理をする
		if (g_Boss[i].use == TRUE)
		{
			g_BossCount++;		// 生きてた敵の数

			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Boss[i].pos;

			// アニメーション  
			//g_Boss[i].countAnim += 1.0f;
			g_Boss[i].countAnim += 0.1f;
			// ボスの（目）の動き
			if ((i == 4) && (g_Boss[i].countAnim > ANIM_WAIT_EYE))
			{
				g_Boss[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Boss[i].patternAnim = (g_Boss[i].patternAnim + 1) % ANIM_EYE_PATTERN_NUM;
			}
			// ボスの（口）の動き
			// 最大HPの6分の5が減るまで動かさない
			if(g_Boss[4].hp <= (BOSS_MAX_HP * 5) / 6)
			{
				if ((i == 5) && (g_Boss[i].countAnim > ANIM_WAIT))
				{
					g_Boss[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Boss[i].patternAnim = (g_Boss[i].patternAnim + 1) % ANIM_PATTERN_NUM;
					//g_Boss[i].patternAnim = 2;
				}
			}
			else if((g_Boss[4].hp >= (BOSS_MAX_HP * 5) / 6))
			{
				// 回復したときは戻す
				g_Boss[5].patternAnim = 0;
			}


			// 移動処理
			if (g_Boss[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Boss[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Boss[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Boss[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Boss[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frameを使て時間経過処理をする
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Boss[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Boss[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}
			}

			// エネミーの攻撃処理（3秒経過するごとに攻撃する）
			//if (GetEnemyBulletAllLostFlg() == TRUE)
			if ((g_Boss[i].hp < BOSS_MAX_HP - 50) && g_bossAtkTime >= 3)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp < BOSS_MAX_HP - 100) && g_bossAtkTime >= 2)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp < BOSS_MAX_HP - 200) && g_bossAtkTime >= 1)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp >= BOSS_MAX_HP - 50) && g_bossAtkTime >= 5)
			{
				SetBossAtkFlg(TRUE);
			}

			if(GetBossAtkFlg() == TRUE)
			{
				atkStartTime += 1;
				BULLETENEMY* bulletEnemy = GetBulletEnemy();

				// 攻撃パターンの取得
				g_Boss[4].patternAtk = GetBossPatternAtk();
				//g_Boss[4].patternAtk = 3;
				// ボスの攻撃パターンの確定
				// ボス（目）の位置が基準
				XMFLOAT3 pos = g_Boss[4].pos;
				SetBulletEnemy(pos, g_Boss[4].patternAtk);

				// 弾が発射されたのでフラグをオフにする
				//SetEnemyBulletAllLostFlg(FALSE);
				// 攻撃フラグをオフにする
				if (atkStartTime >= 120)
				{
					SetBossAtkFlg(FALSE);
					g_bossAtkTime = 0;
					atkStartTime = 0;
				}

			}

			// 移動が終わったらボスとの当たり判定（ボスとは当たらないから未使用）
			{
				PLAYER* player = GetPlayer();

				// ボスの数分当たり判定を行う
				for (int j = 0; j < BOSS_MAX; j++)
				{
					// 生きてるボスと当たり判定をする
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Boss[i].pos, g_Boss[i].w, g_Boss[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE && (player[0].muteki == FALSE))
						{
							//player->hp -= 10;
							// 当たった時の処理
							//player[j].use = FALSE;
						}
					}
				}
			}

		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc(
		"Boss[4] pos.x:%f,pos.y:%f,\nHP:%d,\ng_Boss[5].countAnim:%f,\ng_Boss[5].countAnim:%f,\n sinvalue:%f,\ng_Boss[4].patternAnim:%d,\ng_Boss[5].patternAnim:%d,\ng_Boss[4].patternAtk:%d,\ng_bossTime:%d,\ng_bossAtkTime:%d,\n",
		g_Boss[4].pos.x, g_Boss[4].pos.y,g_Boss[4].hp, g_Boss[4].countAnim, g_Boss[5].countAnim, sinValue, g_Boss[4].patternAnim,g_Boss[5].patternAnim, g_Boss[4].patternAtk, g_bossTime, g_bossAtkTime
	);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(void)
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


	float sinValue = sin(sinTime);

	BG* bg = GetBG();

	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE)			// このボスが使われている？
		{									// Yes

			//ボスの位置やテクスチャー座標を反映
			float px = g_Boss[i].pos.x - bg->pos.x;	// ボスの表示位置X
			float py = g_Boss[i].pos.y - bg->pos.y;	// ボスの表示位置Y
			float pw = g_Boss[i].w;		// ボスの表示幅
			float ph = g_Boss[i].h;		// ボスの表示高さ
			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			if (i == 0)
			{
				px += 20 * sinValue * (-1);
			}
			if (i == 2 || i == 3 || i == 4 || i == 5)
			{
				px += 20 * sinValue;
			}

			// ボスの目の時
			if (i == 4)
			{
				tw = 1.0f / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X;	// テクスチャの幅
				th = 1.0f / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				tx = (float)(g_Boss[i].patternAnim % TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Boss[i].patternAnim / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標
			}

			// ボスの口の時
			if (i == 5)
			{
				tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				tx = (float)(g_Boss[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Boss[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標
			}

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[i].rot.z);


			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// ボスのゲージ
	{
		// 下敷きのゲージ（黒色）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		//ゲージの位置やテクスチャー座標を反映
		float px = 280.0f;		// ゲージの表示位置X
		float py =  10.0f;		// ゲージの表示位置Y
		float pw = 400.0f;		// ゲージの表示幅
		float ph = 70.0f;		// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// ボスのHPに従ってゲージの長さを表示してみる（青色）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		//ゲージの位置やテクスチャー座標を反映（目のときの体力を参照）
		pw = pw * ((float)g_Boss[4].hp / BOSS_MAX_HP);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// HPバーの枠
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

			//ゲージの位置やテクスチャー座標を反映
			float px = 280.0f;		// ゲージの表示位置X
			float py = 10.0f;		// ゲージの表示位置Y
			float pw = 400.0f;		// ゲージの表示幅
			float ph = 70.0f;		// ゲージの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}

}

//=============================================================================
// ボスの攻撃パターンを取得する
//=============================================================================
int GetBossPatternAtk(void)
{
	int ans = GetRand(0, 4);
	// ボス（目）の体力状況によって攻撃パターンを変化させる
	// HP250まではパターン1と2
	if (g_Boss[4].hp > (BOSS_MAX_HP * 5 / 6))
	{
		ans %= 2;
	}
	// HP200まではパターン1～3
	else if ((200.0f < g_Boss[4].hp) && (g_Boss[4].hp <= (BOSS_MAX_HP - 50.0f)))
	{
		ans %= 3;
	}
	// HP100より大きく200以下はパターン1～4
	else if (100.0f < g_Boss[4].hp && g_Boss[4].hp <= (BOSS_MAX_HP - 100.0f))
	{
		ans %= 4;
	}
	// HP100まではパターン1～5（ALL）
	else if (g_Boss[4].hp <= (BOSS_MAX_HP - 200.0f))
	{
		ans %= 5;
		// 体力が0に近づいたら強くする
		if (ans < 3 && (g_Boss[4].hp <= BOSS_MAX_HP / 6))
		{
			ans = (ans + 1) % 3 + 3;
		}
	}

	return ans;
}

//=============================================================================
// ボスが攻撃するかどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetBossAtkFlg(void)
{
	return g_bossAtkFlg;
}

void SetBossAtkFlg(BOOL bossAtkFlg)
{
	g_bossAtkFlg = bossAtkFlg;
}

//=============================================================================
// Boss構造体の先頭アドレスを取得
//=============================================================================
BOSS* GetBoss(void)
{
	return &g_Boss[0];
}


// 生きてるボスの数
int GetBossCount(void)
{
	return g_BossCount;
}

