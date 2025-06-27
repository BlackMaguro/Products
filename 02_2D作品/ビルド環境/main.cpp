//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "debugproc.h"
#include "input.h"

#include "title.h"
#include "bg.h"
#include "field.h"
#include "prologue.h"
#include "tutorial.h"
#include "gameover.h"
#include "player.h"
#include "enemy.h"
#include "bossbefore.h"
#include "boss.h"
#include "epilogue.h"
#include "bullet.h"
#include "bulletEnemy.h"
#include "score.h"
#include "time.h"
#include "result.h"
#include "sound.h"
#include "fade.h"
#include "settings.h"

#include "file.h"

#include "effect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"AshAwakening"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;
BOOL g_bossFlg = FALSE;		// ボス戦か判断するフラグ
BOOL g_clearFlg = FALSE;	// クリアしたか判断するフラグ
BOOL g_creditFlg = TRUE;	// 初めて開いたかのフラグ

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;				// 起動時の画面を設定
//int	g_Mode = MODE_PROLOGUE;				// 起動時の画面を設定
//int	g_Mode = MODE_TUTORIAL;				// 起動時の画面を設定
//int	g_Mode = MODE_GAME;					// 起動時の画面を設定
//int	g_Mode = MODE_BOSSBEFORE;			// 起動時の画面を設定
//int	g_Mode = MODE_BOSS;					// 起動時の画面を設定
//int	g_Mode = MODE_EPILOGUE;				// 起動時の画面を設定
//int	g_Mode = MODE_RESULT;				// 起動時の画面を設定
//int	g_Mode = MODE_GAMEOVER;				// 起動時の画面を設定

BOOL g_LoadGame = FALSE;					// NewGame


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// ウィンドウモードかフルスクリーンモードかの処理
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// YesならWindowモードで起動
		mode = TRUE;
		break;
	case IDNO:		// Noならフルスクリーンモードで起動
		mode = FALSE;	// 環境によって動かない事がある
		break;
	case IDCANCEL:	// CANCELなら終了
	default:
		return -1;
		break;
	}

	// 初期化処理(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif
				// 初めてゲームを起動した場合
				//SetCreditFlg(TRUE);
				// ゲーム終了処理
				if (g_Mode == MODE_END) break;
				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// 描画の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	// カメラの初期化
	InitCamera();

	// ライトを有効化
	SetLightEnable(FALSE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// 設定情報のロード
	LoadSettings();

	// サウンド処理の初期化
	InitSound(hWnd);

	// フェード処理の初期化
	InitFade();


	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// フェードの終了処理
	UninitFade();

	// サウンドの終了処理
	UninitSound();

	// 入力の終了処理
	UninitInput();

	// カメラの終了処理
	UninitCamera();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_PROLOGUE:		// プロローグ画面の更新
		UpdatePrologue();
		break;

	case MODE_TUTORIAL:		// チュートリアル画面の更新
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateEnemy();
		UpdateBullet();
		UpdateTutorial();
		break;

	case MODE_SETTINGS:		// 設定画面の更新
		UpdateSettings();
		break;

	case MODE_GAME:			// ゲーム画面の更新
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateEnemy();
		UpdateBullet();
		UpdateScore();
		UpdateTime();

		if (GetFade() == FADE_NONE)
		{	// 全滅チェック
			int ans = CheckGameClear();
			if (ans == 2)	// プレイヤーが死亡
			{
				SetFade(FADE_OUT, MODE_GAMEOVER);
			}
		}
		break;


	case MODE_BOSSBEFORE:	// ボス前の会話画面の更新
		UpdateBossBefore();
		break;

	case MODE_BOSS:			// ボス戦画面の更新
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateBullet();
		UpdateBoss();
		UpdateBulletEnemy();

		if (GetFade() == FADE_NONE)
		{	// 全滅チェック
			int ans = CheckGameClear();
			if (ans == 2)	// プレイヤーが死亡
			{
				SetFade(FADE_OUT, MODE_GAMEOVER);
			}
			if (ans != 0)
			{
				//SetMode(MODE_RESULT);
				SetFade(FADE_OUT, MODE_EPILOGUE);
			}
		}
		break;

	case MODE_GAMEOVER:		// ゲームオーバー画面の更新
		UpdateGameOver();
		break;

	case MODE_EPILOGUE:		// エピローグ画面の更新
		UpdateEpilogue();
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	}

	UpdateFade();			// フェードの更新処理
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// 2Dの物を描画する処理
	SetViewPort(TYPE_FULL_SCREEN);

	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);


	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		DrawTitle();
		break;

	case MODE_TUTORIAL:		// チュートリアル画面の描画
		DrawBG();
		DrawField();
		DrawEnemy();
		DrawBullet();
		DrawPlayer();
		DrawTutorial();
		break;

	case MODE_PROLOGUE:		// プロローグ画面の描画
		DrawPrologue();
		break;

	case MODE_SETTINGS:		// 設定画面の描画
		DrawSettings();
		break;

	case MODE_GAME:			// ゲーム画面の描画
		DrawBG();
		DrawField();
		DrawBullet();
		DrawEnemy();
		DrawPlayer();
		DrawScore();
		DrawTime();
		break;

	case MODE_BOSSBEFORE:			// ボス前の会話画面の描画
		DrawBossBefore();
		break;

	case MODE_BOSS:			// ボス画面の描画
		DrawBG();
		DrawBoss();
		DrawField();
		DrawBullet();
		DrawBulletEnemy();
		DrawPlayer();
		break;

	case MODE_GAMEOVER:		// ゲームオーバー画面の描画
		DrawGameOver();
		break;

	case MODE_EPILOGUE:		// エピローグ画面の描画
		DrawEpilogue();
		break;

	case MODE_RESULT:		// リザルト画面の描画
		DrawResult();
		break;
	}


	DrawFade();				// フェード画面の描画


