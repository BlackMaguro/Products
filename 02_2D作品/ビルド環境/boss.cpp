//=============================================================================
//
// �{�X���� [boss.cpp]
// Author : 
//
//=============================================================================
#include "boss.h"
#include "bulletEnemy.h"
#include "bg.h"
#include "player.h"
#include "field.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(760)	// �{�X�L�����T�C�Y_��_��
#define TEXTURE_HEIGHT				(450)	// �{�X�L�����T�C�Y_��_�c
#define TEXTURE_MAX					(8)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)						// �A�j���[�V�����p�^�[�����i�{�X�̌��j
#define ANIM_EYE_PATTERN_NUM			(TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X*TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[�����i�{�X�̖ځj
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define ANIM_WAIT_EYE				(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Boss/boss_6_back.png",		// �{�X�̔w��
	"data/TEXTURE/Boss/boss_5_body.png",		// �{�X�̑�
	"data/TEXTURE/Boss/boss_4_face.png",		// �{�X�̓�
	"data/TEXTURE/Boss/boss_3_tongue.png",		// �{�X�̐�
	"data/TEXTURE/Boss/boss_2_eye.png",			// �{�X�̖�
	"data/TEXTURE/Boss/boss_1_mouth.png",		// �{�X�̌�
	"data/TEXTURE/bar_white.png",				// �{�X��HP
	"data/TEXTURE/BOSS/boss_HPBar.png",			// �{�X��HP�o�[
};


static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static BOSS	g_Boss[BOSS_MAX];		// �{�X�\����
BOOL g_bossAtkFlg = FALSE;			// �{�X���U�����邩�̃t���O

static int		g_BossCount = BOSS_MAX;
float g_bossTimeCount = 0.0f;
int	  g_bossTime = 0;
float bossX = (float)SCREEN_CENTER_X;	// �{�X�̒��S���W
float bossBodyY = 320.0f;
int g_bossAtkTime = 0;
int atkStartTime = 0;
float sinTime = 0.0f;


