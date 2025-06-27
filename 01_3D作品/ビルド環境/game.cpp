//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "skysphere.h"
#include "light.h"
#include "meshobj.h"
#include "player.h"
#include "enemy.h"
#include "hp.h"
#include "mark.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "fire.h"
#include "throwMarker.h"
#include "effect.h"
#include "bullet.h"
#include "bomb.h"
#include "score.h"
#include "time.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_PauseFlg = TRUE;	// �|�[�YON/OFF

static PLAYER* g_Player = GetPlayer();

static char* g_TexturName[] = {
	"data/TEXTURE/damage_UI.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �X�J�C�X�t�B�A�̏�����
	InitSkySphere();

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);
	//InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 20, 13.0f, 13.0f);

	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �t�B�[���h�I�u�W�F�N�g�̐ݒu
	InitFieldObj();

	// ���̏�����
	InitFire();

	// �G�t�F�N�g�̂̏�����
	InitExplosion();

	// ������}�[�J�[�̏�����
	InitThrowMarker();

	// �}�[�N�i�G�l�~�[�j�̏�����
	InitMark();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);


	// �e�̏�����
	InitBullet();

	// ���e�̏�����
	InitBomb();

	// �X�R�A�̏�����
	InitScore();

	// �X�R�A�̏�����
	InitTime();

	// �X�R�A�̏�����
	InitHp();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_02_INGAME);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �p�[�e�B�N���̏I������
	UninitParticle();

	// UI�̏I������
	UninitHp();

	// �^�C���̏I������
	UninitTime();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// ���e�̏I������
	UninitBomb();

	// �G�t�F�N�g�̏I������
	UninitExplosion();

	// ���̏I������
	UninitFire();

	// ������}�[�J�[�̏I������
	UninitThrowMarker();

	// �}�[�N�̏I������
	UninitMark();

	// �t�B�[���h�I�u�W�F�N�g�̏I������
	UninitFieldObj();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �X�J�C�X�t�B�A�̏I������
	UninitSkySphere();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{

#ifdef _DEBUG
	//if (GetKeyboardTrigger(DIK_V))
	//{
	//	g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
	//	SetViewPort(g_ViewPortType_Game);
	//}

#endif

	// �Q�[�����ꎞ��~����
	if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
	{
		// SE�Đ�
		PlaySound(SOUND_LABEL_SE_00_KEYUPDOWN);
		g_PauseFlg = g_PauseFlg ? FALSE : TRUE;
	}

	if(g_PauseFlg == FALSE)
		return;

	// �X�J�C�X�t�B�A�����̍X�V
	UpdateSkySphere();

	// �n�ʏ����̍X�V
	UpdateMeshField();


	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �t�B�[���h�I�u�W�F�N�g�̍X�V����
	UpdateFieldObj();

	// ���̍X�V����
	UpdateFire();

	//�G�t�F�N�g�̍X�V����
	UpdateExplosion();

	// ������}�[�J�[�̍X�V����
	UpdateThrowMarker();

	// �e�̍X�V����
	UpdateBullet();

	// ���e�̍X�V����
	UpdateBomb();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();

	// HP�̍X�V����
	UpdateHp();

	// �}�[�N�̍X�V����
	UpdateMark();

	// �^�C���̍X�V����
	UpdateTime();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �V���h�E�}�b�v
	{
		SetLightProjectViewMatrix();
		SetShader(RENDER_MODE_SHADOW_MAP);
		SetRenderTargetView(RENDER_MODE_SHADOW_MAP);
		SetViewPort(TYPE_SHADOWMAP);

		// �t�B�[���h�I�u�W�F�N�g�̕`�揈��
		DrawFieldObj();

		// �v���C���[�̕`�揈��
		DrawPlayer();

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �e�̕`�揈��
		DrawBullet();

		// ���e�̕`�揈��
		DrawBomb();

	}
	// �ʏ�}�b�v
	{
		SetViewPort(TYPE_FULL_SCREEN);
		SetRenderTargetView(RENDER_MODE_NORMAL);
		SetShaderResource(RENDER_MODE_SHADOW_MAP);
		SetShader(RENDER_MODE_NORMAL);
	}
	// �V���h�E�}�b�v

	// �X�J�C�X�t�B�A�̕`�揈��
	DrawSkySphere();

	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �e�̕`�揈��
	DrawBullet();

	// ���e�̕`�揈��
	DrawBomb();

	// �G�t�F�N�g�̕`�揈��
	DrawExplosion();

	// �t�B�[���h�I�u�W�F�N�g�̕`�揈��
	DrawFieldObj();

	// ���̕`�揈��
	DrawFire();

	// ������}�[�J�[�̕`�揈��
	DrawThrowMarker();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();

	// �}�[�N�̕`�揈��
	DrawMark();


	//*****************************************************************************
	// 2D�ŕ`�悷����͈̂ȉ��ɋL�ڂ���
	//*****************************************************************************
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawScore();

	// �^�C���̕`�揈��
	DrawTime();

	// UI�̏���
	DrawHp();

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();			// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();		// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();		// �e�̃|�C���^�[��������
	int enemyCount = GetEnemyCount();	// �G�l�~�[�̐�

	XMFLOAT3 oldPos = player->pos;

	// �G�ƃv���C���[�Ƃ̓����蔻��
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE) continue;
		if (enemy[i].deadFlg == TRUE) continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) && player->muteki == FALSE)
		{
			enemy[i].moveFlg = FALSE;	// �G�l�~�[�̓������~�߂�
			player->muteki = TRUE;		// ���G���ԃt���O���I���ɂ���i���G��ԁj
			player->mutekiTime = 0;		// ���G���Ԃ̃��Z�b�g
			player->pos = oldPos;

			enemy[i].noticeFlg = TRUE;
			enemy[i].isRotLock = FALSE;   // ���������R��

			// �G�L�����N�^�[�̎�ނɂ���ă_���[�W��ύX����
			if (enemy[i].type == ENEMY_TYPE_KNIFEMAN)
			{
				// SE�Đ�
				PlaySound(SOUND_LABEL_SE_04_PLAYERFFROMDAMAGE);
				// �_���[�W���󂯂��Ƃ��̏���
				SetDamageFlg(TRUE);
				player->TakePlayerDamage(ENEMY_ATK);
				
				if (player->GetPlayerHP() <= 0)
				{
					SetFade(FADE_OUT, MODE_GAMEOVER);
				}
			}
		}
	}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == FALSE)
			continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == FALSE) continue;
			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size) && !enemy[j].deadFlg)
			{
				// ������������e�𖢎g�p�ɖ߂�
				bullet[i].use = FALSE;
				// �G�L�����N�^�[�͓|�����
				enemy[j].deadFlg = TRUE;
				enemy[j].dissolveTimer = 0.0f;
				enemy[j].caledTimer = 0.0f;
				// �X�R�A�𑫂�
				AddScore(SCORE_POINT);
			}
		}
	}

	// �����Ă���G�l�~�[���v�Z����
	int aliveCount = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use) aliveCount++;
	}
	// �G�l�~�[�����Z�b�g����
	SetEnemyCount(aliveCount);

	// �����Ă���G�l�~�[�����Ȃ��Ȃ�I��
	if (aliveCount <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

#ifdef _DEBUG
	//PrintDebugProc("enemyCount :%d,\n", enemyCount);
#endif
}

//=============================================================================
// �����蔻�菈���iplayer��enemy�j
// playerOldPos�F�v���C���[�̃G�l�~�[�ɓ�����O�̏���
//=============================================================================
void checkPlayerToEnemy(XMFLOAT3 playerOldPos)
{
	ENEMY* enemy = GetEnemy();			// �G�l�~�[�̃|�C���^�[��������
	PLAYER* player = GetPlayer();		// �v���C���[�̃|�C���^�[��������

	// �G�ƃv���C���[�Ƃ̓����蔻��
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE) continue;
		if (enemy[i].deadFlg == TRUE) continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size -1.0f))
		{
			player->pos = playerOldPos;
		}
	}
}

//=============================================================================
// ���f�t���O�̎擾�i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetPauseFlg(void)
{
	return g_PauseFlg;
}

void SetPauseFlg(BOOL pauseFlg)
{
	g_PauseFlg = pauseFlg;
}




