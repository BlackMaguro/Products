//=============================================================================
//
// �{�X�O�̉�b��ʏ��� [bossbefore.cpp]
// Author : 
//
//=============================================================================
#include "bossbefore.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(20)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO				(480)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO				(100)			// ���S�T�C�Y�i�c�j 
#define TEXTURE_WIDTH_LOGO_MANU			(200)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO_MANU		(60)			// ���S�T�C�Y�i�c�j 
#define TEXTURE_WIDTH_MANU_ARROW		(60)			// ���j���[���̃T�C�Y�i���j 
#define TEXTURE_HEIGHT_MANU_ARROW		(60)			// ���j���[���̃T�C�Y�i�c�j

#define TEXTURE_MANU_ARROW_X			(350)			// ���j���[����X���W 
#define TEXTURE_MANU_ARROW_Y			(320)			// ���j���[����Y���W�i�����l�j


#define TEXTURE_MESSAGE_WIDTH			(600)			// ���b�Z�[�W�̉��� 
#define TEXTURE_MESSAGE_HEIGHT			(150)			// ���b�Z�[�W�̏c��
#define TEXTURE_MESSAGE_PB_X			(450)			// ���b�Z�[�W�i�o�f�B�ƃv���C���[�j��X���W�̕\���l 
#define TEXTURE_MESSAGE_MAN_X			(510)			// ���b�Z�[�W�i�j�j��X���W�̕\���l 
#define TEXTURE_MESSAGE_Y1				(100)			// ���b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_MESSAGE_Y2				(270)			// ���b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_MESSAGE_Y3				(440)			// ���b�Z�[�W��Y���W�̕\���l 

