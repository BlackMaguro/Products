//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = TRUE;		// Fog�̃I���I�t
//static BOOL		g_FogEnable = FALSE;		// Fog�̃I���I�t

// ���C�g�̐ݒ�
static XMMATRIX g_LightViewMatrix;
static XMMATRIX g_LightProjectionMatrix;

// �V���h�E�i��������j
static 	XMFLOAT3 g_pos = { 0.0f, 10.0f,0.0f };
static 	XMFLOAT3 g_dir = { -2.5f, -1.0f,2.5f };
// �V���h�E�i�����܂Łj

//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Direction = XMFLOAT3( -1.0f, -1.0f, 0.2f );			// ���̌���
	g_Light[0].Diffuse   = XMFLOAT4( 1.5f, 1.5f, 1.5f, 1.0f );		// ���̐F
	//g_Light[0].Diffuse   = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );	// ���̐F
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;						// ���s����
	g_Light[0].Enable = TRUE;										// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);										// ���C�g�̐ݒ�



	// �t�H�O�̏������i���̌��ʁj
	//g_Fog.FogStart = 350.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogStart = 100.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 800.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.6f );		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// ���̏ꏊ���`�F�b�N���� shadow

}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{
	static float angle = 0.0f;
	// �t���[�����Ƃɏ�������]������i2�΂����b�ŉ񂷂��𒲐��j
	const float speed = 0.005f;    // �l��傫������Ƒ����Ȃ�
	angle += speed;
	if (angle > XM_2PI) angle -= XM_2PI;

	// �X���i�p�j���Œ�
	const float tilt = XM_PI / 12.0f;
	float cosT = cosf(tilt);
	float sinT = sinf(tilt);

	// �����x�N�g�����v�Z
	XMFLOAT3 dir = {
		-cosT * cosf(angle),  // X
		-sinT,                // Y 
		-cosT * sinf(angle)   // Z
	};

	// ���K�����ăZ�b�g
	XMVECTOR vec = XMLoadFloat3(&dir);
	vec = XMVector3Normalize(vec);
	XMStoreFloat3(&g_Light[0].Direction, vec);

	SetLight(0, &g_Light[0]);
}


//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL GetFogEnable(void)
{
	return(g_FogEnable);
}

//=============================================================================
// �V���h�E�̐ݒ�
//=============================================================================
void SetLightProjectViewMatrix()
{
	PLAYER* player = GetPlayer();
	g_dir = g_Light[0].Direction;

	XMFLOAT3 p_pos = player->pos;
	g_pos = player->pos;

	XMFLOAT3 up = { 0.0f,1.0f,0.0f };
	//XMMATRIX lightView;

	XMVECTOR  l_dir = XMVector3Normalize(XMLoadFloat3(&g_dir));
	float lightHeight = 500.0f;
	XMVECTOR player_pos = XMLoadFloat3(&p_pos);
	XMVECTOR lightPosition = player_pos - l_dir * lightHeight;

	XMMATRIX lightView = XMMatrixLookAtLH(lightPosition, XMLoadFloat3(&p_pos), XMLoadFloat3(&up));
	XMMATRIX lightProjection = XMMatrixOrthographicLH(1000.0f, 1000.0f, 0.1f, 1000.0f);
	XMMATRIX lightProjView = XMMatrixMultiply(lightView, lightProjection);
	lightProjView = XMMatrixTranspose(lightProjView);

	SetLightPVMBuffer(&lightProjView);

}

// �x�N�g�����K���֐�
XMFLOAT3 Normalize(XMFLOAT3 v)
{
	XMVECTOR vec = XMLoadFloat3(&v);
	vec = XMVector3Normalize(vec);
	XMFLOAT3 result;
	XMStoreFloat3(&result, vec);
	return result;
}

