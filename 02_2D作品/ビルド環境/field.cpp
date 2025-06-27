//=============================================================================
//
// �t�B�[���h���� [field.cpp]
// Author : 
//
//=============================================================================
#include "field.h"
#include "player.h"
#include "tutorial.h"
#include "settings.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(10)			// �e�N�X�`���̐�
#define STAGE_MAX					(1)				// �X�e�[�W�̍ő吔

#define MAPCHIP_TEXTURE_WIDTH		(512)			// �}�b�v�`�b�v�e�N�X�`���̉���
#define MAPCHIP_TEXTURE_HEIGHT		(704)			// �}�b�v�`�b�v�e�N�X�`���̏c��
#define TEXTURE_PATTERN_DIVIDE_X	(8)				// �e�N�X�`���̉����̕�����
#define TEXTURE_PATTERN_DIVIDE_Y	(11)			// �e�N�X�`���̏c���̕�����

#define FIELD_SIZE_MAPCHIP_WIDTH			(FIELD_WIDTH/MAPCHIP_WIDTH)				// �t�B�[���h�ɑ΂��鉡���̃`�b�v���i�l�̓`�b�v���j
#define FIELD_SIZE_MAPCHIP_HEIGHT			(FIELD_HEIGHT/MAPCHIP_HEIGHT)			// �t�B�[���h�ɑ΂���c���̃`�b�v���i�l�̓`�b�v���j
#define FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH	(FIELD_TUTORIAL_WIDTH/MAPCHIP_WIDTH)	// �t�B�[���h�ɑ΂��鉡���̃`�b�v���i�l�̓`�b�v���j
#define FIELD_SIZE_BOSS_MAPCHIP_WIDTH		(FIELD_BOSS_WIDTH/MAPCHIP_WIDTH)		// �t�B�[���h�ɑ΂��鉡���̃`�b�v���i�l�̓`�b�v���j

//#define TEXTURE_PATTERN_DEVIDE_HEIGHT	()				// �}�b�v�`�b�v�̉��̕�����
//#define TEXTURE_PATTERN_DEVIDE_WIDTH	()				// �}�b�v�`�b�v�̉��̕�����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int mapNum;			// �}�b�vNo.
static int g_ScreenDispX;	// �X�N���[����X���W
static int g_ScreenDispY;	// �X�N���[����Y���W
int g_chipX;				// �`�b�v��X���W
int g_chipY;				// �`�b�v��Y���W
PLAYER* player;				// �v���C���[�ւ̃A�N�Z�X�p�ϐ�

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Feild/block_black.png",
	"data/TEXTURE/Feild/block_1_ash.png",
	"data/TEXTURE/Feild/block_2_woodBox.png",
	"data/TEXTURE/Feild/block_3_woodBox.png",
	"data/TEXTURE/Feild/block_4_woodBox.png",
	"data/TEXTURE/Feild/block_5_futureBox.png",
	"data/TEXTURE/Feild/block_6_future_arrowBox.png",
	"data/TEXTURE/Feild/block_7_door_future.png",
	"data/TEXTURE/Keyboard/EnterKey.png",
	"data/TEXTURE/arrow_down.png",
};

