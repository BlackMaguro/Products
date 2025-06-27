//=============================================================================
//
// �ݒ��ʏ��� [settings.cpp]
// Author : ����z��
//
//=============================================================================
#include "settings.h"
#include "input.h"
#include "fade.h"
#include "file.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO					(240)			// �^�C�g�����S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO					(120)			// �^�C�g�����S�T�C�Y�i�c�j 
#define TEXTURE_WIDTH_LOGO_SETTINGS			(200)			// ���S�T�C�Y�i���j
#define TEXTURE_HEIGHT_LOGO_SETTINGS		(60)			// ���S�T�C�Y�i�c�j
#define TEXTURE_LOGO_SETTINGS				(80)			// ���S�i�O�p�`�j�T�C�Y
#define TEXTURE_HEIGHT_LOGO_SETTINGS_X		(200)			// �e�탁�j���[���S��X���W�i�����ʒu�j 
#define TEXTURE_HEIGHT_LOGO_SETTINGS_Y		(200)			// �e�탁�j���[���S��Y���W�i�����ʒu�j 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/07_SETTINGS/bg_settings.png",
	"data/TEXTURE/07_SETTINGS/settings_logo1_title.png",
	"data/TEXTURE/07_SETTINGS/settings_logo2_BGM.png",
	"data/TEXTURE/07_SETTINGS/settings_logo3_SE.png",
	"data/TEXTURE/07_SETTINGS/settings_logo4_modoru.png",
	"data/TEXTURE/07_SETTINGS/settings_logo5_triangle.png",
	"data/TEXTURE/07_SETTINGS/settings_logo6_soundMark.png",
	"data/TEXTURE/number16_32_original.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;

