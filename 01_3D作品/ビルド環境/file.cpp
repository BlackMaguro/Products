//=============================================================================
//
// ファイル処理 [file.cpp]
// Author : 白井陽大
//
//=============================================================================

/******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "file.h"
#include "score.h"


/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/



/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/



/*******************************************************************************
マクロ定義
*******************************************************************************/



/*******************************************************************************
* グローバル変数
*******************************************************************************/
SETTINGSDATA	settingsData;	// 設定情報作成場所

/*******************************************************************************
関数名:	void SaveSettings( void )
引数:	void
戻り値:	void
説明:	設定情報データを作成し、ファイルへ出力する
*******************************************************************************/
void SaveSettings()
{
	settingsData.volume = GetVolume();
	settingsData.volumeNum = GetVolumeNum();

	FILE* fpSettings = fopen("settings.bin", "wb");		// ファイルをバイナリ読み込みモードでOpenする
	printf("\n設定情報のセーブ開始・・・");

	// ファイルが無かったら無視（本来なら生成すべき？）
	if (fpSettings != NULL) 
	{
		fwrite(&settingsData, 1, sizeof(SETTINGSDATA), fpSettings);
		fclose(fpSettings);
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}
}

/*******************************************************************************
関数名:	void LoadSETTINGS( void )
引数:	void
戻り値:	void
説明:	設定情報データをファイルから読み込む
*******************************************************************************/
void LoadSettings() {

	FILE* fpSettings = fopen("settings.bin", "rb");		// ファイルをバイナリ読み込みモードでOpenする
	printf("\n設定情報のロード開始・・・");

	// ファイルが無かったら無視（本来なら生成すべきかも）
	if (fpSettings != NULL)
	{
		fread(&settingsData, 1, sizeof(SETTINGSDATA), fpSettings);
		fclose(fpSettings);
		printf("終了！\n");
	}
	else
	{
		printf("設定ファイルが存在しません\n");
		// 初めてゲームを開いた場合1.0fを設定（タイトルで音を鳴らす）
		settingsData.volume = SOUND_VOLUME_MAX;
		settingsData.volumeNum = SETTINGS_VOLUME_FRONT_MAX;
	}	
	SetSoundVolume(settingsData.volume);
	SetVolumeNum(settingsData.volumeNum);
}

//=============================================================================
// 設定ファイルがあるかのチェック
//=============================================================================
int CheckSettingsFile()
{
	FILE* fpSettings;

	printf("\n設定情報データのチェック開始");
	fpSettings = fopen("settings.bin", "rb");

	int ans = 0;
	if (fpSettings != NULL)
	{
		// 設定データが存在している
		ans = 1;
	}
	else
	{
		// 設定データが存在していない
		ans = 0;
	}
	return ans;
}