#define TEXTURE_BOSS_Y					(220)			// ���b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_BOSS_MESSAGE_Y			(450)			// �{�X�̃��b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_BOSS_WIDTH				(760)			// �{�X�̉���
#define TEXTURE_BOSS_HEIGHT				(450)			// �{�X�̍���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_0.png",									// 0_���[�h�ɓ���O�̑I����ʂ�BG�i�`���[�g���A���Ɠ���BG�j
	"data/TEXTURE/BossBefore/bossBefore_logo.png",				// 1_���[�h�ɓ���O�̑I����ʂ̃��S_�^�C�g��
	"data/TEXTURE/tutorial_logo_1_yes.png",						// 2_���[�h�ɓ���O�̑I����ʂ̃��S_�͂�
	"data/TEXTURE/tutorial_logo_2_no.png",						// 3_���[�h�ɓ���O�̑I����ʂ̃��S_������
	"data/TEXTURE/title_menu_arrow.png",						// 4_���j���[�I���̖��
	"data/TEXTURE/BG/bg_boss1.png",								// 5_�{�X�O�̉�b�̔w�i
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_0.png",	// 6_�{�X�O�̉�b�̃��b�Z�[�W0
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_1.png",	// 7_�{�X�O�̉�b�̃��b�Z�[�W1
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_2.png",	// 8_�{�X�O�̉�b�̃��b�Z�[�W2
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_3.png",	// 9_�{�X�O�̉�b�̃��b�Z�[�W3
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_4.png",	// 10_�{�X�O�̉�b�̃��b�Z�[�W4
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_5.png",	// 11_�{�X�O�̉�b�̃��b�Z�[�W5
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_6.png",	// 12_�{�X�O�̉�b�̃��b�Z�[�W6
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_7.png",	// 13_�{�X�O�̉�b�̃��b�Z�[�W7
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_8.png",	// 14_�{�X�O�̉�b�̃��b�Z�[�W8
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_9.png",	// 15_�{�X�O�̉�b�̃��b�Z�[�W9
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_10.png",	// 16_�{�X�O�̉�b�̃��b�Z�[�W10
	"data/TEXTURE/BossBefore/bossBefore_charaMessage_11.png",	// 17_�{�X�O�̉�b�̃��b�Z�[�W11�i�{�X�̙��K�j
	"data/TEXTURE/BossBefore/bossBefore_boss.png",				// 18_�{�X�O�̉�b�̃��b�Z�[�W12�i�{�X�̎p�j
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;
static BOOL						g_BossBeforeFlg = FALSE;	// �{�X�O�̉�b�����s���邩�̃t���O
static BOOL						g_BossVoiceFlg = FALSE;		// �{�X�����K�������̃t���O
int g_BossBefore_menu_select;									// ���[�h�ɓ���O�̑I�����p�ϐ�
float g_bossBeforeTimeCount;									// �{�X�O�̉�b��ʂ̃^�C���ϐ�
int g_bossBeforeMessageCount;									// �{�X�O�̉�b��ʂ̃��b�Z�[�W�𓧖��ɂ���ϐ�
float g_bossBeforeMessageTrans = 1.0f;
float g_bossBeforeBossTrans = 0.0f;
float g_bossBossVoice = 0.0f;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBossBefore(void)
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


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;
	g_BossBefore_menu_select = 0;
	g_bossBeforeTimeCount = 0;
	g_bossBeforeMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBossBefore(void)
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
void UpdateBossBefore(void)
{
	g_bossBeforeTimeCount += 1.0f;
	g_bossBossVoice += 0.01f;

	if (g_bossBossVoice > 1.0f)
	{
		g_bossBossVoice = 1.0f;
	}

	// ���X�Ɍ����Ă���
	if (g_bossBeforeMessageCount >= 11)
	{
		g_bossBeforeBossTrans += 0.01f;
	}

	// ���X�ɓ����ɂȂ�
	if (GetBossBeforeFlg() == TRUE)
	{
		g_bossBeforeMessageTrans -= 0.005f;
		if (g_bossBeforeMessageTrans < 0.0f)
		{
			g_bossBeforeMessageTrans = 0.0f;
		}
	}

	// �{�X�O�̉�b����Enter���������Ƃ�
	if (GetBossBeforeFlg() == TRUE)
	{
		// �G���^�[�L�[�������ꂽ�Ƃ��i�{�X�O�̉�b����Enter���j
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}
	// �{�X�̙��K
	if ((!g_BossVoiceFlg) && g_bossBeforeMessageCount == 11)
	{
		PlaySound(SOUND_LABEL_SE_16_BOSSVOICE);
		g_BossVoiceFlg = TRUE;
	}

	if ((GetBossBeforeFlg() == TRUE) && GetKeyboardTrigger(DIK_RETURN))
	{
		// �{�X�O�̉�b�̃��b�Z�[�W�J�E���g���グ��
		// �J�E���g���グ�����Ȃ�
		if (g_bossBeforeMessageTrans == 0.0f)
		{
			g_bossBeforeMessageCount++;
		}
	}

	// �{�X�O�̉�b�̍Ō�܂ōs�����烂�[�h���{�X��
	if (GetKeyboardTrigger(DIK_RETURN) && g_bossBeforeMessageCount >= 12)
	{		
		// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_BOSS);
	}

	// �{�X�O�̉�b�����{���邩�̏����i�t���O��TRUE�ɂȂ����������j
	if (GetBossBeforeFlg() == FALSE)
	{
		// �T�E���h�̏���
		{
			// �G���^�[�L�[�������ꂽ�Ƃ��i�{�X�O�̉�b�̑O��Enter���j
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
			if (GetKeyboardTrigger(DIK_RETURN) && g_BossBefore_menu_select == 0)
			{
				// �{�X�O�̉�b�t���O���I���ɂ���
				SetBossBeforeFlg(TRUE);
				// �{�X�O�̉�b�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_BossBefore_menu_select == 0)
			{
				// �{�X�O�̉�b�t���O���I���ɂ���
				SetBossBeforeFlg(TRUE);
				// �{�X�O�̉�b�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_BossBefore_menu_select == 0)
			{
				// �{�X�O�̉�b�t���O���I���ɂ���
				SetBossBeforeFlg(TRUE);
				// �{�X�O�̉�b�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_05_BOSSBEFORE);

			}
		}

		// �I�������u�������v�̂Ƃ�
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_BossBefore_menu_select == 1)
			{
				// Enter��������A�X�e�[�W��؂�ւ���
				SetFade(FADE_OUT, MODE_BOSS);
			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_BossBefore_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_BOSS);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_BossBefore_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_BOSS);
			}
		}

		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_BossBefore_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_BossBefore_menu_select -= select;
		}

		g_BossBefore_menu_select = g_BossBefore_menu_select % 2;

		if (g_BossBefore_menu_select < 0)
		{
			g_BossBefore_menu_select = 1;
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("g_bossBeforeMessageCount:%d,\n", g_bossBeforeMessageCount);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBossBefore(void)
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
	float sinValue = (sin(g_bossBeforeTimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������

	// �`���[�g���A�������邩�̊m�F���
	if (g_BossBeforeFlg == FALSE)
	{
		// �{�X�O�̉�b�O�̑I������ʂ�BG�\��
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
		float pw = 800.0f;		// ���S�̉���
		float ph = 130.0f;		// ���S�̏c��

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �{�X�O�̉�b�O�̑I������ʂ̃^�C�g��
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
		//  �{�X�O�̉�b�O�̑I������ʂ̃��j���[�u�͂��v
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_BossBefore_menu_select == 0)
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


		//  �{�X�O�̉�b�O�̑I������ʂ̃��j���[�u�������v
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_BossBefore_menu_select == 1)
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

		pmy += g_BossBefore_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// ���iarrow�j����]������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	// �{�X�O�̉�b�̏�ʊJ�n
	else if(g_BossBeforeFlg != FALSE)
	{
		//==========================================
		// �{�X�O�̉�b�̏�ʂ̏���
		//==========================================
		// �{�X�O�̉�b�̔w�i��`��i�����ƕ\�����Ă���j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// �{�X�O�̉�b�̍����w�i��`��i�ŏ������\���B���ԂƂ��ɏ�����j
		if (g_bossBeforeMessageCount == 0)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_bossBeforeMessageTrans));
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		//==========================================
		// ���b�Z�[�W����
		//==========================================
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float px = SCREEN_WIDTH / 2;
			float py = TEXTURE_MESSAGE_Y1;
			float pw = TEXTURE_MESSAGE_WIDTH;		// ���b�Z�[�W�̉���
			float ph = TEXTURE_MESSAGE_HEIGHT;		// ���b�Z�[�W�̏c��

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �{�X�O�̉�b���b�Z�[�W0�i�o�f�B�u�Ƃ����Ⴍ�v�j
			if (g_bossBeforeMessageCount >= 0 && g_bossBeforeMessageCount <= 1 && g_bossBeforeMessageTrans == 0.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W1�i�v���C���[�u....�v�j
			if (g_bossBeforeMessageCount == 1)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W2�i�j�u�����[��v�j
			if (g_bossBeforeMessageCount >= 2 && g_bossBeforeMessageCount <= 4)
			{
				px = TEXTURE_MESSAGE_MAN_X;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W3�i�v���C���[�u�H�H�H�v�j
			if (g_bossBeforeMessageCount >= 3 && g_bossBeforeMessageCount <= 4)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W3�i�j�u�N�X���ł��񂩂�����H�v�j
			if (g_bossBeforeMessageCount == 4)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y3;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W4�i�o�f�B�u���Ƃ���������v�j
			if (g_bossBeforeMessageCount >= 5 && g_bossBeforeMessageCount <= 7)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W5�i�j�u���ł�...���ˁv�j
			if (g_bossBeforeMessageCount >= 6 && g_bossBeforeMessageCount <= 7)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// TODO�F���炷�iUpdate�ł��ׂ��H�j

			}
			// �{�X�O�̉�b�L�������b�Z�[�W7�i�o�f�B�u���H�v�j
			if (g_bossBeforeMessageCount == 7)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y3;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W8�i�v���C���[�u...!!!�v�j
			if (g_bossBeforeMessageCount >= 8 && g_bossBeforeMessageCount <= 9)
			{
				px = TEXTURE_MESSAGE_PB_X;
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// TODO�F���炷�iUpdate�ł��ׂ��H�j

			}
			// �{�X�O�̉�b�L�������b�Z�[�W9�i�j�u�`�B�I�I����ς�...�D�ɂ��Ă��!�v�j
			if (g_bossBeforeMessageCount == 9)
			{
				px = TEXTURE_MESSAGE_MAN_X;
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�L�������b�Z�[�W10�i�j�u�����I�ō�����I�v�j
			if (g_bossBeforeMessageCount == 10)
			{
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�̃{�X�i�����g�ŗh�炷��j
			if (g_bossBeforeMessageCount >= 11)
			{
				px = SCREEN_WIDTH * 0.5;
				py = TEXTURE_BOSS_Y;
				pw = TEXTURE_BOSS_WIDTH;
				ph = TEXTURE_BOSS_HEIGHT;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_bossBeforeBossTrans));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �{�X�O�̉�b�{�X���b�Z�[�W11�i�{�X�u�O�H�I�I�I�E�E�E___�E�E�E�i�������̓��[���X�M���ɂ��Ă���j�v�j
			if (g_bossBeforeMessageCount >= 11)
			{
				py = TEXTURE_BOSS_MESSAGE_Y;
				pw = TEXTURE_MESSAGE_WIDTH;			// ���b�Z�[�W�̉���
				ph = TEXTURE_MESSAGE_HEIGHT;		// ���b�Z�[�W�̏c��

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}

//=============================================================================
// �{�X�O�̉�b���K�v���ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetBossBeforeFlg(void)
{
	return g_BossBeforeFlg;
}

void SetBossBeforeFlg(BOOL bossBeforeFlg)
{
	g_BossBeforeFlg = bossBeforeFlg;
}