static int g_settingsSelect;					// �ݒ��ʂ̑區�ڂ�I�����邽�߂̕ϐ�
static int g_settingsSelectSide;				// �ݒ��ʂ̉��ʂ≹��炷�I�������邽�߂̕ϐ�
static int g_settingsVolumeNumFront;			// �ݒ��ʂ̌����ڏ�̃{�����[���l
static int g_settingSelectBGMFlag;
static float g_volume;							// �T�E���h�̉��ʏ��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSettings(void)
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

	g_settingsSelect = 0;
	g_settingsSelectSide = 2;			// �ݒ��ʂ̉������̃Z���N�g�ϐ�
	g_settingSelectBGMFlag = 0;

	// �T�E���h�̉��ʂ�ݒ�i�����l��1.0f�j
	int ans = CheckSettingsFile();
	if (ans == 1)
	{
		// �ݒ��񂠂�
		g_volume = GetVolume();
		g_settingsVolumeNumFront = GetVolumeNum();
	}
	else
	{
		// �ݒ���Ȃ�
		g_volume = SOUND_VOLUME_MAX;							// �ݒ��ʂ̎��ۂ̉��ʕϐ��i�����l�j
		g_settingsVolumeNumFront = SETTINGS_VOLUME_FRONT_MAX;	// �ݒ��ʂ̌����ڏ�̉��ʕϐ��i�����l�j
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSettings(void)
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
void UpdateSettings(void)
{
	// �T�E���h�֌W�̏���
	{
		// �G���^�[�L�[�������ꂽ�Ƃ�
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_01_ENTER);
		}
		// �J�[�\���̍��E�L�[���������Ƃ��i�����������E�ɂ���j
		if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_RIGHT))
		{
			PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
		}
	}
	// ���ǂ�ɃJ�[�\��������AEneter�����������Ƃ�
	if (GetKeyboardTrigger(DIK_RETURN) && (g_settingsSelect == 1))
	{
		// �ݒ���̕ۑ�
		SaveSettings();
		// �X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START) && (g_settingsSelect == 1))
	{
		SaveSettings();
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B && (g_settingsSelect == 1)))
	{
		SaveSettings();
		SetFade(FADE_OUT, MODE_TITLE);
	}


	// �ݒ��ʂ̊e���ڂɈړ������Ƃ��ɐԐF���ړ�������
	int select = 1;
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_settingsSelect += select;
	}
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_settingsSelect -= select;
	}

	// �uBGM�v��Enter�����������Ƃ�������Ԃ����邽�߂̃t���O
	if ((g_settingsSelect == 0) && GetKeyboardTrigger(DIK_RETURN))
	{
		g_settingSelectBGMFlag = 1;
	}
	else if (g_settingsSelect != 0)
	{
		g_settingSelectBGMFlag = 0;
		g_settingsSelectSide = 2;		// �����l�ɖ߂�
	}

	// �ݒ��ʂ̊e���ڂőI�������Ƃ��ɐ�����ω�������
	// ���ʂ�I����
	if (g_settingSelectBGMFlag == 1)
	{
		if (GetKeyboardTrigger(DIK_LEFT))
		{
			g_settingsSelectSide -= select;
		}
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			g_settingsSelectSide += select;
		}
		if ((g_settingsSelectSide == 0) && GetKeyboardTrigger(DIK_RETURN))
		{
			g_settingsVolumeNumFront -= select;
		}
		if ((g_settingsSelectSide == 1) && GetKeyboardTrigger(DIK_RETURN))
		{
			g_settingsVolumeNumFront += select;
		}
		// �X�s�[�J�[�}�[�N�����������Ƃ��i����炷�j
		if ((g_settingsSelectSide == 2) && GetKeyboardTrigger(DIK_RETURN))
		{
			StopSound();
			PlaySound(SOUND_LABEL_BGM_05_SETTINGS);
		}
	}

	g_settingsSelect %= 2;
	g_settingsSelectSide %= 3;
	g_settingsVolumeNumFront %= 9;
	// 0�����������
	if (g_settingsSelect < 0) g_settingsSelect = 1;
	if (g_settingsSelectSide < 0) g_settingsSelectSide = 2;
	if (g_settingsVolumeNumFront < 0) g_settingsVolumeNumFront = SETTINGS_VOLUME_FRONT_MAX;
	// volume�̐ݒ�l�̕ύX
	g_volume = (float)0.125f * g_settingsVolumeNumFront;


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("volume:%f\n",g_volume);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSettings(void)
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

	// �ݒ��ʂ̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �ݒ���_�^�C�g���\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	float tx = TEXTURE_WIDTH / 2;
	float ty = 70;

	SetSpriteColor(g_VertexBuffer, tx, ty, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//==========================================
	// �ݒ��ʂ̃��j���[�����i�����J�n�j
	//==========================================
	// �I������Ă����ꍇ�A������Ԃ�����
	float selectColor = 1.0f;
	// �ݒ���_���S_BGM�\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	float tsx = TEXTURE_HEIGHT_LOGO_SETTINGS_X;
	float tsy = TEXTURE_HEIGHT_LOGO_SETTINGS_Y;
	if ((g_settingsSelect == 0))
	{
		selectColor = 0.0f;
	}
	else
	{
		selectColor = 1.0f;
	}
	SetSpriteColor(g_VertexBuffer, tsx, tsy, TEXTURE_WIDTH_LOGO_SETTINGS, TEXTURE_HEIGHT_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, selectColor, selectColor, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
	
	//==========================================
	// �ݒ��ʂ̃��j���[�����i���������j
	//==========================================


	// �ݒ���_���S_���ǂ�\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	// X���W�̓^�C�g���Ɠ����BY���W�̂ݕύX
	tsy = 450;
	if (g_settingsSelect == 1)
	{
		selectColor = 0.0f;
	}
	else
	{
		selectColor = 1.0f;
	}

	SetSpriteColor(g_VertexBuffer, tx, tsy, TEXTURE_WIDTH_LOGO_SETTINGS, TEXTURE_HEIGHT_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, selectColor, selectColor, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	//==========================================
	// �ݒ��ʂ̉��ʂ̎O�p�`�̏����i�����J�n�j
	//==========================================
	{
		// �|���S���̃e�N�X�`���J���[
		// �ʏ펞�͊D�F�ɂ��Ă����i�񊈐����j
		float selectColorVolumeR = 0.5f;
		float selectColorVolumeG = 0.5f;
		float selectColorVolumeB = 0.5f;
		float selectColorVolumeT = 0.5f;

		// �ݒ���_���S_BGM_TriangleLeft�\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// X���W�̂ݕύX
		float bgmTriangleX = 500;
		float bgmTriangleY = TEXTURE_HEIGHT_LOGO_SETTINGS_Y;


		// ������������B
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 0)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColorRotation(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT), 3.14f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �ݒ���_���S_BGM_TriangleRight�\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// X���W�̂ݕύX
		bgmTriangleX = 700;
		// ������������B���E���I������Ă�����
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 1)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �ݒ���_���S_BGM_soundMark�\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// X���W�̂ݕύX
		bgmTriangleX = 800;
		// ������������B�����}�[�N���I������Ă�����
		if ((g_settingSelectBGMFlag == 1) && (g_settingsSelect == 0))
		{
			if (g_settingsSelectSide == 2)
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 0.0f;
				selectColorVolumeB = 0.0f;
			}
			else
			{
				selectColorVolumeR = 1.0f;
				selectColorVolumeG = 1.0f;
				selectColorVolumeB = 1.0f;
			}
			selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmTriangleX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �ݒ���_BGM_�{�����[���i���o�[�\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		int number = g_settingsVolumeNumFront;
		float num = (float)(number % 10);

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = num * tw;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		// X���W�̂ݕύX
		float bgmNumberX = 600;
		if ((g_settingsSelect == 0) && (g_settingSelectBGMFlag == 1))
		{
			selectColorVolumeR = 1.0f;
			selectColorVolumeG = 1.0f;
			selectColorVolumeB = 1.0f;
			//selectColorVolumeT = 1.0f;
		}

		SetSpriteColor(g_VertexBuffer, bgmNumberX, bgmTriangleY, TEXTURE_LOGO_SETTINGS, TEXTURE_LOGO_SETTINGS, tx, ty, tw, th,
			XMFLOAT4(selectColorVolumeR, selectColorVolumeG, selectColorVolumeB, selectColorVolumeT));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}

//=============================================================================
// ���ʏ���Get�ASet
//=============================================================================
// ���ۂ̉��ʒl�̎擾
float GetVolume(void) 
{
	return g_volume;
}

// XAoudio2�̃��\�b�h(SetVolume)�Ɣ��Ȃ��悤�ɂ���
void SetSoundVolume(float volume) 
{
	g_volume = volume;
}

// �ݒ��ʂɕ\���p�̉��ʒl�̎擾
int GetVolumeNum(void)
{
	return g_settingsVolumeNumFront;
}

// XAoudio2�̃��\�b�h(SetVolume)�Ɣ��Ȃ��悤�ɂ���
void SetVolumeNum(int volumeNum)
{
	g_settingsVolumeNumFront = volumeNum;
}

// �ݒ���������񂳂�Ă����Ƃ��̏���
float CheckVolume(float volume) 
{
	if (volume > SOUND_VOLUME_MAX)	// �ő�l���傫���Ƃ�
	{
		volume = 1.0f;
	}
	else if (volume < 0)	// 0��菬�����Ƃ�
	{
		volume = 0.0f;
	}
	return volume;
}




