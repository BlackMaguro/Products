//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "bullet.h"
#include "main.h"
#include "enemy.h"
#include "boss.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(32)	// �e�̉��T�C�Y
#define TEXTURE_HEIGHT				(32)	// �e�̏c�T�C�Y
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/Bullet/bullet00.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bullet[i].w     = TEXTURE_WIDTH;
		g_Bullet[i].h     = TEXTURE_HEIGHT;
		g_Bullet[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
		g_Bullet[i].dir = BULLET_DIR_RIGHT;

		g_Bullet[i].atk = 10;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			XMVECTOR pos  = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
			pos += move;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			BG* bg = GetBG();
			PLAYER* player = GetPlayer();
			if (g_Bullet[i].pos.y < (-g_Bullet[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}
			// ���[�ɂ����Ƃ��̒e�̏���
			if (player->pos.x < SCREEN_WIDTH * 0.5)
			{
				if (g_Bullet[i].pos.x < (player->pos.x - (SCREEN_WIDTH + g_Bullet[i].w * 0.5)))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = false;
				}
				if (g_Bullet[i].pos.x > (player->pos.x + (SCREEN_WIDTH + g_Bullet[i].w * 0.5)))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = false;
				}
			}
			// ���S�ɂ����Ƃ��̏���
			if (player->pos.x > SCREEN_WIDTH * 0.5)
			{
				if (g_Bullet[i].pos.x < (player->pos.x - (SCREEN_WIDTH * 0.5 + g_Bullet[i].w * 0.5)))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = false;
				}
				if (g_Bullet[i].pos.x > (player->pos.x + (SCREEN_WIDTH * 0.5 + g_Bullet[i].w * 0.5)))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = false;
				}
			}

			// �����蔻�菈��
			{
				ENEMY* enemy = GetEnemy();
				int bulletAtk = g_Bullet[i].atk;
				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
							// �����������̏���
							enemy[j].hp -= bulletAtk;
							g_Bullet[i].use = false;
							if (enemy[j].hp <= 0)
							{
								enemy[j].use = FALSE;
								AddScore(100);
							}
							// �G�t�F�N�g����
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
						}
					}
				}
				// �{�X�킾�����ꍇ
				BOSS* boss = GetBoss();

				if (GetBossFlg() == TRUE)
				{
					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < BOSS_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (boss[j].use == TRUE)
						{
							// �{�X�̖ڂ�HP��ۑ����Ă���
							int bossHP_old = boss[4].hp;
							BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
								boss[j].pos, boss[j].w, boss[j].h);
							// �������Ă���Ƃ��i�{�X�̖ځj
							if ((ans == TRUE) && (j == 4))
							{
								// �����������̏���
								// �{�X�̌��i5�j���S�J�̂Ƃ��A�S�_���[�W
								if (boss[5].patternAnim == 0 || boss[5].patternAnim == 4)
								{
									PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
									AddScore(50);
									boss[j].hp -= bulletAtk;
								}
								// �{�X�̌��i5�j�������J���Ă���Ƃ��A���_���[�W
								else if (boss[5].patternAnim == 1 || boss[5].patternAnim == 3)
								{
									PlaySound(SOUND_LABEL_SE_07_ENEMYBULLETHIT);
									AddScore(10);
									boss[j].hp -= (int)(bulletAtk * 0.5);
								}
								// �{�X�̌��i5�j�����Ă��邢��Ƃ��A�m�[�_���[�W
								else if (boss[5].patternAnim == 2)
								{
									// �ꍇ�ɂ���Ă͉񕜂���i0�`30�̒l�j
									boss[j].hp += bulletAtk * GetRand(0,3);
									if (boss[j].hp > BOSS_MAX_HP) boss[j].hp = BOSS_MAX_HP;
									if (boss[j].hp > bossHP_old) PlaySound(SOUND_LABEL_SE_12_BOSSHPHEAL);
								}
								// �G�t�F�N�g����
								g_Bullet[i].use = false;
								//SetEffect(boss[j].pos.x, boss[j].pos.y, 30);
								if (boss[j].hp <= 0)
								{
									// �{�X��|�����ꍇ�A�N���A�t���O���I��
									SetClearFlg(TRUE);
									SetFade(FADE_OUT, MODE_EPILOGUE);		// �Q�[�����N���A�����̂ŃG�s���[�O��ʂ�
									//boss[j].use = FALSE;
								}
							}
						}
					}
				}
			}


			bulletCount++;
		}
	}

	//PrintDebugProc("Bullet[1] pos.x:%f,pos.y:%f,\nBullet[2] pos.x:%f,pos.y:%f,\n", g_Bullet[0].pos.x, g_Bullet[0].pos.y, g_Bullet[1].pos.x, g_Bullet[1].pos.y);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet(XMFLOAT3 pos, int bulletDir)
{
	PLAYER* player = GetPlayer();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos = pos;			// ���W���Z�b�g

			// �o���b�g�̔�ԕ����̊m��
			// �v���C���[���E�������Ă����Ƃ�
			if (bulletDir == BULLET_DIR_RIGHT)
			{
				g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);
			}
			// �v���C���[�����������Ă����Ƃ�
			else if (bulletDir == BULLET_DIR_LEFT)
			{
				g_Bullet[i].move = XMFLOAT3(-BULLET_SPEED, 0.0f, 0.0f);
			}
			// �v���C���[�����������Ă����Ƃ�
			else if (bulletDir == BULLET_DIR_DOWN)
			{
				g_Bullet[i].move = XMFLOAT3(0.0f, BULLET_SPEED, 0.0f);
			}
			// �v���C���[����������Ă����Ƃ�
			else if (bulletDir == BULLET_DIR_UP)
			{
				g_Bullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);
			}
			PlaySound(SOUND_LABEL_SE_03_PLAYERBULLET);
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

