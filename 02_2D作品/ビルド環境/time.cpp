//=============================================================================
//
// �^�C������ [time.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "time.h"
#include "sprite.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�
#define TIME_INITE					(300)	// �^�C���̏����l�i�J������999�j
#define FRAME_RATE					(60)	// �t���[����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/number16_32_original.png",
};


static bool						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Time;						// �^�C���̕b��
static int						g_TimeLittleFlg;			// �c��^�C�������Ȃ��Ƃ��̃t���O
static int						g_TimeLittleSoundFlg;		// �c��^�C�������Ȃ��Ƃ��ɉ����Ȃ炷�t���O
static float					g_LittleSoundTime;			// �c��^�C�������Ȃ��Ƃ��ɉ����Ȃ炷�t���O
static int						g_TimeCount;				// 60�t���[������1�b���擾����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTime(void)
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


	// �^�C���̏�����
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 930.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Time = TIME_INITE;
	g_TimeCount = 0;		// �^�C���J�E���g��0�ɂ���
	g_TimeLittleFlg = 0;
	g_LittleSoundTime = 0.0f;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTime(void)
{
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTime(void)
{
	g_TimeCount += 1;
	if (g_TimeLittleFlg == 1)
	{
		g_LittleSoundTime += 0.16f;
		if (g_LittleSoundTime > 1.0f)
		{
			g_LittleSoundTime = 1.0f;
		}
	}
	if (g_TimeCount == FRAME_RATE)
	{
		g_Time -= 1;
		g_TimeCount = 0;
	}
	// �^�C�����c�菭�Ȃ��Ȃ����Ƃ��t���O�𗧂Ă�B�����炷
	if (g_Time <= 60)
	{
		g_TimeLittleFlg = 1;
	}
	if (g_TimeLittleFlg == 1 && g_LittleSoundTime < 1.0f)
	{
		PlaySound(SOUND_LABEL_SE_10_TIMELOW);
	}
	// �^�C����0�ɂȂ����Ƃ�
	if (g_Time < 0)
	{
		// �Q�[���I�[�o�[��
		SetMode(MODE_GAMEOVER);
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTime(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �^�C���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x - g_w*i;	// �^�C���̕\���ʒuX
		float py = g_Pos.y;			// �^�C���̕\���ʒuY
		float pw = g_w;				// �^�C���̕\����
		float ph = g_h;				// �^�C���̕\������

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		float redColor = 1.0f;		// �����̐F��ύX����ϐ�


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		if (g_TimeLittleFlg == 1) redColor = 0.0f;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, redColor, redColor, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
	}
}


//=============================================================================
// �^�C�������Z����
// ����:add :�ǉ����鎞�ԁB�}�C�i�X���\
//=============================================================================
void AddTime(int add)
{
	g_Time += add;
	if (g_Time > TIME_MAX)
	{
		g_Time = TIME_MAX;
	}

}


int GetTime(void)
{
	return g_Time;
}


void SetTime(int time)
{
	g_Time = time;
}


