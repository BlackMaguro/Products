//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : ����z��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(6)			// �e�ő吔�i���{���o�[�z��j
#define	BULLET_SIZE		(5.0f)		// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class BULLET
{
public:
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	XMFLOAT4	diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F


	BOOL				load;
	DX11_MODEL			model;	// ���f�����
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	float		fDepthZ;		// ���s��
	float		size;			// ���s��
	BOOL		use;			// �g�p���Ă��邩�ǂ���


};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BULLET *GetBullet(void);

