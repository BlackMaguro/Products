//=============================================================================
//
// ���𓊂���Ƃ��̃}�[�J�[���� [throwMarker.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "shadow.h"
#include "throwMarker.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int TEXTURE_MAX (5);			// �e�N�X�`���̐�

static const float	THROWMARKER_WIDTH	(30.0f);	// ���_�T�C�Y
static const float	THROWMARKER_HEIGHT	(60.0f);	// ���_�T�C�Y

static const int TEXANIM_THROWMAKER	(5);	// ������̃}�[�J�[�̃e�N�X�`���A�j���[�V�����̊Ԋu

static const int MAX_THROWMAKER	(3);		// ������̃}�[�J�[�̍ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} THROWMARKER;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexThrowMarker(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static THROWMARKER			g_ThrowMarker[MAX_THROWMAKER];	// ������̃}�[�J�[�̃e�N�X�`��
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char* g_TextureName[] =
{
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_1.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_2.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_3.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_4.png",
	PATH_TEXTURE_BILLBOARD_THROWMAKER "throwMarker_5.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitThrowMarker(void)
{
	MakeVertexThrowMarker();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = g_TexAnim = 0;

	// ������̃}�[�J�[�̏�����
	for (int nCntThrowMarker = 0; nCntThrowMarker < MAX_THROWMAKER; nCntThrowMarker++)
	{
		ZeroMemory(&g_ThrowMarker[nCntThrowMarker].material, sizeof(g_ThrowMarker[nCntThrowMarker].material));
		g_ThrowMarker[nCntThrowMarker].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ThrowMarker[nCntThrowMarker].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ThrowMarker[nCntThrowMarker].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ThrowMarker[nCntThrowMarker].fWidth = THROWMARKER_WIDTH;
		g_ThrowMarker[nCntThrowMarker].fHeight = THROWMARKER_HEIGHT;
		g_ThrowMarker[nCntThrowMarker].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitThrowMarker(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateThrowMarker(void)
{

	for (int nCntThrowMarker = 0; nCntThrowMarker < MAX_THROWMAKER; nCntThrowMarker++)
	{
		if (g_ThrowMarker[nCntThrowMarker].use)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_ThrowMarker[nCntThrowMarker].nIdxShadow, XMFLOAT3(g_ThrowMarker[nCntThrowMarker].pos.x, 0.1f, g_ThrowMarker[nCntThrowMarker].pos.z));
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawThrowMarker(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);
	//SetLightEnable(TRUE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_THROWMAKER; i++)
	{
		if (g_ThrowMarker[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// �����s��i�����s��j��]�u�s�񂳂��ċt�s������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_ThrowMarker[i].scl.x, g_ThrowMarker[i].scl.y, g_ThrowMarker[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_ThrowMarker[i].pos.x, g_ThrowMarker[i].pos.y, g_ThrowMarker[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_ThrowMarker[i].material);

			// �e�N�X�`���ݒ�
			int texNo = i % TEXTURE_MAX;
			{
				// �e�N�X�`���A�j��
				g_TexAnim++;
				if ((g_TexAnim % MAX_THROWMAKER) == 0)
				{
					g_TexNo = (g_TexNo + 1) % TEXTURE_MAX;
				}
				texNo = g_TexNo;
			}
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexThrowMarker(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}
//=============================================================================
// ������}�[�J�[���o�͂���֐�
//=============================================================================
void ShowThrowMarker(const XMFLOAT3& pos)
{
	// �܂��S����
	for (int i = 0; i < MAX_THROWMAKER; i++)
		g_ThrowMarker[i].use = FALSE;
	// �擪�����g��
	g_ThrowMarker[0].pos = pos;
	g_ThrowMarker[0].use = TRUE;
}
//=============================================================================
// ������}�[�J�[�������֐�
//=============================================================================
void HideThrowMarker()
{
	for (int i = 0; i < MAX_THROWMAKER; i++)
		g_ThrowMarker[i].use = FALSE;
}