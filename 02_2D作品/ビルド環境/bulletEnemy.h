//=============================================================================
//
// �G�l�~�[�o���b�g���� [bulletEnemy.h]
// Author : ����z��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_ENEMY_MAX		(8)		// �G�l�~�[�o���b�g��Max��
#define BULLET_ENEMY_SPEED		(5.0f)			// �o���b�g�̈ړ��X�s�[�h

// �e�̕���
enum
{
	BULLET_ENEMY_DIR_DOWN,
	BULLET_ENEMY_DIR_LEFT,
	BULLET_ENEMY_DIR_RIGHT,
	BULLET_ENEMY_DIR_UP,

	BULLET_ENEMY_DIR_MAX
};
// �o���b�g�\����
class BULLETENEMY
{
public:
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int					dir;				// �����i0:�� 1:�E 2:�� 3:���j
	int					atk;				// �U����

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBulletEnemy(void);
void UninitBulletEnemy(void);
void UpdateBulletEnemy(void);
void DrawBulletEnemy(void);

BULLETENEMY *GetBulletEnemy(void);
void SetBulletEnemy(XMFLOAT3 pos, int bossPatternAtk);
BOOL GetEnemyBulletAllLostFlg(void);
void SetEnemyBulletAllLostFlg(BOOL enemyBulletAllLostFlg);
