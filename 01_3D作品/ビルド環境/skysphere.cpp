//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : ����z��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "skysphere.h"
#include "debugproc.h"
#include "shadow.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SKYSPHERE		"data/MODEL/02_SKYSPHERE/skySphere.obj"		// �ǂݍ��ރ��f����
#define SPHERE_SIZE			(100)						// 
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SKYSPHERE g_SkySphere;				// �X�J�C�X�t�B�A
int g_SkySphere_load = 0;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitSkySphere(void)
{
	LoadModel(MODEL_SKYSPHERE, &g_SkySphere.model);
	g_SkySphere.load = TRUE;

	g_SkySphere.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SkySphere.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SkySphere.scl = XMFLOAT3(100.0f, 100.0f, 100.0f);

	g_SkySphere.spd  = 0.0f;			// �ړ��X�s�[�h�N���A

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&g_SkySphere.model, &g_SkySphere.diffuse[0]);

	XMFLOAT3 pos = g_SkySphere.pos;
	

	g_SkySphere.use = TRUE;			// TRUE:�����Ă�

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSkySphere(void)
{
	if (g_SkySphere.load)
	{
		UnloadModel(&g_SkySphere.model);
		g_SkySphere.load = FALSE;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSkySphere(void)
{
	// �X�t�B�A�𓮂���
	float sphereRot = 0.0005f;
	g_SkySphere.rot.z += sphereRot;
	if (g_SkySphere.rot.z > 1.0f)
	{
		g_SkySphere.rot.z = 0.0f;
	}

#ifdef _DEBUG

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSkySphere(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);


	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_SkySphere.scl.x, g_SkySphere.scl.y, g_SkySphere.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_SkySphere.rot.x, g_SkySphere.rot.y, g_SkySphere.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_SkySphere.pos.x, g_SkySphere.pos.y, g_SkySphere.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_SkySphere.mtxWorld, mtxWorld);

	// �����̐ݒ�
	SetRimLight(1);

	// ���f���`��
	DrawModel(&g_SkySphere.model);

	SetRimLight(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
SKYSPHERE* GetSkySphere()
{
	return &g_SkySphere;
}
