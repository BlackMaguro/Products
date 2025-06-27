//=============================================================================
//
// 武器に使用する関数 [weapon.cpp]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include <DirectXMath.h>
#include "weapon.h"

using namespace DirectX;
//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//*****************************************************************************
// エルミート関数
// p0：プレイヤーの座標
// m0：始点接線
// p1：終点
// m1：終点接線
//*****************************************************************************
XMFLOAT3 HermiteInterp(
    const XMFLOAT3& p0,   // 始点
    const XMFLOAT3& m0,   // 始点接線
    const XMFLOAT3& p1,   // 終点
    const XMFLOAT3& m1,   // 終点接線
    float t)              // 進捗
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
