//=============================================================================
//
// �Q�[���I�[�o�[��ʏ��� [gameover.cpp]
// Author : 
//
//=============================================================================
#include "gameover.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO				(480)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO				(200)			// ���S�T�C�Y�i�c�j 
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
	"data/TEXTURE/GameOver/GameOver_bg.png",				// �Q�[���I�[�o�[��ʂ̔w�i
	"data/TEXTURE/GameOver/GameOver_title.png",				// �Q�[���I�[�o�[�̕���
	"data/TEXTURE/GameOver/GameOver_menu_00_retry.png",		// �^�C�g����
	"data/TEXTURE/GameOver/GameOver_menu_01_toTitle.png",	// ���g���C
	"data/TEXTURE/GameOver/GameOver_menu_02_owari.png",		// �����
	"data/TEXTURE/GameOver/GameOver_menu_arrow.png",		// ���j���[�I���̖��

};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static int menu_select;		// ���j���[�Z���N�g�ϐ�
float g_timeCount;
float g_titleTrans;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGameOver(void)
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

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	menu_select = 0;
	g_timeCount = 0.0f;
	g_titleTrans = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameOver(void)
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
void UpdateGameOver(void)
{
	// GameOver�̕����𓧖��ɂ��鎞�̘b
	if(g_titleTrans != 1.0f) g_titleTrans += 1.0f / 300.0f;

	g_timeCount += 1.0f;
	// �u���g���C�v��I��
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 0)
	{// Enter��������A�X�e�[�W��؂�ւ���
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 0)
	{
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 0)
	{
		if (GetBossFlg() == TRUE)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
		else
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}

	// �u�^�C�g���ցv��I��
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 1)
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 1)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 1)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_TITLE);
	}

	// �u�����v��I��
	if (GetKeyboardTrigger(DIK_RETURN) && menu_select == 2)
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START) && menu_select == 2)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}
	else if (IsButtonTriggered(0, BUTTON_B) && menu_select == 2)
	{
		SetBossFlg(FALSE);
		SetFade(FADE_OUT, MODE_END);
	}


	// ���j���[��ʂ�I�������Ƃ��ɖ����ړ�������
	int select = 1;
	if (GetKeyboardTrigger(DIK_DOWN)) menu_select += select;
	if (GetKeyboardTrigger(DIK_UP)) menu_select -= select;

	menu_select = menu_select % 3;

	if (menu_select < 0)
	{
		menu_select = 2;
	}
#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");
	//PrintDebugProc("volume:", menu_select);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameOver(void)
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
	float sinValue = (sin(g_timeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������

	// �^�C�g���̔w�i(BG)��`��
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	float lx = TEXTURE_WIDTH / 2;		// ��ʂ̒����ɔz�u
	float ly = 100;
	float lw = TEXTURE_WIDTH_LOGO;
	float lh = TEXTURE_HEIGHT_LOGO;
	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	SetSpriteColor(g_VertexBuffer, lx, ly, lw, lh, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_titleTrans));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//==========================================
	// ���j���[�����i�����J�n�j
	//==========================================
	// �I������Ă����ꍇ�A������Ԃ�����
	float selectColor = 1.0f;
	// �����x
	float transparentValue;

	// ���j���[�̕\���i���g���C�j
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	// ���j���[�̍��W�iY���̂ݕύX�j
	float lmy = TEXTURE_MANU_ARROW_Y;		// ���Ɠ���Y���W�i�����ʒu�j
	float lmw = TEXTURE_WIDTH_LOGO_MANU;
	float lmh = TEXTURE_HEIGHT_LOGO_MANU;

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

	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// ���j���[�̕\���i�^�C�g���ցj
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	// ���j���[�̍��W�iY���̂ݕύX�j
	lmy += TEXTURE_HEIGHT_LOGO_MANU;
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
	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// ���j���[�̕\���i�����j
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	// ���j���[�̍��W�iY���̂ݕύX�j
	lmy += TEXTURE_HEIGHT_LOGO_MANU;
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

	SetSpriteColor(g_VertexBuffer, lx, lmy, lmw, lmh, tx, ty, tw, th,
		XMFLOAT4(1.0f, selectColor, selectColor, transparentValue));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	//==========================================
	// ���j���[�����i�����I���j
	//==========================================

	// ���j���[�̖��iarrow�j�\���i�ԁj
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
	// ����Y���W
	int tmcy = TEXTURE_MANU_ARROW_Y;
	tmcy += menu_select * TEXTURE_HEIGHT_LOGO_MANU;
	float arrowHeight = TEXTURE_HEIGHT_MANU_ARROW * sinValue;	// ���iarrow�j����]������
	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, TEXTURE_MANU_ARROW_X, (float)tmcy, TEXTURE_WIDTH_MANU_ARROW, arrowHeight, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f));
	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}





