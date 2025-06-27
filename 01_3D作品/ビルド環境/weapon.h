//=============================================================================
//
// ������Ǘ�����N���X [weapon.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "sound.h"
#include "score.h"
#include "meshobj.h"
#include "game.h"
#include "collision.h"
#include "meshfield.h"
#include <DirectXMath.h>
#include <vector>
//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �N���X��`
//*****************************************************************************
class WEAPON
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
	float		size;			// �T�C�Y
	BOOL		use;			// �g�p���Ă��邩�ǂ���


};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
XMFLOAT3 HermiteInterp(const XMFLOAT3& p0, const XMFLOAT3& m0, 
					   const XMFLOAT3& p1, const XMFLOAT3& m1, float t);