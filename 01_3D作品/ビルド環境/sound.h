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
// �}�N����`
//*****************************************************************************
#define SOUND_VOLUME_MAX			(1.0f)		// ���ۂ̃{�����[���ő�l 

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_00_TITLE,				// BGM0�i�^�C�g���j
	SOUND_LABEL_BGM_01_TUTORIAL,			// BGM1�i�`���[�g���A���j
	SOUND_LABEL_BGM_02_INGAME,				// BGM2�i�Q�[���{�ҁj
	SOUND_LABEL_BGM_03_RESULT,				// BGM3�i���U���g��ʁj
	SOUND_LABEL_BGM_04_GAMEOVER,			// BGM4�i�Q�[���I�[�o�[��ʁj
	SOUND_LABEL_BGM_05_SETTINGS,			// BGM5�i�ݒ��ʁj
	SOUND_LABEL_SE_00_KEYUPDOWN,			// SE00�i�J�[�\���̏㉺�L�[���������Ƃ��j
	SOUND_LABEL_SE_01_ENTER,				// SE01�iENTER���������Ƃ��j
	SOUND_LABEL_SE_02_GUNSHOT,				// SE02�i�e�𔭎Ԃ����Ƃ��j
	SOUND_LABEL_SE_03_BULLETTOOBJ,			// SE03�i�e���I�u�W�F�N�g�ɓ��������Ƃ��j
	SOUND_LABEL_SE_04_PLAYERFFROMDAMAGE,	// SE04�i�v���C���[���_���[�W���󂯂��Ƃ��j
	SOUND_LABEL_SE_05_TIMELOW,				// SE05�i���Ԃ����Ȃ��Ȃ����Ƃ��j
	SOUND_LABEL_SE_06_PERSPECTIVECHANGE,	// SE06�i���_��ύX�����Ƃ��j
	SOUND_LABEL_SE_07_GETMONEY,				// SE07�i������ɓ��ꂽ�Ƃ��j
	SOUND_LABEL_SE_08_CHANGEWEAPON,			// SE08�i��������ւ����Ƃ��j
	SOUND_LABEL_SE_09_WOODENSWORD,			// SE09�i�ؓ��Œ@�����Ƃ��j
	SOUND_LABEL_SE_10_THROWBOMB,			// SE10�i���e�𓊂����Ƃ��j
	SOUND_LABEL_SE_11_EXPLOSIONBOMB,		// SE11�i���e�����������Ƃ��j

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