char g_Map_Tutorial[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH + 1] =
{
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{ 3,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,8,0,5},
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

char g_Map[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_MAPCHIP_WIDTH + 1] =
{
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,0,0,0,0,0,0,0,0,0,6,6,6},
	{ 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,5,0,0,0,0,0,5,0,0,0,0,0,5,0,0,0,3,0,0,0,5,0,3,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,6,0,6,0,0,0,6},
	{ 3,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,3,0,0,0,4,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,4,0,0,5,0,5,0,0,3,0,3,0,0,0,3,0,0,0,5,0,0,0,0,0,5,0,5,0,0,0,7,7,0,0,0,0,0,0,6,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,9,0,6},
	{ 3,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,3,0,0,0,0,4,0,0,0,0,0,3,0,0,3,3,5,5,5,5,5,5,5,5,5,5,5,5,3,3,3,3,0,0,0,0,0,4,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,7,7,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,6},
	{ 3,3,5,3,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,0,0,0,4,0,0,3,0,3,3,0,3,3,3,5,5,5,5,5,5,5,5,5,3,5,5,3,3,3,3,3,0,0,0,0,4,0,5,5,5,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,3,3,0,0,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,0,0,0,8,0,6},
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

char g_Map_BOSS[FIELD_SIZE_MAPCHIP_HEIGHT + 1][FIELD_SIZE_BOSS_MAPCHIP_WIDTH + 1] =
{
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

static FIELD	g_Field;

float g_TimeCount;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
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
	g_Field.w     = FIELD_WIDTH;
	if (GetMode() == MODE_TUTORIAL) g_Field.w = FIELD_TUTORIAL_WIDTH;
	if (GetBossFlg() == TRUE) g_Field.w = FIELD_BOSS_WIDTH;
	g_Field.h     = FIELD_HEIGHT;
	g_Field.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.texNo = 0;
	g_Field.fieldNo = GetMode();
	g_Field.goalFlg = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
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
void UpdateField(void)
{
	g_TimeCount++;
	//g_Field.old_pos = g_Field.pos;	// �P�t���O�̏���ۑ�
	g_ScreenDispX = (int)((int)g_Field.pos.x % MAPCHIP_WIDTH + g_Field.pos.x - (int)g_Field.pos.x);
	g_ScreenDispY = (int)((int)g_Field.pos.y % MAPCHIP_HEIGHT + g_Field.pos.y - (int)g_Field.pos.y);

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Field pos.x:%f,pos.y:%f,\n", g_Field.pos.x, g_Field.pos.y);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
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

	player = GetPlayer();
	// sin�l�̎擾
	float sinValue = (sin(g_TimeCount * 0.1f) + 1) / 2;	// 0.1f���|���Ċɂ₩�ɕω�������
	// �}�b�v�`�b�v�̉����i�`�b�v���j
	int fieldWeightChipNum = FIELD_SIZE_MAPCHIP_WIDTH;

	// �`���[�g���A���Ȃ�Ε`���ύX
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if (g_Field.fieldNo == MODE_TUTORIAL)
	{
		fieldWeightChipNum = FIELD_SIZE_TUTORIAL_MAPCHIP_WIDTH;
	}

	// �{�X��ł���Ή����̕`���ς���
	if (GetBossFlg() == TRUE)
	{
		fieldWeightChipNum = FIELD_BOSS_WIDTH / MAPCHIP_WIDTH;
	}
	// �}�b�v�`�b�v�̕`��
	for (int y = 0; y < FIELD_SIZE_MAPCHIP_HEIGHT + 2; y++)
	{
		for (int x = 0; x < fieldWeightChipNum + 2; x++)
		{
			// �t�B�[���h�̔ԍ����擾
			int chipNum = g_Map[y][x];
			if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
			{
				chipNum = g_Map_Tutorial[y][x];
			}
			if (GetBossFlg() == TRUE)
			{
				chipNum = g_Map_BOSS[y][x];
			}
			if (chipNum == 0) continue;

			float fx = (float)((0.0f - g_Field.pos.x) + (x * MAPCHIP_WIDTH) - (player->w * 0.5));
			float fy = (float)((0.0f - g_Field.pos.y) + (float)(y * MAPCHIP_HEIGHT) - (player->h * 0.5));
			float fw = (float)MAPCHIP_WIDTH;
			float fh = (float)MAPCHIP_HEIGHT;

			float tw = 1.0f;												// �e�N�X�`���̉���
			float th = 1.0f;												// �e�N�X�`���̏c��
			float tx = 0.0f;												// �e�N�X�`���̍���X���W
			float ty = 0.0f;												// �e�N�X�`���̍���Y���W

			// 1�̎��̕`�揈��
			if (chipNum == 1)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			if (chipNum == 2)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			if (chipNum == 3)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// ���t���̖ؔ��i�ʉ߉\�j
			if (chipNum == 4)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// ��ꂽ�ؔ��i�ʉߕs�j
			if (chipNum == 5)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// �ߖ����̃u���b�N�i�ʍs�s�j
			if (chipNum == 6)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
			// �ߖ����̖��t���u���b�N�i�ʍs�j
			if (chipNum == 7)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// �ߖ����̔��i�ʍs�j
			if (chipNum == 8)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			// Enter�L�[
			if (chipNum == 9 && g_Field.goalFlg == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

				fw = 80.0f;
				fh = 100.0f;

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
			}
			// ���L�[�i�`���[�g���A���j
			if (chipNum == 10)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);
				
				// TODO�F�J�E���g���i��b���ǂ��܂Ői�񂾂��j�ɂ����fx�̈ʒu��ω�������B�������͐����𑝂₷���ȁB
				fy += 10 * sinValue;

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					fx, fy, fw, fh, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, sinValue));
			}


			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

// �w�肵�����W�ɏ�����Ă���field�f�[�^���擾����
char GetField(int y, int x)
{
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if (g_Field.fieldNo == MODE_TUTORIAL)
	{
		return g_Map_Tutorial[y][x];
	}
	if (GetBossFlg() == TRUE) return g_Map_BOSS[y][x];
	return g_Map[y][x];
}

void SetField(void)
{

}

//// �w�肵�����W��OK�ȏꏊ�����m�F����
int CheckField(int y, int x)
{
	int ans = 0;	// �Ƃ肠����OK���Z�b�g���Ă���

	// NG�ȏꏊ�H
	char data = g_Map[y][x];
	//if ((g_Field.fieldNo == MODE_TUTORIAL) && (GetTutorialFlg() == TRUE))
	if ((g_Field.fieldNo == MODE_TUTORIAL))
	{
		data = g_Map_Tutorial[y][x] ;
	}
	if (GetBossFlg() == TRUE) data = g_Map_BOSS[y][x];

	switch (data)
	{
	case 1:
		ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	case 2:
		ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	case 3:
		ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	case 5:
		ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	case 6:
		ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	case 8:
		ans = 2;	// ���̏ꏊ�͔��������̂�2���Z�b�g����
		break;
	case 10:
		ans = 3;	// ���̏ꏊ�̓`���[�g���A���|�C���g�������̂�3���Z�b�g����
		break;
	default:
		//ans = 1;	// ���̏ꏊ��NG�������̂�1���Z�b�g����
		break;
	}
	return ans;		// ���ʂ�Ԃ�
}


//=============================================================================
// Field�\���̂̐擪�A�h���X���擾
//=============================================================================
FIELD* GetField(void)
{
	return &g_Field;
}

//=============================================================================
// �v���C���[���}�b�v�`�b�v��̂ǂ��ɂ��邩�̔���i�C�x���g�Ƃ��A�A�A�j
//=============================================================================
void CheckPlayerToChipNum(float y, float x)
{
	PLAYER* player = GetPlayer();
	int playerX = (int)((x + player->w * 0.5) / MAPCHIP_WIDTH);
	int playerY = (int)(y / MAPCHIP_HEIGHT);

	// �t�B�[���h�̉����ɂ���̂����m�F����i���S�_�j
	int ans = CheckField(playerY, playerX);

	switch (ans)
	{
	// �������̔��ɂ���̂ł���΁A
	case 2:
		g_Field.goalFlg = TRUE;
		break;
	// �`���[�g���A���|�C���g1�ɂ���̂ł���΁A
	case 3:
		SetPLayerToPoint(TRUE);
		break;
	default:
		g_Field.goalFlg = FALSE;
		SetPLayerToPoint(FALSE);
		break;
	}
}
