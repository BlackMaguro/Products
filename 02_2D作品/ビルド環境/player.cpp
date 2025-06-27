//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "tutorial.h"
#include "input.h"
#include "bg.h"
#include "field.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// �L�����T�C�Y(200/2)
#define TEXTURE_HEIGHT				(64)	// 
#define TEXTURE_MAX					(8)		// �e�N�X�`���̐�
#define FRAME_RATE					(60)	// �t���[����


#define PLAYER_MAX_HP				(100)	// �v���C���[�̍ő�HP

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// �W�����v�̍���

#define JUMP_Y						(3)	// �W�����v�̔{��
#define EDGE_MAX					(4)	// Edge�i�Ӂj�̍ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	//"data/TEXTURE/char01_old.png",
	"data/TEXTURE/player_chara_walk.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/bar_white.png",
	"data/TEXTURE/HPBar.png",
	"data/TEXTURE/item_1.png",
	"data/TEXTURE/player_face_normal_HP_1.png",
	"data/TEXTURE/player_face_frame.png",
	"data/TEXTURE/instructions.png",
};


static BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static BOOL		g_bossFlg = FALSE;			// �{�X�킩���f����t���O
static BOOL		g_instructionFlg = FALSE;	// ������@���o�������f����t���O

static PLAYER	g_Player[PLAYER_MAX];		// �v���C���[�\����
static int		g_PlayerCount = PLAYER_MAX;	// �����Ă�v���C���[�̐�

static int      g_jumpCnt = 0;
static int		g_TimeCount = 0;
static int		g_MutekiTimeCount = 0;

