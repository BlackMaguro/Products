//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "tutorial.h"
#include "input.h"
#include "bg.h"
#include "field.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "fade.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// キャラサイズ(200/2)
#define TEXTURE_HEIGHT				(64)	// 
#define TEXTURE_MAX					(8)		// テクスチャの数
#define FRAME_RATE					(60)	// フレーム数


#define PLAYER_MAX_HP				(100)	// プレイヤーの最大HP

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// ジャンプの高さ

#define JUMP_Y						(3)	// ジャンプの倍率
#define EDGE_MAX					(4)	// Edge（辺）の最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	//"data/TEXTURE/char01_old.png",
	"data/TEXTURE/player_chara_walk.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/bar_white.png",
	"data/TEXTURE/HPBar.png",
	"data/TEXTURE/item_1.png",
	"data/TEXTURE/player_face_normal_HP_1.png",
	"data/TEXTURE/player_face_frame.png",
	"data/TEXTURE/instructions.png",
};


static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static BOOL		g_bossFlg = FALSE;			// ボス戦か判断するフラグ
static BOOL		g_instructionFlg = FALSE;	// 操作方法を出すか判断するフラグ

static PLAYER	g_Player[PLAYER_MAX];		// プレイヤー構造体
static int		g_PlayerCount = PLAYER_MAX;	// 生きてるプレイヤーの数

static int      g_jumpCnt = 0;
static int		g_TimeCount = 0;
static int		g_MutekiTimeCount = 0;

