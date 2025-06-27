//=============================================================================
//
// �t�B�[���h���� [field.h]
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
#define FIELD_WIDTH					(8000)			// �ʏ�X�e�[�W
#define FIELD_TUTORIAL_WIDTH		(2000)			// �ʏ�X�e�[�W
#define FIELD_BOSS_WIDTH			(960)			// �{�X�X�e�[�W
#define FIELD_HEIGHT				(640)			// �t�B�[���h�̏c��
#define MAPCHIP_WIDTH				(64)			// �}�b�v�`�b�v�̉���
#define MAPCHIP_HEIGHT				(64)			// �}�b�v�`�b�v�̏c��

#define SCREEN_SIZE_MAPCHIP_WIDTH			(SCREEN_WIDTH/MAPCHIP_WIDTH)	// �X�N���[���ɑ΂��鉡���̃`�b�v���i�l�̓`�b�v���j


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class FIELD
{
public:
	XMFLOAT3	pos;		// �|���S���̍��W
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�
	int			fieldNo;	// �g�p���Ă���t�B�[���h�̔ԍ�

	float		scrl;		// �X�N���[��
	XMFLOAT3	old_pos;	// �P�t���O�̃|���S���̍��W
	BOOL	goalFlg;		// �S�[���ɂ��邩�̃t���O

};

enum
{
	FIELD_LABEL_NONE,
	FIELD_LABEL_BLOCK,

	FIELD_LABEL_MAX,

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);
void CheckPlayerToChipNum(float y, float x);
char GetField(int y, int x);
int CheckField(int y, int x);

FIELD* GetField(void);



