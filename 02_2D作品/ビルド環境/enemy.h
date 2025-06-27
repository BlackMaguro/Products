//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : ����z��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(18)		// �G�l�~�[��Max�l��

#define ENEMY_G_MAX_HP	(30)	// �G�l�~�[��HP1
#define ENEMY_P_MAX_HP	(50)	// �G�l�~�[��HP2
#define ENEMY_R_MAX_HP	(100)	// �G�l�~�[��HP3

#define ENEMY_G_MAX_ATK	(10)	// �G�l�~�[�̍U����1
#define ENEMY_P_MAX_ATK	(20)	// �G�l�~�[�̍U����2
#define ENEMY_R_MAX_ATK	(30)	// �G�l�~�[�̍U����3

enum 
{
	ENEMY_TYPE_G,		// �G�l�~�[�^�C�v�i�΁j
	ENEMY_TYPE_P,		// �G�l�~�[�^�C�v�i���j
	ENEMY_TYPE_R,		// �G�l�~�[�^�C�v�i�ԁj
	ENEMY_TYPE_GB,		// �G�l�~�[�^�C�v�i�΋ʁj
	ENEMY_TYPE_PB,		// �G�l�~�[�^�C�v�i���ʁj
	ENEMY_TYPE_RB,		// �G�l�~�[�^�C�v�i�ԋʁj
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

class ENEMY
{
public:

	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x
	float		colPos;			// �Փ˂����Ƃ��̍��W
	int			colDir;			// �Փ˂����Ƃ��̌����i0:�� 1:�E 2:�� 3:���j

	float		time;			// ���`��ԗp
	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	int			hp;				// �G�l�~�[��HP
	int			atk;			// �G�l�~�[�̍U����
	int			type;			// �G�l�~�[�̎�ށi0:�΁A1:���A2:�ԁA3:�΋ʁA4:���ʁA5:�ԋʁj
	BOOL		ballFlg;		// �G�l�~�[���ʂ̏�Ԃ�

	//INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	//int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	//float				move_time;			// ���s����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

int GetEnemyCount(void);
BOOL CollisionEnemyToMapchip(XMFLOAT3 vector, int i);
void SetCollisionEnemyPos(int x, int y, int i, int colDir);



