//=============================================================================
//
// �G�l�~�[�֌W�̏��� [enemy.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "character.h"
#include "behaviorTree.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int	 MAX_ENEMY				(11);			// �G�l�~�[�̐�
static const int	 ENEMY_MAX_HP			(30);			// �G�l�~�[��HP
static const int	 ENEMY_ATK				(15);			// �G�l�~�[�̍U����
static const float	 ENEMY_VIGILANCE_LENGTH	(250.0f);		// �G�l�~�[���x�����鋗��
static const float	 ENEMY_NOTICE_LENGTH	(200.0f);		// �G�l�~�[���������鋗��
static const float	 ENEMY_SIZE				(5.0f);			// �����蔻��̑傫��
static const float   ENEMY_VIGILANCE_LENGTH_SQ = ENEMY_VIGILANCE_LENGTH * ENEMY_VIGILANCE_LENGTH; // �x�����鋗������

// @brief �G�l�~�[�̎��
enum EnemyType
{
	ENEMY_TYPE_KNIFEMAN = 0,		// �i�C�t�ōU������G�l�~�[
	ENEMY_TYPE_BULLETMAN,			// �e�ōU�����Ă���G�l�~�[�iTODO�F�����\��j
	
	ENEMY_TYPE_MAX,					// �G�l�~�[�̍ő���
};

// @brief �G�l�~�[�̏��
enum EnemyState
{
	STATE_IDLE = 0,		// �ҋ@���
	STATE_WANDER,		// �������
	STATE_CHASE,		// �ǂ�������

	STATE_MAX,			// ��Ԃ̍ő吔
};

// @brief �G�l�~�[�̃p�[�c
enum EnemyPartsIndex
{
	// �G�l�~�[�̃p�[�c�ibody��g_Player�Őݒ�j
	ENEMY_HEAD = 0,		// ���ihead�j
	ENEMY_ARM_L,		// ���r�iarm_Left�j
	ENEMY_ARM_R,		// �E�r�iarm_Right�j
	ENEMY_LEG_L,		// �����ilef_Left�j
	ENEMY_LEG_R,		// �E���ilef_Right�j
	// ����
	ENEMY_KNIFE,		// �i�C�t�iKnife�j		

	ENEMY_PARTS_COUNT	// �p�[�c�̍ő吔
};
// @brief �G�l�~�[�̓����e�[�u��
enum EnemyMoveTableIndex
{
	// �ҋ@
	ENEMY_MT_HEAD = 0,       // move_tbl_enemy_head
	ENEMY_MT_ARM_L,          // move_tbl_l_enemy_arm
	ENEMY_MT_ARM_R,          // move_tbl_r_enemy_arm
	ENEMY_MT_LEG_L,          // move_tbl_l_enemy_leg
	ENEMY_MT_LEG_R,          // move_tbl_r_enemy_leg
	// �U���i�i�C�t�j
	ENEMY_MT_ATTACK_WEAPON_GUN,	// move_tbl_weapon_knife

