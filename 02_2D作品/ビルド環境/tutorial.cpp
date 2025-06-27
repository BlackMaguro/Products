//=============================================================================
//
// �`���[�g���A����ʏ��� [tutorial.cpp]
// Author : ����z��
//
//=============================================================================
#include "tutorial.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(22)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO				(480)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO				(100)			// ���S�T�C�Y�i�c�j 
#define TEXTURE_WIDTH_LOGO_MANU			(200)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO_MANU		(60)			// ���S�T�C�Y�i�c�j 
#define TEXTURE_WIDTH_MANU_ARROW		(60)			// ���j���[���̃T�C�Y�i���j 
#define TEXTURE_HEIGHT_MANU_ARROW		(60)			// ���j���[���̃T�C�Y�i�c�j

#define TEXTURE_MANU_ARROW_X			(350)			// ���j���[����X���W 
#define TEXTURE_MANU_ARROW_Y			(320)			// ���j���[����Y���W�i�����l�j 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_0.png",							// 0_���[�h�ɓ���O�̑I����ʂ�BG
	"data/TEXTURE/Tutorial/tutorial_logo_0.png",		// 1_���[�h�ɓ���O�̑I����ʂ̃��S_�^�C�g��
	"data/TEXTURE/tutorial_logo_1_yes.png",				// 2_���[�h�ɓ���O�̑I����ʂ̃��S_�͂�
	"data/TEXTURE/tutorial_logo_2_no.png",				// 3_���[�h�ɓ���O�̑I����ʂ̃��S_������
	"data/TEXTURE/title_menu_arrow.png",				// 4_���j���[�I���̖��
	"data/TEXTURE/bg_tutorial.png",						// 5_�`���[�g���A����ʂ�BG
	"data/TEXTURE/Tutorial/tutorial_message_1.png",		// 6_�`���[�g���A����ʂ̃��b�Z�[�W1
	"data/TEXTURE/Tutorial/tutorial_message_2.png",		// 7_�`���[�g���A����ʂ̃��b�Z�[�W2
	"data/TEXTURE/Tutorial/tutorial1_Keyboard1.png",	// 8_�`���[�g���A����ʂ̃L�[�{�[�h�\���i�J�[�\���L�[�j
	"data/TEXTURE/arrow_down.png",						// 9_�`���[�g���A����ʂ̖��\��
	"data/TEXTURE/Tutorial/tutorial_message_3.png",		// 10_�`���[�g���A����ʂ̃��b�Z�[�W3
	"data/TEXTURE/Tutorial/tutorial1_Keyboard2.png",	// 11_�`���[�g���A����ʂ̃L�[�{�[�h�\���iJ�j
	"data/TEXTURE/Tutorial/tutorial_message_4.png",		// 12_�`���[�g���A����ʂ̃��b�Z�[�W4
	"data/TEXTURE/Tutorial/tutorial_message_5.png",		// 13_�`���[�g���A����ʂ̃��b�Z�[�W5
	"data/TEXTURE/Tutorial/tutorial1_Keyboard3.png",	// 14_�`���[�g���A����ʂ̃L�[�{�[�h�\���iSPACE�j
	"data/TEXTURE/Tutorial/tutorial_message_6.png",		// 15_�`���[�g���A����ʂ̃��b�Z�[�W6
	"data/TEXTURE/Tutorial/tutorial_message_7.png",		// 16_�`���[�g���A����ʂ̃��b�Z�[�W7
	"data/TEXTURE/Tutorial/tutorial1_Keyboard4.png",	// 17_�`���[�g���A����ʂ̃L�[�{�[�h�\���iH�j
	"data/TEXTURE/Tutorial/tutorial_message_8.png",		// 18_�`���[�g���A����ʂ̃��b�Z�[�W8
	"data/TEXTURE/Tutorial/tutorial_message_9.png",		// 19_�`���[�g���A����ʂ̃��b�Z�[�W9
	"data/TEXTURE/Tutorial/tutorial_message_10.png",	// 20_�`���[�g���A����ʂ̃��b�Z�[�W10
	"data/TEXTURE/Tutorial/tutorial_message_11.png",	// 21_�`���[�g���A����ʂ̃��b�Z�[�W11
};

