//=============================================================================
//
// UI���� [hp.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "fade.h"
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "sprite.h"
#include "hp.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// player��HP�e�N�X�`��
static const float PLAYER_HP_X				  (20.0f);		// �v���C���[HP��X���W
static const float PLAYER_HP_Y				  (450.0f);		// �v���C���[HP��X���W
static const float PLAYER_HP_WIDTH			  (300.0f);		// �v���C���[HP�̕�
static const float PLAYER_HP_HEIGHT			  (40.0f);		// �v���C���[HP�̏c
// �G�l�~�[�̕����e�N�X�`���i���Z�́j
static const float ENEMY_COUNT_TEXTURE_X	  (80.0f);		// �G�l�~�[���e�N�X�`����X���W
static const float ENEMY_COUNT_TEXTURE_Y	  (450.0f);		// �G�l�~�[���e�N�X�`����X���W
static const float ENEMY_COUNT_TEXTURE_WIDTH  (300.0f);		// �G�l�~�[�������e�N�X�`���̕�
static const float ENEMY_COUNT_TEXTURE_HEIGHT (130.0f);		// �G�l�~�[�������e�N�X�`���̏c
// �G�l�~�[�̎c�萔�e�N�X�`��
static const float ENEMY_COUNT_NUM_X		(80.0f);		// �G�l�~�[���e�N�X�`����X���W
static const float ENEMY_COUNT_NUM_Y		(450.0f);		// �G�l�~�[���e�N�X�`����X���W
static const float ENEMY_COUNT_NUM_WIDTH	(35.0f);		// �G�l�~�[���e�N�X�`���̕�
static const float ENEMY_COUNT_NUM_HEIGHT	(80.0f);		// �G�l�~�[���e�N�X�`���̏c
// �_���[�W�G�t�F�N�g
static const float DAMAGE_EFFECT_WIDTH		(960.0f);			// �_���[�W�G�t�F�N�g�̃t�B���^�[
static const float DAMAGE_EFFECT_HEIGHT		(540.0f);		// �_���[�W�G�t�F�N�g�̃t�B���^�[
// P�L�[
static const float PKEY_TEXTURE_X			(400.0f);	// P�L�[�e�N�X�`����X���W
static const float PKEY_TEXTURE_Y			(450.0f);	// P�L�[�e�N�X�`����X���W
static const float PKEY_TEXTURE_WIDTH		(200.0f);	// P�L�[�e�N�X�`���̕�
static const float PKEY_TEXTURE_HEIGHT		(80.0f);	// P�L�[�e�N�X�`���̏c
// ��������̕\��
static const float OPERATION_TEXTURE_X		(10.0f);	// ��������e�N�X�`����X���W
static const float OPERATION_TEXTURE_Y		(110.0f);	// ��������e�N�X�`����X���W
static const float OPERATION_TEXTURE_WIDTH	(200.0f);	// ��������e�N�X�`���̕�
static const float OPERATION_TEXTURE_HEIGHT (200.0f);	// ��������e�N�X�`���̏c
// �A�C�R���̕\���֌W
static const float ICON_SIZE		  (60.0f);				// �A�C�R���̑傫���i�c������j
static const float ICON_GUN_X		  (20.0f);				// �A�C�R���i�e�j��X���W
static const float ICON_GUN_Y		  (370.0f);				// �A�C�R���i�e�j��Y���W
static const float ICON_WOODENSWORD_X (90.0f);				// �A�C�R���i�ؓ��j��X���W
static const float ICON_WOODENSWORD_Y (370.0f);				// �A�C�R���i�ؓ��j��Y���W
static const float ICON_BOMB_X		  (160.0f);				// �A�C�R���i���e�j��X���W
static const float ICON_BOMB_Y		  (370.0f);				// �A�C�R���i���e�j��Y���W
static const float ICON_BG_BLACK_X	  (SCREEN_CENTER_X);	// �A�C�R���i���w�i�j��X���W
static const float ICON_BG_BLACK_Y	  (10.0f);				// �A�C�R���i���w�i�j��Y���W
static const float ICON_EYE_X		  (SCREEN_CENTER_X);	// �A�C�R���i�ځj��X���W
static const float ICON_EYE_Y		  (10.0f);				// �A�C�R���i�ځj��Y���W


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[UI_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TextureName[] = {
	"data/TEXTURE/bar_white.png",								// 0:�̗̓Q�[�W
	"data/TEXTURE/damage_UI.png",								// 1:�_���[�W���̐Ԃ��w�i
	"data/TEXTURE/04_INGAME/01_ICON/icon_01_gun.png",			// 2:�A�C�R��_�e
	"data/TEXTURE/04_INGAME/02_UI/EnemyCount.png",				// 3:�G�l�~�[�̎c�萔�e�N�X�`��
	"data/TEXTURE/number16_32_original.png",					// 4:�����̃e�N�X�`��
	"data/TEXTURE/04_INGAME/01_ICON/icon_00_bg_BLAKC.png",		// 5:�A�C�R��_�w�i_��
	"data/TEXTURE/04_INGAME/01_ICON/icon_02_eye.png",			// 6:�A�C�R��_��
	"data/TEXTURE/04_INGAME/02_UI/01_PKey.png",					// 7:UI_P�L�[
	"data/TEXTURE/04_INGAME/02_UI/02_Operation.png",			// 8:UI_�������
	"data/TEXTURE/04_INGAME/01_ICON/icon_03_woodenSword.png",	// 9:�A�C�R��_�ؓ�
	"data/TEXTURE/04_INGAME/01_ICON/icon_04_bomb.png",			// 10:�A�C�R��_���e
};


static bool						g_Use;						// true:�g���Ă���  false:���g�p
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static UI						g_UI;						// �|���S���f�[�^
static int						g_TexNo;					// �e�N�X�`���ԍ�
static BOOL						g_DamageFlg = FALSE;		// �e�N�X�`���ԍ�
static float g_DamageTimeCount = 0.0f;
static float g_DamageEffectTrans = 1.0f;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitHp(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < UI_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
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


	// �̗͂̏�����
	g_Use   = true;
	g_Pos   = { 920.0f, 20.0f, 0.0f };
	g_DamageFlg = FALSE;				// �_���[�W�t���O���I�t
	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHp(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < UI_TEXTURE_MAX; i++)
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
void UpdateHp(void)
{
	g_DamageTimeCount += 1.0f;
	if (g_DamageFlg == TRUE)
	{
		g_DamageEffectTrans -= 0.01f;
		if (g_DamageEffectTrans < 0.0f)
		{
			g_DamageFlg = FALSE;
			g_DamageEffectTrans = 1.0f;
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("EnemyCount :%d", g_EnemyCount);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHp(void)
{
	PLAYER* player = GetPlayer();
	float damageEffectTime = 0.0f;

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

	// �e�N�X�`��_����
	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	//=============================================================================
	// �v���C���[HP
	//=============================================================================
	{
		// �v���C���[��HP�Q�[�W�i�w�i�̍��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_HP_GAUGE]);
		material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = PLAYER_HP_X;			// �Q�[�W�̕\���ʒuX
		float py = PLAYER_HP_Y;			// �Q�[�W�̕\���ʒuY
		float pw = PLAYER_HP_WIDTH;		// �Q�[�W�̕\����
		float ph = PLAYER_HP_HEIGHT;	// �Q�[�W�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �v���C���[��HP�ɉ����Ē�����ύX�i�ԐF�̃o�[�j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_HP_GAUGE]);
		material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		pw = pw * ((float)player->GetPlayerHP() / PLAYER_MAX_HP);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	//=============================================================================
	// �A�C�R��
	//=============================================================================
	// �A�C�R���i�e�j�\��
	{
		float pw = ICON_SIZE;		// �\����
		float ph = ICON_SIZE;		// �\������
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_GUN]);
		
		if (player->weaponType == PLAYER_WEAPON_GUN)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = ICON_GUN_X;		// �\���ʒuX
		float py = ICON_GUN_Y;		// �\���ʒuY


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	// �A�C�R���i�ؓ��j�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_WOODSWORD]);
		if (player->weaponType == PLAYER_WEAPON_WOODENSWORD)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = ICON_WOODENSWORD_X;		// �\���ʒuX
		float py = ICON_WOODENSWORD_Y;		// �\���ʒuY
		float pw = ICON_SIZE;				// �\����
		float ph = ICON_SIZE;				// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	// �A�C�R���i���e�j�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_BOMB]);
		if (player->weaponType == PLAYER_WEAPON_BOMB)
		{
			material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
		}
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = ICON_BOMB_X;		// �\���ʒuX
		float py = ICON_BOMB_Y;		// �\���ʒuY
		float pw = ICON_SIZE;		// �\����
		float ph = ICON_SIZE;		// �\������

		// 1���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	// �A�C�R��_BG�̔w�i
	if (IsAnyEnemyNotice())
	{
		// �A�C�R��_BG�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_BG_BLACK]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = ICON_BG_BLACK_X;	// �\���ʒuX
		float py = ICON_BG_BLACK_Y;	// �\���ʒuY
		float pw = ICON_SIZE;		// �\����
		float ph = ICON_SIZE;		// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �A�C�R��_�ځi�C�Â��Ă���Ƃ��͕\������j
		{
			// �A�C�R��_�ڂ̕\��
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ICON_EYE]);
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			SetMaterial(material);

			//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
			float px = ICON_EYE_X;		// �\���ʒuX
			float py = ICON_EYE_Y;		// �\���ʒuY
			float pw = ICON_SIZE;		// �\����
			float ph = ICON_SIZE;		// �\������

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}
	//=============================================================================
	// P�L�[�Ƒ������
	//=============================================================================
	// P�L�[�̕\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_P_KEY]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = PKEY_TEXTURE_X;			// P�L�[�̕\���ʒuX
		float py = PKEY_TEXTURE_Y;			// P�L�[�̕\���ʒuY
		float pw = PKEY_TEXTURE_WIDTH;		// P�L�[�̕\����
		float ph = PKEY_TEXTURE_HEIGHT;		// P�L�[�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
	// ��������̕\��
	if(!GetPauseFlg())
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_INSTRUCTION]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�Q�[�W�̈ʒu��e�N�X�`�����W�𔽉f
		float px = OPERATION_TEXTURE_X;			// ��������̕\���ʒuX
		float py = OPERATION_TEXTURE_Y;			// ��������̕\���ʒuY
		float pw = OPERATION_TEXTURE_WIDTH;		// ��������̕\����
		float ph = OPERATION_TEXTURE_HEIGHT;	// ��������̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}


	//=============================================================================
	// �G�l�~�[���i�c��N�́j
	//=============================================================================
	// �c��G�l�~�[���e�N�X�`���̕\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_ENEMY_COUNT]);
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�e�N�X�`���̈ʒu��e�N�X�`�����W�𔽉f
		float px = SCREEN_WIDTH - ENEMY_COUNT_TEXTURE_WIDTH;	// �c��G�l�~�[���e�N�X�`���̕\���ʒuX
		float py = SCREEN_HEIGHT - ENEMY_COUNT_TEXTURE_HEIGHT;	// �c��G�l�~�[���e�N�X�`���̕\���ʒuY
		float pw = ENEMY_COUNT_TEXTURE_WIDTH;					// �c��G�l�~�[���e�N�X�`���̉���
		float ph = ENEMY_COUNT_TEXTURE_HEIGHT;					// �c��G�l�~�[���e�N�X�`���̏c��

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	//=============================================================================
	// �_���[�W�G�t�F�N�g
	//=============================================================================
	if (g_DamageFlg)
	{
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_DamageEffectTrans);
		SetMaterial(material);
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_DAMAGE_RED_BG]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 0.0f;					// �Q�[�W�̕\���ʒuX
		float py = 0.0f;					// �Q�[�W�̕\���ʒuY
		float pw = DAMAGE_EFFECT_WIDTH;		// �Q�[�W�̕\����
		float ph = DAMAGE_EFFECT_HEIGHT;	// �Q�[�W�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	// �G�l�~�[���̕\���i�����j
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_NUMBER]);

	// ��������������
	int enemyNum = GetEnemyCount();
	int enemyNumDigit = enemyNum / 10 + 1;
	for (int i = 0; i < enemyNumDigit; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(enemyNum % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH - ENEMY_COUNT_NUM_WIDTH * i - 100.0f;	// �c��G�l�~�[���̕\���ʒuX
		float py = SCREEN_HEIGHT - ENEMY_COUNT_NUM_HEIGHT + 10.0f;			// �c��G�l�~�[���̕\���ʒuY
		float pw = ENEMY_COUNT_NUM_WIDTH;							// �c��G�l�~�[���̉���
		float ph = ENEMY_COUNT_NUM_HEIGHT;							// �c��G�l�~�[���̏c��

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
		enemyNum /= 10;
	}

}

//=============================================================================
// �_���[�W���󂯂����̔��f�i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetDamageFlg()
{
	return g_DamageFlg;
}

void SetDamageFlg(BOOL damageFlg)
{
	g_DamageFlg = damageFlg;
}




