//=============================================================================
//
// �{�X�O�̉�b��ʏ��� [bossbefore.h]
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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBossBefore(void);
void UninitBossBefore(void);
void UpdateBossBefore(void);
void DrawBossBefore(void);

BOOL GetBossBeforeFlg(void);
void SetBossBeforeFlg(BOOL bossBeforeFlg);