	ENEMY_MT_MAX			 // �e�[�u���̍ő吔
};

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class ENEMY : public CHARACTER
{
public:

	// �t���O
	BOOL				deadFlg;			// ���S���邩���m�F����t���O
	BOOL				moveFlg;			// �������m�F����t���O
	BOOL				noticeFlg;			// �C�Â��Ă��邩�̃t���O
	BOOL				prevNoticeFlg;		// 1�O�ɋC�Â��Ă��邩�̃t���O
	BOOL				isRotLock;			// �������Œ肷�邽�߂̃t���O
	BOOL				isIdleInitialized;  // Idle�̏������t���O
	BOOL				isWanderInitialized;// Wander�̏������t���O

	// �`��p�f�[�^
	XMFLOAT4			D_Diffuse;			// ���f���̃f�B�]���u
	float				dissolveTimer;		// �f�B�]���u�̎��ԁi�����j
	float				caledTimer;			// �f�B�]���u�̎��ԁi�o���j
	XMFLOAT3			flyDir;				// ��U�����x�N�g��
	float				flySpeed;			// ��U���x
	XMFLOAT3			rotVel;				// ��]���x�i���W�A��/�b�j

	// ����
	XMFLOAT3			movePoint;				// �ړ���̃|�C���g
	int					movePointChangeCount;	// �������ς���J�E���g
	float				moveTimer;				// �G�l�~�[�̌����ύX�^�C�}�[

	// �X�e�[�^�X
	int					hp;					// �G�l�~�[�̗̑�
	int					atk;				// �G�l�~�[�̍U����
	int					type;				// �G�l�~�[�̎�ށi0:�i�C�t�ōU������A1:�e�ōU������l�j
	int					state;				// �G�l�~�[�̏��
	
	// �A�h���X
	BTNode				*behaviorTree;		// �r�w�C�r�A�c���[�̃A�h���X
	ENEMY				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

};
// @brief �G�l�~�[�p�[�c�̃A�j���[�V�����̍\����
struct EnemyPartDesc 
{
	int							partIndex;			// ENEMY_HEAD, ENEMY_ARM_L�Ȃ�
	const char					*modelId;			// MODEL_ENEMY_HEAD, MODEL_ENEMY_LARM�Ȃ�
	INTERPOLATION_DATA const	*animTbl;			// �A�j���[�V�����e�[�u��
	size_t						animTblSize;		// sizeof(...) / sizeof(...)
	int							parentPart;			// �e�p�[�c�������ENEMY_ARM_L�Ȃ�
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static const int	ENEMY_PARTS_MAX(6);					// �G�l�~�[�̃p�[�c�̐�
extern ENEMY g_EnemyParts[MAX_ENEMY][ENEMY_PARTS_MAX];	// �v���C���[�̃p�[�c�p
extern BOOL	 g_IsSwordDebrisActive;						// �v���C���[���ؓ��ōU���������Ƃ������t���O

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
/**
 * @brief �G�l�~�[�̃��\�[�X������������B
 * @note InitGame() �̒��ň�x�����Ăяo���B
 * @return S_OK ���� �^ E_FAIL ���������s
 */
HRESULT InitEnemy(void);

/**
 * @brief �G�l�~�[�̃��\�[�X���������B
 * @note UninitGame() �̒��ň�x�����Ăяo���B
 */
void UninitEnemy(void);

/**
 * @brief ���t���[���A�S�G�l�~�[���X�V����B
 * @note UpdateGame() �̒���DrawEnemy�̑O�ɌĂяo���B
 */
void UpdateEnemy(void);

/**
 * @brief ���t���[���A�S�G�l�~�[��`�悷��B
 * @note DrawGame() �̒���UpdateEnemy�̌�ɌĂяo���B
 */
void DrawEnemy(void);

/**
 * @brief ���݃A�N�e�B�u�ȃG�l�~�[�̐����擾����B
 * @return 0�ȏ�̃G�l�~�[��
 */
int GetEnemyCount(void);

/**
 * @brief �A�N�e�B�u�ȃG�l�~�[����ݒ肷��B
 * @param[in] enemyCount 0�ȏ�̃G�l�~�[��
 */
void SetEnemyCount(int enemyCount);

/**
 * @brief �G�l�~�[�z��ւ̃|�C���^���擾����B
 * @return �L���� ENEMY*�z��ւ̃|�C���^
 */
ENEMY* GetEnemy(void);

/**
 * @brief �G�l�~�[�{�̂ƃp�[�c������������B
 * @param[in] eNum ����������G�l�~�[���iMAX_ENEMY�ȉ��j
 */
void EnemyPartsInit(int eNum);

/**
 * @brief �G�l�~�[���A�C�h����Ԃɂ��鏈��
 * @param[in,out] enemy �����Ώۂ̃G�l�~�[
 */
void EnemyIdle(ENEMY& enemy);

/**
 * @brief �G�l�~�[�������_���ɜp�j�����鏈��
 * @param[in,out] enemy �����Ώۂ̃G�l�~�[
 */
void EnemyWander(ENEMY &enemy);

/**
 * @brief �v���C���[��ǐՂ��鏈��
 * @param[in,out] enemy �����Ώۂ̃G�l�~�[
 */
void EnemyChase(ENEMY& enemy);

/**
 * @brief �G�l�~�[���S���Ƀ��f�����o���o���ɂ��鏈��
 * @param[in] idx �����ΏۂƂȂ�G�l�~�[�̃C���f�b�N�X
 */
void SpawnEnemyDebris(int idx);

/**
 * @brief ���t���[���A�S�f�u���̕����^�`����X�V����B
 * @param[in] dt �t���[���Ԋu�i�b�j
 */
void UpdateEnemyDebris(float dt);

/**
 * @brief �e�p�[�c�̕����X�V�ƕ\���t���O�̃��Z�b�g���܂Ƃ߂čs��
 *
 *  ������UpdateEnemyDebris(float)���Ăяo���A
 *  �S�p�[�c�����ł����� g_IsSwordDebrisActive��FALSE�ɖ߂�
 */
void UpdateEnemyDebris(void);

/**
 * @brief ���S�t���O�������Ă���G�l�~�[�̃f�B�]���u���J�n�����A
 *        �����臒l�𒴂�����{�́E�p�[�c���\���ɂ���
 * @note UpdateEnemy() �̒��Ŗ��t���[���Ăяo��
 */
void UpdateEnemyDissolve(int idx);

/**
 * @brief enemy���m���d�Ȃ�Ȃ��ɂ��鏈��
 * @note UpdateEnemy() �̒��Ŗ��t���[���Ăяo��
 */
void UpdateEnemyPreventOverlap(int idx);

/**
 * @brief �p�[�c�K�w�A�j���[�V�������ԂōX�V����B
 * @param[in,out] part �����Ώۂ̃p�[�c�iENEMY�z��̊e�v�f�j
 */
void UpdateEnemyPartAnimation(ENEMY& part);

/**
 * @brief �G�l�~�[���Ƃ̈ړ��^�C�}�[���X�V���鏈��
 * @param[in,out] enemy �����Ώۂ̃G�l�~�[
 */
void EnemyMoveTimerUpdate(ENEMY& enemy);

/**
 * @brief �c�����̃p�[�c�����邩�𔻒肷��B
 * @return TRUE: 1�ȏ�A�N�e�B�u�ȃp�[�c����
 * @return FALSE: ���ׂď���
 */
BOOL IsAnyDebrisActive(void);

/**
 * @brief �w��G�l�~�[��noticeFlg���擾����B
 * @param[in] enemy ����Ώۂ̃G�l�~�[
 * @return TRUE: �v���C���[�𔭌���
 * @return FALSE: �������Ă��Ȃ�
 */
BOOL GetEnemyNoticeFlg(ENEMY& enemy);

/**
 * @brief �S�G�l�~�[�� noticeFlg ����ł�TRUE�����`�F�b�N����B
 * @return TRUE: �����ꂩ�̃G�l�~�[���������
 * @return FALSE: �S�G�l�~�[�����������
 */
BOOL IsAnyEnemyNotice();

// @brief ENEMY�̃��[���h�s������B
void EnemyBuildWorldTransforms(const ENEMY& enemy,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,	XMMATRIX& mtxTranslate);


// FSM�ł̃G�l�~�[����i���͖��g�p�j
//void CheckEnemyState(int enemyIndex, ENEMY& enemy);
//void UpdateEnemyState(ENEMY& enemy);