//=============================================================================
//
// �Q�[����ʏ��� [game.h]
// Author : ����z��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �f�B���N�g���p�X
#define PATH_TEXTURE_INGAME		"data/TEXTURE/04_INGAME/"
#define PATH_TEXTURE_ICON		"data/TEXTURE/04_INGAME/01_ICON/"
#define PATH_TEXTURE_EFFECT		"data/TEXTURE/04_INGAME/04_EFFECT/"
#define PATH_MODEL_PLAYER		"data/MODEL/01_PLAYER/"
#define PATH_MODEL_WEAPON		"data/MODEL/05_WEAPON/"
#define PATH_MODEL_ENEMY		"data/MODEL/06_ENEMY/"
#define PATH_TEXTURE_BILLBOARD_THROWMAKER	"data/TEXTURE/04_INGAME/05_BILLBOARD/01_throwMarker/"
#define PATH_TEXTURE_EFFECT_EXPLOSION		"data/TEXTURE/04_INGAME/04_EFFECT/01_explosion/"


// INGAME�Ŏg�p����萔
static const float DELTATIME = 1.0f / FRAME_RATE;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
void CheckHit(void);
void checkPlayerToEnemy(XMFLOAT3 playerOldPos);
void SetPauseFlg(BOOL pauseFlg);
BOOL GetPauseFlg(void);

