//=============================================================================
//
// �v���C���[�֌W�̏��� [player.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "fade.h"
#include "player.h"
#include "hp.h"
#include "enemy.h"
#include "throwMarker.h"
#include "collision.h"
#include "meshobj.h"
#include "sound.h"
#include "score.h"
#include "time.h"
#include "shadow.h"
#include "bullet.h"
#include "bomb.h"
#include "game.h"
#include "debugproc.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER				PATH_MODEL_PLAYER "player_body_1.obj"			// �ǂݍ��ރ��f����

#define	MODEL_PLAYER_HEAD			PATH_MODEL_PLAYER "player_head_1.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LSHD			PATH_MODEL_PLAYER "player_shoulder_left_1.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_RSHD			PATH_MODEL_PLAYER "player_shoulder_right_1.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LARM			PATH_MODEL_PLAYER "player_arm_left_1.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_RARM			PATH_MODEL_PLAYER "player_arm_right_1.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LLEG			PATH_MODEL_PLAYER "player_leg_left_1.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_RLEG			PATH_MODEL_PLAYER "player_leg_right_1.obj"		// �ǂݍ��ރ��f����

#define	MODEL_PLAYER_WEAPON_GUN				PATH_MODEL_WEAPON "gun001.obj"				// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_WEAPON_WOODENSWORD		PATH_MODEL_WEAPON "woodenSword.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_WEAPON_BOMB			PATH_MODEL_WEAPON "bomb001.obj"				// �ǂݍ��ރ��f����

static const float PLAYER_START_X = -590.0f;			// �X�^�[�g�ʒu�iX�j
static const float PLAYER_START_Y =  12.0f;				// �X�^�[�g�ʒu�iX�j
static const float PLAYER_START_Z = -500.0f;			// �X�^�[�g�ʒu�iX�j

static const float	PLAYER_MAP_TOP		( MAP_H * 0.5 - 10.0f);
static const float	PLAYER_MAP_DOWN		(-MAP_H * 0.5 + 10.0f);
static const float	PLAYER_MAP_LEFT		(-MAP_W * 0.5 + 10.0f);
static const float	PLAYER_MAP_RIGHT	( MAP_W * 0.5 - 10.0f);

static const float	VALUE_MOVE	(1.5f);						// �ړ���
#define	VALUE_ROTATE			(D3DX_PI * 0.02f)			// ��]��

static const float PLAYER_OFFSET_Y	(12.0f);				// �v���C���[�̑��������킹��

static const int PLAYER_PARTS_MAX	(10);					// �v���C���[�̃p�[�c�̐�
static const float FRICTION_NUM		(0.7f);					// �v���C���[�̖��C�W��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static void PlayerMoveWalk(void);
static void PlayerAttackGun(void);
static void PlayerMoveStay(void);
static void AttackReturnStay(void);
static void PlayerAttackWoodenSword(void);
static void PlayerAttackBomb(void);
static void HandlePlayerAttackInput(void);
static bool IsAttackAnimFinished(void);
static void CheckWoodenSwordHit(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[�i�{�́j
static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static float		roty = 0.0f;

static float		g_Player_AnimTime;

static const float	g_BombPlayerScl = 0.2f;
float				g_BombSCL = g_BombPlayerScl;

static const float kGravity = GRAVITY;						// bomb_�d��
static const float kLaunchSpeed = BOMB_LAUNCH_SPEED;		// bomb_�����x
static const float kLaunchElevation = BOMB_LAUNCH_UP_ANGLE;	// bomb_�p�x

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
//=============================================================================
// �ҋ@���[�V�����e�[�u��
//=============================================================================
// ���ihead�j
static INTERPOLATION_DATA stay_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 6.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f),60 },
};
// �����ishoulder_Left�j
static INTERPOLATION_DATA stay_tbl_l_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// ���r�iarm_Left�j
static INTERPOLATION_DATA stay_tbl_l_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// �E���ishoulder_Right�j
static INTERPOLATION_DATA stay_tbl_r_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// �E�r�iarm_Right�j
static INTERPOLATION_DATA stay_tbl_r_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.5f, 2.3f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.3f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// �����ileg_Left�j
static INTERPOLATION_DATA stay_tbl_l_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};
// �E���ileg_Right�j
static INTERPOLATION_DATA stay_tbl_r_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};
// �e�iweapon_gun�j
static INTERPOLATION_DATA stay_tbl_weapon_gun[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-2.3f, -5.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
	{ XMFLOAT3(-2.3f, -4.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
};
// �ؓ��iweapon_woodenSword�j
static INTERPOLATION_DATA stay_tbl_weapon_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.3f, -3.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
	{ XMFLOAT3(-3.3f, -4.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(0.5f, 0.5f, 0.5f), 30},
};
// ���e�iweapon_bomb�j
static INTERPOLATION_DATA stay_tbl_weapon_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.3f, -3.7f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(g_BombSCL, g_BombSCL, g_BombSCL), 30},
	{ XMFLOAT3(-3.3f, -4.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),     XMFLOAT3(g_BombSCL, g_BombSCL, g_BombSCL), 30},
};
//=============================================================================
// �U���i�e�j���[�V�����e�[�u��
//=============================================================================
// �e�̓���iweapon_gun�j
static INTERPOLATION_DATA move_tbl_weapon_gun_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	//{ XMFLOAT3(2.3f, -5.0f, 0.0f), XMFLOAT3(1.3f, 0.3f, 0.3f),	   XMFLOAT3(0.3f, 0.3f, 0.3f), 15},
};
// �E�r��O�ɂ����iarm_Right_move�j
static INTERPOLATION_DATA move_tbl_r_arm_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// �E����O�ɂ����ishoulder_Right_move�j
static INTERPOLATION_DATA move_tbl_r_shoulder_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// �U���i�ؓ��j���[�V�����e�[�u��
//=============================================================================
// �ؓ��̓���iweapon_woodenSword�j
static INTERPOLATION_DATA move_tbl_weapon_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,5.0f), XMFLOAT3(0.0f, XM_PI / 2, XM_PI / 2),  XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
	{ XMFLOAT3(0.0f, 2.0f,5.0f), XMFLOAT3(0.0f, XM_PI / 2, 0.0f),		XMFLOAT3(0.5f, 0.5f, 0.5f), 15},
};
// �E�r��O�ɂ���
static INTERPOLATION_DATA move_tbl_r_arm_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, XM_PI / 2),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// �E����O�ɂ���
static INTERPOLATION_DATA move_tbl_r_shoulder_woodenSword[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, XM_PI / 2),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// �U���i���e�j���[�V�����e�[�u��
//=============================================================================
// ���e�𓊂��铮��iweapon_bomb�j
static INTERPOLATION_DATA move_tbl_weapon_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.0f, 0.0f, 0.0f), 15},
	{ XMFLOAT3(0.0f, 2.0f,8.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),        XMFLOAT3(0.0f, 0.0f, 0.0f), 15},
};
// �E�r��O�ɂ���
static INTERPOLATION_DATA move_tbl_r_arm_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
// �E����O�ɂ����ishoulder_Right_move�j
static INTERPOLATION_DATA move_tbl_r_shoulder_bomb[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-0.3f, XM_PI / 2, 0.0f),       XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
};
//=============================================================================
// �������[�V�����e�[�u��
//=============================================================================
// ���ihead�j
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 8, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 8, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// �����ishoulder_Left�j
static INTERPOLATION_DATA move_tbl_l_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, -1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, -1.3f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// �E���ishoulder_Right�j
static INTERPOLATION_DATA move_tbl_r_shoulder[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 1.3f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 1.3f),		  XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// ���r�iarm_Left�j
static INTERPOLATION_DATA move_tbl_l_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, -1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, -1.3f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// �E�r�iarm_Right�j
static INTERPOLATION_DATA move_tbl_r_arm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 1.3f),	   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(-1.5f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 1.3f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
};
// �����ileg_Left�j
static INTERPOLATION_DATA move_tbl_l_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(1.0f, -4.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};
// �E���ileg_Right�j
static INTERPOLATION_DATA move_tbl_r_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-1.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};

