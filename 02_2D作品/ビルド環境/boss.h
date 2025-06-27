//=============================================================================
//
// �{�X���� [boss.h]
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
#define BOSS_MAX		(6)		// �{�X��Max�l��
#define BOSS_MAX_HP		(300)	// �{�X��MaxHP
#define BOSS_MAX_ATK	(50)	// �{�X��Max�U����


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

class BOSS
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

	float		time;			// ���`��ԗp
	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	int			hp;				// BOSS��HP
	int			atk;			// BOSS�̍U����
	BOOL		moveFlg;		// BOSS�������Ă悢���̃t���O
	int			patternAtk;		// BOSS���U���p�^�[���i���o�[

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(void);

BOSS* GetBoss(void);
BOOL GetBossAtkFlg(void);
void SetBossAtkFlg(BOOL bossAtkFlg);

int GetBossCount(void);
int GetBossPatternAtk(void);




