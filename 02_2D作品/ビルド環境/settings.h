//=============================================================================
//
// �ݒ��ʏ��� [settings.h]
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
#define SETTINGS_VOLUME_FRONT_MAX		(8)			// �ݒ��ʂ̌����ڏ�̃{�����[�����l 


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class SETTINGS
{
public:

	float volume;	// �T�E���h�̎��ۂ̉���
	int volumeNum;	// �T�E���h�̌����ڏ�̉���
	//float seVolume[SOUND_LABEL_MAX];	// �T�E���h�̉���

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSettings(void);
void UninitSettings(void);
void UpdateSettings(void);
void DrawSettings(void);

float GetVolume(void);
void SetSoundVolume(float volume);
int GetVolumeNum(void);
void SetVolumeNum(int volumeNum);
float CheckVolume(float volume);