static BOOL						g_PointToPlayer;			// �|�C���g�ɓ��B���������m�F����t���O
static BOOL						g_Point0;					// �`���[�g���A�����s�����̂����m�F����t���O�i�����\���j
static BOOL						g_Point1;					// �`���[�g���A�����s�����̂����m�F����t���O�i�W�����v�j
static BOOL						g_Point2;					// �`���[�g���A�����s�����̂����m�F����t���O�i�G�l�~�[��|���j
static BOOL						g_Point3;					// �`���[�g���A�����s�����̂����m�F����t���O�i�񕜁j
static BOOL						g_Point4;					// �`���[�g���A�����s�����̂����m�F����t���O�i����Enter�j
static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;
static BOOL						g_TutorialFlg = FALSE;		// �`���[�g���A�������s���邩�̃t���O
int g_tutorial_menu_select;									// ���[�h�ɓ���O�̑I�����p�ϐ�
int g_tutorialMessageCount;									// �`���[�g���A����ʂ̃��b�Z�[�W�E�B���h�E��

static TUTORIAL	g_Tutorial;
float g_tutorialTimeCount;									// �`���[�g���A����ʂ̃^�C���ϐ�
static XMFLOAT3 g_PlayerInitPos;							// �`���[�g���A����ʂ̃v���C���[�̏����ʒu