static char* MODEL_PARTS[PLAYER_PARTS_MAX] =
{
	MODEL_PLAYER_HEAD,
	MODEL_PLAYER_LSHD,
	MODEL_PLAYER_RSHD,
	MODEL_PLAYER_LARM,
	MODEL_PLAYER_RARM,
	MODEL_PLAYER_LLEG,
	MODEL_PLAYER_RLEG,
	MODEL_PLAYER_WEAPON_GUN,
	MODEL_PLAYER_WEAPON_WOODENSWORD,
	MODEL_PLAYER_WEAPON_BOMB,
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	// �ҋ@���[�V����
	stay_tbl_head,				// 0
	stay_tbl_l_shoulder,		// 1
	stay_tbl_r_shoulder,		// 2
	stay_tbl_l_arm,				// 3
	stay_tbl_r_arm,				// 4
	stay_tbl_l_leg,				// 5
	stay_tbl_r_leg,				// 6
	stay_tbl_weapon_gun,		// 7

	move_tbl_weapon_gun_move,	// 8
	move_tbl_r_arm_move,		// 9
	move_tbl_r_shoulder_move,	// 10

	// �������[�V����
	move_tbl_head,				// 11
	move_tbl_l_shoulder,		// 12
	move_tbl_r_shoulder,		// 13
	move_tbl_l_arm,				// 14
	move_tbl_r_arm,				// 15
	move_tbl_l_leg,				// 16
	move_tbl_r_leg,				// 17

	// �ؓ�
	stay_tbl_weapon_woodenSword,		// 18
	move_tbl_weapon_woodenSword,		// 19
	move_tbl_r_arm_woodenSword,			// 20
	move_tbl_r_shoulder_woodenSword,	// 21

	// ���e
	stay_tbl_weapon_bomb,			// 22
	move_tbl_weapon_bomb,			// 23
	move_tbl_r_arm_bomb,			// 24
	move_tbl_r_shoulder_bomb,		// 25

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player_AnimTime = 0.0f;

	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	ZeroMemory(&g_Player.material, sizeof(g_Player.material));
	g_Player.material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_Player.pos = XMFLOAT3(PLAYER_START_X, PLAYER_START_Y, PLAYER_START_Z);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.SetPlayerHP(PLAYER_MAX_HP);
	g_Player.motion = PLAYER_STAY;				// �ҋ@���[�V�������Z�b�g
	g_Player.weaponType = PLAYER_WEAPON_GUN;	// ����̎��
	g_Player.muteki = FALSE;					// ���G��Ԃ̃t���O
	g_Player.mutekiTime = 0;					// ���G����
	g_Player.spd = 0.0f;						// �ړ��X�s�[�h�N���A
	g_Player.time = 0.0f;						// �v���C���[�̎���

	g_Player.showFallPoint = FALSE;
	g_Player.fallPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Player.use = TRUE;			// TRUE:�����Ă�
	g_Player.moveFlg = FALSE;
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	GetModelDiffuse(&g_Player.model, &g_Player.diffuse[0]);

	// �L�[�����������̃v���C���[�̌���
	roty = 0.0f;

	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������


	// �K�w�A�j���[�V�����̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// ��

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Parts[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Parts[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = FALSE;

		// �p�[�c�̃Z�b�g���s��
		PlayerPartsInit(i);

	}

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		ZeroMemory(&g_Parts[i].material, sizeof(g_Parts[i].material));
		g_Parts[i].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		GetModelDiffuse(&g_Parts[i].model, &g_Parts[i].diffuse[0]);
	}

	// �N�H�[�^�j�I���̏�����
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// �p�[�c�̉������
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();
	g_Player_AnimTime += 1.0f;
	XMFLOAT3 prevPos = g_Player.pos;

	// ���G��ԂȂ疳�G���Ԃ��v������
	UpdateInvincibility();

	// �{�����\���Ă����ԁi���킪BOMB�ōU�����łȂ��Ƃ��j
	UpdateThrowMarkerInPlayer();

	// ���[�V�����^�C�v�̊m�F�A����ύX�Aplayer�̈ړ�����
	UpdateProcessMovementAndActionInput();

	// �J�������_�ɂ���ē���������ύX����
	UpdateApplyMovement(prevPos);

	// player�Ƃ̊e�퓖���蔻�菈��
	UpdateResolveCollisions(prevPos);
	 
	// ����i�e�Ɣ��e�j�̔��ˏ���
	UpdateWeaponActions();

	// �K�w�A�j���[�V�����̏���
	UpdatePlayerPartsAnimation();

	// �p������̏���
	UpdateOrientation();

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif

#ifdef _DEBUG
	// �f�o�b�O�\��
	//PrintDebugProc("Player X:%f Y:%f Z:%f N:%f\nroty:%f\n,g_Player_AnimTime�F%f\n,g_Player.weaponType�F%d\n", 
	//				g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y, roty, g_Player_AnimTime, g_Player.weaponType);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	//=============================================================================
	// ���f���̕`��
	//=============================================================================
	{
		// ���G���Ԓ��Ȃ�_�ł�����
		DrawApplyInvincibilityToModel(g_Player.model, g_Player.diffuse, g_Player.model.SubsetNum);

		// ���[���h�}�g���b�N�X�̑g�ݗ��āi�{�f�B�j
		BuildWorldTransforms(g_Player, mtxWorld, mtxScl, mtxRot, quatMatrix, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

		// �������C�g�̐ݒ�
		SetRimLight(0);		// �I�t

		// ���f���`��
		DrawModel(&g_Player.model);

		//=============================================================================
		// �p�[�c�̕`��
		//=============================================================================
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// ���G��ԂȂ�F��ύX����
			DrawApplyInvincibilityToModel(g_Parts[i].model, g_Parts[i].diffuse, g_Parts[i].model.SubsetNum);

			// ���[���h�}�g���b�N�X�̑g�ݗ��āi�p�[�c�j
			BuildPartsWorldTransforms(g_Parts[i], mtxWorld, mtxScl, mtxRot, quatMatrix, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
			{
				// g_Player.mtxWorld���w���Ă���
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������
			if (g_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// ����̕`��𕪂���
			if(!ShowDrawParts(i)) continue;

			// ���f���`��
			DrawModel(&g_Parts[i].model);
		}

		SetRimLight(0);
	}
	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// �v���C���[�̌��������擾�i�A�N�Z�T�֐��j
//=============================================================================
float GetPlayerRoty(void)
{
	return roty;
}
void SetPlayerRoty(float newRoty)
{
	roty = newRoty;
}

//=============================================================================
// �v���C���[��HP���擾����i�A�N�Z�T�֐��j
//=============================================================================
int PLAYER::GetPlayerHP(void) const
{
	return g_Player.hp;
}
void PLAYER::SetPlayerHP(int hp)
{
	g_Player.hp = hp;
}

//=============================================================================
// �v���C���[�Ƀ_���[�W��^���郁�\�b�h
// @param damage�F�_���[�W��
// note�Fplayer�Ƀ_���[�W��^����Ƃ��͂��̃��\�b�h��K���Ă�
// note�F �ő�l�ȏ�̒l�̏ꍇ�APLAYER_MAX_HP���i�[����
//=============================================================================
void PLAYER::TakePlayerDamage(int damage)
{
	if (damage >= PLAYER_MAX_HP) damage = PLAYER_MAX_HP;
	SetPlayerHP(g_Player.hp - damage);
}

//=============================================================================
// ����̃A�j���[�V�������Ǘ�����
//=============================================================================
namespace
{
	struct WeaponAnimDef
	{
		int		partIndex;		// �p�[�c�C���f�b�N�X
		int		attackTblNo;	// �U���e�[�u��No.
		float	attackDuration;	// �U������
		int		stayTblNo;		// �ҋ@�e�[�u��No.
		int		stayTblMax;		// �ҋ@�e�[�u���̍ő�l
	};

	static const WeaponAnimDef g_WeaponDefs[PLAYER_WEAPON_MAX] =
	{
		// �e
		{
			PLAYER_GUN,
			MT_ATTACK_WEAPON_GUN,
			30.0f,
			MT_STAY_WEAPON_GUN,
			sizeof(stay_tbl_weapon_gun) / sizeof(INTERPOLATION_DATA)
		},
		// �ؓ�
		{
			PLAYER_WOODENSWORD,
			MT_ATTACK_WEAPON_WOODENSWORD,
			30.0f,
			MT_STAY_WEAPON_WOODENSWORD,
			sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA)
		},
		// ���e
		{
			PLAYER_BOMB,
			MT_ATTACK_WEAPON_BOMB,
			30.0f,
			MT_STAY_WEAPON_BOMB,
			sizeof(stay_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA)
		},
	};
}

//=============================================================================
// �v���C���[�Ƀp�[�c���Z�b�g���郁�\�b�h
// @param pNum �F�p�[�c�ԍ�
// note�Fnull�͋��e���Ă��Ȃ�
//=============================================================================
void PlayerPartsInit(int pNum)
{
	// �p�[�c�̓ǂݍ���
	for (int pNum = 0; pNum < PLAYER_PARTS_MAX; pNum++)
	{
		g_Parts[pNum].use = TRUE;
		g_Parts[pNum].load = TRUE;
		g_Parts[pNum].parent = &g_Player;	// �e���Z�b�g
		g_Parts[pNum].tblNo = pNum;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g

		switch (pNum)
		{
		case PLAYER_HEAD:// ���iHEAD�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_SHOULDER_L:// �����ishoulder_Left�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_SHOULDER_R:// �E���ishoulder_Right�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_ARM_L:// ���r�iarm_Left�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_ARM_R:// �E�r�iarm_Right�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_LEG_L:// �����ileg_Left�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_LEG_R:// �E���iright_Left�j
			g_Parts[pNum].tblMax = sizeof(stay_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_GUN:// �e�iGun�j
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;		// �e���Z�b�g
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_gun) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_WOODENSWORD:// �ؓ��iwoodenSword�j
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;
			g_Parts[pNum].tblNo = MT_STAY_WEAPON_WOODENSWORD;
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		case PLAYER_BOMB:// ���e�ibomb�j
			g_Parts[pNum].parent = g_Parts[PLAYER_ARM_R].parent;
			g_Parts[pNum].tblNo = MT_STAY_WEAPON_BOMB;
			g_Parts[pNum].tblMax = sizeof(stay_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			break;
		default:
			break;
		}
		LoadModel(MODEL_PARTS[pNum], &g_Parts[pNum].model);
	}
}

//=============================================================================
// �v���C���[�̍U�����[�V�������Ǘ����郁�\�b�h
//=============================================================================
static void HandlePlayerAttackInput(void)
{
	// �U���g���K�[�͋���
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
	{
		switch (g_Player.weaponType)
		{
		case PLAYER_WEAPON_GUN:
			PlayerAttackGun();
			break;
		case PLAYER_WEAPON_WOODENSWORD:
			PlaySound(SOUND_LABEL_SE_09_WOODENSWORD);
			PlayerAttackWoodenSword();
			CheckWoodenSwordHit();
			break;
		case PLAYER_WEAPON_BOMB:
			PlayerAttackBomb();
			break;
		default:
			// TODO�F���̕��킪�������炱���ɒǉ�
			break;
		}
	}
}

//=============================================================================
// �v���C���[�̑ҋ@���[�V���������s���郁�\�b�h
//=============================================================================
static void PlayerMoveStay(void)
{
	g_Player.motion = PLAYER_STAY;
	// ���ihead�j
	g_Parts[PLAYER_HEAD].tblNo = MT_STAY_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_HEAD].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �����ishoulder_Left�j
	g_Parts[PLAYER_SHOULDER_L].tblNo = MT_STAY_SHOULDER_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_L].tblMax = sizeof(move_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// ���r�iarm_Left�j
	g_Parts[PLAYER_ARM_L].tblNo = MT_STAY_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_L].tblMax = sizeof(move_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// �E�r�iarm_Right�j
	g_Parts[PLAYER_ARM_R].tblNo = MT_STAY_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E���ishoulder_Right�j
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_STAY_SHOULDER_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// �����ilef_Left�j
	g_Parts[PLAYER_LEG_L].tblNo = MT_STAY_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_LEG_L].tblMax = sizeof(move_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E���ilef_Right�j
	g_Parts[PLAYER_LEG_R].tblNo = MT_STAY_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_LEG_R].tblMax = sizeof(move_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
}

//=============================================================================
// �v���C���[�̕��s���[�V���������s���郁�\�b�h
//=============================================================================
static void PlayerMoveWalk(void)
{
	// �����̃A�j���[�V�����ɕύX
	g_Player.motion = PLAYER_WALK;
	// ���ihead�j
	g_Parts[PLAYER_HEAD].tblNo = MT_WALK_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_HEAD].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �����ishoulder_Left�j
	g_Parts[PLAYER_SHOULDER_L].tblNo = MT_WALK_SHOULDER_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_L].tblMax = sizeof(move_tbl_l_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E���ishoulder_Right�j
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_WALK_SHOULDER_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// ���r�iarm_Left�j
	g_Parts[PLAYER_ARM_L].tblNo = MT_WALK_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_L].tblMax = sizeof(move_tbl_l_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E�r�iarm_Right�j
	g_Parts[PLAYER_ARM_R].tblNo = MT_WALK_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �����ilef_Left�j
	g_Parts[PLAYER_LEG_L].tblNo = MT_WALK_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_LEG_L].tblMax = sizeof(move_tbl_l_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E���ilef_Right�j
	g_Parts[PLAYER_LEG_R].tblNo = MT_WALK_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_LEG_R].tblMax = sizeof(move_tbl_r_leg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
}

//=============================================================================
// �v���C���[�̍U�����[�V�����i�e�j�����s���郁�\�b�h
//=============================================================================
static void PlayerAttackGun(void)
{
	// �e�����̃��[�V�����ɕύX
	g_Player.motion = PLAYER_ATTACK_GUN;
	// �e�������[�V�����ɕύX
	g_Player_AnimTime = 0.0f;

	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_GUN_SHOULDER_R;										// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_move) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E�r�̓���
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_GUN_ARM_R;													// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_move) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �e�̓���
	g_Parts[PLAYER_GUN].tblNo = MT_ATTACK_WEAPON_GUN;													// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_GUN].tblMax = sizeof(move_tbl_weapon_gun_move) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
}
//=============================================================================
// �v���C���[�̍U�����[�V�����i�ؓ��j�����s���郁�\�b�h
//=============================================================================
static void PlayerAttackWoodenSword(void)
{
	// �ؓ��Ő؂郂�[�V�����ɕύX
	g_Player.motion = PLAYER_ATTACK_WOODENSWORD;
	// �ؓ��Ő؂郂�[�V�����ɕύX
	g_Player_AnimTime = 0.0f;
	// �E���̓���
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_WOODENSWORD_SHOULDER_R;										// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_woodenSword) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E�r�̓���
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_WOODENSWORD_ARM_R;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_woodenSword) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �ؓ��̓���
	g_Parts[PLAYER_WOODENSWORD].tblNo = MT_ATTACK_WEAPON_WOODENSWORD;											// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_WOODENSWORD].tblMax = sizeof(stay_tbl_weapon_woodenSword) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
}
//=============================================================================
// �U�����[�V�����i���e�j�����s���郁�\�b�h
//=============================================================================
static void PlayerAttackBomb(void)
{
	// ���e�𓊂��郂�[�V�����ɕύX
	g_Player.motion = PLAYER_ATTACK_BOMB;
	g_Player_AnimTime = 0.0f;

	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_ATTACK_BOMB_SHOULDER_R;											// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(move_tbl_r_shoulder_bomb) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// �E�r�̓���
	g_Parts[PLAYER_ARM_R].tblNo = MT_ATTACK_BOMB_ARM_R;														// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(move_tbl_r_arm_bomb) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	// ���e�̓���
	g_Parts[PLAYER_BOMB].tblNo = MT_ATTACK_WEAPON_BOMB;													// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PLAYER_BOMB].tblMax = sizeof(move_tbl_weapon_bomb) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
}
//=============================================================================
// �v���C���[�̍U�����[�V��������ҋ@���[�V�����ɖ߂����\�b�h
//=============================================================================
static void AttackReturnStay(void)
{
	// �E���ƉE�r�͋���
	g_Parts[PLAYER_SHOULDER_R].tblNo = MT_STAY_SHOULDER_R;
	g_Parts[PLAYER_SHOULDER_R].tblMax = sizeof(stay_tbl_r_shoulder) / sizeof(INTERPOLATION_DATA);
	g_Parts[PLAYER_ARM_R].tblNo = MT_STAY_ARM_R;
	g_Parts[PLAYER_ARM_R].tblMax = sizeof(stay_tbl_r_arm) / sizeof(INTERPOLATION_DATA);

	// ����̎�ނ��擾����
	const WeaponAnimDef& def = g_WeaponDefs[g_Player.weaponType];

	g_Parts[def.partIndex].tblNo = def.stayTblNo;
	g_Parts[def.partIndex].tblMax = def.stayTblMax;

	// �v���C���[�̏�Ԃ�ҋ@�ɖ߂�
	g_Player.motion = PLAYER_STAY;
}

//=============================================================================
// ����̎�ނɂ���ĕ`��𕪂���
// @param partsIndex�F�p�[�c�̃C���f�b�N�X�̒l
// note�Fnull�͑z�肵�Ă��Ȃ�
//=============================================================================
BOOL ShowDrawParts(int partsIndex)
{
	switch (partsIndex)
	{
	case PLAYER_GUN:
		return g_Player.weaponType == PLAYER_WEAPON_GUN;
	case PLAYER_WOODENSWORD:
		return g_Player.weaponType == PLAYER_WEAPON_WOODENSWORD;
	case PLAYER_BOMB:
		// ���e���[����������`�悵�Ȃ�
		if (GetBombNum() <= 0) return FALSE;
		return g_Player.weaponType == PLAYER_WEAPON_BOMB;
	default:
		// ����ȊO�̃p�[�c�͏�ɕ`�悷��
		return TRUE;
	}
}

//=============================================================================
// �A�j���[�V�������������Ă��邩���f����
// note�Fnull�͑z�肵�Ă��Ȃ�
//=============================================================================
static bool IsAttackAnimFinished(void)
{
	const WeaponAnimDef& def = g_WeaponDefs[g_Player.weaponType];

	return (g_Parts[def.partIndex].tblNo == def.attackTblNo) && (g_Player_AnimTime > def.attackDuration);
}

//=============================================================================
// �ؓ��ł̓����蔻��
//=============================================================================
static void CheckWoodenSwordHit(void)
{
	// �U���A�j�����̂ݔ���
	if (g_Player.motion != PLAYER_ATTACK_WOODENSWORD) return;

	ENEMY *enemy = GetEnemy();

	// �v���C���[�ʒu�{�����x�N�g�� * ���[�` �œ����蔻��_�����
	float rad = g_Player.rot.y;  // y����]�p
	XMFLOAT3 forward = { sinf(rad), 0.0f, cosf(rad) };
	XMFLOAT3 forward180 = { -forward.x, 0.0f, -forward.z };
	const float reach = 20.0f;    // ���̓͂�����
	const float swordRadius = 10.0f; // ���G���A�̔��a

	XMFLOAT3 hitPos = 
	{
		g_Player.pos.x + forward180.x * reach,
		g_Player.pos.y + 1.0f,
		g_Player.pos.z + forward180.z * reach
	};

	// BC�Ŕ���
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		if (!enemy[i].use) continue;

		if (CollisionBC(hitPos, enemy[i].pos, swordRadius, ENEMY_SIZE))
		{
			// �ؓ��ōU�������t���O
			g_IsSwordDebrisActive = TRUE;
			// �G�l�~�[���o���o���ɂ���
			SpawnEnemyDebris(i);
			// ���������ꍇ
			// �G�L�����N�^�[�͓|�����
			enemy[i].deadFlg = TRUE;
			// �X�R�A�𑫂�
			AddScore(SCORE_POINT);
		}
	}
}

