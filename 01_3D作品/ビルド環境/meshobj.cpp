//=============================================================================
//
// �������f������ [meshobj.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "collision.h"
#include "light.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "bomb.h"
#include "sound.h"
#include "meshobj.h"
#include "debugproc.h"
#include "shadow.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BUILDING			"data/MODEL/03_FILEDOBJ/field_obj_bill001.obj"				// �ǂݍ��ރ��f����
#define	MODEL_STREETLIGHT		"data/MODEL/03_FILEDOBJ/field_obj_streetLight001.obj"		// �ǂݍ��ރ��f����

static const float STREETLIGHT_WIDTH	(10.0f);			// �X���̉���
static const float STREETLIGHT_HEIGHT	(30.0f);			// �X���̏c��
static const float STREETLIGHT_DEPTH	(10.0f);			// �X���̉��s��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
std::vector<FIELDOBJ> g_FieldObjBuilding;
std::vector<FIELDOBJ> g_FieldObjLight;

int g_FieldObjBuilding_load = 0;

int g_StreetBuildingInterval = 100;

int g_StreetLightInterval = 400;
int g_StreetLightHeightNum = (int)MAP_H / g_StreetLightInterval;
int g_StreetLightWidthNum = (int)MAP_W / g_StreetLightInterval;

float g_LightChangeCount = 0.0f;
BOOL g_LightChangeFlg = FALSE;
float g_LightColor = 0.0f;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFieldObj(void)
{
	float buildingIntervalZ = 0.0f;

	//// �r���p�x�N�^���N���A���K�v�Ȃ�e�ʗ\��
	//g_FieldObjBuilding.clear();
	//g_FieldObjBuilding.reserve(MAX_BUILDING_HEIGHT_NUM * MAX_BUILDING_WIDTH_NUM);
	// �r���̏���������
	for (int i = 0; i < MAX_BUILDING_HEIGHT_NUM; i++)
	{
		for (int j = 0; j < MAX_BUILDING_WIDTH_NUM; j++)
		{
			// ���S�t�߂̓X�L�b�v
			if ((i == 0 || i == 3) && j == 3)       continue;
			if ((i == 1 || i == 2) && (j == 2 || j == 3)) continue;

			// �ݒ�l�����[�J���I�u�W�F�N�g�ɋl�߂�
			FIELDOBJ obj;
			obj.type = OBJ_TYPE_BUILDING;
			LoadModel(MODEL_BUILDING, &obj.model);
			obj.load = TRUE;

			if (i == 2) buildingIntervalZ = 200.0f;

			obj.pos = XMFLOAT3(
				-400.0f + j * 150.0f,
				-2.0f,
				400.0f + i * -200.0f - buildingIntervalZ);
			obj.rot = XMFLOAT3(0, 0, 0);
			obj.scl = XMFLOAT3(3, 3, 3);
			obj.collisionSize = XMFLOAT3 { 
				FIELD_OBJ_SIZE_BUILDING,
				FIELD_OBJ_SIZE_BUILDING,
				FIELD_OBJ_SIZE_BUILDING
			};

			GetModelDiffuse(&obj.model, &obj.diffuse[0]);

			// �x�N�^�ɒǉ�
			g_FieldObjBuilding.push_back(obj);
		}
	}
	//// �X���̏���������
	//g_FieldObjLight.clear();
	//g_FieldObjLight.reserve(MAX_STREETLIGHT_HEIGHT_NUM * MAX_STREETLIGHT_WIDTH_NUM);

	for (int i = 0; i < MAX_STREETLIGHT_HEIGHT_NUM; i++)
	{
		for (int j = 0; j < MAX_STREETLIGHT_WIDTH_NUM; j++)
		{
			FIELDOBJ obj;
			obj.type = OBJ_TYPE_STREETLIGHT;
			LoadModel(MODEL_STREETLIGHT, &obj.model);
			obj.load = TRUE;

			obj.pos = XMFLOAT3(
				-560.0f + j * 560.0f,
				0.0f,
				560.0f + i * -560.0f);
			obj.rot = XMFLOAT3(0.0f, -1.5f, 0.0f);
			obj.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			obj.collisionSize = XMFLOAT3 {
				STREETLIGHT_WIDTH,
				STREETLIGHT_HEIGHT,
				STREETLIGHT_DEPTH
			};

			// �f�B�t���[�Y��ۑ�
			GetModelDiffuse(&obj.model, &obj.diffuse[0]);

			g_FieldObjLight.push_back(obj);
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFieldObj(void)
{
	// �r���̃����[�X
	for (auto& obj : g_FieldObjBuilding) 
	{
		if (obj.load) {
			UnloadModel(&obj.model);
			obj.load = FALSE;
		}
	}
	g_FieldObjBuilding.clear();
	// �X���̃����[�X
	for (auto& obj : g_FieldObjLight) {
		if (obj.load) {
			UnloadModel(&obj.model);
			obj.load = FALSE;
		}
	}
	g_FieldObjLight.clear();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateFieldObj(void)
{
	//// ���t���[���J�E���g����
	g_LightChangeCount += 1.0f;
	int randomLight = GetRand(3,9);
	if (g_LightChangeCount > FRAME_RATE * randomLight)
	{
		g_LightChangeFlg = !g_LightChangeFlg;
		g_LightChangeCount = 0.0f;
	}

	size_t lightCount = g_FieldObjLight.size();

	// �|�C���g���C�g
	if (g_LightChangeFlg == FALSE)
	{
		for (size_t idx = 0; idx < lightCount; idx++) {
			// SetLightData �̃C���f�b�N�X�͌��� +1 �����킹��
			int lightIndex = 1 + static_cast<int>(idx);

			// VECTOR �� idx �Ԗڂ����o��
			const FIELDOBJ& obj = g_FieldObjLight[idx];

			// �J������|�C���g���C�g�͓�������
			LIGHT* light = GetLightData(1);
			XMFLOAT3  pos = obj.pos;
			pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1, 1, 1, 1);
			light->Ambient = XMFLOAT4(1, 1, 1, 1);
			// �t���O�ɉ����Ď�ނ����؂�ւ�
			light->Type = (g_LightChangeFlg ? LIGHT_TYPE_NONE : LIGHT_TYPE_POINT);
			light->Enable = TRUE;

			SetLightData(lightIndex, light);
		}
	}

#ifdef _DEBUG

#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFieldObj(void)
{
	XMMATRIX mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �r���`��
	for (auto& builds : g_FieldObjBuilding)
	{
		if (!builds.load) continue;

		// ���[���h�s�����C�ɑg�ݗ���
		mtxWorld = XMMatrixScaling(builds.scl.x, builds.scl.y, builds.scl.z)
			* XMMatrixRotationRollPitchYaw(builds.rot.x, builds.rot.y + XM_PI, builds.rot.z)
			* XMMatrixTranslation(builds.pos.x, builds.pos.y, builds.pos.z);

		SetWorldMatrix(&mtxWorld);
		XMStoreFloat4x4(&builds.mtxWorld, mtxWorld);

		// �`��
		DrawModel(&builds.model);
	}

	// �X���`��
	for (auto& lightPole : g_FieldObjLight)
	{
		if (!lightPole.load) continue;

		mtxWorld = XMMatrixScaling(lightPole.scl.x, lightPole.scl.y, lightPole.scl.z)
			* XMMatrixRotationRollPitchYaw(lightPole.rot.x, lightPole.rot.y + XM_PI, lightPole.rot.z)
			* XMMatrixTranslation(lightPole.pos.x, lightPole.pos.y, lightPole.pos.z);

		SetWorldMatrix(&mtxWorld);
		XMStoreFloat4x4(&lightPole.mtxWorld, mtxWorld);

		DrawModel(&lightPole.model);
	}
	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

// �v���C���[�ƃt�B�[���h�I�u�W�F�N�g�Ƃ̓����蔻��
void checkPlayerFieldObj(const XMFLOAT3& playerOldPos)
{
	PLAYER* player = GetPlayer();
	// �����Ƃ̓����蔻��
	for (auto& obj : g_FieldObjBuilding) 
	{
		
		if (!obj.load) continue;

		if (CollisionBB_3D(
			player->pos, player->size, player->size, player->size,
			obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
		{
			// �Փ˂�����ЂƂ܂����̈ʒu�ɖ߂��ă��^�[��
			player->pos = playerOldPos;
			return;
		}
	}
	// �X���Ƃ̓����蔻��
	for (auto& obj : g_FieldObjLight) 
	{
		if (!obj.load) continue;

		if (CollisionBB_3D(
			player->pos, player->size, player->size, player->size,
			obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
		{
			player->pos = playerOldPos;
			return;
		}
	}
}

void checkBulletFieldObj()
{
	BULLET* bullets = GetBullet();

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (!bullets[i].use) continue;

		// �����ւ̓����蔻��
		for (auto& obj : g_FieldObjBuilding)
		{
			if (!obj.load) continue;

			if (CollisionBB_3D(
				bullets[i].pos,
				bullets[i].size, bullets[i].size, bullets[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				bullets[i].use = FALSE;
				goto NEXT_BULLET;
			}
		}

		// �X���ւ̓����蔻��
		for (auto& obj : g_FieldObjLight)
		{
			if (!obj.load) continue;
			if (CollisionBB_3D(
				bullets[i].pos,
				bullets[i].size, bullets[i].size, bullets[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				bullets[i].use = FALSE;
				break;
			}
		}

	NEXT_BULLET:
		;		// �󕶁i�������Ȃ��j
	}
}

void checkBombFieldObj()
{
	BOMB* bombs = GetBomb();

	for (int i = 0; i < MAX_BOMB; i++)
	{
		if (!bombs[i].use || i >= MAX_BOMB) continue;

		// �����ւ̓����蔻��
		for (auto& obj : g_FieldObjBuilding)
		{
			if (!obj.load) continue;

			if (CollisionBB_3D(
				bombs[i].pos,
				bombs[i].size, bombs[i].size, bombs[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				Explode(i);
				bombs[i].use = FALSE;
				goto NEXT_BOMB;
			}
		}

		// �X���ւ̓����蔻��
		for (auto& obj : g_FieldObjLight)
		{
			if (!obj.load) continue;
			if (CollisionBB_3D(
				bombs[i].pos,
				bombs[i].size, bombs[i].size, bombs[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				Explode(i);
				bombs[i].use = FALSE;
				break;
			}
		}

	NEXT_BOMB:
		;		// �󕶁i�������Ȃ��j
	}
}


BOOL checkEnemyFieldObj(const XMFLOAT3& enemyOldPos)
{
	ENEMY* enemies = GetEnemy();

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!enemies[i].use)   continue;

		// �����Ƃ̓����蔻��
		for (auto& obj : g_FieldObjBuilding)
		{
			if (!obj.load) continue;

			if (CollisionBB_3D(
				enemies[i].pos, enemies[i].size, enemies[i].size, enemies[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				// �Փ˂����猳�̈ʒu�ɖ߂��� true
				enemies[i].pos = enemyOldPos;
				return TRUE;
			}
		}

		// �X���Ƃ̓����蔻��
		for (auto& obj : g_FieldObjLight)
		{
			if (!obj.load) continue;

			if (CollisionBB_3D(
				enemies[i].pos, enemies[i].size, enemies[i].size, enemies[i].size,
				obj.pos, obj.collisionSize.x, obj.collisionSize.y, obj.collisionSize.z))
			{
				enemies[i].pos = enemyOldPos;
				return TRUE;
			}
		}
	}

	return FALSE;
}


//=============================================================================
// �t�B�[���h�I�u�W�F�N�g�i�r���j�̎擾
// 	vector �̐擪�v�f�ւ̃|�C���^��Ԃ�
//=============================================================================
const std::vector<FIELDOBJ>& GetFieldObjBuildings()
{
	return g_FieldObjBuilding;
}
//=============================================================================
// �t�B�[���h�I�u�W�F�N�g�i���C�g�j�̎擾
//=============================================================================
const std::vector<FIELDOBJ>& GetFieldObjLights()
{
	return g_FieldObjLight;
}