// �v���C���[�̃|�C���g�ʒu
int playerPointPos;
// �`���[�g���A���|�C���g�̈ʒu
float g_tutorialPoint0;
float g_tutorialPoint1;
float g_tutorialPoint2;
float g_tutorialPoint3;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	PLAYER* player = GetPlayer();
	// �ϐ��̏�����
	g_PointToPlayer = FALSE;
	g_Point0 = TRUE;
	g_Point1 = TRUE;
	g_Point2 = TRUE;
	g_Point3 = TRUE;
	g_Point4 = TRUE;
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;
	playerPointPos = 0;
	g_PlayerInitPos = player->pos;		// �v���C���[�̏����l���擾���Ă���
	g_tutorialPoint0 = g_PlayerInitPos.x;
	g_tutorialPoint1 = 576.0f;
	g_tutorialPoint2 = 768.0f;
	g_tutorialPoint3 = 1600.0f;
	g_tutorial_menu_select = 0;
	g_tutorialTimeCount = 0;
	g_tutorialMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
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
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{
	// �N���A�t���O���I�t�ɂ���
	SetClearFlg(FALSE);

	g_tutorialTimeCount += 1.0f;

	// �`���[�g���A������Enter���������Ƃ�
	if (GetTutorialFlg() == TRUE)
	{
		// Enter�L�[�������ꂽ�Ƃ��i�`���[�g���A������Enter���j
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}

	// �v���C���[�̏ꏊ�̊m�F
	PLAYER* player = GetPlayer();
	// �G�l�~�[�̊m�F
	ENEMY* enemy = GetEnemy();
	CheckPlayerToChipNum(player->pos.y, player->pos.x);
	// �`���[�g���A���|�C���g�ɗ������ǂ����̊m�F
	CheckPlayerTutorialPoint(player->pos.x);

	// �`���[�g���A���|�C���g 0�i���b�Z�[�W1�A2�i�l��0��1�j�j
	//if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (player->pos.x == g_PlayerInitPos.x))
	if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (g_PointToPlayer == TRUE))
	//if ((g_Point0 == TRUE) && (GetTutorialFlg() == TRUE) && (g_tutorialMessageCount <= 2))
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g��������
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 2)
			{
				g_tutorialMessageCount = 2;
			}
		}

		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 2)
		{
			player->tutorialMove = TRUE;
			g_Point0 = FALSE;
		}

	}
	 //�`���[�g���A���|�C���g1�ig_tutorialMessageCount��2�j
	if ((g_Point1 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint1))
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g��������
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 3)
			{
				g_tutorialMessageCount = 3;
			}
		}
		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 3)
		{
		 	player->tutorialMove = TRUE;
			g_Point1 = FALSE;
		}
	}
	//�`���[�g���A���|�C���g2�ig_tutorialMessageCount��3�j
	if ((g_Point2 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint2))
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g���グ��
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 5)
			{
				g_tutorialMessageCount = 5;
			}
		}
		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 5)
		{
			player->tutorialMove = TRUE;
			g_Point2 = FALSE;
		}
	}
	//�`���[�g���A���|�C���g3�ig_tutorialMessageCount��5�j
	if ((g_Point3 == TRUE) && (enemy->use == FALSE))
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g��������
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 7)
			{
				g_tutorialMessageCount = 7;
			}
		}
		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 7)
		{
			g_Point3 = FALSE;
		}
	}
	// �񕜃{�^�����������Ƃ�
	if ((g_tutorialMessageCount == 7) && GetKeyboardTrigger(DIK_H))
	{
		PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
		player->hp += 100;
		player->healItem -= 1;
		if (player->hp > 100) player->hp = 100;
		player->tutorialMove = TRUE;
		playerPointPos = 3;
	}
	// �񕜃{�^������������Amessage8���o�Ă���Ƃ�
	if ((g_tutorialMessageCount == 7) && (playerPointPos == 3))
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g���グ��
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 8)
			{
				g_tutorialMessageCount = 8;
			}
		}
		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 8)
		{
			player->tutorialMove = TRUE;
		}
	}
	//�`���[�g���A���|�C���g4�B�ϐ���3�Ȃ̂Œ��Ӂig_tutorialMessageCount��8�j
	if ((g_Point4 == TRUE) && (g_PointToPlayer == TRUE) && (player->pos.x == g_tutorialPoint3) && enemy->use == FALSE)
	{
		// �v���C���[�̓������~�߂�
		player->tutorialMove = FALSE;

		// Enter�L�[�������ꂽ��J�E���g��������
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_tutorialMessageCount += 1;
			if (g_tutorialMessageCount > 11)
			{
				g_tutorialMessageCount = 11;
			}
		}
		// �`���[�g���A���|�C���g�̐������I�������
		if (g_tutorialMessageCount == 11)
		{
			player->tutorialMove = TRUE;
			g_Point4 = FALSE;
		}
	}


	// �`���[�g���A�������{���邩�̏����i�t���O��TRUE�ɂȂ����������j
	if(GetTutorialFlg() == FALSE)
	{
		// �T�E���h�̏���
		{
			// Enter�L�[�������ꂽ�Ƃ��i�`���[�g���A���O��Enter���j
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				PlaySound(SOUND_LABEL_SE_01_ENTER);
			}
			// �J�[�\���̏㉺�L�[���������Ƃ�
			if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_UP))
			{
				PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			}
		}
		// �I�������u�͂��v�̂Ƃ�
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_tutorial_menu_select == 0)
			{
				// �`���[�g���A���t���O���I���ɂ���
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_tutorial_menu_select == 0)
			{
				// �`���[�g���A���t���O���I���ɂ���
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_tutorial_menu_select == 0)
			{
				// �`���[�g���A���t���O���I���ɂ���
				SetTutorialFlg(TRUE);
				PlaySound(SOUND_LABEL_BGM_03_TUTORIAL);
			}
		}

		// �I�������u�������v�̂Ƃ�
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_tutorial_menu_select == 1)
			{
				// Enter��������A�X�e�[�W��؂�ւ���
				SetFade(FADE_OUT, MODE_GAME);
			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_tutorial_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_tutorial_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
		}

		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_tutorial_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_tutorial_menu_select -= select;
		}

		g_tutorial_menu_select = g_tutorial_menu_select % 2;

		if (g_tutorial_menu_select < 0)
		{
			g_tutorial_menu_select = 1;
		}
	}