static float    g_gravity = 1.0f;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};
FIELD* field = GetField();


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_PlayerCount = 0;						// �����Ă�v���C���[�̐�

	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_PlayerCount++;
		g_Player[i].use = TRUE;
		//g_Player[i].pos = XMFLOAT3(7764.0f, 448.0f, 0.0f);	// ���S�_����\��
		g_Player[i].pos = XMFLOAT3(280.0f, 448.0f, 0.0f);	// ���S�_����\��
		if (GetMode() == MODE_TUTORIAL)
		{
			g_Player[i].pos = XMFLOAT3(100.0f, 448.0f, 0.0f);	// ���S�_����\��
		}
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].hp = PLAYER_MAX_HP;						// �v���C���[��HP
		//if (GetBossFlg() == TRUE) g_Player[i].hp = 20;		// �f�o�b�O�p
		g_Player[i].muteki = FALSE;							// ���G��Ԃ̃t���O
		g_Player[i].tutorialMove = TRUE;					// �`���[�g���A�����̈ړ��t���O
		g_Player[i].mutekiTime = 0;							// ���G����
		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;
		g_Player[i].healItem = 3;							// �񕜃A�C�e��

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Player[i].vector = XMINT2(0, 0);					// �ړ��ʃx�N�g��
		g_Player[i].colPos = 0.0f;							// �Փ˂����Ƃ��̍��W�p
		g_Player[i].colDir = EDGE_LEFT;						// �Փ˂����Ƃ��̌���


		g_Player[i].dir = CHAR_DIR_RIGHT;					// �E����
		g_Player[i].moving = FALSE;							// �ړ����t���O
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;

		// �W�����v�̏�����
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// ���g�p
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	g_TimeCount += 1;
	g_Player[0].mutekiTime += 1;
	if (g_Load == FALSE) return;
	g_PlayerCount = 0;				// �����Ă�v���C���[�̐�

	// 5�b�o�߂����疳�G�t���O��FALSE�ɂ���
	if (g_Player[0].mutekiTime > FRAME_RATE * 5)
	{
		g_Player[0].muteki = FALSE;
	}

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			g_PlayerCount++;		// �����Ă�v���C���[�̐�
			
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Player[i].pos;

			// ���g�p
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			// �W�����v�������Ƃ��Ɏc�����W�����v������
			pos_old.y += g_Player[i].jumpY;
			g_Player[i].offset[0] = pos_old;

			// �A�j���[�V����  
			if (g_Player[i].moving == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}

			// �L�[���͂ňړ� 
			{
				float speed = g_Player[i].move.x;
				float vertical = 0.0f;			// �c�̈ړ���
				float horizontal = 0.0f;		// ���̈ړ���
				float gravity = 8.0f;

				g_Player[i].moving = FALSE;
				g_Player[i].dash = FALSE;

				// �`���[�g���A�����œ���̏ꏊ�܂ŗ����瓮�����~�߂�
				if (g_Player[0].tutorialMove == TRUE)
				{
					// C�L�[�������ꂽ�Ƃ��_�b�V������
					if (GetKeyboardPress(DIK_C) || IsButtonPressed(0, BUTTON_A))
					{
						PlaySound(SOUND_LABEL_SE_13_DASH);
						speed *= 4;
						g_Player[i].dash = TRUE;
					}
					// �ʏ�}�b�v�ł͏�Ɖ��ɂ͌����Ȃ�
					if (GetBossFlg() == TRUE)
					{
						if (GetKeyboardPress(DIK_DOWN))
						{
							vertical += speed;
							g_Player[i].dir = CHAR_DIR_DOWN;
							g_Player[i].moving = TRUE;
						}
						else if (GetKeyboardPress(DIK_UP))
						{
							g_Player[i].dir = CHAR_DIR_UP;
							g_Player[i].moving = TRUE;
						}
						// �Q�[���p�b�h�ł̏���
						if (IsButtonPressed(0, BUTTON_DOWN))
						{
							g_Player[i].pos.y += speed;
							g_Player[i].dir = CHAR_DIR_DOWN;
							g_Player[i].moving = TRUE;
						}
						else if (IsButtonPressed(0, BUTTON_UP))
						{
							g_Player[i].pos.y -= speed;
							g_Player[i].dir = CHAR_DIR_UP;
							g_Player[i].moving = TRUE;
						}

					}

					if (GetKeyboardPress(DIK_RIGHT))
					{
						horizontal += speed;
						g_Player[i].dir = CHAR_DIR_RIGHT;
						g_Player[i].moving = TRUE;
					}
					else if (GetKeyboardPress(DIK_LEFT))
					{
						horizontal -= speed;
						g_Player[i].dir = CHAR_DIR_LEFT;
						g_Player[i].moving = TRUE;
					}

					// �Q�[���p�b�h�ňړ�����
					if (IsButtonPressed(0, BUTTON_RIGHT))
					{
						g_Player[i].pos.x += speed;
						g_Player[i].dir = CHAR_DIR_RIGHT;
						g_Player[i].moving = TRUE;
					}
					else if (IsButtonPressed(0, BUTTON_LEFT))
					{
						g_Player[i].pos.x -= speed;
						g_Player[i].dir = CHAR_DIR_LEFT;
						g_Player[i].moving = TRUE;
					}


					// �͋ƃW�����v����
					if (g_jumpCnt > 0)
					{
						vertical += g_jump[g_jumpCnt] * JUMP_Y;
						g_jumpCnt++;
						if (g_jumpCnt >= PLAYER_JUMP_CNT_MAX)
						{
							g_jumpCnt = 0;
						}
					}

					// J�L�[�������ꂽ�Ƃ��W�����v����
					if ((g_jumpCnt == 0) && (GetKeyboardTrigger(DIK_J)))
					{
						PlaySound(SOUND_LABEL_SE_09_JUMP);
						vertical += g_jump[g_jumpCnt];
						g_jumpCnt++;
					}

					// �񕜂���
					if ((GetMode() == MODE_GAME) || (GetMode() == MODE_BOSS))
					{
						if (GetKeyboardTrigger(DIK_H))
						{
							if (g_Player[i].healItem > 0)
							{
								PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
								g_Player[i].healItem -= 1;
								g_Player[i].hp += 100;
								if (g_Player[i].hp > 100) g_Player[i].hp = 100;
							}
						}
					}
					if ((GetMode() == MODE_TUTORIAL) && (GetTutorialMessageCount() >= 7))
					{
						if (GetKeyboardTrigger(DIK_H))
						{
							if (g_Player[i].healItem > 0)
							{
								PlaySound(SOUND_LABEL_SE_11_PLAYERHPHEAL);
								g_Player[i].healItem -= 1;
								g_Player[i].hp += 100;
								if (g_Player[i].hp > 100) g_Player[i].hp = 100;
							}
						}
					}


					// ����������o��
					if ((GetMode() == MODE_TUTORIAL) && (GetTutorialMessageCount() >= 11) && GetKeyboardTrigger(DIK_F))
					{
						PlaySound(SOUND_LABEL_SE_08_FKEY);
						g_instructionFlg = !g_instructionFlg;
					}
					else if((GetMode() == MODE_GAME) && GetKeyboardTrigger(DIK_F))
					{
						PlaySound(SOUND_LABEL_SE_08_FKEY);
						g_instructionFlg = !g_instructionFlg;
					}
				}

				// �W�����v�������H�i��������j
				//if (g_Player[i].jump == TRUE)
				//{
				//	float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
				//	float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);
				//	//g_Player[i].jumpY = y;

				//	vertical += y;

				//	g_Player[i].jumpCnt++;
				//	if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
				//	{
				//		g_Player[i].jump = FALSE;
				//		g_Player[i].jumpCnt = 0;
				//		g_Player[i].jumpY = 0.0f;
				//	}

				//}
				//// �W�����v�{�^���������H
				//else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_J)))
				//{
				//	g_Player[i].jump = TRUE;
				//	g_Player[i].jumpCnt = 0;
				//	g_Player[i].jumpY = 0.0f;
				//}
				// �W�����v�������H�i�����܂Łj


				// MAP�O�`�F�b�N
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// �ړ��ł���ꏊ���̃`�F�b�N�i�d�́j
				if (CollisionPlayerToMapchip(XMFLOAT3(0.0f, gravity, 0.0f), i) == FALSE)
				{
					// �ړ��ʂ𑫂�
					g_Player[i].pos.y += gravity;
				}
				else
				{
					// �ՓˑO�ɖ߂�
					g_Player[i].pos.y = g_Player[i].colPos;
				}

				// �ړ��ł���ꏊ���̃`�F�b�N�i�������j
				if (CollisionPlayerToMapchip(XMFLOAT3(horizontal, 0.0f, 0.0f), i) == FALSE)
				{
					// �ړ��ʂ𑫂�
					g_Player[i].pos.x += horizontal;
				}
				else
				{
					// �ՓˑO�ɖ߂�
					g_Player[i].pos.x = g_Player[i].colPos;
				}
				// �ړ��ł���ꏊ���̃`�F�b�N�i�c�����j
				if (CollisionPlayerToMapchip(XMFLOAT3(0.0f, vertical, 0.0f), i) == FALSE)
				{
					// �ړ��ʂ𑫂�
					g_Player[i].pos.y += vertical;
				}
				else
				{
					// �ՓˑO�ɖ߂�
					g_Player[i].pos.y = g_Player[i].colPos;
				}

				// �v���C���[�������ɂ��邩�̔���i�C�x���g�n�j
				CheckPlayerToChipNum(g_Player[i].pos.y, g_Player[i].pos.x);
				
				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// �v���C���[�̗����ʒu����t�B�[���h�̃X�N���[�����W���v�Z����
				field->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (field->pos.x < 0) field->pos.x = 0;												// ���[�܂ŗ������̏���
				if (field->pos.x > field->w - SCREEN_WIDTH) field->pos.x = field->w - SCREEN_WIDTH;	// �[�܂ŗ������̏���

				field->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (field->pos.y < 0) field->pos.y = 0;
				if (field->pos.y > field->h - SCREEN_HEIGHT) field->pos.y = field->h - SCREEN_HEIGHT;

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				if(GetMode() != MODE_BOSS)
				{
					ENEMY* enemy = GetEnemy();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (enemy[j].use == TRUE)
						{
							XMFLOAT3 posCollision = g_Player[i].pos;
							posCollision.y += g_Player[i].jumpY;
							BOOL ans = CollisionBB(posCollision, g_Player[i].w, g_Player[i].h,
								enemy[j].pos, enemy[j].w, enemy[j].h);
							// �������Ă���H
							if ((ans == TRUE) && (g_Player[i].muteki == FALSE))
							{
								// �����������̏���
								PlaySound(SOUND_LABEL_SE_05_PLAYERDAMAGE);
								g_Player[i].hp -= enemy[j].atk;
								g_Player[i].muteki = TRUE;
								g_Player[0].mutekiTime = 0;		// ���G���Ԃ̃��Z�b�g
								// HP���[���ɂȂ�����
								if (g_Player[i].hp <= 0)
								{
									g_Player[i].use = FALSE;
									SetMode(MODE_GAMEOVER);
								}
							}
						}
					}
				}

				BULLET* bullet = GetBullet();
				ENEMY* enemy = GetEnemy();

				// �o���b�g�����i�L�[�{�[�h�j
				if (GetKeyboardTrigger(DIK_SPACE))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					int bulletDir = g_Player[i].dir;	// �e�̕������m�肳����
					SetBullet(pos, bulletDir);
				}
				// �o���b�g�����i�Q�[���p�b�h�j 
				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					int bulletDir = g_Player[i].dir;	// �e�̕������m�肳����
					SetBullet(pos, bulletDir);
				}
				// �S�[����Enter�����������Ƃ�
				if (GetKeyboardTrigger(DIK_RETURN) && field->goalFlg == TRUE)
				{// Enter��������A�{�X�t���O�𗧂ĂăX�e�[�W��؂�ւ���
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// �Q�[����ʂɔ�΂�
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// �{�X��ʂɔ�΂�
					}
				}
				else if (IsButtonTriggered(0, BUTTON_START) && field->goalFlg == TRUE)
				{
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// �Q�[����ʂɔ�΂�
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// �{�X��ʂɔ�΂�
					}
				}
				else if (IsButtonTriggered(0, BUTTON_B) && field->goalFlg == TRUE)
				{
					PlaySound(SOUND_LABEL_SE_06_DOORENTER);
					if (GetMode() == MODE_TUTORIAL && (enemy->use == FALSE))
					{
						SetFade(FADE_OUT, MODE_GAME);		// �Q�[����ʂɔ�΂�
					}
					if (GetMode() == MODE_GAME)
					{
						SetFade(FADE_OUT, MODE_BOSSBEFORE);		// �{�X��ʂɔ�΂�
					}
				}

			}
		}
	}

	// ������Z�[�u����i�`���[�g���A���ƃ{�X��ł̓Z�[�u�s�j
	if (GetKeyboardTrigger(DIK_S) && (GetBossFlg() != TRUE) && (GetTutorialFlg() != TRUE))
	{
		PlaySound(SOUND_LABEL_SE_14_SAVEDATAMAKE);
		SaveData();
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player pos.x:%f,pos.y:%f,\nHP�F%d\ng_instructionFlg�F%d\n", g_Player[0].pos.x, g_Player[0].pos.y, g_Player[0].hp,g_instructionFlg);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	// �_���[�W���󂯂��Ƃ��ɓ_�ł�����
	float flashing = 1.0f;
	if (g_Player[0].muteki == TRUE)
	{
		flashing = (float)(g_TimeCount % 2);
	}
	else
	{
		flashing = 1.0f;
	}

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă��邩
		{

			// �v���C���[�̕��g��`��
			if (g_Player[i].dash)
			{	// �_�b�V�����������g����
				DrawPlayerOffset(i);
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;		// �v���C���[�̕\����
			float ph = g_Player[i].h;		// �v���C���[�̕\������


			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, flashing),
				g_Player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);


			// �v���C���[��HP�Q�[�W�i���j
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 220.0f;		// �Q�[�W�̕\���ʒuX
				float py = 460.0f;		// �Q�[�W�̕\���ʒuY
				float pw = 180.0f;		// �Q�[�W�̕\����
				float ph =  70.0f;		// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);


				// �v���C���[��HP�ɉ����Ē�����ύX�i�ԐF�̃o�[�j
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				pw = pw * ((float)g_Player[i].hp / PLAYER_MAX_HP);
				ph = 50.0f;		// �Q�[�W�̕\������

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			// �v���C���[��HP�o�[
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 130.0f;		// �Q�[�W�̕\���ʒuX
				float py = 440.0f;		// �Q�[�W�̕\���ʒuY
				float pw = 300.0f;		// �Q�[�W�̕\����
				float ph = 100.0f;		// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// HP�A�C�e���̕\��
			if (g_Player[i].healItem > 0)
			{
				int healItem = g_Player[i].healItem;

				for(int i = 0; i < healItem; i++)
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

					//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
					float px = 450.0f + i * 74.0f;		// �Q�[�W�̕\���ʒuX
					float py = 450.0f;		// �Q�[�W�̕\���ʒuY
					float pw = 64.0f;		// �Q�[�W�̕\����
					float ph = 64.0f;		// �Q�[�W�̕\������

					float tw = 1.0f;		// �e�N�X�`���̕�
					float th = 1.0f;		// �e�N�X�`���̍���
					float tx = 0.0f;		// �e�N�X�`���̍���X���W
					float ty = 0.0f;		// �e�N�X�`���̍���Y���W

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteLTColor(g_VertexBuffer,
						px, py, pw, ph,
						tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}
			}
			// �v���C���[�̕\��
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 25.0f;		// �Q�[�W�̕\���ʒuX
				float py = 440.0f;		// �Q�[�W�̕\���ʒuY
				float pw = 85.0f;		// �Q�[�W�̕\����
				float ph = 85.0f;		// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			// �v���C���[�̕\��̘g�i�t���[���j
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 10.0f;		// �Q�[�W�̕\���ʒuX
				float py = 425.0f;		// �Q�[�W�̕\���ʒuY
				float pw = 115.0f;		// �Q�[�W�̕\����
				float ph = 115.0f;		// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			// ������@�̕\��
			if(g_instructionFlg == TRUE)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 600;		// �Q�[�W�̕\���ʒuX
				float py = 20.0f;				// �Q�[�W�̕\���ʒuY
				float pw = 300.0f;				// �Q�[�W�̕\����
				float ph = 200.0f;				// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}


}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


