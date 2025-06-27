//=============================================================================
//
// �t�@�C������ [file.cpp]
// Author : 
//
//=============================================================================

/******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "file.h"
#include "score.h"


/*******************************************************************************
* �}�N����`
*******************************************************************************/



/*******************************************************************************
* �\���̒�`
*******************************************************************************/



/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/



/*******************************************************************************
�}�N����`
*******************************************************************************/



/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
SAVEDATA		save;			// �Z�[�u�f�[�^�쐬�ꏊ
SETTINGSDATA	settingsData;	// �ݒ���쐬�ꏊ
FIELD	field;					// �t�B�[���h���쐬�ꏊ

/*******************************************************************************
�֐���:	void SaveData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���쐬���A�t�@�C���֏o�͂���
*******************************************************************************/
void SaveData(void)
{
	{	// �v���C���[�f�[�^���Z�[�u����
		PLAYER *player = GetPlayer();

		// �v���C���[�̐l�����Z�[�u����
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			save.player[i] = player[i];
		}
	}

	{	// �G�l�~�[�f�[�^���Z�[�u����
		ENEMY *enemy = GetEnemy();

		// �G�l�~�[�̐l�����Z�[�u����
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			save.enemy[i] = enemy[i];
		}
	}


	// �X�R�A�f�[�^���Z�[�u����
	save.score = GetScore();


	// �Z�[�u�f�[�^�̃`�F�b�N�T�������߂�
	{
		char *adr = (char *)&save;	// �f�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int  sum = 0;				// �v�Z����`�F�b�N�T��

		save.sum = 0;				// �Z�[�u�f�[�^���̃`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];
		}

		save.sum = sum;
	}



	// SAVEDATA�\���̂��ƑS�����t�@�C���ɏo�͂���
	FILE *fp;

	printf("\n�Z�[�u�J�n�E�E�E");
	fp = fopen("savedata.bin", "wb");			// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
	{	
		fwrite(&save, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

}


/*******************************************************************************
�֐���:	void LoadData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���t�@�C������ǂݍ���
*******************************************************************************/
void LoadData(void)
{
	PLAYER *player = GetPlayer();	// �v���C���[�̃A�h���X���擾����
	ENEMY  *enemy  = GetEnemy();	// �G�l�~�[�̃A�h���X���擾����

	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen("savedata.bin", "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}


	// �Z�[�u�f�[�^�̃`�F�b�N�T���������Ă��邩���ׂ�
	{
		char *adr = (char *)&save;	// �f�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int  sum = 0;				// �v�Z����`�F�b�N�T��
		int  org = save.sum;		// �Z�[�u�f�[�^���̃`�F�b�N�T��

		save.sum = 0;				// �Z�[�u�f�[�^���̃`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];
		}

		// ���X�̃`�F�b�N�T���ƍČv�Z�����`�F�b�N�T�������������ׂĂ���
		if (sum != org)
		{
			// �f�[�^�������񂳂�Ă���I
			return;
		}
	}



	// �v���C���[�̐l�����Aplayer���[�N�֖߂�
	{	// �v���C���[�f�[�^�����[�h����
		PLAYER *player = GetPlayer();

		// �v���C���[�̐l�������[�h����
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			player[i] = save.player[i];
		}
	}

	{	// �G�l�~�[�f�[�^�����[�h����
		ENEMY *enemy = GetEnemy();

		// �G�l�~�[�̐l�������[�h����
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			enemy[i] = save.enemy[i];
		}
	}

	// �X�R�A�f�[�^�����[�h����
	SetScore(save.score);

}

/*******************************************************************************
�֐���:	void SaveSettings( void )
����:	void
�߂�l:	void
����:	�ݒ���f�[�^���쐬���A�t�@�C���֏o�͂���
*******************************************************************************/
void SaveSettings()
{
	settingsData.volume = GetVolume();
	settingsData.volumeNum = GetVolumeNum();

	FILE* fpSettings = fopen("settings.bin", "wb");		// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����
	printf("\n�ݒ���̃Z�[�u�J�n�E�E�E");

	// �t�@�C�������������疳���i�{���Ȃ琶�����ׂ��H�j
	if (fpSettings != NULL) 
	{
		fwrite(&settingsData, 1, sizeof(SETTINGSDATA), fpSettings);
		fclose(fpSettings);
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}
}

/*******************************************************************************
�֐���:	void LoadSETTINGS( void )
����:	void
�߂�l:	void
����:	�ݒ���f�[�^���t�@�C������ǂݍ���
*******************************************************************************/
void LoadSettings() {

	FILE* fpSettings = fopen("settings.bin", "rb");		// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����
	printf("\n�ݒ���̃��[�h�J�n�E�E�E");

	// �t�@�C�������������疳���i�{���Ȃ琶�����ׂ��H�j
	if (fpSettings != NULL)
	{
		fread(&settingsData, 1, sizeof(SETTINGSDATA), fpSettings);
		fclose(fpSettings);
		printf("�I���I\n");
	}
	else
	{
		printf("�ݒ�t�@�C�������݂��܂���\n");
		// ���߂ăQ�[�����J�����ꍇ1.0f��ݒ�i�^�C�g���ŉ���炷�j
		settingsData.volume = SOUND_VOLUME_MAX;
		settingsData.volumeNum = SETTINGS_VOLUME_FRONT_MAX;
	}	
	SetSoundVolume(settingsData.volume);
	SetVolumeNum(settingsData.volumeNum);
}

//=============================================================================
// �ݒ�t�@�C�������邩�̃`�F�b�N
//=============================================================================
int CheckSettingsFile()
{
	FILE* fpSettings;

	printf("\n�ݒ���f�[�^�̃`�F�b�N�J�n");
	fpSettings = fopen("settings.bin", "rb");

	int ans = 0;
	if (fpSettings != NULL)
	{
		// �ݒ�f�[�^�����݂��Ă���
		ans = 1;
	}
	else
	{
		// �ݒ�f�[�^�����݂��Ă��Ȃ�
		ans = 0;
	}
	return ans;
}