#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放
	StopSound();

	// モードを変える前に全部メモリを解放

	// タイトル画面の終了処理
	UninitTitle();

	// BGの終了処理
	UninitBG();

	// フィールドの終了処理
	UninitField();

	// プロローグの終了処理
	UninitPrologue();

	// チュートリアルの終了処理
	UninitTutorial();

	// プレイヤーの終了処理
	UninitPlayer();

	// エネミーの終了処理
	UninitEnemy();

	// ボス前の会話終了処理
	UninitBossBefore();

	// ボスの終了処理
	UninitBoss();

	// バレットの終了処理
	UninitBullet();

	// エネミーバレットの終了処理
	UninitBulletEnemy();

	// スコアの終了処理
	UninitScore();

	// タイムの終了処理
	UninitTime();

	// エピローグの終了処理
	UninitEpilogue();

	// リザルトの終了処理
	UninitResult();

	// エフェクトの終了処理
	UninitEffect();

	// 設定の終了処理
	UninitSettings();

	// ゲームオーバーの終了処理
	UninitGameOver();


	g_Mode = mode;	// 次のモードをセットしている

	// チュートリアルでなければフラグをオフにする
	if (g_Mode != MODE_TUTORIAL) SetTutorialFlg(FALSE);
	// ボス戦であればフラグをTRUEに。そうでなければFALSEに。
	if (g_Mode == MODE_BOSS) g_bossFlg = TRUE;

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_00_TITLE);
		break;

	case MODE_PROLOGUE:
		// プロローグ画面の初期化
		InitPrologue();
		break;

	case MODE_TUTORIAL:
		// チュートリアル画面の初期化
		InitBG();
		InitField();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitTutorial();
		break;

	case MODE_SETTINGS:
		// 設定画面の初期化
		InitSettings();
		PlaySound(SOUND_LABEL_BGM_01_SETTINGS);
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitBG();
		InitField();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitScore();
		InitTime();

		// ロードゲームだったらすべての初期化が終わった後にセーブデータを読み込む
		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ロードしたからフラグをClearする
		}
		PlaySound(SOUND_LABEL_BGM_04_GAMEMODE);
		break;

	case MODE_BOSSBEFORE:
		// ボス前の会話画面の初期化
		InitBossBefore();
		break;

	case MODE_BOSS:
		// ゲーム画面の初期化
		InitBG();
		InitField();
		InitPlayer();
		InitBullet();
		InitBoss();
		InitBulletEnemy();

		PlaySound(SOUND_LABEL_BGM_06_BOSS);
		break;


	case MODE_GAMEOVER:
		// ゲームオーバー画面の初期化
		InitGameOver();
		PlaySound(SOUND_LABEL_BGM_07_GAMEOVER);
		break;

	case MODE_EPILOGUE:		// エピローグ画面の初期化
		InitEpilogue();
		break;

	case MODE_RESULT:
		InitResult();
		PlaySound(SOUND_LABEL_BGM_09_RESULT);
		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// 全滅チェック
//=============================================================================
int CheckGameClear(void)
{

	// エネミーの数が０ならプレイヤーの勝ちとする
	int cnt = GetEnemyCount();
	if (cnt <= 0)
	{
		return 1;	// プレイヤーの勝ち
	}

	// プレイヤーの数が０ならエネミーの勝ちとする
	cnt = GetPlayerCount();
	if (cnt <= 0)
	{
		return 2;	// エネミーの勝ち
	}

	return 0;		// ゲーム継続
}


//=============================================================================
// ニューゲームかロードゲームかをセットする
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}


//=============================================================================
// ボス戦かどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetBossFlg(void)
{
	return g_bossFlg;
}

void SetBossFlg(BOOL bossFlg)
{
	g_bossFlg = bossFlg;
}

//=============================================================================
// 最初に開いたかどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetCreditFlg(void)
{
	return g_creditFlg;
}

void SetCreditFlg(BOOL creditFlg)
{
	g_creditFlg = creditFlg;
}

//=============================================================================
// クリアしているかどうかの判断をする（アクセサ関数）
//=============================================================================
BOOL GetClearFlg(void)
{
	return g_clearFlg;
}

void SetClearFlg(BOOL clearFlg)
{
	g_clearFlg = clearFlg;
}

//=============================================================================
// ランダムな値を取得する
//=============================================================================
// ランダム取得
int GetRand(int min, int max)
{
	static int flagRand = 0;
	static std::mt19937 g_mt;

	if (flagRand == 0)
	{
		// ランダム生成準備
		std::random_device rnd;	// 非決定的な乱数生成器
		g_mt.seed(rnd());		// メルセンヌ・ツイスタ版　引数は初期SEED
		flagRand = 1;
	}

	std::uniform_int_distribution<> random(min, max);	// 生成ランダムは0～100の範囲
	int answer = random(g_mt);
	return answer;
}

