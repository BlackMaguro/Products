//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "game.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const float TEXTURE_X	  (125.0f);		// �X�R�A�e�N�X�`����X���W
static const float TEXTURE_Y	  (75.0f);		// �X�R�A�e�N�X�`����Y���W
static const float TEXTURE_WIDTH  (20.0f);		// �e�N�X�`���T�C�Y�i���j
static const float TEXTURE_HEIGHT (45.0f);		// �e�X�N�`���T�C�Y�i�����j

static const float ICON_DOLL_X		(0.0f);			// �A�C�R���i�h���j��X���W
static const float ICON_DOLL_Y		(50.0f);		// �A�C�R���i�h���j��Y���W
static const float ICON_DOLL_WIDTH	(60.0f);		// �A�C�R���i�h���j�̃T�C�Y�i���j
static const float ICON_DOLL_HEIGHT	(50.0f);		// �A�C�R���i�h���j�̃T�C�Y�i�����j

static const float TEXTURE_SCORE_PLUS_X		 (170.0f);	// �v���X�}�[�N�e�N�X�`����X���W
static const float TEXTURE_SCORE_PLUS_Y		 (75.0f);	// �v���X�}�[�N�e�N�X�`����Y���W
static const float TEXTURE_SCORE_PLUS_WIDTH	 (30.0f);	// �v���X�}�[�N�e�N�X�`���̃T�C�Y�i���j
static const float TEXTURE_SCORE_PLUS_HEIGHT (30.0f);	// �v���X�}�[�N�e�N�X�`���̃T�C�Y�i�����j

static const float  TEXTURE_GAINSCORE_X		(240.0f);	// �X�R�A�e�N�X�`����X���W
static const float  TEXTURE_GAINSCORE_Y		(75.0f);	// �X�R�A�e�N�X�`����Y���W

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
namespace
{
	enum 
	{
		TEXTURE_SCORE_NUMBER = 0,		// �X�R�A�̐���
		TEXTURE_SCORE_DOLL,				// �X�R�A�̃h���}�[�N
		TEXTURE_SCORE_DOLL_PLUS,		// ���Z�����X�R�A
		
		TEXTURE_SCORE_MAX				// �X�R�A�e�N�X�`���̍ő吔
	};
}
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_SCORE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_SCORE_MAX] = {
	"data/TEXTURE/number16_32_original.png",			// �����̃e�N�X�`��
	PATH_TEXTURE_INGAME "01_ICON/icon_00_doll.png",		// �h���̃A�C�R��
	PATH_TEXTURE_INGAME "02_UI/doll_score_plus.png",		// ���Z�X�R�A
};


static BOOL				g_Use;							// TRUE:�g���Ă���  FALSE:���g�p
static float			g_w, g_h;						// ���ƍ���
static XMFLOAT3			g_Pos;							// �|���S���̍��W

static XMFLOAT3			g_GainScorePos;					// �|���S���̍��W
static int				g_CurrentScore;					// ���݂̃X�R�A
static int				g_GainValue;					// ����l�������X�R�A
static int				g_DisplayedScore;				// ��ʂɕ\�����̃X�R�A
static const int		SCORE_STEP = 1;				    // 1�t���[�������葝������_��

static BOOL				g_ShowGainScoreFlg = FALSE;		// �|�b�v�A�b�v�\�����t���O
static float			g_TransTime = 1.0f;				// �����ɂȂ�܂ł̎���

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_SCORE_MAX; i++)
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


	// �X�R�A�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { TEXTURE_X, TEXTURE_Y, 0.0f };
	g_GainScorePos = { TEXTURE_GAINSCORE_X, TEXTURE_GAINSCORE_Y, 0.0f };
	g_CurrentScore = 0;
	g_DisplayedScore = 0;
	g_GainValue = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_SCORE_MAX; i++)
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
void UpdateScore(void)
{
	float transTimeDec = 0.01f;
	// �X�R�A���ő�l�ȉ��Ȃ�t���[���P�ʂő��₷
	if (g_DisplayedScore < g_CurrentScore)
	{
		int restNum = g_CurrentScore - g_DisplayedScore;
		int step = (restNum < SCORE_STEP) ? restNum : SCORE_STEP;
		g_DisplayedScore += step;
		g_TransTime -= transTimeDec;
		if(SCORE_MAX < g_DisplayedScore) g_DisplayedScore = g_CurrentScore;
	}
	if(g_TransTime < 0.0f)
	{
		g_TransTime = 0.0f;
		g_ShowGainScoreFlg = FALSE;
	}
#ifdef _DEBUG	// �f�o�b�O����\������

	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_NUMBER]);

	// ��������������
	int number = g_DisplayedScore;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x - g_w*i;	// �X�R�A�̕\���ʒuX
		float py = g_Pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_w;				// �X�R�A�̕\����
		float ph = g_h;				// �X�R�A�̕\������

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
	}
	
	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W
	// �h���i���j�̃A�C�R���\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_DOLL]);
		material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = ICON_DOLL_X;			// �\���ʒuX
		float py = ICON_DOLL_Y;			// �\���ʒuY
		float pw = ICON_DOLL_WIDTH;		// �\����
		float ph = ICON_DOLL_HEIGHT;	// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	// ���Z�X�R�A�̕\��
	if(g_ShowGainScoreFlg)
	{
		// �v���X�}�[�N�e�N�X�`��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_DOLL_PLUS]);
		material.Diffuse = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			TEXTURE_SCORE_PLUS_X, TEXTURE_SCORE_PLUS_Y, 
			TEXTURE_SCORE_PLUS_WIDTH, TEXTURE_SCORE_PLUS_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f,1.0f,1.0f, g_TransTime));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �l���������z�̕`��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SCORE_NUMBER]);

		int value = g_GainValue;
		for (int i = 0; i < SCORE_GAIN_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(value % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_GainScorePos.x - g_w * i;	// �X�R�A�̕\���ʒuX
			float py = g_GainScorePos.y;			// �X�R�A�̕\���ʒuY
			float pw = g_w;				// �X�R�A�̕\����
			float ph = g_h;				// �X�R�A�̕\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_TransTime));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			value /= 10;
		}
	}

}


//=============================================================================
// �X�R�A�����Z����
// @param add :�ǉ�����_��
// note �F�@�����̓}�C�i�X���󂯎��邪�\����̓v���X�ɂȂ��Ă��܂�
//=============================================================================
void AddScore(int add)
{
	PlaySound(SOUND_LABEL_SE_07_GETMONEY);

	// �܂��|�b�v�A�b�v�J�n����i�c�莞�Ԃ� 0.9�ȏ�j�Ȃ�ݐ�
	if (g_ShowGainScoreFlg && g_TransTime > 0.9f)
	{
		g_GainValue += add;
	}
	else
	{
		g_GainValue = add;
		g_ShowGainScoreFlg = TRUE;
	}
	g_TransTime = 1.0f;

	// ���X�R�A�͏�ɉ��Z
	g_CurrentScore += add;
	if (g_CurrentScore > SCORE_MAX)
		g_CurrentScore = SCORE_MAX;

}

int GetScore(void)
{
	return g_CurrentScore;
}

