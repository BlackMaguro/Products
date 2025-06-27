//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : ����z��
//
//=============================================================================
#include "title.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(10)				// �e�N�X�`���̐�

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
FILE* fp = fopen("savedata.bin", "rb");								// �Z�[�u�f�[�^�̗L��

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title/title_bg.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/title/title_01.png",					// �^�C�g��
	"data/TEXTURE/title_menu_00_hajimekara.png",		// �͂��߂���
	"data/TEXTURE/title_menu_01_tudukikara.png",		// �Â�����
	"data/TEXTURE/title_menu_02_settings.png",			// �����Ă�
	"data/TEXTURE/title_menu_03_owari.png",				// �����
	"data/TEXTURE/title_menu_arrow.png",				// ���j���[�I���̖��
	"data/TEXTURE/title/title_02.png",					// �^�C�g���i�N���A��j
	"data/TEXTURE/title/title_softname.png",			// �^�C�g���O�ɉ�Ѓ��S������
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static float g_creditTrans = 1.0f;
static int menu_select;
float g_titleTimeCount;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	menu_select = 0;	// ���j���[�Z���N�g�ϐ�
	g_titleTimeCount = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	g_titleTimeCount += 1.0f;
	if (g_titleTimeCount > 180.0f)
	{
		g_creditTrans -= 0.01f;
		if (g_creditTrans < 0.0f)
		{
			SetCreditFlg(FALSE);
			g_creditTrans = 0.0f;
		}
	}


	if (GetCreditFlg() != TRUE)
	{
		// �T�E���h�֌W�̏���
		{
			// �G���^�[�L�[�������ꂽ�Ƃ�
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

		// �u�͂��߂���v��I��
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 0)
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_PROLOGUE);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 0)
		{

			SetFade(FADE_OUT, MODE_PROLOGUE);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 0)
		{
			SetFade(FADE_OUT, MODE_PROLOGUE);
		}

		// �Z�[�u�f�[�^�����[�h����
		// �u�Â�����v��I��
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 1 && fp != NULL)
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 1 && fp != NULL)
		{
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 1 && fp != NULL)
		{
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}

		// �u�����Ă��v��I��
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 2)
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_SETTINGS);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 2)
		{
			SetFade(FADE_OUT, MODE_SETTINGS);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 2)
		{
			SetFade(FADE_OUT, MODE_SETTINGS);
		}

		// �u�����v��I��
		if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 3)
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_END);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 3)
		{
			SetFade(FADE_OUT, MODE_END);
		}
		else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 3)
		{
			SetFade(FADE_OUT, MODE_END);
		}


		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			// SE�F�J�[�\���㉺���������Ƃ�
			PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			menu_select += select;
			// �Z�[�u�f�[�^���Ȃ�����������ړ������Ȃ�
			if ((menu_select == 1) && (fp == NULL))
			{
				menu_select = 2;
			}
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			// SE�F�J�[�\���㉺���������Ƃ�
			PlaySound(SOUND_LABEL_SE_00_MOUSEUPDOWN);
			menu_select -= select;
			// �Z�[�u�f�[�^���Ȃ�����������ړ������Ȃ�
			if ((menu_select == 1) && (fp == NULL))
			{
				menu_select = 0;
			}
		}

		menu_select = menu_select % 4;

		if (menu_select < 0)
		{
			menu_select = 3;
		}

#ifdef _DEBUG	// �f�o�b�O����\������
		//PrintDebugProc("Player:�� �� �� ���@Space\n");
		PrintDebugProc("g_creditTrans:%f.\n", g_creditTrans);
#endif
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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
	float sinValue = (sin(g_titleTimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������
	// �^�C�g����ʂ̑O�̉�Ж��i���j�\��
	if(GetCreditFlg() == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_creditTrans));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	else if(GetCreditFlg() != TRUE)
	{
		// �^�C�g���̔w�i�iBG�j��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// �^�C�g�����S_�^�C�g���\��
		// �e�N�X�`���ݒ�
		if (GetClearFlg() == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);
		}
		else
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		}
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float tx = SCREEN_WIDTH / 2;
		float ty = 100;
		float lw = 680.0f;		// ���S�̉���
		float lh = 100.0f;		// ���S�̏c��

		SetSpriteColor(g_VertexBuffer, tx, ty, lw, lh, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//==========================================
		// ���j���[�����i�����J�n�j
		//==========================================
		// �I������Ă����ꍇ�A������Ԃ�����
		float selectColor = 1.0f;
		// �����x
		float transparentValue;

		// ���j���[�̕\���i�͂��߂���j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// ���j���[�̍��W�iY���̂ݕύX�j
		float tmy = TEXTURE_MANU_ARROW_Y;		// ���Ɠ���Y���W�i�����ʒu�j
		if (menu_select == 0)
		{
			selectColor = 0.0f;
			transparentValue = sinValue + 0.3f;
		}
		else
		{
			selectColor = 1.0f;
			transparentValue = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���j���[�̕\���i�Â�����j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// ���j���[�̍��W�iY���̂ݕύX�j
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
		if (menu_select == 1)
		{
			selectColor = 0.0f;
			transparentValue = sinValue + 0.3f;
		}
		else
		{
			selectColor = 1.0f;
			transparentValue = 1.0f;
		}

		// �Z�[�u�f�[�^��������������̈ʒu���Ƃ΂�
		if (fp == NULL)
		{
			SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));
		}

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���j���[�̕\���i�����Ă��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// ���j���[�̍��W�iY���̂ݕύX�j
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
		if (menu_select == 2)
		{
			selectColor = 0.0f;
			transparentValue = sinValue + 0.3f;
		}
		else
		{
			selectColor = 1.0f;
			transparentValue = 1.0f;
		}
		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���j���[�̕\���i�����j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// ���j���[�̍��W�iY���̂ݕύX�j
		tmy += TEXTURE_HEIGHT_LOGO_MANU;
		if (menu_select == 3)
		{
			selectColor = 0.0f;
			transparentValue = sinValue + 0.3f;
		}
		else
		{
			selectColor = 1.0f;
			transparentValue = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, tx, tmy, TEXTURE_WIDTH_LOGO_MANU, TEXTURE_HEIGHT_LOGO_MANU, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		//==========================================
		// ���j���[�����i�����I���j
		//==========================================

		// ���j���[�̖��iarrow�j�\���i�ԁj
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		// ����Y���W
		float tmcy = (float) TEXTURE_MANU_ARROW_Y;
		tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW * sinValue;	// ���iarrow�j����]������
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, TEXTURE_MANU_ARROW_X, tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}





