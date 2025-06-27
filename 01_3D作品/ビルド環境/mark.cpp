//=============================================================================
//
// �}�[�N���� [mark.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "mark.h"
#include "game.h"
#include "enemy.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(2)				// �e�N�X�`���̐�

#define	MARK_WIDTH			(10.0f)			// ���_�T�C�Y
#define	MARK_HEIGHT			(20.0f)			// ���_�T�C�Y

#define	OFFSET_Y			(10.0f)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		width;			// ��
	float		height;			// ����
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} MARK;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexMark(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static MARK							g_Mark[MAX_ENEMY];	// �}�[�N�̃e�N�X�`��

namespace {
	// �e�e�N�X�`���������e�����A���Œ�`
	// �e�N�X�`���̓ǂݍ���
	static char* g_TextureName[] = 
	{
		PATH_TEXTURE_INGAME "03_ENEMY/01_question.png",
		PATH_TEXTURE_INGAME "03_ENEMY/02_exclamation.png",
	};
	enum TextureIndex
	{
		TEXTURE_QUESTION,		// �H�i�͂Ăȁj�}�[�N
		TEXTURE_EXCLAMATION,	// �I�i�r�b�N���j�}�[�N
		TEXTURE_COUNT			// �e�N�X�`���̐�
	};

}

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMark(void)
{
	MakeVertexMark();

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

	// �}�[�N�̏�����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// ���m�t���O�̏�����
		ZeroMemory(&g_Mark[i].material, sizeof(g_Mark[i].material));
		g_Mark[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Mark[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Mark[i].scl = XMFLOAT3(0.1f, 0.3f, 0.1f);
		g_Mark[i].width = MARK_WIDTH;
		g_Mark[i].height = MARK_HEIGHT;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMark(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMark(void)
{
	ENEMY* enemy = GetEnemy();
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// �G�l�~�[�̓���ɍ��W���X�V����
		g_Mark[i].pos = enemy[i].pos;
		g_Mark[i].pos.y += OFFSET_Y;
	}

#ifdef _DEBUG

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMark(void)
{
	ENEMY* enemy = GetEnemy();

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);
	//SetLightEnable(TRUE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �r���[�}�g���b�N�X���擾
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă�o�[�W����
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
		mtxScl = XMMatrixScaling(g_Mark[i].scl.x, g_Mark[i].scl.y, g_Mark[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Mark[i].pos.x, g_Mark[i].pos.y, g_Mark[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �}�e���A���ݒ�
		SetMaterial(g_Mark[i].material);

		// �e�N�X�`���̐ݒ�
		int texNo = TEXTURE_QUESTION;
		if (enemy[i].noticeFlg)	// ���m��Ԃ̎��̓e�N�X�`����ύX
		{
			texNo = TEXTURE_EXCLAMATION;
		}

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);

	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexMark(void)
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