//=============================================================================
// �����ʒu�̌v�Z
// note�Fnull�͑z�肵�Ă��Ȃ�
//=============================================================================
XMFLOAT3 PLAYER::PredictBombLanding() const
{
	// �v���C���[�̌��݈ʒu�E����
	XMFLOAT3 player = g_Player.pos;
	float yaw = g_Player.rot.y + XM_PI;
	float elev = kLaunchElevation;

	// �����x�N�g��
	XMVECTOR dirH = XMVectorSet(sinf(yaw), 0, cosf(yaw), 0);
	dirH = XMVector3Normalize(dirH);
	XMFLOAT3 fwd; XMStoreFloat3(&fwd, dirH);

	// �����x�N�g������
	float vx = fwd.x * kLaunchSpeed;
	float vz = fwd.z * kLaunchSpeed;
	float vy = sinf(elev) * kLaunchSpeed;

	// �n�ʍ������Œ� 10.0f �Ƃ��Čv�Z
	float y0 = player.y;
	float yGround = 10.0f;

	// ���ʎ�
	float disc = vy * vy + 2.0f * kGravity * (y0 - yGround);
	if (disc < 0.0f) 
	{
		return { player.x, yGround, player.z };
	}
	// �n�ʓ��B����
	float T = (vy + sqrtf(disc)) / kGravity;

	// ���n�_�� XZ
	XMFLOAT3 land = 
	{
		player.x + vx * T,
		yGround,
		player.z + vz * T
	};

	constexpr float kLerpFactor = 0.5f;

	land.x = player.x + (land.x - player.x) * kLerpFactor;
	land.z = player.z + (land.z - player.z) * kLerpFactor;

	return land;
}
//=============================================================================
// ���G���Ԃ̊Ǘ����s���֐�
// note�F5�b�i300�t���[���j�o�߂�����t���O��FALSE
//=============================================================================
void UpdateInvincibility()
{
	if (g_Player.muteki)
	{
		g_Player.mutekiTime += 1;
		// 5�b�o�߂����疳�G�t���O��FALSE�ɂ���
		if (g_Player.mutekiTime > MUTEKI_TIME_MAX)
		{
			g_Player.muteki = FALSE;
			g_Player.mutekiTime = 0;
		}
	}
}
//=============================================================================
// �����}�[�J�[�̍X�V���s���֐�
//=============================================================================
void UpdateThrowMarkerInPlayer()
{
	if (g_Player.weaponType == PLAYER_WEAPON_BOMB
		&& g_Player.motion != PLAYER_ATTACK_BOMB)
	{
		g_Player.showFallPoint = TRUE;
		XMFLOAT3 point = g_Player.PredictBombLanding();
		ShowThrowMarker(point);
	}
	else
	{
		HideThrowMarker();
		g_Player.showFallPoint = FALSE;
	}
}
//=============================================================================
// ���[�V�����̊m�F�Ɛ���ƕ���ύX���s���֐�
//=============================================================================
void UpdateProcessMovementAndActionInput()
{
	// ���C�W���������Ă���
	g_Player.spd *= FRICTION_NUM;

	// ���̓`�F�b�N
	const BOOL moveLeft	 = GetKeyboardPress(DIK_LEFT)  || IsButtonPressed(0, BUTTON_LEFT);
	const BOOL moveRight = GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT);
	const BOOL moveUp	 = GetKeyboardPress(DIK_UP)	   || IsButtonPressed(0, BUTTON_UP);
	const BOOL moveDown  = GetKeyboardPress(DIK_DOWN)  || IsButtonPressed(0, BUTTON_DOWN);

	// �����ƈړ��t���O�̌v�Z
	if (moveLeft || moveRight || moveUp || moveDown)
	{
		g_Player.moveFlg = TRUE;
		g_Player.spd = VALUE_MOVE;

		// 8�����̌�����ݒ�
		if (moveLeft && moveUp)			roty = XM_PI * 0.75f;
		else if (moveLeft && moveDown)	roty = XM_PI * 0.25f;
		else if (moveRight && moveUp)	roty = -XM_PI * 0.75f;
		else if (moveRight && moveDown)	roty = -XM_PI * 0.25f;
		else if (moveLeft)				roty = XM_PI * 0.5f;
		else if (moveRight)				roty = -XM_PI * 0.5f;
		else if (moveUp)				roty = XM_PI;
		else							roty = 0.0f;
	}
	else
	{
		g_Player.moveFlg = FALSE;
	}

	// �����E�ҋ@�E�U���̐؂�ւ�
	if (g_Player.moveFlg)
	{
		PlayerMoveWalk();
	}
	else
	{
		HandlePlayerAttackInput();
		if (g_Player.motion	   != PLAYER_ATTACK_GUN
			&& g_Player.motion != PLAYER_ATTACK_WOODENSWORD
			&& g_Player.motion != PLAYER_ATTACK_BOMB)
		{
			PlayerMoveStay();
		}
	}

	// �U�������őҋ@�ɖ߂�
	if (IsAttackAnimFinished())
	{
		AttackReturnStay();
	}

	// ����ؑ�
	if (g_Player.motion != PLAYER_ATTACK_GUN
		&& g_Player.motion != PLAYER_ATTACK_WOODENSWORD
		&& g_Player.motion != PLAYER_ATTACK_BOMB)
	{
		if (GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_08_CHANGEWEAPON);
			g_Player.weaponType = ((g_Player.weaponType + 1) % PLAYER_WEAPON_MAX);
		}
	}
}
//=============================================================================
// �J�����̎��_�ύX�ƍX�V���s���֐�
//=============================================================================
void UpdateApplyMovement(const XMFLOAT3& prevPos)
{
	// �J�������_���擾
	CAMERA* cam = GetCamera();

	if (!GetFirstPersonFlg())
	{
		// 3�l�̎��_�Froty �ɃJ������]�𑫂��Č���������
		g_Player.rot.y = roty + cam->rot.y;

		// �����������Ɉړ�
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}
	else
	{
		// 1�l�̎��_�F��ɃJ��������
		g_Player.rot.y = cam->rot.y;

		g_Player.pos.x += sinf(roty + cam->rot.y) * g_Player.spd;
		g_Player.pos.z += cosf(roty + cam->rot.y) * g_Player.spd;
	}
}
//=============================================================================
// �v���C���[���t�B�[���h�O�ɏo�Ȃ��悤�ɂ���֐�
// @param prevPos�F�v���C���[�̑O���W
// note�F���݂͂��̎����ōs���B�����RayCast�Ŕ��肳����iTODO�j
//=============================================================================
void checkPlayerOverField(const XMFLOAT3& prevPos)
{
	// �}�b�v�O�ɏo���猳�̈ʒu�ɖ߂�
	if (g_Player.pos.x < PLAYER_MAP_LEFT
		|| g_Player.pos.x > PLAYER_MAP_RIGHT
		|| g_Player.pos.z < PLAYER_MAP_DOWN
		|| g_Player.pos.z > PLAYER_MAP_TOP)
	{
		g_Player.pos = prevPos;
	}
}
//=============================================================================
// �v���C���[�Ƃ̓����蔻����Ǘ�����֐�
// @param prevPos�F�v���C���[�̑O���W
// note�FNull�͋��e���Ă��Ȃ�
//=============================================================================
void UpdateResolveCollisions(const XMFLOAT3& prevPos)
{
	// �t�B�[���h�̊O�ɏo�Ȃ��悤�ɂ��鏈��
	checkPlayerOverField(prevPos);

	// �t�B�[���h�I�u�W�F�N�g�Ƃ̓����蔻��
	checkPlayerFieldObj(prevPos);

	// �G�l�~�[�Ƃ��������ꍇ�A���蔲���Ȃ��悤�ɖ߂�
	checkPlayerToEnemy(prevPos);

	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 hitPosition;		// ��_
	XMFLOAT3 normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
	BOOL ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	if (ans)
	{
		g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
		g_Player.UpVector = normal;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		g_Player.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
}
//=============================================================================
// �v���C���[����̔��ˁE�����������Ǘ�����֐�
// note�FNull�͋��e���Ă��Ȃ�
//=============================================================================
void UpdateWeaponActions()
{
	// ���ʂ̔��˃g���K�[����
	bool trigger = (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
		&& g_Player.motion != PLAYER_WALK;

	// �e�̔���
	if (g_Player.weaponType == PLAYER_WEAPON_GUN && trigger)
	{
		PlaySound(SOUND_LABEL_SE_02_GUNSHOT);

		XMFLOAT3 bulletPos = g_Player.pos;
		bulletPos.y += 1.2f;
		// roty �ɉ������I�t�Z�b�g
		if (roty > 1.47f && roty < 2.0f) bulletPos.x -= 8.0f;
		if (roty < 0.0f)                  bulletPos.x += 8.0f;

		SetBullet(bulletPos, g_Player.rot);
	}
	// ���e�̓���
	else if (g_Player.weaponType == PLAYER_WEAPON_BOMB && GetBombNum() > 0 && trigger)
	{
		PlaySound(SOUND_LABEL_SE_10_THROWBOMB);

		XMFLOAT3 bombPos = g_Player.pos;
		bombPos.y += 1.2f;
		if (roty < 0.0f) bombPos.x += 8.0f;

		SetBomb(bombPos, g_Player.rot);
	}
}
//=============================================================================
// �v���C���[�̃p�[�c�𐧌䂷��֐�
//  @param tbl�F�A�j���[�V�����e�[�u��
//  @param part�Fplayer�̍\����
//=============================================================================
void UpdateSinglePart(INTERPOLATION_DATA const* tbl, PLAYER& part)
{
	if (!part.use || part.tblMax <= 0)
		return;

	int   nowNo = static_cast<int>(part.time);
	int   maxNo = part.tblMax;
	int   nextNo = (nowNo + 1) % maxNo;
	float frac = part.time - float(nowNo);  // 0.0�`1.0 �̕�ԌW��

	// ���݂̏�Ԃ����[�h����
	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);
	XMVECTOR nextPos = XMLoadFloat3(&tbl[nextNo].pos);
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);

	// ���̏�Ԃ����[�h����
	XMVECTOR nextRot = XMLoadFloat3(&tbl[nextNo].rot);
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);
	XMVECTOR nextScl = XMLoadFloat3(&tbl[nextNo].scl);

	// Lerp �ňꔭ���
	XMVECTOR interpPos = XMVectorLerp(nowPos, nextPos, frac);
	XMVECTOR interpRot = XMVectorLerp(nowRot, nextRot, frac);
	XMVECTOR interpScl = XMVectorLerp(nowScl, nextScl, frac);

	// ���ʂ��i�[����
	XMStoreFloat3(&part.pos, interpPos);
	XMStoreFloat3(&part.rot, interpRot);
	XMStoreFloat3(&part.scl, interpScl);

	// time �����t���[���������i�߂�
	part.time += 1.0f / tbl[nowNo].frame;
	if (part.time >= float(maxNo))
		part.time -= float(maxNo);
}

