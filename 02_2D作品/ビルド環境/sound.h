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
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_00_TITLE,			// BGM0（タイトル）
	SOUND_LABEL_BGM_01_SETTINGS,		// BGM1（せってい）
	SOUND_LABEL_BGM_02_PROLOGUE,		// BGM2（プロローグ）
	SOUND_LABEL_BGM_03_TUTORIAL,		// BGM3（チュートリアル）
	SOUND_LABEL_BGM_04_GAMEMODE,		// BGM4（ゲーム本編）
	SOUND_LABEL_BGM_05_BOSSBEFORE,		// BGM5（ボス前の会話）
	SOUND_LABEL_BGM_06_BOSS,			// BGM6（ボス戦）
	SOUND_LABEL_BGM_07_GAMEOVER,		// BGM7（ゲームオーバー）
	SOUND_LABEL_BGM_08_EPPILOGUE,		// BGM8（エピローグ）
	SOUND_LABEL_BGM_09_RESULT,			// BGM9（リザルト）
	SOUND_LABEL_SE_00_MOUSEUPDOWN,		// SE0（カーソルの上下キーを押したとき）
	SOUND_LABEL_SE_01_ENTER,			// SE1（Enterを押したとき）
	SOUND_LABEL_SE_02_MESSAGECLICK,		// SE2（メッセージでEnterを押したとき）
	SOUND_LABEL_SE_03_PLAYERBULLET,		// SE3（プレイヤーが弾を発射したとき）
	SOUND_LABEL_SE_04_BOSSFIRE,			// SE4（ボスが弾を発射したとき）
	SOUND_LABEL_SE_05_PLAYERDAMAGE,		// SE5（プレイヤーがダメージを受けたとき）
	SOUND_LABEL_SE_06_DOORENTER,		// SE6（扉でEnterを押したとき）
	SOUND_LABEL_SE_07_ENEMYBULLETHIT,	// SE7（エネミーに弾が当たったとき）
	SOUND_LABEL_SE_08_FKEY,				// SE8（Fキーが押されたとき）
	SOUND_LABEL_SE_09_JUMP,				// SE9（プレイヤーのジャンプ音）
	SOUND_LABEL_SE_10_TIMELOW,			// SE10（時間が少なくなったとき）
	SOUND_LABEL_SE_11_PLAYERHPHEAL,		// SE11（プレイヤーのHPが回復したとき）
	SOUND_LABEL_SE_12_BOSSHPHEAL,		// SE12（ボスのHPが回復したとき）
	SOUND_LABEL_SE_13_DASH,				// SE13（プレイヤーがダッシュしたとき）
	SOUND_LABEL_SE_14_SAVEDATAMAKE,		// SE14（セーブデータを作成したとき）
	SOUND_LABEL_SE_15_TITLESTART,		// SE15（はじめてゲームを起動したとき（未使用））
	SOUND_LABEL_SE_16_BOSSVOICE,		// SE16（ボスの咆哮）

	SOUND_LABEL_MAX,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SOUND_VOLUME_MAX			(1.0f)		// 実際のボリューム最大値 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);