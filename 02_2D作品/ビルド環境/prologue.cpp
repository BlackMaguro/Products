//=============================================================================
//
// �v�����[�O��ʏ��� [prologue.cpp]
// Author : 
//
//=============================================================================
#include "prologue.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(14)				// �e�N�X�`���̐�

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
	"data/TEXTURE/bg_0.png",									// 0_���[�h�ɓ���O�̑I����ʂ�BG�i�`���[�g���A���Ɠ���BG�j
	"data/TEXTURE/Prologue/prologue_logo_0.png",				// 1_���[�h�ɓ���O�̑I����ʂ̃��S_�^�C�g��
	"data/TEXTURE/tutorial_logo_1_yes.png",						// 2_���[�h�ɓ���O�̑I����ʂ̃��S_�͂�
	"data/TEXTURE/tutorial_logo_2_no.png",						// 3_���[�h�ɓ���O�̑I����ʂ̃��S_������
	"data/TEXTURE/title_menu_arrow.png",						// 4_���j���[�I���̖��
	"data/TEXTURE/Prologue/prologue_bg.png",					// 5_�v�����[�O�̔w�i
	"data/TEXTURE/Prologue/prologue_message_0.png",				// 6_�v�����[�O�̃��b�Z�[�W0
	"data/TEXTURE/Prologue/prologue_message_1.png",				// 7_�v�����[�O�̃��b�Z�[�W1
	"data/TEXTURE/Prologue/prologue_message_2.png",				// 8_�v�����[�O�̃��b�Z�[�W2
	"data/TEXTURE/Prologue/prologue_message_3.png",				// 9_�v�����[�O�̃��b�Z�[�W3
	"data/TEXTURE/Prologue/prologue_charaMessage_0.png",		// 10_�v�����[�O�̃L�������b�Z�[�W0
	"data/TEXTURE/Prologue/prologue_charaMessage_1.png",		// 11_�v�����[�O�̃L�������b�Z�[�W1
	"data/TEXTURE/Prologue/prologue_charaMessage_2.png",		// 12_�v�����[�O�̃L�������b�Z�[�W2
	"data/TEXTURE/Prologue/prologue_charaMessage_3.png",		// 13_�v�����[�O�̃L�������b�Z�[�W2
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;
static BOOL						g_PrologueFlg = FALSE;		// �v�����[�O�����s���邩�̃t���O
int g_prologue_menu_select;									// ���[�h�ɓ���O�̑I�����p�ϐ�
float g_prologueTimeCount;									// �v�����[�O��ʂ̃^�C���ϐ�
int g_prologueMessageCount;									// �v�����[�O��ʂ̃��b�Z�[�W�𓧖��ɂ���ϐ�
float g_prologueMessageTrans = 1.0f;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPrologue(void)
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
	g_prologue_menu_select = 0;
	g_prologueTimeCount = 0;
	g_prologueMessageCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPrologue(void)
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
void UpdatePrologue(void)
{
	// �N���A�t���O���I�t�ɂ���
	SetClearFlg(FALSE);

	g_prologueTimeCount += 1.0f;

	// �v�����[�O����Enter���������Ƃ�
	if (GetPrologueFlg() == TRUE)
	{
		// �G���^�[�L�[�������ꂽ�Ƃ��i�v�����[�O����Enter���j
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_02_MESSAGECLICK);
		}
	}


	if (g_prologueMessageCount >= 4)
	{
		g_prologueMessageTrans -= 0.01f;
		if (g_prologueMessageTrans < 0.0f)
		{
			g_prologueMessageTrans = 0.0f;
		}
	}

	if ((GetPrologueFlg() == TRUE) && GetKeyboardTrigger(DIK_RETURN))
	{
		// �v�����[�O�̃��b�Z�[�W�J�E���g���グ��
		if ((g_prologueMessageCount == 4) && (g_prologueMessageTrans != 0.0f))
		{
			g_prologueMessageCount = 4;
		}
		g_prologueMessageCount++;

	}

	// �v�����[�O�̍Ō�܂ōs�����烂�[�h���`���[�g���A����
	if (GetKeyboardTrigger(DIK_RETURN) && g_prologueMessageCount >= 8)
	{		
		// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}

	// �v�����[�O�����{���邩�̏����i�t���O��TRUE�ɂȂ����������j
	if (GetPrologueFlg() == FALSE)
	{
		// �T�E���h�̏���
		{
			// �G���^�[�L�[�������ꂽ�Ƃ��i�v�����[�O�O��Enter���j
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
			if (GetKeyboardTrigger(DIK_RETURN) && g_prologue_menu_select == 0)
			{
				// �v�����[�O�t���O���I���ɂ���
				SetPrologueFlg(TRUE);
				// �v�����[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_prologue_menu_select == 0)
			{
				// �v�����[�O�t���O���I���ɂ���
				SetPrologueFlg(TRUE);
				// �v�����[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_prologue_menu_select == 0)
			{
				// �v�����[�O�t���O���I���ɂ���
				SetPrologueFlg(TRUE);
				// �v�����[�O�̉���ݒ�
				PlaySound(SOUND_LABEL_BGM_02_PROLOGUE);

			}
		}

		// �I�������u�������v�̂Ƃ�
		{
			if (GetKeyboardTrigger(DIK_RETURN) && g_prologue_menu_select == 1)
			{
				// Enter��������A�X�e�[�W��؂�ւ���
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			// �Q�[���p�b�h�œ��͏���
			else if (IsButtonTriggered(0, BUTTON_START) && g_prologue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else if (IsButtonTriggered(0, BUTTON_B) && g_prologue_menu_select == 1)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
		}

		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		int select = 1;
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_prologue_menu_select += select;
		}
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_prologue_menu_select -= select;
		}

		g_prologue_menu_select = g_prologue_menu_select % 2;

		if (g_prologue_menu_select < 0)
		{
			g_prologue_menu_select = 1;
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("g_prologueMessageCount:%d,\n", g_prologueMessageCount);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPrologue(void)
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
	float sinValue = (sin(g_prologueTimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������

	// �`���[�g���A�������邩�̊m�F���
	if (g_PrologueFlg == FALSE)
	{
		// �v�����[�O�O�̑I������ʂ�BG�\��
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

		// �v�����[�O�O�̑I������ʂ̃^�C�g��
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
		//  �v�����[�O�O�̑I������ʂ̃��j���[�u�͂��v
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			py = TEXTURE_MANU_ARROW_Y;
			pw = TEXTURE_WIDTH_LOGO_MANU;
			ph = TEXTURE_HEIGHT_LOGO_MANU;
			if (g_prologue_menu_select == 0)
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


		//  �v�����[�O�O�̑I������ʂ̃��j���[�u�������v
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			py = TEXTURE_MANU_ARROW_Y + TEXTURE_HEIGHT_LOGO_MANU;
			if (g_prologue_menu_select == 1)
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

		pmy += g_prologue_menu_select * TEXTURE_HEIGHT_LOGO_MANU;
		float arrowHeight = pmh * sinValue;				// ���iarrow�j����]������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, pmx, pmy, pmw, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	// �v�����[�O�̊J�n
	else if(g_PrologueFlg != FALSE)
	{
		// �v�����[�O�̍ŏ���`��i�����ƕ\�����Ă���j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		//==========================================
		// ���b�Z�[�W�����i�����J�n�j
		//==========================================
		{

			// �v�����[�O���b�Z�[�W0�i......�̂�j
			if (g_prologueMessageCount == 0)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O���b�Z�[�W1�i....����..�̂�j
			if (g_prologueMessageCount == 1)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O���b�Z�[�W2�i....�N����..�̂�j
			if (g_prologueMessageCount == 2)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O���b�Z�[�W3�i�˂�..�N����!�̂�j
			if (g_prologueMessageCount >= 3 && g_prologueMessageCount < 5)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_prologueMessageTrans));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

		//==========================================
		// ���b�Z�[�W�����i�����J�n�j
		//==========================================
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float px = SCREEN_WIDTH / 2;
			float py = 100.0f;
			float pw = 600.0f;		// ���b�Z�[�W�̉���
			float ph = 150.0f;		// ���b�Z�[�W�̏c��

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �v�����[�O�L�������b�Z�[�W0�i�o�f�B�j
			if (g_prologueMessageCount >= 4 && g_prologueMessageCount <= 5 && g_prologueMessageTrans == 0.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O�L�������b�Z�[�W1�i�v���C���[�j
			if (g_prologueMessageCount == 5)
			{
				py = 400.0f;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O�L�������b�Z�[�W2�i�o�f�B�j
			if (g_prologueMessageCount >= 6 && g_prologueMessageCount <= 7)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// �v�����[�O�L�������b�Z�[�W3�i�v���C���[�j
			if (g_prologueMessageCount == 7)
			{
				py = 400.0f;

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}

//=============================================================================
// �v�����[�O���K�v���ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetPrologueFlg(void)
{
	return g_PrologueFlg;
}

void SetPrologueFlg(BOOL prologueFlg)
{
	g_PrologueFlg = prologueFlg;
}





