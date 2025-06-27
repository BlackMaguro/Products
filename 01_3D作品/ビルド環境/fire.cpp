//=============================================================================
//
// ������ [fire.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "fire.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(8)				// �e�N�X�`���̐�

#define	FIRE_WIDTH			(30.0f)			// ���_�T�C�Y
#define	FIRE_HEIGHT			(60.0f)			// ���_�T�C�Y

#define TEXANIM_FIRE		(9)				// ���̃e�N�X�`���A�j���[�V�����̊Ԋu

#define	MAX_FIRE			(256)			// ���̍ő吔

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

} FIRE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexFire(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static FIRE					g_Fire[MAX_FIRE];	// ���̃e�N�X�`��
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char *g_TextureName[] =
{
	"data/TEXTURE/04_INGAME/01_FIRE/Fire1.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire2.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire3.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire4.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire5.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire6.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire7.png",
	"data/TEXTURE/04_INGAME/01_FIRE/Fire8.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFire(void)
{
	MakeVertexFire();

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

	// ���̏�����
	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		ZeroMemory(&g_Fire[nCntFire].material, sizeof(g_Fire[nCntFire].material));
		g_Fire[nCntFire].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Fire[nCntFire].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Fire[nCntFire].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Fire[nCntFire].fWidth = FIRE_WIDTH;
		g_Fire[nCntFire].fHeight = FIRE_HEIGHT;
		g_Fire[nCntFire].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	// ���̐ݒ�
	SetFire(XMFLOAT3(-300.0f, -3.0f, 550.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(180.0f,  -3.0f, 310.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(250.0f,  -3.0f, 550.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(90.0f,   -3.0f, 300.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(250.0f,  -3.0f,-300.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetFire(XMFLOAT3(220.0f,  -3.0f,-600.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFire(void)
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
void UpdateFire(void)
{

	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		if(g_Fire[nCntFire].use)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_Fire[nCntFire].nIdxShadow, XMFLOAT3(g_Fire[nCntFire].pos.x, 0.1f, g_Fire[nCntFire].pos.z));
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFire(void)
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
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_FIRE; i++)
	{
		if(g_Fire[i].use)
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
			mtxScl = XMMatrixScaling(g_Fire[i].scl.x, g_Fire[i].scl.y, g_Fire[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Fire[i].pos.x, g_Fire[i].pos.y, g_Fire[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Fire[i].material);

			// �e�N�X�`���ݒ�
			int texNo = i % TEXTURE_MAX;
			{
				// �e�N�X�`���A�j��
				g_TexAnim++;
				if ((g_TexAnim % TEXANIM_FIRE) == 0)
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
HRESULT MakeVertexFire(void)
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
// ���̃p�����[�^���Z�b�g
//=============================================================================
int SetFire(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxFire = -1;

	for(int nCntFire = 0; nCntFire < MAX_FIRE; nCntFire++)
	{
		if(!g_Fire[nCntFire].use)
		{
			g_Fire[nCntFire].pos = pos;
			g_Fire[nCntFire].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Fire[nCntFire].fWidth = fWidth;
			g_Fire[nCntFire].fHeight = fHeight;
			g_Fire[nCntFire].use = TRUE;

			// �e�̐ݒ�
			g_Fire[nCntFire].nIdxShadow = CreateShadow(g_Fire[nCntFire].pos, 0.5f, 0.5f);

			nIdxFire = nCntFire;

			break;
		}
	}

	return nIdxFire;
}
