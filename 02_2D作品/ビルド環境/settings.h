//=============================================================================
//
// 設定画面処理 [settings.h]
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
#define SETTINGS_VOLUME_FRONT_MAX		(8)			// 設定画面の見た目上のボリューム数値 


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class SETTINGS
{
public:

	float volume;	// サウンドの実際の音量
	int volumeNum;	// サウンドの見た目上の音量
	//float seVolume[SOUND_LABEL_MAX];	// サウンドの音量

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSettings(void);
void UninitSettings(void);
void UpdateSettings(void);
void DrawSettings(void);

float GetVolume(void);
void SetSoundVolume(float volume);
int GetVolumeNum(void);
void SetVolumeNum(int volumeNum);
float CheckVolume(float volume);
