//=============================================================================
//
// ���𓊂���Ƃ��̃}�[�J�[���� [throwMarker.h]
// Author : ����z��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitThrowMarker(void);
void UninitThrowMarker(void);
void UpdateThrowMarker(void);
void DrawThrowMarker(void);

void ShowThrowMarker(const XMFLOAT3& pos);
void HideThrowMarker(void);