//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ����z��
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_00_TITLE,			// BGM0�i�^�C�g���j
	SOUND_LABEL_BGM_01_SETTINGS,		// BGM1�i�����Ă��j
	SOUND_LABEL_BGM_02_PROLOGUE,		// BGM2�i�v�����[�O�j
	SOUND_LABEL_BGM_03_TUTORIAL,		// BGM3�i�`���[�g���A���j
	SOUND_LABEL_BGM_04_GAMEMODE,		// BGM4�i�Q�[���{�ҁj
	SOUND_LABEL_BGM_05_BOSSBEFORE,		// BGM5�i�{�X�O�̉�b�j
	SOUND_LABEL_BGM_06_BOSS,			// BGM6�i�{�X��j
	SOUND_LABEL_BGM_07_GAMEOVER,		// BGM7�i�Q�[���I�[�o�[�j
	SOUND_LABEL_BGM_08_EPPILOGUE,		// BGM8�i�G�s���[�O�j
	SOUND_LABEL_BGM_09_RESULT,			// BGM9�i���U���g�j
	SOUND_LABEL_SE_00_MOUSEUPDOWN,		// SE0�i�J�[�\���̏㉺�L�[���������Ƃ��j
	SOUND_LABEL_SE_01_ENTER,			// SE1�iEnter���������Ƃ��j
	SOUND_LABEL_SE_02_MESSAGECLICK,		// SE2�i���b�Z�[�W��Enter���������Ƃ��j
	SOUND_LABEL_SE_03_PLAYERBULLET,		// SE3�i�v���C���[���e�𔭎˂����Ƃ��j
	SOUND_LABEL_SE_04_BOSSFIRE,			// SE4�i�{�X���e�𔭎˂����Ƃ��j
	SOUND_LABEL_SE_05_PLAYERDAMAGE,		// SE5�i�v���C���[���_���[�W���󂯂��Ƃ��j
	SOUND_LABEL_SE_06_DOORENTER,		// SE6�i����Enter���������Ƃ��j
	SOUND_LABEL_SE_07_ENEMYBULLETHIT,	// SE7�i�G�l�~�[�ɒe�����������Ƃ��j
	SOUND_LABEL_SE_08_FKEY,				// SE8�iF�L�[�������ꂽ�Ƃ��j
	SOUND_LABEL_SE_09_JUMP,				// SE9�i�v���C���[�̃W�����v���j
	SOUND_LABEL_SE_10_TIMELOW,			// SE10�i���Ԃ����Ȃ��Ȃ����Ƃ��j
	SOUND_LABEL_SE_11_PLAYERHPHEAL,		// SE11�i�v���C���[��HP���񕜂����Ƃ��j
	SOUND_LABEL_SE_12_BOSSHPHEAL,		// SE12�i�{�X��HP���񕜂����Ƃ��j
	SOUND_LABEL_SE_13_DASH,				// SE13�i�v���C���[���_�b�V�������Ƃ��j
	SOUND_LABEL_SE_14_SAVEDATAMAKE,		// SE14�i�Z�[�u�f�[�^���쐬�����Ƃ��j
	SOUND_LABEL_SE_15_TITLESTART,		// SE15�i�͂��߂ăQ�[�����N�������Ƃ��i���g�p�j�j
	SOUND_LABEL_SE_16_BOSSVOICE,		// SE16�i�{�X�̙��K�j

	SOUND_LABEL_MAX,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SOUND_VOLUME_MAX			(1.0f)		// ���ۂ̃{�����[���ő�l 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);