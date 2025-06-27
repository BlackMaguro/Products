//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SOUND_VOLUME_MAX			(1.0f)		// 実際のボリューム最大値 

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_00_TITLE,				// BGM0（タイトル）
	SOUND_LABEL_BGM_01_TUTORIAL,			// BGM1（チュートリアル）
	SOUND_LABEL_BGM_02_INGAME,				// BGM2（ゲーム本編）
	SOUND_LABEL_BGM_03_RESULT,				// BGM3（リザルト画面）
	SOUND_LABEL_BGM_04_GAMEOVER,			// BGM4（ゲームオーバー画面）
	SOUND_LABEL_BGM_05_SETTINGS,			// BGM5（設定画面）
	SOUND_LABEL_SE_00_KEYUPDOWN,			// SE00（カーソルの上下キーを押したとき）
	SOUND_LABEL_SE_01_ENTER,				// SE01（ENTERを押したとき）
	SOUND_LABEL_SE_02_GUNSHOT,				// SE02（弾を発車したとき）
	SOUND_LABEL_SE_03_BULLETTOOBJ,			// SE03（弾がオブジェクトに当たったとき）
	SOUND_LABEL_SE_04_PLAYERFFROMDAMAGE,	// SE04（プレイヤーがダメージを受けたとき）
	SOUND_LABEL_SE_05_TIMELOW,				// SE05（時間が少なくなったとき）
	SOUND_LABEL_SE_06_PERSPECTIVECHANGE,	// SE06（視点を変更したとき）
	SOUND_LABEL_SE_07_GETMONEY,				// SE07（金を手に入れたとき）
	SOUND_LABEL_SE_08_CHANGEWEAPON,			// SE08（武器を入れ替えたとき）
	SOUND_LABEL_SE_09_WOODENSWORD,			// SE09（木刀で叩いたとき）
	SOUND_LABEL_SE_10_THROWBOMB,			// SE10（爆弾を投げたとき）
	SOUND_LABEL_SE_11_EXPLOSIONBOMB,		// SE11（爆弾が爆発したとき）

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

