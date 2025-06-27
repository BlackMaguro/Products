//=============================================================================
//
// ボス前の会話画面処理 [bossbefore.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBossBefore(void);
void UninitBossBefore(void);
void UpdateBossBefore(void);
void DrawBossBefore(void);

BOOL GetBossBeforeFlg(void);
void SetBossBeforeFlg(BOOL bossBeforeFlg);
