//=============================================================================
//
// �v���C���[�֌W�̏��� [player.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "character.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int	MAX_PLAYER		(1);			// �v���C���[�̐�
static const int	PLAYER_MAX_HP	(100);			// �v���C���[�̍ő�HP
static const float 	PLAYER_SIZE		(5.0f);			// �����蔻��̑傫��
static const int	MUTEKI_TIME_MAX (300);			// ���G���Ԃ̍ő吔

// @brief �v���C���[�̃p�[�c���Ǘ�����
enum PlayerPartsIndex
{
	// �v���C���[�̃p�[�c�ibody��g_Player�Őݒ�j
	PLAYER_HEAD = 0,	// ���ihead�j
	PLAYER_SHOULDER_L,	// �����ishoulder_Left�j
	PLAYER_SHOULDER_R,	// �E���ishoulder_Right�j
	PLAYER_ARM_L,		// ���r�iarm_Left�j
	PLAYER_ARM_R,		// �E�r�iarm_Right�j
	PLAYER_LEG_L,		// �����ilef_Left�j
	PLAYER_LEG_R,		// �E���ilef_Right�j
	// ����
	PLAYER_GUN,			// �e�iGun�j		
	PLAYER_WOODENSWORD,	// �ؓ��iWoodenSword�j
	PLAYER_BOMB,		// ���e�iBomb�j

	PLAYER_PARTS_COUNT
};
// @brief �v���C���[�̓����e�[�u�����Ǘ�����
enum PlayerMoveTableIndex
{
	// �ҋ@
	MT_STAY_HEAD = 0,       // stay_tbl_head
	MT_STAY_SHOULDER_L,     // stay_tbl_l_shoulder
	MT_STAY_SHOULDER_R,     // stay_tbl_r_shoulder
	MT_STAY_ARM_L,          // stay_tbl_l_arm
	MT_STAY_ARM_R,          // stay_tbl_r_arm
	MT_STAY_LEG_L,          // stay_tbl_l_leg
	MT_STAY_LEG_R,          // stay_tbl_r_leg
	MT_STAY_WEAPON_GUN,     // stay_tbl_weapon_gun
	// �U���i�e�j
	MT_ATTACK_WEAPON_GUN,		// move_tbl_weapon_gun_move
	MT_ATTACK_GUN_ARM_R,        // move_tbl_r_arm_move
	MT_ATTACK_GUN_SHOULDER_R,   // move_tbl_r_shoulder_move
	// �������[�V����
	MT_WALK_HEAD,           // move_tbl_head
	MT_WALK_SHOULDER_L,     // move_tbl_l_shoulder
	MT_WALK_SHOULDER_R,     // move_tbl_r_shoulder
	MT_WALK_ARM_L,          // move_tbl_l_arm
	MT_WALK_ARM_R,          // move_tbl_r_arm
	MT_WALK_LEG_L,          // move_tbl_l_leg
	MT_WALK_LEG_R,          // move_tbl_r_leg
	// �ҋ@�i�ؓ��j
	MT_STAY_WEAPON_WOODENSWORD,			// stay_tbl_weapon_woodenSword
	// �U���i�ؓ��j
	MT_ATTACK_WEAPON_WOODENSWORD,		// move_tbl_weapon_woodenSword_move
	MT_ATTACK_WOODENSWORD_ARM_R,        // move_tbl_r_arm_move
	MT_ATTACK_WOODENSWORD_SHOULDER_R,	// move_tbl_r_shoulder_move

	MT_STAY_WEAPON_BOMB,				// stay_tbl_weapon_bomb
	// �U���i���e�j
	MT_ATTACK_WEAPON_BOMB,				// move_tbl_weapon_bomb
	MT_ATTACK_BOMB_ARM_R,				// move_tbl_r_arm_bomb
	MT_ATTACK_BOMB_SHOULDER_R,			// move_tbl_r_shoulder_bomb

	MT_MAX
};

// @brief �v���C���[�̏�Ԃ��Ǘ�����
enum PlayerStateIndex
{
	PLAYER_STAY = 0,				// �ҋ@���
	PLAYER_WALK,					// ���s���
	PLAYER_ATTACK_GUN,				// �U���A�N�V�����i�e�j
	PLAYER_ATTACK_WOODENSWORD,		// �U���A�N�V�����i�ؓ��j
	PLAYER_ATTACK_BOMB,				// �U���A�N�V�����i���e�j
	PLAYER_MOVE_MAX
};
// @brief �v���C���[�̕�����Ǘ�����
enum PlayerWeaponIndex
{
	PLAYER_WEAPON_GUN = 0,		// �e
	PLAYER_WEAPON_WOODENSWORD,	// �ؓ�
	PLAYER_WEAPON_BOMB,			// ���e

