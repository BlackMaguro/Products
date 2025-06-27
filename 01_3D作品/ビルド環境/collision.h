//=============================================================================
//
// 当たり判定処理 [collision.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);
BOOL CollisionBB_3D(XMFLOAT3 mpos, float mw, float mh, float mz, XMFLOAT3 ypos, float yw, float yh, float yz);
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);
BOOL CollisionSector(XMFLOAT3 pos, XMFLOAT3 center, XMFLOAT3 direction, float radius, float angle);
BOOL CollisionSphereAABB(const XMFLOAT3& sC, float r, const XMFLOAT3& bC, const XMFLOAT3& h);

float dotProduct(XMVECTOR *v1, XMVECTOR *v2);
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2);
BOOL RayCast(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 pos0, XMFLOAT3 pos1, XMFLOAT3 *hit, XMFLOAT3 *normal);

//XMFLOAT3 CollisionSlideVector(const XMFLOAT3& moveDir, const XMFLOAT3& wallNormal);

