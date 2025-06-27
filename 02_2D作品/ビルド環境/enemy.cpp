//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "main.h"
#include "bg.h"
#include "player.h"
#include "field.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// �L�����T�C�Y(200/2)
#define TEXTURE_HEIGHT				(64)	// 
#define TEXTURE_MAX					(6)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X		(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_BALL_DIVIDE_X	(6)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y		(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)			// �A�j���[�V�����p�^�[����
#define ANIM_PATTERN_BALL_NUM		(TEXTURE_PATTERN_BALL_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT						(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/enemy_1_maggotsG.png",				// 0_�}�S�b�c_��_�ʏ�
	"data/TEXTURE/Enemy/enemy_2_maggotsP.png",				// 1_�}�S�b�c_��_�ʏ�
	"data/TEXTURE/Enemy/enemy_3_maggotsR.png",				// 2_�}�S�b�c_��_�ʏ�
	"data/TEXTURE/Enemy/enemy_4_maggotsG.png",				// 3_�}�S�b�c_��_�ۂ�
	"data/TEXTURE/Enemy/enemy_5_maggotsP.png",				// 4_�}�S�b�c_��_�ۂ�
	"data/TEXTURE/Enemy/enemy_6_maggotsR.png",				// 5_�}�S�b�c_��_�ۂ�
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�\����

static int		g_EnemyCount = ENEMY_MAX;

static int g_EnemyMAX = ENEMY_MAX;		// �G�l�~�[�̍ő吔��ύX���邽�߂̕ϐ�

// �O���[���}�S�b�c�i�ʁj
static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦					����
	{ XMFLOAT3(2688.0f, 320.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(3392.0f, 320.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};

static INTERPOLATION_DATA g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(4352.0f,448.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(5632.0f,448.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.1f, 1.1f, 1.0f),	60 },
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
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


	if (GetMode() == MODE_TUTORIAL)
	{
		g_EnemyMAX = 1;
	}
	else if(GetMode() == MODE_GAME)
	{
		g_EnemyMAX = ENEMY_MAX;
	}
	// �G�l�~�[�\���̂̏�����
	g_EnemyCount = 0;
	for (int i = 0; i < g_EnemyMAX; i++)
	{
		g_EnemyCount++;
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = 0;
		g_Enemy[i].hp = ENEMY_G_MAX_HP;					// �G�l�~�[��HP���Z�b�g
		g_Enemy[i].atk = ENEMY_G_MAX_ATK;				// �G�l�~�[�̍U���͂��Z�b�g
		g_Enemy[i].type = ENEMY_TYPE_G;					// �G�l�~�[�̎�ނ��Z�b�g
		g_Enemy[i].ballFlg = FALSE;						// �G�l�~�[�̋ʂ̏��

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].colPos = 0.0f;							// �Փ˂����Ƃ��̍��W�p
		g_Enemy[i].colDir = EDGE_LEFT;						// �Փ˂����Ƃ��̌���

		g_Enemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �G�l�~�[�̎�ނɂ���Ēl��ύX
		// �p�[�v���}�S�b�c�̏ꍇ�i���̃E�W���j
		if (i == 4 || i == 6 || i == 7 || i == 11 || i == 12 || i == 15)
		{
			g_Enemy[i].hp = ENEMY_P_MAX_HP;
			g_Enemy[i].atk = ENEMY_P_MAX_ATK;
			g_Enemy[i].type = ENEMY_TYPE_P;

		}
		// ���b�h�}�S�b�c�̏ꍇ�i�Ԃ̃E�W���j
		if (i == 8 || i == 11 || i == 13 || i == 14 || i == 16 || i == 17)
		{
			g_Enemy[i].hp = ENEMY_R_MAX_HP;
			g_Enemy[i].atk = ENEMY_R_MAX_ATK;
			g_Enemy[i].type = ENEMY_TYPE_R;
		}
		// �G�l�~�[���ʂ̏�Ԃ��𔻒f����
		if (i == 5 || i == 12 || i == 13 || i == 15 || i == 16 || i == 17)
		{
			g_Enemy[i].ballFlg = TRUE;							// �G�l�~�[�̋ʂ̏��
			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// �ړ���
		}

	}

	g_Enemy[0].pos = XMFLOAT3(1000.0f, 448.0f, 0.0f);	// �G�l�~�[1�i�O���[���j
	// �`���[�g���A���p�G�l�~�[�i�O���[���}�S�b�g�j
	if (GetMode() == MODE_TUTORIAL) 
	{
		g_Enemy[0].pos = XMFLOAT3(1200.0f, 448.0f, 0.0f);
	}

	g_Enemy[1].pos = XMFLOAT3(960.0f, 256.0f, 0.0f);	// �G�l�~�[2�i�O���[���j
	g_Enemy[2].pos = XMFLOAT3(1600.0f, 440.0f, 0.0f);	// �G�l�~�[3�i�O���[���j
	g_Enemy[3].pos = XMFLOAT3(1472.0f, 256.0f, 0.0f);	// �G�l�~�[4�i�O���[���j
	g_Enemy[4].pos = XMFLOAT3(2176.0f, 384.0f, 0.0f);	// �G�l�~�[5�i�p�[�v���j
	g_Enemy[5].pos = XMFLOAT3(2816.0f, 320.0f, 0.0f);	// �G�l�~�[6�i�O���[���F�ʁj
	g_Enemy[6].pos = XMFLOAT3(3072.0f, 320.0f, 0.0f);	// �G�l�~�[7�i�p�[�v���j
	g_Enemy[7].pos = XMFLOAT3(3456.0f, 128.0f, 0.0f);	// �G�l�~�[8�i�p�[�v���A�E�A�����Ƃ���j
	g_Enemy[8].pos = XMFLOAT3(4160.0f, 384.0f, 0.0f);	// �G�l�~�[9�i���b�h�A�������Ƃ������j
	g_Enemy[9].pos = XMFLOAT3(4352.0f, 448.0f, 0.0f);	// �G�l�~�[10�i�O���[���j
	// �؂̔����W�����v����ӏ��̃G�l�~�[
	g_Enemy[10].pos = XMFLOAT3(4672.0f, 448.0f, 0.0f);	// �G�l�~�[11�i�p�[�v���F�E�j
	g_Enemy[11].pos = XMFLOAT3(4900.0f, 448.0f, 0.0f);	// �G�l�~�[12�i���b�h�j
	g_Enemy[12].pos = XMFLOAT3(5300.0f, 448.0f, 0.0f);	// �G�l�~�[13�i�p�[�v���F�ʁj
	g_Enemy[13].pos = XMFLOAT3(5500.0f, 448.0f, 0.0f);	// �G�l�~�[14�i���b�h�F�ʁj
	// �������X�e�[�W�̃G�l�~�[
	g_Enemy[14].pos = XMFLOAT3(6300.0f, 320.0f, 0.0f);	// �G�l�~�[15�i���b�h�j
	g_Enemy[15].pos = XMFLOAT3(7200.0f, 128.0f, 0.0f);	// �G�l�~�[16�i�p�[�v���F�ʁj
	g_Enemy[16].pos = XMFLOAT3(6700.0f, 448.0f, 0.0f);	// �G�l�~�[17�i���b�h�F�ʁF���`��ԁj
	g_Enemy[17].pos = XMFLOAT3(7552.0f, 256.0f, 0.0f);	// �G�l�~�[18�i���b�h�F�ʁj


	// 5�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[5].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[5].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[5].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 13�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[13].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[13].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[13].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{

	if (g_Load == FALSE) return;
	g_EnemyCount = 0;			// �����Ă�G�l�~�[�̐�

	for (int i = 0; i < g_EnemyMAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Enemy[i].use == TRUE)
		{
			g_EnemyCount++;		// �����Ă��G�̐�
			
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// �A�j���[�V����
			g_Enemy[i].countAnim += 1.0f;
			// �ʏ�̓���
			if (g_Enemy[i].ballFlg != TRUE)
			{
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}
			}
			// �ʂ̎��̓���
			else if(g_Enemy[i].ballFlg == TRUE)
			{
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_BALL_NUM;
				}
			}

			// �d�͂�ݒ�
			float gravity = 4.0f;
			float vertical = 0.0f;			// �c�̈ړ���
			float horizontal = -g_Enemy[i].move.x;		// ���̈ړ���

			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾
				
				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�
				
				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���
				
				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���
				
				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}
			// �ړ������i�ȈՔŁj
			// �ړ��ł���ꏊ���̃`�F�b�N�i�d�́j
			if (CollisionEnemyToMapchip(XMFLOAT3(0.0f, gravity, 0.0f), i) == FALSE)
			{
				// �ړ��ʂ𑫂�
				g_Enemy[i].pos.y += gravity;
			}
			else
			{
				// �ՓˑO�ɖ߂�
				g_Enemy[i].pos.y = g_Enemy[i].colPos;
			}
			// �ړ��ł���ꏊ���̃`�F�b�N�i�������j
			if (CollisionEnemyToMapchip(XMFLOAT3(horizontal, 0.0f, 0.0f), i) == FALSE)
			{
				// �ړ��ʂ𑫂�
				g_Enemy[i].pos.x += horizontal * 0.1f;
			}
			else
			{
				// �ړ������𔽓]������
				g_Enemy[i].move.x *= (-1.0f);

				// �ՓˑO�ɖ߂�
				g_Enemy[i].pos.x = g_Enemy[i].colPos;
			}
			// �ړ��ł���ꏊ���̃`�F�b�N�i�c�����j
			if (CollisionEnemyToMapchip(XMFLOAT3(0.0f, vertical, 0.0f), i) == FALSE)
			{
				// �ړ��ʂ𑫂�
				g_Enemy[i].pos.y += vertical;
			}
			else
			{
				// �ՓˑO�ɖ߂�
				g_Enemy[i].pos.y = g_Enemy[i].colPos;
			}


			// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
			{
				PLAYER* player = GetPlayer();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < g_EnemyMAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w, g_Enemy[i].h,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE && (player[0].muteki == FALSE))
						{
							player->hp -= 10;
						}
					}
				}
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc(
		"Enemy[0] pos.x:%f,pos.y:%f,\nEnemy[1] pos.x:%f,pos.y:%f,\nEnemy[5] pos.x:%f,pos.y:%f,\nEnemy[5] HP:%d,\n",
		g_Enemy[0].pos.x, g_Enemy[0].pos.y,
		g_Enemy[1].pos.x, g_Enemy[1].pos.y,
		g_Enemy[5].pos.x, g_Enemy[5].pos.y,
		g_Enemy[5].hp
		);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < g_EnemyMAX; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[i].w;					// �G�l�~�[�̕\����
			float ph = g_Enemy[i].h;					// �G�l�~�[�̕\������
			float tw = 1.0f;							// �e�N�X�`���̕�
			float th = 1.0f;							// �e�N�X�`���̍���
			float tx = 0.0f;							// �e�N�X�`���̍���X���W
			float ty = 0.0f;							// �e�N�X�`���̍���Y���W

			// �G�l�~�[�i�ʏ�j
			if (g_Enemy[i].ballFlg != TRUE)
			{
				tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �΂̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_G)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// ���̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_P)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// ���̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_R)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}
			// �G�l�~�[�i�ʁj
			else if (g_Enemy[i].ballFlg == TRUE)
			{
				tw = 1.0f / TEXTURE_PATTERN_BALL_DIVIDE_X;	// �e�N�X�`���̕�
				th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_BALL_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_BALL_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �΂̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_G)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// ���̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_P)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// �Ԃ̃E�W��
				if (g_Enemy[i].type == ENEMY_TYPE_R)
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
						g_Enemy[i].rot.z);
				}
				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}