	PLAYER_WEAPON_MAX			// ����̍ő吔
};

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class PLAYER : public CHARACTER
{
private:
	int					hp;				// �v���C���[��HP
public:
	// �X�e�[�^�X
	int					motion;			// �v���C���[�̃��[�V����
	int					weaponType;		// ����̎��

	// ����
	float				dir;			// �v���C���[�̌���
	float				jumpY;			// �W�����v�̍���
	float				jumpYMax;		// �W�����v�̍ő�l
	int					jumpCnt;		// �W�����v���̃J�E���g

	// �t���O
	BOOL				jump;			// �W�����v�t���O
	BOOL				muteki;			// ���G�̃t���O
	BOOL				showFallPoint;	// �����鏀������
	BOOL				moveFlg;		// �ړ��t���O

	// ���̑�
	int					mutekiTime;		// ���G�̎���
	XMFLOAT3			fallPoint;		// �����n�_
	PLAYER				*parent;		// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// HP�p�̃C���^�[�t�F�[�X
	int GetPlayerHP(void) const;			// HP���擾����
	void SetPlayerHP(int hp);				// HP���擾����

	// @brief �v���C���[�Ƀ_���[�W��^����B
	// @param[in] damage �_���[�W�ʁi0 �ȏ�j�B�Ăяo����AHP��0�����ɂȂ�ꍇ��0���i�[����B
	void TakePlayerDamage(int damage);		// �_���[�W��^���郁�\�b�h
public:

	// @brief �������e�̒��n�_��\������
	// @return �\�������n�_�̃��[���h���W
	XMFLOAT3 PredictBombLanding() const;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// @brief �v���C���[�̃��\�[�X������������B
// @return S_OK  ���� �^ E_FAIL ���s
HRESULT InitPlayer(void);

// @brief �v���C���[�̃��\�[�X���������B
void UninitPlayer(void);

// @brief ���t���[���v���C���[�̍X�V����
void UpdatePlayer(void);

// @brief �v���C���[�̕`�揈��
void DrawPlayer(void);

// @brief �v���C���[�C���X�^���X�ւ̃|�C���^���擾����B
// @return �L���� PLAYER*�iNULL �͕Ԃ��Ȃ��j
PLAYER* GetPlayer(void);

// @brief ���G���Ԃ̌o�ߏ������s���A�t���O���Ǘ�����
void UpdateInvincibility(void);

// @brief ���e�̓����}�[�J�[�̕\���^��\�����X�V����B
void UpdateThrowMarkerInPlayer(void);

// @brief �ړ����A�N�V�������͂��܂Ƃ߂ď�������B
void UpdateProcessMovementAndActionInput(void);

// @brief �J�����̎��_�ύX�ƍX�V���s��
// @param prevPos �O�̍��W
void UpdateApplyMovement(const XMFLOAT3& prevPos);

// @brief �Փ˔���ƒn�`�ւ�Y���W�␳���܂Ƃ߂čs���B
// @param prevPos �O�̍��W
void UpdateResolveCollisions(const XMFLOAT3& prevPos);

// @brief ���픭�ˁE�����������܂Ƃ߂čs��
void UpdateWeaponActions(void);

// @brief 1�p�[�c���̊K�w�A�j���[�V�������ԂōX�V����B
// @param tbl  ��ԃf�[�^�ւ̃|�C���^
// @param part �Ώۂ̃p�[�c�ig_Parts�j
void UpdateSinglePart(INTERPOLATION_DATA const* tbl, PLAYER& part);

// @brief g_Parts�S�̂̊K�w�A�j���[�V�������X�V����B
void UpdatePlayerPartsAnimation(void);

// @brief �v���C���[�̎p����n�ʖ@������slerp�ōX�V����B
void UpdateOrientation(void);

// @brief �C�Ӄ��f���ɖ��G�_�ŃG�t�F�N�g��K�p����B
// @param model       �`�惂�f�����
// @param diffuseCols �T�u�Z�b�g���Ƃ̒ʏ�F�z��
// @param subsetNum   �T�u�Z�b�g��
void DrawApplyInvincibilityToModel(DX11_MODEL& model, const XMFLOAT4 diffuseCols[], int subsetNum);

// @brief �t�B�[���h�O�ɏo����oldPos�Ɉʒu��߂��B
// @param oldPos �O�̍��W
void checkPlayerOverField(const XMFLOAT3& oldPos);

// @brief �v���C���[�̃p�[�c�z�������������B
// @param pNum �p�[�c���iPLAYER_PARTS_MAX �ȉ��j
void PlayerPartsInit(int pNum);

// @brief �v���C���[��Y���̉�]�p���擾����B
// @return ���W�A��
float GetPlayerRoty(void);

// @brief �v���C���[��Y���̉�]�p��ݒ肷��B
// @param roty ���W�A��
void SetPlayerRoty(float roty);

// @brief �p�[�c���`��\���`�F�b�N����B
// @param partsIndex �p�[�c�C���f�b�N�X
// @return TRUE: �`�悷��^FALSE: �X�L�b�v
BOOL ShowDrawParts(int partsIndex);

// @brief PLAYER�̃��[���h�s������B
void BuildWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot, XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate);

// @brief �p�[�c�p�̃��[���h�s������iXM_PI�̃I�t�Z�b�g�Ȃ��j
void BuildPartsWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld, XMMATRIX& mtxScl,
	XMMATRIX& mtxRot, XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate);

