//=============================================================================
//
// ����Ɏg�p����֐� [weapon.cpp]
// Author : ����z��
//
//=============================================================================
#pragma once
#include <DirectXMath.h>
#include "weapon.h"

using namespace DirectX;
//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//*****************************************************************************
// �G���~�[�g�֐�
// p0�F�v���C���[�̍��W
// m0�F�n�_�ڐ�
// p1�F�I�_
// m1�F�I�_�ڐ�
//*****************************************************************************
XMFLOAT3 HermiteInterp(
    const XMFLOAT3& p0,   // �n�_
    const XMFLOAT3& m0,   // �n�_�ڐ�
    const XMFLOAT3& p1,   // �I�_
    const XMFLOAT3& m1,   // �I�_�ڐ�
    float t)              // �i��
{
    float t2 = t * t;
    float t3 = t2 * t;
    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    XMVECTOR P0 = XMLoadFloat3(&p0);
    XMVECTOR M0 = XMLoadFloat3(&m0);
    XMVECTOR P1 = XMLoadFloat3(&p1);
    XMVECTOR M1 = XMLoadFloat3(&m1);

    XMVECTOR result =
        XMVectorScale(P0, h00) +
        XMVectorScale(M0, h10) +
        XMVectorScale(P1, h01) +
        XMVectorScale(M1, h11);

    XMFLOAT3 out;
    XMStoreFloat3(&out, result);
    return out;
}
