//=============================================================================
//
// �o���b�g���� [bullet.h]
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
#define BULLET_MAX		(5)		// �o���b�g��Max��
#define BULLET_SPEED	(6.0f)		// �o���b�g�̈ړ��X�s�[�h

// �e�̕���
enum
{
	BULLET_DIR_DOWN,
	BULLET_DIR_LEFT,
	BULLET_DIR_RIGHT,
	BULLET_DIR_UP,

	BULLET_DIR_MAX
};
// �o���b�g�\����
class BULLET
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
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(XMFLOAT3 pos, int dir);


