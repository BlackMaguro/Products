//=============================================================================
//
// �e���ˏ��� [bomb.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "weapon.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const int	MAX_BOMB	= 3;		// ���e�ő吔
static const float	GRAVITY		= 9.8f;		// �d��
static const float	BOMB_SIZE	= 5.0f;		// �����蔻��̑傫��
static const float	BOMB_LAUNCH_UP_ANGLE = XM_PI / 10;		// ���e�𓊂���p�x
static const float	BOMB_LAUNCH_SPEED = 100.0f;				// ���e�̏����x

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class BOMB : public WEAPON
{
public:
	XMFLOAT3	vel;				// ���x�x�N�g��
	XMFLOAT3    startPos;			// ���ˎ��̍��W
	XMFLOAT3    fallPoint;			// �����n�_
	float       timer;				// �o�ߎ��ԁi�b�j
	float       elevation;			// �p�i���W�A���j
	float       explosionRadius;	// �������锼�a
	BOOL		isApex;				// ���_�ɓ��B������
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBomb(void);
void UninitBomb(void);
void UpdateBomb(void);
void DrawBomb(void);

int SetBomb(XMFLOAT3 pos, XMFLOAT3 rot);
int GetBombNum(void);
void Explode(int index);
BOOL checkBombToEnemy(int index);
BOMB *GetBomb(void);