//=============================================================================
// �v���C���[�̃p�[�c��S�čX�V����֐�
//=============================================================================
void UpdatePlayerPartsAnimation()
{
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// tblNo���g���Đ������e�[�u�������o��
		int tableIndex = g_Parts[i].tblNo;
		UpdateSinglePart(g_MoveTblAdr[tableIndex], g_Parts[i]);
	}
}
//=============================================================================
// �v���C���[�̎p��������s���֐�
// note�F�t�B�[���h�𓮂������Ɏg�p����
//=============================================================================
void UpdateOrientation()
{
	// UpdateResolveCollisions �� UpVector �ɃZ�b�g�ς݂̖@�����g��
	XMVECTOR upRef = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR surface = XMLoadFloat3(&g_Player.UpVector);

	// �@���Ə�����x�N�g���̊O�� �� ��]��
	XMVECTOR axis = XMVector3Cross(upRef, surface);
	float    len = XMVectorGetX(XMVector3Length(axis));

	// len �� 0�i�@������������̂��́j�Ȃ玲��upRef�Ƀt�H�[���o�b�N
	if (len < 1e-6f) {
		axis = upRef;
	}
	else {
		axis = XMVector3Normalize(axis);
	}

	// ��]�p�� asin(len) �Ŏ擾
	float angle = asinf(len);

	// �ڕW�p���N�H�[�^�j�I��
	XMVECTOR targetQuat = XMQuaternionRotationNormal(axis, angle);

	// ���݂̃N�H�[�^�j�I���� slerp �ŕ��
	XMVECTOR currentQuat = XMLoadFloat4(&g_Player.Quaternion);
	XMVECTOR blended = XMQuaternionSlerp(currentQuat, targetQuat, 0.05f);

	XMStoreFloat4(&g_Player.Quaternion, blended);
}
//=============================================================================
// �v���C���[�̖��G��Ԃ�`�悷��֐�
// DX11_MODEL& model�F�Ώۂ̃��f��
// XMFLOAT4 diffuseCols�F���f���̃J���[
// int subsetNum�F�_�Ŏ��̃J���[
// note�F
//=============================================================================
void DrawApplyInvincibilityToModel(DX11_MODEL& model,const XMFLOAT4 diffuseCols[], int subsetNum)
{
	// �_�ł��銴�o
	const int blinkInterval = 1;
	//bool flashOn = (g_Player.muteki && (g_Player.mutekiTime % 2) == 1);
	bool flashOn = (g_Player.muteki && ((g_Player.mutekiTime / blinkInterval) % 2) == 1);

	for (int i = 0; i < subsetNum; i++)
	{
		SetModelDiffuse(&model, i, flashOn ? XMFLOAT4{ 0.7f,0.0f,0.0f,1.0f }
		: diffuseCols[i]);
	}
}
//=============================================================================
// �v���C���[�̖{�́ibody�j��`�悷��ۂ̃��[���h�s���g�ݗ��Ă�֐�
// @param mtxWorld�F������̃��[���h�s��
// @param mtxScl�F�X�P�[���s��
// @param mtxRot�F�I�C���[��]�s��
// @param quatMatrix�F�N�H�[�^�j�I����]�s��
// @param mtxTranslate�F���s�ړ��s��
// note�F�p�[�c�͕ʂ̊֐����g���đg�ݗ��Ă�
//=============================================================================
void BuildWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld,
	XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,
	XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate
)
{
	// ������
	mtxWorld = XMMatrixIdentity();
	// �X�P�[��
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// ��]�i�I�C���[���烉�W�A���j
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// �N�H�[�^�j�I��
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	// ���s�ړ�
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
}
//=============================================================================
// �v���C���[�̃p�[�c��`�悷��ۂ̃��[���h�s���g�ݗ��Ă�֐�
// @param mtxWorld�F������̃��[���h�s��
// @param mtxScl�F�X�P�[���s��
// @param mtxRot�F�I�C���[��]�s��
// @param quatMatrix�F�N�H�[�^�j�I����]�s��
// @param mtxTranslate�F���s�ړ��s��
// note�F�p�[�c�̎��ɂ�player.rot.y��XM_PI�𑫂��Ȃ�
//=============================================================================
void BuildPartsWorldTransforms(const PLAYER& player,
	XMMATRIX& mtxWorld,
	XMMATRIX& mtxScl,
	XMMATRIX& mtxRot,
	XMMATRIX& quatMatrix,
	XMMATRIX& mtxTranslate
)
{
	// ������
	mtxWorld = XMMatrixIdentity();
	// �X�P�[��
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// ��]�i�I�C���[���烉�W�A���j
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// �N�H�[�^�j�I��
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	// ���s�ړ�
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
}