#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("g_tutorialMessageCount:%d,\nplayerPointPos:%d,\n", g_tutorialMessageCount, playerPointPos);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// sin�l�̎擾
	float sinValue = (sin(g_tutorialTimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������

	PLAYER* player = GetPlayer();
	ENEMY* enemy = GetEnemy();
	BG* bg = GetBG();

	// �`���[�g���A�������邩�̊m�F���
	if (g_TutorialFlg == FALSE)
	{
		// �`���[�g���A���O�̑I������ʂ�BG�\��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float px = SCREEN_WIDTH / 2;
		float py = 100.0f;
		float pw = 700.0f;		// ���S�̉���
		float ph = 130.0f;		// ���S�̏c��

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �`���[�g���A���O�̑I������ʂ̃^�C�g��
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}


		// �I������Ă����ꍇ�A������Ԃ�����
		float selectColor = 1.0f;
		// �����x
		float menuTransValue = 1.0f;
		//  �`���[�g���A���O�̑I������ʂ̃��j���[�u�͂��v
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_tutorial_menu_select == 0)
			{
				selectColor = 0.0f;
				menuTransValue = sinValue + 0.3f;
			}
			else
			{
				selectColor = 1.0f;
				menuTransValue = 1.0f;
			}

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, selectColor, selectColor, menuTransValue));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}


		//  �`���[�g���A���O�̑I������ʂ̃��j���[�u�������v
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_tutorial_menu_select == 1)
			{
				selectColor = 0.0f;
				menuTransValue = sinValue + 0.3f;
			}
			else
			{
				selectColor = 1.0f;
				menuTransValue = 1.0f;
			}

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, selectColor, selectColor, menuTransValue));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// ���j���[�̖��iarrow�j�\���i�ԁj
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		// ����Y���W
		float pmx = TEXTURE_MANU_ARROW_X;
		float pmy = TEXTURE_MANU_ARROW_Y;
		float pmw = TEXTURE_WIDTH_MANU_ARROW;
		float pmh = TEXTURE_HEIGHT_MANU_ARROW;

		pmy += g_tutorial_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// ���iarrow�j����]������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}
	// �`���[�g���A���̊J�n
	else if(g_TutorialFlg != FALSE)
	{
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float px = SCREEN_WIDTH / 2;
		float py = 100.0f;
		float pw = 600.0f;		// ���S�̉���
		float ph = 150.0f;		// ���S�̏c��

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		//==========================================
		// ���b�Z�[�W�����i�����J�n�j
		//==========================================
		// �`���[�g���A�����b�Z�[�W1
		{
			// �`���[�g���A���|�C���g0���b�Z�[�W1
			if (g_tutorialMessageCount == 0)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g0���b�Z�[�W2
			if (g_tutorialMessageCount == 1)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g1���b�Z�[�W
			if (g_tutorialMessageCount == 2 && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g2���b�Z�[�W
			if (g_tutorialMessageCount == 3 && (playerPointPos == 2))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}
			// �`���[�g���A���|�C���g2���b�Z�[�W
			if (g_tutorialMessageCount == 4 && (playerPointPos == 2))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g3���b�Z�[�W1
			if (g_tutorialMessageCount == 5 && (enemy->use == FALSE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g3���b�Z�[�W2
			if (g_tutorialMessageCount == 6 && (enemy->use == FALSE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g4���b�Z�[�W1�imessage_8�j
			if (g_tutorialMessageCount == 7 && (playerPointPos == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g5���b�Z�[�W1�imessage_9�j
			if (g_tutorialMessageCount == 8 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[19]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g5���b�Z�[�W2�imessage_10�j
			if (g_tutorialMessageCount == 9 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �`���[�g���A���|�C���g5���b�Z�[�W3�imessage_11�j
			if (g_tutorialMessageCount == 10 && (playerPointPos == 4))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[21]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}

		//==========================================
		// �L�[�{�[�h�̕\��
		//==========================================
		{
			// �ړ��ƃ_�b�V���̕\���i�`���[�g���A��0�j
			if ((g_tutorialMessageCount == 2) && (g_Point0 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= 64.0f))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				//float px = 0 - bg->pos.x + g_PlayerInitPos.x;
				float px = 0 - bg->pos.x + 200.0f;
				float py = 0 - bg->pos.y + 230.0f;
				float pw = 150.0f;		// �摜�̉���
				float ph = 200.0f;		// �摜�̏c��

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �W�����v�iJ�j�̕\���i�`���[�g���A��1�j
			//if ((g_tutorialMessageCount == 2) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= g_tutorialPoint2))
			if ((g_tutorialMessageCount == 3) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				float px = 0 - bg->pos.x + 490.0f;
				float py = 0 - bg->pos.y + 340.0f;
				float pw = 100.0f;		// �摜�̉���
				float ph = 100.0f;		// �摜�̏c��

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// ���������iSPACE�j�̕\���i�`���[�g���A��2�j
			if ((g_tutorialMessageCount == 5) && (g_Point2 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 2) && (enemy->use == TRUE))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

				float px = 0 - bg->pos.x + 1000.0f;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = 100.0f;		// �摜�̉���
				float ph = 100.0f;		// �摜�̏c��

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �����ӂ��iH�j�̕\���i�`���[�g���A��3�j
			if ((g_tutorialMessageCount == 7) && (g_Point3 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 2) && (enemy->use == FALSE) && (player->healItem == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

				float px = 0 - bg->pos.x + 1100.0f;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = 100.0f;		// �摜�̉���
				float ph = 100.0f;		// �摜�̏c��

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue + 0.2f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
		//==========================================
		// ���̕\��
		//==========================================
		{
			// �|�C���g1�̂Ƃ���̕\��
			if ((g_tutorialMessageCount == 2) && (g_Point0 == FALSE) && (g_PointToPlayer == FALSE) && (player->pos.x >= 64.0f))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint1;
				float py = 0 - bg->pos.y + 448.0f;
				float pw = MAPCHIP_WIDTH;		// �摜�̉���
				float ph = MAPCHIP_HEIGHT;		// �摜�̏c��

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �|�C���g2�̂Ƃ���̕\��
			if ((g_tutorialMessageCount == 3) && (g_Point1 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 1))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint2;
				float py = 0 - bg->pos.y + 320.0f;
				float pw = MAPCHIP_WIDTH;		// �摜�̉���
				float ph = MAPCHIP_HEIGHT;		// �摜�̏c��

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �|�C���g3�̂Ƃ���̕\��
			if ((g_tutorialMessageCount == 8) && (g_Point3 == FALSE) && (g_PointToPlayer == FALSE) && (playerPointPos == 3))
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				float px = 0 - bg->pos.x + g_tutorialPoint3;
				float py = 0 - bg->pos.y + 448.0f;
				float pw = MAPCHIP_WIDTH;		// �摜�̉���
				float ph = MAPCHIP_HEIGHT;		// �摜�̏c��

				py += 10 * sinValue;

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}

//=============================================================================
// Tutorial�\���̂̐擪�A�h���X���擾
//=============================================================================
TUTORIAL* GetTutorial(void)
{
	return &g_Tutorial;
}

//=============================================================================
// �`���[�g���A���|�C���g�ɗ������ǂ����̔��f������
//=============================================================================
void CheckPlayerTutorialPoint(float playerPosX)
{
	// �`�F�b�N�|�C���g0�i�����ʒu�j
	if (playerPosX == g_tutorialPoint0 && (g_Point0 == TRUE))
	{
		playerPointPos = 0;
		SetPLayerToPoint(TRUE);
	}
	else if(playerPosX == g_tutorialPoint1 && (g_Point1 == TRUE))
	{
		playerPointPos = 1;
		SetPLayerToPoint(TRUE);
	}
	else if (playerPosX == g_tutorialPoint2 && (g_Point2 == TRUE))
	{
		playerPointPos = 2;
		SetPLayerToPoint(TRUE);
	}
	else if (playerPosX == g_tutorialPoint3 && (g_Point4 == TRUE) && g_tutorialMessageCount >= 8)
	{
		playerPointPos = 4;		// 3�͑O�̒i�K�i�񕜁j�Ŏg�p��
		SetPLayerToPoint(TRUE);
	}
	else
	{
		SetPLayerToPoint(FALSE);
	}
}

//=============================================================================
// �`���[�g���A�����K�v���ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetTutorialFlg(void)
{
	return g_TutorialFlg;
}

void SetTutorialFlg(BOOL tutorialFlg)
{
	g_TutorialFlg = tutorialFlg;
}

//=============================================================================
// �v���C���[���`���[�g���A���|�C���g�ɂ��邩�ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetPLayerToPoint(void)
{
	return g_PointToPlayer;
}

void SetPLayerToPoint(BOOL PointToPlayer)
{
	g_PointToPlayer = PointToPlayer;
}

//=============================================================================
// �`���[�g���A�����̃v���C���[�̓����𐧌�����i�A�N�Z�T�֐��j
//=============================================================================
int GetTutorialMessageCount(void)
{
	return g_tutorialMessageCount;
}




