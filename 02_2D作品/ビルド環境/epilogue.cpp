//=============================================================================
//
// �G�s���[�O��ʏ��� [epilogue.cpp]
// Author : 
//
//=============================================================================
#include "epilogue.h"
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
#define TEXTURE_MESSAGE_Y1				(100)			// ���b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_MESSAGE_Y2				(270)			// ���b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_MESSAGE_Y3				(440)			// ���b�Z�[�W��Y���W�̕\���l 

#define TEXTURE_BUDDY_Y					(270)			// �o�f�B��Y���W�̕\���l 
#define TEXTURE_BUDDY_MESSAGE_Y			(450)			// �o�f�B�̃��b�Z�[�W��Y���W�̕\���l 
#define TEXTURE_BUDDY_WIDTH				(540)			// �o�f�B�̉���
#define TEXTURE_BUDDY_HEIGHT			(540)			// �o�f�B�̍���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_0.png",								// 0_���[�h�ɓ���O�̑I����ʂ�BG�i�`���[�g���A���Ɠ���BG�j
	"data/TEXTURE/Epilogue/epilogue_logo.png",				// 1_���[�h�ɓ���O�̑I����ʂ̃��S_�^�C�g��
	"data/TEXTURE/tutorial_logo_1_yes.png",					// 2_���[�h�ɓ���O�̑I����ʂ̃��S_�͂�
	"data/TEXTURE/tutorial_logo_2_no.png",					// 3_���[�h�ɓ���O�̑I����ʂ̃��S_������
	"data/TEXTURE/title_menu_arrow.png",					// 4_���j���[�I���̖��
	"data/TEXTURE/BG/bg_boss1.png",							// 5_�G�s���[�O�̔w�i1
	"data/TEXTURE/Epilogue/epilogue_charaMessage_0.png",	// 6_�G�s���[�O�̃��b�Z�[�W0
	"data/TEXTURE/Epilogue/epilogue_charaMessage_1.png",	// 7_�G�s���[�O�̃��b�Z�[�W1
	"data/TEXTURE/Epilogue/epilogue_charaMessage_2.png",	// 8_�G�s���[�O�̃��b�Z�[�W2
	"data/TEXTURE/Epilogue/epilogue_charaMessage_3.png",	// 9_�G�s���[�O�̃��b�Z�[�W3
	"data/TEXTURE/Epilogue/epilogue_charaMessage_4.png",	// 10_�G�s���[�O�̃��b�Z�[�W4
	"data/TEXTURE/Epilogue/epilogue_charaMessage_5.png",	// 11_�G�s���[�O�̃��b�Z�[�W5
	"data/TEXTURE/Epilogue/epilogue_charaMessage_6.png",	// 12_�G�s���[�O�̃��b�Z�[�W6
	"data/TEXTURE/Epilogue/epilogue_charaMessage_7.png",	// 13_�G�s���[�O�̃��b�Z�[�W7
	"data/TEXTURE/Epilogue/buddy.png",						// 14_�G�s���[�O�̃o�f�B�̎p
	"data/TEXTURE/Epilogue/epilogue_charaMessage_8.png",	// 15_�G�s���[�O�̃��b�Z�[�W8
	"data/TEXTURE/Epilogue/epilogue_bg.png",				// 16_�G�s���[�O�̍Ō��BG
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;
static BOOL						g_EpilogueFlg = FALSE;	// �G�s���[�O�����s���邩�̃t���O
int g_Epilogue_menu_select;									// ���[�h�ɓ���O�̑I�����p�ϐ�
float g_EpilogueTimeCount;									// �G�s���[�O��ʂ̃^�C���ϐ�
int g_EpilogueMessageCount;									// �G�s���[�O��ʂ̃��b�Z�[�W�𓧖��ɂ���ϐ�
float g_EpilogueMessageTrans = 1.0f;
float g_EpilogueBuddyTrans = 0.0f;
float g_EpilogueBGTrans = 0.0f;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEpilogue(void)
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
	g_Epilogue_menu_select = 0;
	g_EpilogueTimeCount = 0;
	g_EpilogueMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEpilogue(void)
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
void UpdateEpilogue(void)
{
	g_EpilogueTimeCount += 1.0f;

	// ���X�Ɏp��������悤�ɂ���i�Ō��BG�p�j
	if (g_EpilogueMessageCount >= 10)
	{
		g_EpilogueBGTrans += 0.01f;
		if (g_EpilogueBGTrans > 1.0f) g_EpilogueBGTrans = 1.0f;
	}
	// ���X�Ɏp��������悤�ɂ���i�o�f�B�p�j
	if (g_EpilogueMessageCount >= 8)
	{
		g_EpilogueBuddyTrans += 0.01f;
		if (g_EpilogueBuddyTrans > 1.0f) g_EpilogueBuddyTrans = 1.0f;
	}

	// ���X�ɓ����ɂȂ�
	if (GetEpilogueFlg() == TRUE)
	{
		g_EpilogueMessageTrans -= 0.005f;
		if (g_EpilogueMessageTrans < 0.0f)
		{
			g_EpilogueMessageTrans = 0.0f;
		}
	}

	// �G�s���[�O����Enter���������Ƃ�
	if (GetEpilogueFlg() == TRUE)
	{
		// �G���^�[�L�[�������ꂽ�Ƃ��i�G�s���[�O����Enter���j
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}

	// �G�s���[�O�̃��b�Z�[�W�J�E���g���グ��
	if ((GetEpilogueFlg() == TRUE) && GetKeyboardTrigger(DIK_RETURN))
	{
		// �J�E���g���グ�����Ȃ�
		if (g_EpilogueMessageTrans == 0.0f)
		{
			g_EpilogueMessageCount++;
		}
	}

	// �G�s���[�O�̍Ō�܂ōs�����烂�[�h�����U���g��
	if (GetKeyboardTrigger(DIK_RETURN) && g_EpilogueMessageCount >= 11)
	{		
		// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_RESULT);
	}

	// �G�s���[�O�����{���邩�̏����i�t���O��TRUE�ɂȂ����������j
	if (GetEpilogueFlg() == FALSE)
	{
		// �T�E���h�̏���
		{
			// �G���^�[�L�[�������ꂽ�Ƃ��i�G�s���[�O�O��Enter���j
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
			if (GetKeyboardTrigger(DIK_RETURN) && g_Epilogue_menu_select == 0)
			{
				// �G�s���[�O�t���O���I���ɂ���
				SetEpilogueFlg(TRUE);
				// �G�s���[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_08_EPPILOGUE);

			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_Epilogue_menu_select == 0)
			{
				// �G�s���[�O�t���O���I���ɂ���
				SetEpilogueFlg(TRUE);
				// �G�s���[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_08_EPPILOGUE);

			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_Epilogue_menu_select == 0)
			{
				// �G�s���[�O�t���O���I���ɂ���
				SetEpilogueFlg(TRUE);
				// �G�s���[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_08_EPPILOGUE);

			}
		}

		// �I�������u�������v�̂Ƃ�
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_Epilogue_menu_select == 1)
			{
				// Enter��������A�X�e�[�W��؂�ւ���
				SetFade(FADE_OUT, MODE_RESULT);
			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_Epilogue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_RESULT);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_Epilogue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}

		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_Epilogue_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_Epilogue_menu_select -= select;
		}

		g_Epilogue_menu_select = g_Epilogue_menu_select % 2;

		if (g_Epilogue_menu_select < 0)
		{
			g_Epilogue_menu_select = 1;
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("g_EpilogueMessageCount:%d,\n", g_EpilogueMessageCount);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEpilogue(void)
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
	float sinValue = (sin(g_EpilogueTimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������

	// �`���[�g���A�������邩�̊m�F���
	if (g_EpilogueFlg == FALSE)
	{
		// �G�s���[�O�O�̑I������ʂ�BG�\��
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

		// �G�s���[�O�O�̑I������ʂ̃^�C�g��
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
		//  �G�s���[�O�O�̑I������ʂ̃��j���[�u�͂��v
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_Epilogue_menu_select == 0)
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


		//  �G�s���[�O�O�̑I������ʂ̃��j���[�u�������v
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_Epilogue_menu_select == 1)
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

		pmy += g_Epilogue_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// ���iarrow�j����]������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	// �G�s���[�O�̊J�n
	else if(g_EpilogueFlg != FALSE)
	{
		//==========================================
		// �G�s���[�O�̏���
		//==========================================
		// �G�s���[�O�̔w�i��`��i�����ƕ\�����Ă���j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// �G�s���[�O�̍����w�i��`��i�ŏ������\���B���ԂƂ��ɏ�����j
		if (g_EpilogueMessageCount == 0)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_EpilogueMessageTrans));
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

			// �G�s���[�O���b�Z�[�W0�i�o�f�B�u�{���ɏ����������,,,,�v�j
			if (g_EpilogueMessageCount >= 0 && g_EpilogueMessageCount <= 1 && g_EpilogueMessageTrans == 0.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�L�������b�Z�[�W1�i�v���C���[�u....�v�j
			if (g_EpilogueMessageCount == 1)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O���b�Z�[�W2�i�o�f�B�u�p�p�Ȃ�Ēm��Ȃ��v�j
			if (g_EpilogueMessageCount >= 2 && g_EpilogueMessageCount <= 3)
			{

				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�L�������b�Z�[�W3�i�v���C���[�u....�I�I�I�v�j
			if (g_EpilogueMessageCount == 3)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O���b�Z�[�W4�i�o�f�B�u����܂Ƃ������ǁA�A�A�v�j
			if (g_EpilogueMessageCount >= 4 && g_EpilogueMessageCount <= 5)
			{
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�L�������b�Z�[�W5�i�v���C���[�u....�R�N���v�j
			if (g_EpilogueMessageCount == 5)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O���b�Z�[�W6�i�o�f�B�u���肪�Ƃ��I�v�j
			if (g_EpilogueMessageCount >= 6 && g_EpilogueMessageCount <= 7)
			{
				py = TEXTURE_MESSAGE_Y1;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}
			// �G�s���[�O���b�Z�[�W7�i�v���C���[�u....���v�j
			if (g_EpilogueMessageCount == 7)
			{
				py = TEXTURE_MESSAGE_Y2;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�̃o�f�B�i�S�g�j
			if (g_EpilogueMessageCount >= 8)
			{
				px = SCREEN_WIDTH * 0.5;
				py = TEXTURE_BUDDY_Y;
				pw = TEXTURE_BUDDY_WIDTH;
				ph = TEXTURE_BUDDY_HEIGHT;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EpilogueBuddyTrans));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�{�X���b�Z�[�W11�i�o�f�B�u���Ⴀ���������I�v�j
			if (g_EpilogueMessageCount == 9)
			{
				py = TEXTURE_BUDDY_MESSAGE_Y;
				pw = TEXTURE_MESSAGE_WIDTH;			// ���b�Z�[�W�̉���
				ph = TEXTURE_MESSAGE_HEIGHT;		// ���b�Z�[�W�̏c��

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �G�s���[�O�̍Ō��BG
			if (g_EpilogueMessageCount >= 10)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EpilogueBGTrans));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}

//=============================================================================
// �G�s���[�O���K�v���ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetEpilogueFlg(void)
{
	return g_EpilogueFlg;
}

void SetEpilogueFlg(BOOL EpilogueFlg)
{
	g_EpilogueFlg = EpilogueFlg;
}





