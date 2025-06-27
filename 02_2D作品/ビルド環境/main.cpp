//=============================================================================
//
// ���C������ [main.cpp]
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
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"AshAwakening"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;
BOOL g_bossFlg = FALSE;		// �{�X�킩���f����t���O
BOOL g_clearFlg = FALSE;	// �N���A���������f����t���O
BOOL g_creditFlg = TRUE;	// ���߂ĊJ�������̃t���O

#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;				// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_PROLOGUE;				// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_TUTORIAL;				// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_GAME;					// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_BOSSBEFORE;			// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_BOSS;					// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_EPILOGUE;				// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_RESULT;				// �N�����̉�ʂ�ݒ�
//int	g_Mode = MODE_GAMEOVER;				// �N�����̉�ʂ�ݒ�

BOOL g_LoadGame = FALSE;					// NewGame


//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// �E�B���h�E���[�h���t���X�N���[�����[�h���̏���
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
		mode = TRUE;
		break;
	case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = FALSE;	// ���ɂ���ē����Ȃ���������
		break;
	case IDCANCEL:	// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif
				// ���߂ăQ�[�����N�������ꍇ
				//SetCreditFlg(TRUE);
				// �Q�[���I������
				if (g_Mode == MODE_END) break;
				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
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
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �`��̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J�����̏�����
	InitCamera();

	// ���C�g��L����
	SetLightEnable(FALSE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �ݒ���̃��[�h
	LoadSettings();

	// �T�E���h�����̏�����
	InitSound(hWnd);

	// �t�F�[�h�����̏�����
	InitFade();


	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�̏I������
	UninitSound();

	// ���͂̏I������
	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����X�V
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_PROLOGUE:		// �v�����[�O��ʂ̍X�V
		UpdatePrologue();
		break;

	case MODE_TUTORIAL:		// �`���[�g���A����ʂ̍X�V
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateEnemy();
		UpdateBullet();
		UpdateTutorial();
		break;

	case MODE_SETTINGS:		// �ݒ��ʂ̍X�V
		UpdateSettings();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateEnemy();
		UpdateBullet();
		UpdateScore();
		UpdateTime();

		if (GetFade() == FADE_NONE)
		{	// �S�Ń`�F�b�N
			int ans = CheckGameClear();
			if (ans == 2)	// �v���C���[�����S
			{
				SetFade(FADE_OUT, MODE_GAMEOVER);
			}
		}
		break;


	case MODE_BOSSBEFORE:	// �{�X�O�̉�b��ʂ̍X�V
		UpdateBossBefore();
		break;

	case MODE_BOSS:			// �{�X���ʂ̍X�V
		UpdateBG();
		UpdateField();
		UpdatePlayer();
		UpdateBullet();
		UpdateBoss();
		UpdateBulletEnemy();

		if (GetFade() == FADE_NONE)
		{	// �S�Ń`�F�b�N
			int ans = CheckGameClear();
			if (ans == 2)	// �v���C���[�����S
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

	case MODE_GAMEOVER:		// �Q�[���I�[�o�[��ʂ̍X�V
		UpdateGameOver();
		break;

	case MODE_EPILOGUE:		// �G�s���[�O��ʂ̍X�V
		UpdateEpilogue();
		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;
	}

	UpdateFade();			// �t�F�[�h�̍X�V����
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// 2D�̕���`�悷�鏈��
	SetViewPort(TYPE_FULL_SCREEN);

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);


	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		DrawTitle();
		break;

	case MODE_TUTORIAL:		// �`���[�g���A����ʂ̕`��
		DrawBG();
		DrawField();
		DrawEnemy();
		DrawBullet();
		DrawPlayer();
		DrawTutorial();
		break;

	case MODE_PROLOGUE:		// �v�����[�O��ʂ̕`��
		DrawPrologue();
		break;

	case MODE_SETTINGS:		// �ݒ��ʂ̕`��
		DrawSettings();
		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��
		DrawBG();
		DrawField();
		DrawBullet();
		DrawEnemy();
		DrawPlayer();
		DrawScore();
		DrawTime();
		break;

	case MODE_BOSSBEFORE:			// �{�X�O�̉�b��ʂ̕`��
		DrawBossBefore();
		break;

	case MODE_BOSS:			// �{�X��ʂ̕`��
		DrawBG();
		DrawBoss();
		DrawField();
		DrawBullet();
		DrawBulletEnemy();
		DrawPlayer();
		break;

	case MODE_GAMEOVER:		// �Q�[���I�[�o�[��ʂ̕`��
		DrawGameOver();
		break;

	case MODE_EPILOGUE:		// �G�s���[�O��ʂ̕`��
		DrawEpilogue();
		break;

	case MODE_RESULT:		// ���U���g��ʂ̕`��
		DrawResult();
		break;
	}


	DrawFade();				// �t�F�[�h��ʂ̕`��


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
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
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS�������������
	StopSound();

	// ���[�h��ς���O�ɑS�������������

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// BG�̏I������
	UninitBG();

	// �t�B�[���h�̏I������
	UninitField();

	// �v�����[�O�̏I������
	UninitPrologue();

	// �`���[�g���A���̏I������
	UninitTutorial();

	// �v���C���[�̏I������
	UninitPlayer();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �{�X�O�̉�b�I������
	UninitBossBefore();

	// �{�X�̏I������
	UninitBoss();

	// �o���b�g�̏I������
	UninitBullet();

	// �G�l�~�[�o���b�g�̏I������
	UninitBulletEnemy();

	// �X�R�A�̏I������
	UninitScore();

	// �^�C���̏I������
	UninitTime();

	// �G�s���[�O�̏I������
	UninitEpilogue();

	// ���U���g�̏I������
	UninitResult();

	// �G�t�F�N�g�̏I������
	UninitEffect();

	// �ݒ�̏I������
	UninitSettings();

	// �Q�[���I�[�o�[�̏I������
	UninitGameOver();


	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	// �`���[�g���A���łȂ���΃t���O���I�t�ɂ���
	if (g_Mode != MODE_TUTORIAL) SetTutorialFlg(FALSE);
	// �{�X��ł���΃t���O��TRUE�ɁB�����łȂ����FALSE�ɁB
	if (g_Mode == MODE_BOSS) g_bossFlg = TRUE;

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_00_TITLE);
		break;

	case MODE_PROLOGUE:
		// �v�����[�O��ʂ̏�����
		InitPrologue();
		break;

	case MODE_TUTORIAL:
		// �`���[�g���A����ʂ̏�����
		InitBG();
		InitField();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitTutorial();
		break;

	case MODE_SETTINGS:
		// �ݒ��ʂ̏�����
		InitSettings();
		PlaySound(SOUND_LABEL_BGM_01_SETTINGS);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitBG();
		InitField();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitScore();
		InitTime();

		// ���[�h�Q�[���������炷�ׂĂ̏��������I�������ɃZ�[�u�f�[�^��ǂݍ���
		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ���[�h��������t���O��Clear����
		}
		PlaySound(SOUND_LABEL_BGM_04_GAMEMODE);
		break;

	case MODE_BOSSBEFORE:
		// �{�X�O�̉�b��ʂ̏�����
		InitBossBefore();
		break;

	case MODE_BOSS:
		// �Q�[����ʂ̏�����
		InitBG();
		InitField();
		InitPlayer();
		InitBullet();
		InitBoss();
		InitBulletEnemy();

		PlaySound(SOUND_LABEL_BGM_06_BOSS);
		break;


	case MODE_GAMEOVER:
		// �Q�[���I�[�o�[��ʂ̏�����
		InitGameOver();
		PlaySound(SOUND_LABEL_BGM_07_GAMEOVER);
		break;

	case MODE_EPILOGUE:		// �G�s���[�O��ʂ̏�����
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
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// �S�Ń`�F�b�N
//=============================================================================
int CheckGameClear(void)
{

	// �G�l�~�[�̐����O�Ȃ�v���C���[�̏����Ƃ���
	int cnt = GetEnemyCount();
	if (cnt <= 0)
	{
		return 1;	// �v���C���[�̏���
	}

	// �v���C���[�̐����O�Ȃ�G�l�~�[�̏����Ƃ���
	cnt = GetPlayerCount();
	if (cnt <= 0)
	{
		return 2;	// �G�l�~�[�̏���
	}

	return 0;		// �Q�[���p��
}


//=============================================================================
// �j���[�Q�[�������[�h�Q�[�������Z�b�g����
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}


//=============================================================================
// �{�X�킩�ǂ����̔��f������i�A�N�Z�T�֐��j
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
// �ŏ��ɊJ�������ǂ����̔��f������i�A�N�Z�T�֐��j
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
// �N���A���Ă��邩�ǂ����̔��f������i�A�N�Z�T�֐��j
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
// �����_���Ȓl���擾����
//=============================================================================
// �����_���擾
int GetRand(int min, int max)
{
	static int flagRand = 0;
	static std::mt19937 g_mt;

	if (flagRand == 0)
	{
		// �����_����������
		std::random_device rnd;	// �񌈒�I�ȗ���������
		g_mt.seed(rnd());		// �����Z���k�E�c�C�X�^�Ł@�����͏���SEED
		flagRand = 1;
	}

	std::uniform_int_distribution<> random(min, max);	// ���������_����0�`100�͈̔�
	int answer = random(g_mt);
	return answer;
}

