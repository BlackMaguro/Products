//=============================================================================
//
// ゲーム画面処理 [game.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// ディレクトリパス
#define PATH_TEXTURE_INGAME		"data/TEXTURE/04_INGAME/"
#define PATH_TEXTURE_ICON		"data/TEXTURE/04_INGAME/01_ICON/"
#define PATH_TEXTURE_EFFECT		"data/TEXTURE/04_INGAME/04_EFFECT/"
#define PATH_MODEL_PLAYER		"data/MODEL/01_PLAYER/"
#define PATH_MODEL_WEAPON		"data/MODEL/05_WEAPON/"
#define PATH_MODEL_ENEMY		"data/MODEL/06_ENEMY/"
#define PATH_TEXTURE_BILLBOARD_THROWMAKER	"data/TEXTURE/04_INGAME/05_BILLBOARD/01_throwMarker/"
#define PATH_TEXTURE_EFFECT_EXPLOSION		"data/TEXTURE/04_INGAME/04_EFFECT/01_explosion/"


// INGAMEで使用する定数
static const float DELTATIME = 1.0f / FRAME_RATE;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
void CheckHit(void);
void checkPlayerToEnemy(XMFLOAT3 playerOldPos);
void SetPauseFlg(BOOL pauseFlg);
BOOL GetPauseFlg(void);