// �{�X�̑̂̓���
static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦					����
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.1f, 1.1f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.9f, 0.9f, 1.0f),	60 },
	{ XMFLOAT3(bossX,  bossBodyY, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(3000.0f, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBoss(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// �{�X�\���̂̏�����
	g_BossCount = 0;
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_BossCount++;
		g_Boss[i].use = TRUE;
		g_Boss[i].pos = XMFLOAT3(bossX, 320.0f, 0.0f);	// ���S�_����\��
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		//g_Boss[i].w = TEXTURE_WIDTH;
		g_Boss[i].w = 0.0f;
		g_Boss[i].h = 0.0f;
		g_Boss[i].texNo = 0;

		g_Boss[i].countAnim = 0;
		g_Boss[i].patternAnim = 0;

		g_Boss[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Boss[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Boss[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Boss[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Boss[i].hp = BOSS_MAX_HP;				// �{�X��HP���Z�b�g
		g_Boss[i].atk = (int)(BOSS_MAX_ATK * 0.5);		// �{�X�̍ő�U���͂̔������Z�b�g
		g_Boss[i].moveFlg = TRUE;				// �{�X�������Ă��邩�̃t���O
		g_Boss[i].patternAtk = 0;				// �{�X�̍U���p�^�[��
	}
	// �{�X�̔w��
	g_Boss[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Boss[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Boss[0].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Boss[0].w = 760.0f;
	g_Boss[0].h = 450.0f;

	// �{�X�̑�
	g_Boss[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Boss[1].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Boss[1].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Boss[1].w = 760.0f;
	g_Boss[1].h = 450.0f;

	// �{�X�̓�
	g_Boss[2].w = 198.0f;
	g_Boss[2].h = 306.0f;
	g_Boss[2].pos = XMFLOAT3(bossX, 310.0f, 0.0f);

	// �{�X�̐�
	g_Boss[3].w = 64.0f;
	g_Boss[3].h = 54.0f;
	g_Boss[3].pos = XMFLOAT3(bossX, 385.0f, 0.0f);

	// �{�X�̖�
	g_Boss[4].w = 64.0f;
	g_Boss[4].h = 64.0f;
	g_Boss[4].pos = XMFLOAT3(bossX, 360.0f, 0.0f);

	//// �{�X�̌�
	g_Boss[5].w = 110.0f;
	g_Boss[5].h = 165.0f;
	g_Boss[5].pos = XMFLOAT3(bossX, 360.0f, 0.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBoss(void)
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
void UpdateBoss(void)
{
	if (g_Load == FALSE) return;
	g_BossCount = 0;			// �����Ă�{�X�̐�
	g_bossTimeCount += 1.0f;

	if (g_bossTimeCount == 60.0f)
	{
		g_bossTime += 1;
		g_bossAtkTime += 1;
		g_bossTimeCount = 0.0f;
	}
	sinTime += 0.016f;
	float sinValue = sin(sinTime);
	for (int i = 0; i < BOSS_MAX; i++)
	{
		// �����Ă�{�X��������������
		if (g_Boss[i].use == TRUE)
		{
			g_BossCount++;		// �����Ă��G�̐�

			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Boss[i].pos;

			// �A�j���[�V����  
			//g_Boss[i].countAnim += 1.0f;
			g_Boss[i].countAnim += 0.1f;
			// �{�X�́i�ځj�̓���
			if ((i == 4) && (g_Boss[i].countAnim > ANIM_WAIT_EYE))
			{
				g_Boss[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Boss[i].patternAnim = (g_Boss[i].patternAnim + 1) % ANIM_EYE_PATTERN_NUM;
			}
			// �{�X�́i���j�̓���
			// �ő�HP��6����5������܂œ������Ȃ�
			if(g_Boss[4].hp <= (BOSS_MAX_HP * 5) / 6)
			{
				if ((i == 5) && (g_Boss[i].countAnim > ANIM_WAIT))
				{
					g_Boss[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Boss[i].patternAnim = (g_Boss[i].patternAnim + 1) % ANIM_PATTERN_NUM;
					//g_Boss[i].patternAnim = 2;
				}
			}
			else if((g_Boss[4].hp >= (BOSS_MAX_HP * 5) / 6))
			{
				// �񕜂����Ƃ��͖߂�
				g_Boss[5].patternAnim = 0;
			}


			// �ړ�����
			if (g_Boss[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Boss[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
				g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
				g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}
			}

			// �G�l�~�[�̍U�������i3�b�o�߂��邲�ƂɍU������j
			//if (GetEnemyBulletAllLostFlg() == TRUE)
			if ((g_Boss[i].hp < BOSS_MAX_HP - 50) && g_bossAtkTime >= 3)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp < BOSS_MAX_HP - 100) && g_bossAtkTime >= 2)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp < BOSS_MAX_HP - 200) && g_bossAtkTime >= 1)
			{
				SetBossAtkFlg(TRUE);
			}
			if ((g_Boss[i].hp >= BOSS_MAX_HP - 50) && g_bossAtkTime >= 5)
			{
				SetBossAtkFlg(TRUE);
			}

			if(GetBossAtkFlg() == TRUE)
			{
				atkStartTime += 1;
				BULLETENEMY* bulletEnemy = GetBulletEnemy();

				// �U���p�^�[���̎擾
				g_Boss[4].patternAtk = GetBossPatternAtk();
				//g_Boss[4].patternAtk = 3;
				// �{�X�̍U���p�^�[���̊m��
				// �{�X�i�ځj�̈ʒu���
				XMFLOAT3 pos = g_Boss[4].pos;
				SetBulletEnemy(pos, g_Boss[4].patternAtk);

				// �e�����˂��ꂽ�̂Ńt���O���I�t�ɂ���
				//SetEnemyBulletAllLostFlg(FALSE);
				// �U���t���O���I�t�ɂ���
				if (atkStartTime >= 120)
				{
					SetBossAtkFlg(FALSE);
					g_bossAtkTime = 0;
					atkStartTime = 0;
				}

			}

			// �ړ����I�������{�X�Ƃ̓����蔻��i�{�X�Ƃ͓�����Ȃ����疢�g�p�j
			{
				PLAYER* player = GetPlayer();

				// �{�X�̐��������蔻����s��
				for (int j = 0; j < BOSS_MAX; j++)
				{
					// �����Ă�{�X�Ɠ����蔻�������
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Boss[i].pos, g_Boss[i].w, g_Boss[i].h,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE && (player[0].muteki == FALSE))
						{
							//player->hp -= 10;
							// �����������̏���
							//player[j].use = FALSE;
						}
					}
				}
			}

		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc(
		"Boss[4] pos.x:%f,pos.y:%f,\nHP:%d,\ng_Boss[5].countAnim:%f,\ng_Boss[5].countAnim:%f,\n sinvalue:%f,\ng_Boss[4].patternAnim:%d,\ng_Boss[5].patternAnim:%d,\ng_Boss[4].patternAtk:%d,\ng_bossTime:%d,\ng_bossAtkTime:%d,\n",
		g_Boss[4].pos.x, g_Boss[4].pos.y,g_Boss[4].hp, g_Boss[4].countAnim, g_Boss[5].countAnim, sinValue, g_Boss[4].patternAnim,g_Boss[5].patternAnim, g_Boss[4].patternAtk, g_bossTime, g_bossAtkTime
	);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBoss(void)
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


	float sinValue = sin(sinTime);

	BG* bg = GetBG();

	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE)			// ���̃{�X���g���Ă���H
		{									// Yes

			//�{�X�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Boss[i].pos.x - bg->pos.x;	// �{�X�̕\���ʒuX
			float py = g_Boss[i].pos.y - bg->pos.y;	// �{�X�̕\���ʒuY
			float pw = g_Boss[i].w;		// �{�X�̕\����
			float ph = g_Boss[i].h;		// �{�X�̕\������
			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			if (i == 0)
			{
				px += 20 * sinValue * (-1);
			}
			if (i == 2 || i == 3 || i == 4 || i == 5)
			{
				px += 20 * sinValue;
			}

			// �{�X�̖ڂ̎�
			if (i == 4)
			{
				tw = 1.0f / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				th = 1.0f / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				tx = (float)(g_Boss[i].patternAnim % TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Boss[i].patternAnim / TEXTURE_BOSS_EYE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W
			}

			// �{�X�̌��̎�
			if (i == 5)
			{
				tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				tx = (float)(g_Boss[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Boss[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W
			}

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[i].rot.z);


			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// �{�X�̃Q�[�W
	{
		// ���~���̃Q�[�W�i���F�j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 280.0f;		// �Q�[�W�̕\���ʒuX
		float py =  10.0f;		// �Q�[�W�̕\���ʒuY
		float pw = 400.0f;		// �Q�[�W�̕\����
		float ph = 70.0f;		// �Q�[�W�̕\������

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


		// �{�X��HP�ɏ]���ăQ�[�W�̒�����\�����Ă݂�i�F�j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f�i�ڂ̂Ƃ��̗̑͂��Q�Ɓj
		pw = pw * ((float)g_Boss[4].hp / BOSS_MAX_HP);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// HP�o�[�̘g
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

			//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = 280.0f;		// �Q�[�W�̕\���ʒuX
			float py = 10.0f;		// �Q�[�W�̕\���ʒuY
			float pw = 400.0f;		// �Q�[�W�̕\����
			float ph = 70.0f;		// �Q�[�W�̕\������

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

//=============================================================================
// �{�X�̍U���p�^�[�����擾����
//=============================================================================
int GetBossPatternAtk(void)
{
	int ans = GetRand(0, 4);
	// �{�X�i�ځj�̗̑͏󋵂ɂ���čU���p�^�[����ω�������
	// HP250�܂ł̓p�^�[��1��2
	if (g_Boss[4].hp > (BOSS_MAX_HP * 5 / 6))
	{
		ans %= 2;
	}
	// HP200�܂ł̓p�^�[��1�`3
	else if ((200.0f < g_Boss[4].hp) && (g_Boss[4].hp <= (BOSS_MAX_HP - 50.0f)))
	{
		ans %= 3;
	}
	// HP100���傫��200�ȉ��̓p�^�[��1�`4
	else if (100.0f < g_Boss[4].hp && g_Boss[4].hp <= (BOSS_MAX_HP - 100.0f))
	{
		ans %= 4;
	}
	// HP100�܂ł̓p�^�[��1�`5�iALL�j
	else if (g_Boss[4].hp <= (BOSS_MAX_HP - 200.0f))
	{
		ans %= 5;
		// �̗͂�0�ɋ߂Â����狭������
		if (ans < 3 && (g_Boss[4].hp <= BOSS_MAX_HP / 6))
		{
			ans = (ans + 1) % 3 + 3;
		}
	}

	return ans;
}

//=============================================================================
// �{�X���U�����邩�ǂ����̔��f������i�A�N�Z�T�֐��j
//=============================================================================
BOOL GetBossAtkFlg(void)
{
	return g_bossAtkFlg;
}

void SetBossAtkFlg(BOOL bossAtkFlg)
{
	g_bossAtkFlg = bossAtkFlg;
}

//=============================================================================
// Boss�\���̂̐擪�A�h���X���擾
//=============================================================================
BOSS* GetBoss(void)
{
	return &g_Boss[0];
}


// �����Ă�{�X�̐�
int GetBossCount(void)
{
	return g_BossCount;
}