static float    g_gravity = 1.0f;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};
FIELD* field = GetField();


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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

	g_PlayerCount = 0;						// 生きてるプレイヤーの数

	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_PlayerCount++;
		g_Player[i].use = TRUE;
		//g_Player[i].pos = XMFLOAT3(7764.0f, 448.0f, 0.0f);	// 中心点から表示
		g_Player[i].pos = XMFLOAT3(280.0f, 448.0f, 0.0f);	// 中心点から表示
		if (GetMode() == MODE_TUTORIAL)
		{
			g_Player[i].pos = XMFLOAT3(100.0f, 448.0f, 0.0f);	// 中心点から表示
		}
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].hp = PLAYER_MAX_HP;						// プレイヤーのHP
		//if (GetBossFlg() == TRUE) g_Player[i].hp = 20;		// デバッグ用
		g_Player[i].muteki = FALSE;							// 無敵状態のフラグ
		g_Player[i].tutorialMove = TRUE;					// チュートリアル中の移動フラグ
		g_Player[i].mutekiTime = 0;							// 無敵時間
		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;
		g_Player[i].healItem = 3;							// 回復アイテム

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Player[i].vector = XMINT2(0, 0);					// 移動量ベクトル
		g_Player[i].colPos = 0.0f;							// 衝突したときの座標用
		g_Player[i].colDir = EDGE_LEFT;						// 衝突したときの向き


		g_Player[i].dir = CHAR_DIR_RIGHT;					// 右向き
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// 分身用
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	g_TimeCount += 1;
	g_Player[0].mutekiTime += 1;
	if (g_Load == FALSE) return;
	g_PlayerCount = 0;				// 生きてるプレイヤーの数

	// 5秒経過したら無敵フラグをFALSEにする
	if (g_Player[0].mutekiTime > FRAME_RATE * 5)
	{
		g_Player[0].muteki = FALSE;
	}

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			g_PlayerCount++;		// 生きてるプレイヤーの数
			
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;

			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			// ジャンプをしたときに残像もジャンプさせる
			pos_old.y += g_Player[i].jumpY;
			g_Player[i].offset[0] = pos_old;

			// アニメーション  
			if (g_Player[i].moving == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}

			// キー入力で移動 
			{
				float speed = g_Player[i].move.x;
				float vertical = 0.0f;			// 縦の移動量
				float horizontal = 0.0f;		// 横の移動量
				float gravity = 8.0f;

				g_Player[i].moving = FALSE;
				g_Player[i].dash = FALSE;

				// チュートリアル中で特定の場所まで来たら動きを止める
				if (g_Player[0].tutorialMove == TRUE)
				{
					// Cキーが押されたときダッシュする
					if (GetKeyboardPress(DIK_C) || IsButtonPressed(0, BUTTON_A))
					{
						PlaySound(SOUND_LABEL_SE_13_DASH);
						speed *= 4;
						g_Player[i].dash = TRUE;
					}
					// 通常マップでは上と下には向けない
					if (GetBossFlg() == TRUE)
					{
						if (GetKeyboardPress(DIK_DOWN))
						{
							vertical += speed;
							g_Player[i].dir = CHAR_DIR_DOWN;
							g_Player[i].moving = TRUE;
						}
						else if (GetKeyboardPress(DIK_UP))
						{
							g_Player[i].dir = CHAR_DIR_UP;
							g_Player[i].moving = TRUE;
						}
						// ゲームパッドでの処理
						if (IsButtonPressed(0, BUTTON_DOWN))
						{
							g_Player[i].pos.y += speed;
							g_Player[i].dir = CHAR_DIR_DOWN;
							g_Player[i].moving = TRUE;
						}
						else if (IsButtonPressed(0, BUTTON_UP))
						{
							g_Player[i].pos.y -= speed;
							g_Player[i].dir = CHAR_DIR_UP;
							g_Player[i].moving = TRUE;
						}

					}

					if (GetKeyboardPress(DIK_RIGHT))
					{
						horizontal += speed;
						g_Player[i].dir = CHAR_DIR_RIGHT;
						g_Player[i].moving = TRUE;
					}
					else if (GetKeyboardPress(DIK_LEFT))
					{
						horizontal -= speed;
						g_Player[i].dir = CHAR_DIR_LEFT;
						g_Player[i].moving = TRUE;
					}

					// ゲームパッドで移動処理
					if (IsButtonPressed(0, BUTTON_RIGHT))
					{
						g_Player[i].pos.x += speed;
						g_Player[i].dir = CHAR_DIR_RIGHT;
						g_Player[i].moving = TRUE;
					}
					else if (IsButtonPressed(0, BUTTON_LEFT))
					{
						g_Player[i].pos.x -= speed;
						g_Player[i].dir = CHAR_DIR_LEFT;
						g_Player[i].moving = TRUE;
					}


					// 力業ジャンプ処理
					if (g_jumpCnt > 0)
					{
						vertical += g_jump[g_jumpCnt] * JUMP_Y;
						g_jumpCnt++;
						if (g_jumpCnt >= PLAYER_JUMP_CNT_MAX)
						{
							g_jumpCnt = 0;
						}
					}

					// Jキーが押されたときジャンプする
					if ((g_jumpCnt == 0) && (GetKeyboardTrigger(DIK_J)))
					{
						PlaySound(SOUND_LABEL_SE_09_JUMP);
						vertical += g_jump[g_jumpCnt];
						g_jumpCnt++;
					}

					// 回復する
					if ((GetMode() == MODE_GAME) || (GetMode() == MODE_BOSS))
					{
						if (GetKeyboardTrigger(DIK_H))
						{
							if (g_Player[i].healItem > 0)
							{
								PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
								g_Player[i].healItem -= 1;
								g_Player[i].hp += 100;
								if (g_Player[i].hp > 100) g_Player[i].hp = 100;
							}
						}
					}
					if ((GetMode() == MODE_TUTORIAL) && (GetTutorialMessageCount() >= 7))
					{
						if (GetKeyboardTrigger(DIK_H))
						{
							if (g_Player[i].healItem > 0)
							{
								PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
								g_Player[i].healItem -= 1;
								g_Player[i].hp += 100;
								if (g_Player[i].hp > 100) g_Player[i].hp = 100;
							}
						}
					}


					// 操作説明を出す
					if ((GetMode() == MODE_TUTORIAL) && (GetTutorialMessageCount() >= 11) && GetKeyboardTrigger(DIK_F))
					{
						PlaySound(SOUND_LABEL_SE_08_FKEY);
						g_instructionFlg = !g_instructionFlg;
					}
					else if((GetMode() == MODE_GAME) && GetKeyboardTrigger(DIK_F))
					{
						PlaySound(SOUND_LABEL_SE_08_FKEY);
						g_instructionFlg = !g_instructionFlg;
					}
				}

				// ジャンプ処理中？（ここから）
				//if (g_Player[i].jump == TRUE)
				//{
				//	float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
				//	float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);
				//	//g_Player[i].jumpY = y;

				//	vertical += y;

				//	g_Player[i].jumpCnt++;
				//	if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
				//	{
				//		g_Player[i].jump = FALSE;
				//		g_Player[i].jumpCnt = 0;
				//		g_Player[i].jumpY = 0.0f;
				//	}

				//}
				//// ジャンプボタン押した？
				//else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_J)))
				//{
				//	g_Player[i].jump = TRUE;
				//	g_Player[i].jumpCnt = 0;
				//	g_Player[i].jumpY = 0.0f;
				//}
				// ジャンプ処理中？（ここまで）


				// MAP外チェック
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// 移動できる場所かのチェック（重力）
				if (CollisionPlayerToMapchip(XMFLOAT3(0.0f, gravity, 0.0f), i) == FALSE)
				{
					// 移動量を足す
					g_Player[i].pos.y += gravity;
				}
				else
				{
					// 衝突前に戻す
					g_Player[i].pos.y = g_Player[i].colPos;
				}

				// 移動できる場所かのチェック（横方向）
				if (CollisionPlayerToMapchip(XMFLOAT3(horizontal, 0.0f, 0.0f), i) == FALSE)
				{
					// 移動量を足す
					g_Player[i].pos.x += horizontal;
				}
				else
				{
					// 衝突前に戻す
					g_Player[i].pos.x = g_Player[i].colPos;
				}
				// 移動できる場所かのチェック（縦方向）
				if (CollisionPlayerToMapchip(XMFLOAT3(0.0f, vertical, 0.0f), i) == FALSE)
				{
					// 移動量を足す
					g_Player[i].pos.y += vertical;
				}
				else
				{
					// 衝突前に戻す
					g_Player[i].pos.y = g_Player[i].colPos;
				}

				// プレイヤーが何処にいるかの判定（イベント系）
				CheckPlayerToChipNum(g_Player[i].pos.y, g_Player[i].pos.x);
				
				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// プレイヤーの立ち位置からフィールドのスクロール座標を計算する
				field->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (field->pos.x < 0) field->pos.x = 0;												// 左端まで来た時の処理
				if (field->pos.x > field->w - SCREEN_WIDTH) field->pos.x = field->w - SCREEN_WIDTH;	// 端まで来た時の処理

				field->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (field->pos.y < 0) field->pos.y = 0;
				if (field->pos.y > field->h - SCREEN_HEIGHT) field->pos.y = field->h - SCREEN_HEIGHT;

				// 移動が終わったらエネミーとの当たり判定
				if(GetMode() != MODE_BOSS)
				{
					ENEMY* enemy = GetEnemy();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (enemy[j].use == TRUE)
						{
							XMFLOAT3 posCollision = g_Player[i].pos;
							posCollision.y += g_Player[i].jumpY;
							BOOL ans = CollisionBB(posCollision, g_Player[i].w, g_Player[i].h,
								enemy[j].pos, enemy[j].w, enemy[j].h);
							// 当たっている？
							if ((ans == TRUE) && (g_Player[i].muteki == FALSE))
							{
								// 当たった時の処理
								PlaySound(SOUND_LABEL_SE_05_PLAYERDAMAGE);
								g_Player[i].hp -= enemy[j].atk;
								g_Player[i].muteki = TRUE;
								g_Player[0].mutekiTime = 0;		// 無敵時間のリセット
								// HPがゼロになったら
								if (g_Player[i].hp <= 0)
								{
									g_Player[i].use = FALSE;
									SetMode(MODE_GAMEOVER);
								}
							}
						}
					}
				}

				BULLET* bullet = GetBullet();
				ENEMY* enemy = GetEnemy();

				// バレット処理（キーボード）
				if (GetKeyboardTrigger(DIK_SPACE))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					int bulletDir = g_Player[i].dir;	// 弾の方向を確定させる
					SetBullet(pos, bulletDir);
				}
				// バレット処理（ゲームパッド） 
				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					int bulletDir = g_Player[i].dir;	// 弾の方向を確定させる
					SetBullet(pos, bulletDir);
				}
				// ゴールでEnterを押下したとき
				if (GetKeyboardTrigger(DIK_RETURN) && field->goalFlg == TRUE)
				{// Enter押したら、ボスフラグを立ててステージを切り替える
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// ゲーム画面に飛ばす
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// ボス画面に飛ばす
					}
				}
				else if (IsButtonTriggered(0, BUTTON_START) && field->goalFlg == TRUE)
				{
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// ゲーム画面に飛ばす
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// ボス画面に飛ばす
					}
				}
				else if (IsButtonTriggered(0, BUTTON_B) && field->goalFlg == TRUE)
				{
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// ゲーム画面に飛ばす
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// ボス画面に飛ばす
					}
				}

			}
		}
	}

	// 現状をセーブする（チュートリアルとボス戦ではセーブ不可）
	if (GetKeyboardTrigger(DIK_S) && (GetBossFlg() != TRUE) && (GetTutorialFlg() != TRUE))
	{
		PlaySound(SOUND_LABEL_SE_14_SAVEDATAMAKE);
		SaveData();
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player pos.x:%f,pos.y:%f,\nHP：%d\ng_instructionFlg：%d\n", g_Player[0].pos.x, g_Player[0].pos.y, g_Player[0].hp,g_instructionFlg);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	// ダメージを受けたときに点滅させる
	float flashing = 1.0f;
	if (g_Player[0].muteki == TRUE)
	{
		flashing = (float)(g_TimeCount % 2);
	}
	else
	{
		flashing = 1.0f;
	}

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われているか
		{

			// プレイヤーの分身を描画
			if (g_Player[i].dash)
			{	// ダッシュ中だけ分身処理
				DrawPlayerOffset(i);
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = g_Player[i].w;		// プレイヤーの表示幅
			float ph = g_Player[i].h;		// プレイヤーの表示高さ


			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, flashing),
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);


			// プレイヤーのHPゲージ（黒）
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 220.0f;		// ゲージの表示位置X
				float py = 460.0f;		// ゲージの表示位置Y
				float pw = 180.0f;		// ゲージの表示幅
				float ph =  70.0f;		// ゲージの表示高さ

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


				// プレイヤーのHPに応じて長さを変更（赤色のバー）
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				//ゲージの位置やテクスチャー座標を反映
				pw = pw * ((float)g_Player[i].hp / PLAYER_MAX_HP);
				ph = 50.0f;		// ゲージの表示高さ

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// プレイヤーのHPバー
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 130.0f;		// ゲージの表示位置X
				float py = 440.0f;		// ゲージの表示位置Y
				float pw = 300.0f;		// ゲージの表示幅
				float ph = 100.0f;		// ゲージの表示高さ

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
			// HPアイテムの表示
			if (g_Player[i].healItem > 0)
			{
				int healItem = g_Player[i].healItem;

				for(int i = 0; i < healItem; i++)
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

					//ゲージの位置やテクスチャー座標を反映
					float px = 450.0f + i * 74.0f;		// ゲージの表示位置X
					float py = 450.0f;		// ゲージの表示位置Y
					float pw = 64.0f;		// ゲージの表示幅
					float ph = 64.0f;		// ゲージの表示高さ

					float tw = 1.0f;		// テクスチャの幅
					float th = 1.0f;		// テクスチャの高さ
					float tx = 0.0f;		// テクスチャの左上X座標
					float ty = 0.0f;		// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteLTColor(g_VertexBuffer,
						px, py, pw, ph,
						tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}
			}
			// プレイヤーの表情
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 25.0f;		// ゲージの表示位置X
				float py = 440.0f;		// ゲージの表示位置Y
				float pw = 85.0f;		// ゲージの表示幅
				float ph = 85.0f;		// ゲージの表示高さ

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

			// プレイヤーの表情の枠（フレーム）
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 10.0f;		// ゲージの表示位置X
				float py = 425.0f;		// ゲージの表示位置Y
				float pw = 115.0f;		// ゲージの表示幅
				float ph = 115.0f;		// ゲージの表示高さ

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
			// 操作方法の表示
			if(g_instructionFlg == TRUE)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				//ゲージの位置やテクスチャー座標を反映
				float px = 600;		// ゲージの表示位置X
				float py = 20.0f;				// ゲージの表示位置Y
				float pw = 300.0f;				// ゲージの表示幅
				float ph = 200.0f;				// ゲージの表示高さ

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


}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


