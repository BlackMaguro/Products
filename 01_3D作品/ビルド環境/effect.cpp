//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "bomb.h"
#include "shadow.h"
#include "effect.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int TEXTURE_MAX (16);			// �e�N�X�`���̐�

static const float	EFFECT_EXPLOSION_WIDTH	(200.0f);	// ���_�T�C�Y
static const float	EFFECT_EXPLOSION_HEIGHT	(200.0f);	// ���_�T�C�Y

static const int TEXANIM_EXPLOSION	(5);	// �G�t�F�N�g�̃e�N�X�`���A�j���[�V�����̊Ԋu

static const int MAX_EFFECT	(MAX_BOMB);		// �G�t�F�N�g�̍ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexExplosion(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static EXPLOSION			g_Explosion[MAX_EFFECT];	// �G�t�F�N�g�̃e�N�X�`��
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char* g_TextureName[] =
{
	PATH_TEXTURE_EFFECT_EXPLOSION "01.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "02.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "03.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "04.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "05.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "06.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "07.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "08.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "09.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "10.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "11.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "12.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "13.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "14.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "15.png",
	PATH_TEXTURE_EFFECT_EXPLOSION "16.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitExplosion(void)
{
	MakeVertexExplosion();

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

	// �����G�t�F�N�g�̏�����
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		ZeroMemory(&g_Explosion[i].material, sizeof(g_Explosion[i].material));
		g_Explosion[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Explosion[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Explosion[i].fWidth = EFFECT_EXPLOSION_WIDTH;
		g_Explosion[i].fHeight = EFFECT_EXPLOSION_HEIGHT;
		g_Explosion[i].passedFrame = 0;
		g_Explosion[i].existFrame = 0;
		g_Explosion[i].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitExplosion(void)
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
void UpdateExplosion(void)
{
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (!g_Explosion[i].use) continue;

		// �e�̈ʒu�ݒ�
		SetPositionShadow(g_Explosion[i].nIdxShadow, XMFLOAT3(g_Explosion[i].pos.x, 0.1f, g_Explosion[i].pos.z));

		if (g_Explosion[i].passedFrame++ >= g_Explosion[i].existFrame)
		{
			g_Explosion[i].use = FALSE;
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
void DrawExplosion(void)
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

	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (g_Explosion[i].use)
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
			mtxScl = XMMatrixScaling(g_Explosion[i].scl.x, g_Explosion[i].scl.y, g_Explosion[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Explosion[i].pos.x, g_Explosion[i].pos.y, g_Explosion[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Explosion[i].material);

			// �e�N�X�`���ݒ�
			int texNo = i % TEXTURE_MAX;
			{
				// �e�N�X�`���A�j��
				g_TexAnim++;
				if ((g_TexAnim % MAX_EFFECT) == 0)
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
HRESULT MakeVertexExplosion(void)
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

	float fWidth = EFFECT_EXPLOSION_WIDTH;
	float fHeight = EFFECT_EXPLOSION_HEIGHT;

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
// �����G�t�F�N�g���o�͂���֐�
//=============================================================================
void ShowExplosion(const XMFLOAT3& pos, int durationTime)
{
	float explosionYOffset = 80.0f;
	for (int i = 0; i < MAX_EFFECT; i++)
	{
		if (!g_Explosion[i].use)
		{
			g_Explosion[i].pos = { pos.x, pos.y - explosionYOffset, pos.z };
			g_Explosion[i].use = TRUE;
			g_Explosion[i].passedFrame = 0;
			g_Explosion[i].existFrame = durationTime;
			break;
		}
	}
}
//=============================================================================
// �����G�t�F�N�g�������֐�
//=============================================================================
void HideExplosion()
{
	for (int i = 0; i < MAX_EFFECT; i++)
		g_Explosion[i].use = FALSE;
}