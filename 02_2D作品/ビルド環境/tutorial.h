//=============================================================================
//
// �`���[�g���A����ʏ��� [tutorial.h]
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


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


class TUTORIAL
{
public:
	//BOOL point;		// �`���[�g���A���̃|�C���g�ɓ��B�������̃t���O
	//XMFLOAT3	pos;		// �|���S���̍��W
	//float		w, h;		// ���ƍ���
	//int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�
	//float		scrl;		// �X�N���[��
	//XMFLOAT3	old_pos;	// �P�t���O�̃|���S���̍��W
	//float		scrl2;		// �X�N���[��

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);

BOOL GetPLayerToPoint(void);
void SetPLayerToPoint(BOOL PointToPlayer);
void CheckPlayerTutorialPoint(float playerPosX);
int GetTutorialMessageCount(void);
TUTORIAL* GetTutorial(void);