// 生きてるプレイヤーの数
int GetPlayerCount(void)
{
	return g_PlayerCount;
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// プレイヤーとマップチップ（壁）との当たり判定
//=============================================================================
BOOL CollisionPlayerToMapchip(XMFLOAT3 move,int i)
{
	// 各頂点の情報
	int leftX = (int)((g_Player[i].pos.x + move.x) / MAPCHIP_WIDTH);
	int rightX = (int)((g_Player[i].pos.x + g_Player[i].w + move.x - 1.0f) / MAPCHIP_WIDTH);
	int topY = (int)((g_Player[i].pos.y + move.y) / MAPCHIP_HEIGHT);
	int bottomY = (int)((g_Player[i].pos.y + g_Player[i].h + move.y - 1.0f) / MAPCHIP_HEIGHT);

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
					g_Player[i].colDir = j;
					SetCollisionPos(x,y,i,j);
					return TRUE;
				// 扉に触れていた場合
				case 2:
					field->goalFlg = TRUE;
				//	g_Player[i].colDir = j;
				//	SetCollisionPos(x, y, i, j);
				default:
					break;
				}
			}
		}
	}
	return FALSE;
}

// 接触した箇所の情報の取得
void SetCollisionPos(int x,int y,int i,int colDir)
{
	XMFLOAT3 mapchipPos = XMFLOAT3((float)x * MAPCHIP_WIDTH, (float)y * MAPCHIP_HEIGHT, 0.0f);

	switch (colDir)
	{
	// プレイヤーの左側
	case EDGE_LEFT:
		g_Player[i].colPos = mapchipPos.x + MAPCHIP_WIDTH;
		break;
	// プレイヤーの右側
	case EDGE_RIGHT:
		g_Player[i].colPos = mapchipPos.x - g_Player[i].w;
		break;
	// プレイヤーの上側
	case EDGE_TOP:
		g_Player[i].colPos = mapchipPos.y + MAPCHIP_HEIGHT;
		break;
	// プレイヤーの下側
	case EDGE_BOTTOM:
		g_Player[i].colPos = mapchipPos.y - g_Player[i].h;
		break;
	
	default:
		break;
	}
}
