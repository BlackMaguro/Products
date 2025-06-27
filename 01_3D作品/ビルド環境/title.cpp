//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO			(80)			// ���S�T�C�Y�i�c�j
#define TEXTURE_WIDTH_LOGO_MANU		(200)			// ���j���[�̑I�����̃T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO_MANU	(60)			// ���j���[�̑I�����̃T�C�Y�i�c�j
#define TEXTURE_WIDTH_MANU_ARROW	(100)			// ���j���[�I���̖��̃T�C�Y�i���j 
#define TEXTURE_HEIGHT_MANU_ARROW	(60)			// ���j���[�I���̖��̃T�C�Y�i�c�j

#define TEXTURE_MANU_ARROW_X		(350)			// ���j���[����X���W 
#define TEXTURE_MANU_ARROW_Y		(320)			// ���j���[����Y���W�i�����l�j 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/01_BG/01_title.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/02_TITLE/title_softname.png",				// �^�C�g���O�Ƀ��S������
	"data/TEXTURE/02_TITLE/title_menu_00_start.png",		// �͂��߂���
	"data/TEXTURE/02_TITLE/title_menu_01_settings.png",		// �����Ă�
	"data/TEXTURE/02_TITLE/title_menu_02_end.png",			// �����
	"data/TEXTURE/02_TITLE/title_menu_arrow2.png",			// ���j���[�I���̖��
	"data/TEXTURE/02_TITLE/title_menu_03_UserGuide.png",	// ���������߂�
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;
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

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_00_TITLE);

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

	// �N���W�b�g�\�L���I�������
	if (GetCreditFlg() != TRUE)
	{
		// �T�E���h�֌W�̏���
		{
			// Enter�A�Q�[���p�b�h��B���X�^�[�g�������ꂽ�Ƃ��̉�
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				PlaySound(SOUND_LABEL_SE_01_ENTER);
			}
			// �J�[�\���̏㉺�L�[���������Ƃ�
			if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_UP) ||
				IsButtonTriggered(0, BUTTON_UP) || IsButtonTriggered(0, BUTTON_DOWN))
			{
				PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
			}
		}

		// Enter�A�Q�[���p�b�h��B���X�^�[�g�������ꂽ��A�X�e�[�W��؂�ւ���
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			// �͂��߂��I����
			if (menu_select == 0)
			{
				// TODO:�`���[�g���A���ɔ�΂�
				SetFade(FADE_OUT, MODE_GAME);
			}
			else if (menu_select == 1)
			{
				// TODO:�����ʂɔ�΂�
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			// �����Ă���I����
			else if (menu_select == 2)
			{
				// TODO:�ݒ��ʂɔ�΂�
				SetFade(FADE_OUT, MODE_SETTINGS);
			}
			// ������I����
			else if (menu_select == 3)
			{
				// TODO:�ݒ��ʂɔ�΂�
				SetFade(FADE_OUT, MODE_END);
			}
		}

		if (flag_alpha == TRUE)
		{
			alpha -= 0.02f;
			if (alpha <= 0.0f)
			{
				alpha = 0.0f;
				flag_alpha = FALSE;
			}
		}
		else
		{
			alpha += 0.02f;
			if (alpha >= 1.0f)
			{
				alpha = 1.0f;
				flag_alpha = TRUE;
			}
		}

		// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
		{
			int select = 1;
			if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN))
			{
				menu_select += select;
			}
			if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
			{
				menu_select -= select;
			}

			menu_select = menu_select % 4;

			if (menu_select < 0)
			{
				menu_select = 3;
			}

		}
	}





#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

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

	// �^�C�g����ʂ̑O�Ƀ��S�\��
	if (GetCreditFlg() == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_creditTrans));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	else if (GetCreditFlg() != TRUE)
	{
		// �^�C�g���̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		//==========================================
		// ���j���[�����i�����J�n�j
		//==========================================
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float tx = 780.0f;		// ���j���[�̑I������X���W
		float ty = 300.0f;		// ���j���[�̑I������Y���W
		float lw = 680.0f;		// ���j���[�̑I�����̉���
		float lh = 100.0f;		// ���j���[�̑I�����̏c��

		// �I������Ă����ꍇ�A������Ԃ�����
		float selectColor = 1.0f;
		// �����x
		float transparentValue;

		float tmy = TEXTURE_MANU_ARROW_Y;		// ���Ɠ���Y���W�i�����ʒu�j
		// ���j���[�̕\���i�͂��߂���j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			// ���j���[�̍��W�iY���̂ݕύX�j
			if (menu_select == 0)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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
		}


		// ���j���[�̕\���i������@�j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			// ���j���[�̍��W�iY���̂ݕύX�j
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 1)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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
		}


		// ���j���[�̕\���i�����Ă��j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			// ���j���[�̍��W�iY���̂ݕύX�j
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 2)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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

		}

		// ���j���[�̕\���i�����j
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			// ���j���[�̍��W�iY���̂ݕύX�j
			tmy += TEXTURE_HEIGHT_LOGO_MANU;
			if (menu_select == 3)
			{
				selectColor = 0.0f;
				transparentValue = alpha;
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

		}



		// ���j���[�̖��iarrow�j�\��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);
			tx -= 160.0f;
			// ����Y���W
			float tmcy = (float)TEXTURE_MANU_ARROW_Y;
			tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
			float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW;

			if (GetMode() == MODE_TITLE)
			{
				arrowHeight *= alpha;	// ���iarrow�j����]������
			}
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, tx, tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}

}