// �����Ă�v���C���[�̐�
int GetPlayerCount(void)
{
	return g_PlayerCount;
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;		// �v���C���[�̕\����
		float ph = g_Player[no].h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �v���C���[�ƃ}�b�v�`�b�v�i�ǁj�Ƃ̓����蔻��
//=============================================================================
BOOL CollisionPlayerToMapchip(XMFLOAT3 move,int i)
{
	// �e���_�̏��
	int leftX = (int)((g_Player[i].pos.x + move.x) / MAPCHIP_WIDTH);
	int rightX = (int)((g_Player[i].pos.x + g_Player[i].w + move.x - 1.0f) / MAPCHIP_WIDTH);
	int topY = (int)((g_Player[i].pos.y + move.y) / MAPCHIP_HEIGHT);
	int bottomY = (int)((g_Player[i].pos.y + g_Player[i].h + move.y - 1.0f) / MAPCHIP_HEIGHT);

	XMINT2 edgeList[EDGE_MAX][2]
	{
		// ����
		{
			XMINT2(leftX,topY),
			XMINT2(leftX,bottomY)
		},
		// �E��
		{
			XMINT2(rightX,topY),
			XMINT2(rightX,bottomY)
		},
		// ���
		{
			XMINT2(leftX,topY),
			XMINT2(rightX,topY)
		},
		// ����
		{
			XMINT2(leftX,bottomY),
			XMINT2(rightX,bottomY)
		},
	};

	// �g�p����ӂ����߂�
	BOOL isCheckEdge[EDGE_MAX]
	{
		move.x < 0 ? TRUE : FALSE,		// �v���C���[�͍������B������ӂ͉E��
		move.x > 0 ? TRUE : FALSE,		// �v���C���[�͉E�����B������ӂ͍���
		move.y < 0 ? TRUE : FALSE,		// �v���C���[�͏�����B������ӂ͉���
		move.y > 0 ? TRUE : FALSE,		// �v���C���[�͉������B������ӂ͏��
	};

	for (int j = 0; j < (int)EDGE_MAX; j++)
	{
		if (isCheckEdge[j] == FALSE) continue;

		for (int y = edgeList[j][0].y; y <= edgeList[j][1].y; y++)
		{
			for (int x = edgeList[j][0].x; x <= edgeList[j][1].x; x++)
			{
				int ans = CheckField(y, x);

				switch (ans)
				{
				case 1:
					g_Player[i].colDir = j;
					SetCollisionPos(x,y,i,j);
					return TRUE;
				// ���ɐG��Ă����ꍇ
				case 2:
					field->goalFlg = TRUE;
				//	g_Player[i].colDir = j;
				//	SetCollisionPos(x, y, i, j);
				default:
					break;
				}
			}
		}
	}
	return FALSE;
}

// �ڐG�����ӏ��̏��̎擾
void SetCollisionPos(int x,int y,int i,int colDir)
{
	XMFLOAT3 mapchipPos = XMFLOAT3((float)x * MAPCHIP_WIDTH, (float)y * MAPCHIP_HEIGHT, 0.0f);

	switch (colDir)
	{
	// �v���C���[�̍���
	case EDGE_LEFT:
		g_Player[i].colPos = mapchipPos.x + MAPCHIP_WIDTH;
		break;
	// �v���C���[�̉E��
	case EDGE_RIGHT:
		g_Player[i].colPos = mapchipPos.x - g_Player[i].w;
		break;
	// �v���C���[�̏㑤
	case EDGE_TOP:
		g_Player[i].colPos = mapchipPos.y + MAPCHIP_HEIGHT;
		break;
	// �v���C���[�̉���
	case EDGE_BOTTOM:
		g_Player[i].colPos = mapchipPos.y - g_Player[i].h;
		break;
	
	default:
		break;
	}
}
