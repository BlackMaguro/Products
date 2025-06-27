//=============================================================================
//
// �t�B�[���h�I�u�W�F�N�g���f������ [enemy.h]
// Author : ����z��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int MAX_FIELD_OBJ_BUILDING(25);	// �t�B�[���h�I�u�W�F�N�g�̐��i�r���j
static const int MAX_FIELD_OBJ_STREETLIGHT(36);	// �t�B�[���h�I�u�W�F�N�g�̐��i�X���j
static const int MAX_FIELD_OBJ(MAX_FIELD_OBJ_BUILDING + MAX_FIELD_OBJ_STREETLIGHT); // �t�B�[���h�I�u�W�F�N�g�̑���

static const int  BUILDING_INTERVAL			(200);			// �t�B�[���h�I�u�W�F�N�g�̐��i�r���j
static const int  MAX_BUILDING_HEIGHT_NUM	(4);			// �t�B�[���h�I�u�W�F�N�g�̐��i�r���j
static const int  MAX_BUILDING_WIDTH_NUM	(6);			// �t�B�[���h�I�u�W�F�N�g�̐��i�r���j

static const int  MAX_STREETLIGHT_HEIGHT_NUM	(3);		// �t�B�[���h�I�u�W�F�N�g�̐��i���C�g�j
static const int  MAX_STREETLIGHT_WIDTH_NUM		(3);		// �t�B�[���h�I�u�W�F�N�g�̐��i���C�g�j

static const float	FIELD_OBJ_SIZE_BUILDING		(90.0f);	// �����蔻��̑傫���i�r���j
static const float	FIELD_OBJ_SIZE_STREETLIGHT	(10.0f);	// �����蔻��̑傫���i�X���j

//*****************************************************************************
// �񋓌^��`
//*****************************************************************************
// @brief �I�u�W�F�N�g�̎��
enum OBJ_TYPE
{
	OBJ_TYPE_BUILDING = 0,
	OBJ_TYPE_STREETLIGHT,

	MAX_OBJ_TYPE,
};

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class FIELDOBJ
{
public:
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	OBJ_TYPE			type;				// �I�u�W�F�N�g�̎��
	XMFLOAT3			collisionSize;		// �����蔻��̑傫��

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFieldObj(void);
void UninitFieldObj(void);
void UpdateFieldObj(void);
void DrawFieldObj(void);

const std::vector<FIELDOBJ>& GetFieldObjBuildings(void);
const std::vector<FIELDOBJ>& GetFieldObjLights(void);
void checkPlayerFieldObj(const XMFLOAT3& playerOldPos);
BOOL checkEnemyFieldObj(const XMFLOAT3& enemyOldPos);
void checkBulletFieldObj(void);
void checkBombFieldObj(void);