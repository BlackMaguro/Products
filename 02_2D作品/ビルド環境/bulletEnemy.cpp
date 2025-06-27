//=============================================================================
//
// �o���b�g���� [bulletEnemy.cpp]
// Author : 
//
//=============================================================================
#include "bulletEnemy.h"
#include "main.h"
#include "enemy.h"
#include "boss.h"
#include "player.h"
#include "collision.h"
#include "sound.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// �e�̉��T�C�Y
#define TEXTURE_HEIGHT				(84)	// �e�̏c�T�C�Y
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define COOLDOWNTIME				(30)	// �N�[���_�E���̒l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/Bullet/bullet_enemy_01.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BOOL g_enemyBulletAllLostFlg = TRUE;		// ���˂����e�����ׂď��������ǂ����̊m�F�i�ŏ��͉����Ȃ�����TRUE�j
static BULLETENEMY	g_BulletEnemy[BULLET_ENEMY_MAX];	// �o���b�g�\����
float enemyBulletCooldown = (float) COOLDOWNTIME;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBulletEnemy(void)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		g_BulletEnemy[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_BulletEnemy[i].w     = TEXTURE_WIDTH;
		g_BulletEnemy[i].h     = TEXTURE_HEIGHT;
		g_BulletEnemy[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_BulletEnemy[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BulletEnemy[i].texNo = 0;

		g_BulletEnemy[i].countAnim = 0;
		g_BulletEnemy[i].patternAnim = 0;

		g_BulletEnemy[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
		g_BulletEnemy[i].dir = BULLET_ENEMY_DIR_DOWN;

		g_BulletEnemy[i].atk = (int)(BOSS_MAX_ATK * 0.5);			// �{�X�̍ő�U���͂̔���
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBulletEnemy(void)
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBulletEnemy(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// ���������o���b�g�̐�
	//int bulletLostCount = 0;			// �������o���b�g�̐�
	//BOOL activeBulletFlg = FALSE;		// �o���b�g�����邩�Ȃ���
	// �N�[���_�E���^�C��
	enemyBulletCooldown -= 1.0f;
	if (enemyBulletCooldown < 0) enemyBulletCooldown = (float)COOLDOWNTIME;

	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		if (g_BulletEnemy[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			//activeBulletFlg = TRUE;
			//g_enemyBulletAllLostFlg = FALSE;
			// �A�j���[�V����  
			g_BulletEnemy[i].countAnim++;
			if ((g_BulletEnemy[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_BulletEnemy[i].patternAnim = (g_BulletEnemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			XMVECTOR pos  = XMLoadFloat3(&g_BulletEnemy[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_BulletEnemy[i].move);
			pos += move;
			XMStoreFloat3(&g_BulletEnemy[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			BG* bg = GetBG();
			BOSS* boss = GetBoss();
			if (g_BulletEnemy[i].pos.y < (-g_BulletEnemy[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_BulletEnemy[i].use = false;
			}
			if (g_BulletEnemy[i].pos.y > (bg->h + g_BulletEnemy[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_BulletEnemy[i].use = false;
			}
			// ���S�ɂ����Ƃ��̏���
			if (g_BulletEnemy[i].pos.x < (boss->pos.x - (SCREEN_WIDTH * 0.5 + g_BulletEnemy[i].w * 0.5)))		// �e�̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_BulletEnemy[i].use = false;
			}
			if (g_BulletEnemy[i].pos.x > (boss->pos.x + (SCREEN_WIDTH * 0.5 + g_BulletEnemy[i].w * 0.5)))		// �e�̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_BulletEnemy[i].use = false;
			}

			PLAYER* player = GetPlayer();
			// �����蔻�菈��
			{
				// �{�X�킾�����ꍇ
				if (GetBossFlg() == TRUE)
				{
					// �v���C���[�̐��������蔻����s��
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[j].use == TRUE)
						{

							BOOL ans = CollisionBB(g_BulletEnemy[i].pos, g_BulletEnemy[i].w, g_BulletEnemy[i].h,
								player[j].pos, player[j].w, player[j].h);
							// �������Ă���Ƃ��i�e�j
							if ((ans == TRUE) && (player[0].muteki == FALSE))
							{
								// TODO�F����ǉ��\��
								//if (boss[j].hp > bossHP_old) PlaySound();
								if ((ans == TRUE) && (player[j].muteki == FALSE))
								{
									// �����������̏���
									PlaySound(SOUND_LABEL_SE_05_PLAYERDAMAGE);
									player[j].hp -= (int)(BOSS_MAX_ATK * 0.5);
									player[j].muteki = TRUE;
									player[j].mutekiTime = 0;		// ���G���Ԃ̃��Z�b�g
								}
								g_BulletEnemy[i].use = false;
								// �G�t�F�N�g����
								//SetEffect(boss[j].pos.x, boss[j].pos.y, 30);
								if (player[j].hp <= 0)
								{
									player[j].use = FALSE;
									SetMode(MODE_GAMEOVER);
								}
							}
						}
					}
				}
			}

			bulletCount++;
		}
	}

	//PrintDebugProc("bulletCount:%d,\nbulletLostCount:%d,\n", bulletCount, bulletLostCount);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBulletEnemy(void)
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
	BOSS* boss = GetBoss();

	for (int i = 0; i < BULLET_ENEMY_MAX; i++)
	{
		if (g_BulletEnemy[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_BulletEnemy[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_BulletEnemy[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_BulletEnemy[i].w;		// �o���b�g�̕\����
			float ph = g_BulletEnemy[i].h;		// �o���b�g�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �p�^�[��2�̂Ƃ�����傫������
			if (boss[4].patternAtk == 1 || boss[4].patternAtk == 3)
			{
				pw = (float)(g_BulletEnemy[i].w * 1.5);		// �o���b�g�̕\����
				ph = (float)(g_BulletEnemy[i].h * 1.5);		// �o���b�g�̕\������
			}

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_BulletEnemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLETENEMY * GetBulletEnemy(void)
{
	return &g_BulletEnemy[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBulletEnemy(XMFLOAT3 pos,int bossPatternAtk)
{
	//PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();
	XMFLOAT3 atkPos = pos;
	int enemyBulletMAX = BULLET_ENEMY_MAX;
	// �e�̏����ݒ�
	if (bossPatternAtk == 0 || bossPatternAtk == 1) enemyBulletMAX = 1;

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < enemyBulletMAX; i++)
	{
		if (enemyBulletCooldown == 0.0f)
		{
			// �����_���l�̎擾
			int random = GetRand(1, 9);
			if (g_BulletEnemy[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
			{
				g_BulletEnemy[i].use = TRUE;			// �g�p��Ԃ֕ύX����
				g_BulletEnemy[i].pos = atkPos;			// ���W���Z�b�g

				// �{�X�̍U���p�^�[���ɂ���Ēe�̓�����ω�������
				// �p�^�[��1�F�΂�f���i���j
				if (bossPatternAtk == 0)
				{
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// �p�^�[��2�F�΂�f���i��j
				else if (bossPatternAtk == 1)
				{
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// �p�^�[��3�F�΂��~�炷
				else if (bossPatternAtk == 2)
				{
					// �����Əo���ʒu�̕ύX
					g_BulletEnemy[i].pos.y -= 200.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				// �p�^�[��3�F�΂��~�炷�i��j
				else if (bossPatternAtk == 3)
				{
					// �����Əo���ʒu�̕ύX
					g_BulletEnemy[i].pos.y -= 200.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, BULLET_ENEMY_SPEED, 0.0f);
				}
				else if (bossPatternAtk == 4)
				{
					// �����Əo���ʒu�̕ύX
					g_BulletEnemy[i].pos.y -= 300.0f;
					g_BulletEnemy[i].pos.x = (float)(50 + random * 100);
					g_BulletEnemy[i].move = XMFLOAT3(0.0f, 8.0f , 0.0f);
				}
				PlaySound(SOUND_LABEL_SE_04_BOSSFIRE);
				return;							// 1���Z�b�g�����̂ŏI������
			}
		}
	}
}

//=============================================================================
// �{�X�����˂����e���S�ď��������ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetEnemyBulletAllLostFlg(void)
{
	return g_enemyBulletAllLostFlg;
}

void SetEnemyBulletAllLostFlg(BOOL enemyBulletAllLostFlg)
{
	g_enemyBulletAllLostFlg = enemyBulletAllLostFlg;
}