// �����Ă�G�l�~�[�̐�
int GetEnemyCount(void)
{
	return g_EnemyCount;
}

//=============================================================================
// �G�l�~�[�ƃ}�b�v�`�b�v�i�ǁj�Ƃ̓����蔻��
//=============================================================================
BOOL CollisionEnemyToMapchip(XMFLOAT3 move, int i)
{
	// �e���_�̏��
	int leftX = (int)((g_Enemy[i].pos.x + move.x) / MAPCHIP_WIDTH);
	int rightX = (int)((g_Enemy[i].pos.x + g_Enemy[i].w + move.x - 1.0f) / MAPCHIP_WIDTH);
	int topY = (int)((g_Enemy[i].pos.y + move.y) / MAPCHIP_HEIGHT);
	int bottomY = (int)((g_Enemy[i].pos.y + g_Enemy[i].h + move.y - 1.0f) / MAPCHIP_HEIGHT);

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
					g_Enemy[i].colDir = j;
					SetCollisionEnemyPos(x, y, i, j);
					return TRUE;
				default:
					break;
				}
			}
		}
	}
	return FALSE;
}

// �ڐG�����ӏ��̏��̎擾
void SetCollisionEnemyPos(int x, int y, int i, int colDir)
{
	XMFLOAT3 mapchipPos = XMFLOAT3((float)x * MAPCHIP_WIDTH, (float)y * MAPCHIP_HEIGHT, 0.0f);

	switch (colDir)
	{
		// �G�l�~�[�̍���
	case EDGE_LEFT:
		g_Enemy[i].colPos = mapchipPos.x + MAPCHIP_WIDTH;
		break;
		// �G�l�~�[�̉E��
	case EDGE_RIGHT:
		g_Enemy[i].colPos = mapchipPos.x - g_Enemy[i].w;
		break;
		// �G�l�~�[�̏㑤
	case EDGE_TOP:
		g_Enemy[i].colPos = mapchipPos.y + MAPCHIP_HEIGHT;
		break;
		// �G�l�~�[�̉���
	case EDGE_BOTTOM:
		g_Enemy[i].colPos = mapchipPos.y - g_Enemy[i].h;
		break;

	default:
		break;
	}
}

