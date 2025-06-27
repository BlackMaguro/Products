//=============================================================================
//
// BG���� [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"
#include "main.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_TUTORIAL_WIDTH		(2000)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_BOSS_WIDTH			(960)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(640)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/tutorial_bg.png",			// 0_�`���[�g���A���X�e�[�W��BG
	"data/TEXTURE/BG/bg_sky.png",				// 1_�`���[�g���A���X�e�[�W�̋�
	"data/TEXTURE/BG/bg_stage1.png",			// 2_�X�e�[�W1��BG
	"data/TEXTURE/BG/bg_boss1.png",				// 3_�{�X�X�e�[�W��BG
};

static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w     = TEXTURE_WIDTH;
	if (GetBossFlg() == TRUE) g_BG.w = TEXTURE_BOSS_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;
	g_BG.bgNo = GetMode();	// �g�p����BG��\������

	g_BG.scrl  = 0.0f;		// TEX�X�N���[��
	g_BG.scrl2 = 0.0f;		// TEX�X�N���[��

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// �P�t���O�̏���ۑ�

	g_BG.scrl -= 0.0f;		// 0.005f;		// �X�N���[��


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("BG pos.x:%f,pos.y:%f,\ng_BG.texNo:%d\ng_BG.bgNo:%d,\ng_BG.w:%f,", g_BG.pos.x, g_BG.pos.y, g_BG.texNo, g_BG.bgNo,g_BG.w);
#endif

	// �`���[�g���A����`��
	if (g_BG.bgNo == MODE_TUTORIAL)
	{
		g_BG.w = TEXTURE_TUTORIAL_WIDTH;
		//g_BG.h = TEXTURE_HEIGHT;
		// ���`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_TUTORIAL_WIDTH);
			g_BG.scrl += tx * 0.001f;
			g_BG.scrl += 0.001f;

			SetSpriteLTColor(g_VertexBuffer,
				0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_HEIGHT,
				g_BG.scrl, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// �`���[�g���A���̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}

	// stage1��`��
	if ((g_BG.bgNo == MODE_GAME) && GetBossFlg() != TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
			
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �{�X��̔w�i��`��
	if (g_BG.bgNo == MODE_BOSS)
	{
		g_BG.w = TEXTURE_BOSS_WIDTH;
		//g_BG.h = TEXTURE_HEIGHT;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}





