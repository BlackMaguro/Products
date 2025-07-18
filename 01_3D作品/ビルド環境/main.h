//=============================================================================
//
// メイン処理 [main.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <random>

#include <DirectXMath.h>


// 本来はヘッダに書かない
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)


//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(960)			// ウインドウの幅
#define SCREEN_HEIGHT	(540)			// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH  * 0.5)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT  * 0.5)	// ウインドウの中心Ｙ座標

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H * 0.5)
#define	MAP_DOWN		(-MAP_H * 0.5)
#define	MAP_LEFT		(-MAP_W * 0.5)
#define	MAP_RIGHT		(MAP_W * 0.5)
#define	FRAME_RATE		(60.0f)		// フレーム数

// 補間用のデータ構造体を定義
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// 頂点座標
	XMFLOAT3	rot;		// 回転
	XMFLOAT3	scl;		// 拡大縮小
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
};

enum
{
	MODE_TITLE = 0,			// タイトル画面
	MODE_TUTORIAL,			// ゲーム説明画面
	MODE_GAME,				// ゲーム画面
	MODE_RESULT,			// リザルト画面
	MODE_GAMEOVER,			// ゲームーオーバー画面
	MODE_SETTINGS,			// せってい画面
	MODE_END,				// ゲーム終了
	MODE_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);

BOOL GetCreditFlg(void);
void SetCreditFlg(BOOL flg);
int GetRand(int min, int max);

