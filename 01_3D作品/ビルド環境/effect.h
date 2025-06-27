//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : ����z��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	int			passedFrame;	// �o�߂����t���[����
	int			existFrame;		// �p������t���[�����i���܂ő��݂��邩�j
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} EXPLOSION;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

void ShowExplosion(const XMFLOAT3& pos,int durationTime);
void HideExplosion(void);