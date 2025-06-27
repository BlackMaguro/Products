//=============================================================================
//
// �G�l�~�[�^�X�N�̏��� [enemyTasks.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include "behaviorTree.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"

using std::vector;
extern PLAYER* GetPlayer();

//-----------------------------------------------------------------------------
// �v���C���[�Ƃ̋������`�F�b�N���A���͈͓����ǂ����𔻒肷��^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief �v���C���[�Ƃ̐��������𔻒肷��
 * @param[in] enemy    ����Ώۂ̃G�l�~�[
 * @param[in] distance �v���C���[�Ƃ̔��苗���i臒l�j
 *
 * @retval SUCCESS �v���C���[��distance�ȉ��͈͓̔��ɂ���
 * @retval FAILURE �v���C���[���͈͊O�̂��߁AnoticeFlg��FALSE�ɂ��ĕԂ�
 */
class CheckPlayerDistanceTask : public BTNode 
{
    ENEMY* enemy;           // ����Ώۂ̃G�l�~�[
    float distance;         // ���肷�鋗��
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy    ����Ώۂ̃G�l�~�[
     * @param[in] distance �������l�ƂȂ鋗��
     */
    CheckPlayerDistanceTask(ENEMY* enemy, float distance)
        : enemy(enemy), distance(distance) {}

    /**
     * @brief �����`�F�b�N�����s
     * @return SUCCESS�^FAILURE
     */
    BTStatus Tick() override 
    {
        PLAYER* player = GetPlayer();
        float dx = player->pos.x - enemy->pos.x;
        float dz = player->pos.z - enemy->pos.z;
        float dist = sqrtf(dx * dx + dz * dz);

        // �O�̃t���O��ۑ�
        enemy->prevNoticeFlg = enemy->noticeFlg;

        if (dist <= distance) {
            return SUCCESS;
        }
        else {
            enemy->noticeFlg = FALSE; // ���������ꍇ
            return FAILURE;
        }
    }
};

//-----------------------------------------------------------------------------
// ���̏�ň�莞�ԑҋ@����^�X�N�iIdle�j
//-----------------------------------------------------------------------------
/**
 * @brief �G�l�~�[�����̏�ŃA�C�h����Ԃɂ���
 *
 * ����Ăяo�����Ƀ^�C�}�[���Z�b�g���AmoveTimer��0�ȉ��ɂȂ�܂�
 * ���t���[�� EnemyIdle() ���Ăяo���B
 *
 * @retval RUNNING �ҋ@��
 * @retval SUCCESS �ҋ@�I���imoveTimer <= 0�j
 */
class EnemyIdleTask : public BTNode 
{
    ENEMY* enemy;
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy �Ώۂ̃G�l�~�[
     */
    EnemyIdleTask(ENEMY* enemy) : enemy(enemy)
    {
        enemy->isIdleInitialized = FALSE; // Idle�J�n���ɏ�����
    }

    // �ҋ@�s�������s����
    BTStatus Tick() override 
    {
        // �^�X�N�ɏ��߂ē˓�������moveTimer��������
        if (!enemy->isIdleInitialized)
        {
            EnemyMoveTimerUpdate(*enemy);
            enemy->isIdleInitialized = TRUE;
            enemy->isRotLock = TRUE;
        }

        EnemyIdle(*enemy);

        /**
         * @brief �ҋ@�A�N�V���������s
         * @return RUNNING�^SUCCESS
         * @note �ҋ@���Ԃ��I��������SUCCESS��Ԃ�
         */
        if (enemy->moveTimer <= 0.0f)
        {
            enemy->isIdleInitialized = FALSE;
            enemy->isRotLock = FALSE;
            enemy->movePointChangeCount = 1;  // ����Wander�p�̈ړ��|�C���g�ݒ菀��
            return SUCCESS;
        }
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// �����_���ȕ����Ɉړ�����^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief �G�l�~�[�������_���ɜp�j������
 *
 * ����Ăяo�����Ɉړ��^�C�}�[���Z�b�g���AEnemyWander() ���Ăяo���B
 * moveTimer �� 0 �ȉ��ɂȂ�� SUCCESS ��Ԃ��B
 */
class WanderTask : public BTNode 
{
    ENEMY* enemy;
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy �Ώۂ̃G�l�~�[
     */
    WanderTask(ENEMY* enemy) : enemy(enemy) {}

    // �p�j�s�������s����
    BTStatus Tick() override 
    {
        if (!enemy->isIdleInitialized)  // IdleTask ����ڍs���Ă�������̂�
        {
            EnemyMoveTimerUpdate(*enemy);
            enemy->isIdleInitialized = TRUE;
        }

        enemy->isRotLock = FALSE; // �ړ����͌������b�N����

        EnemyWander(*enemy);

        /**
         * @brief �p�j�A�N�V���������s
         * @return RUNNING�^SUCCESS
         * @note �p�j���Ԃ��I��������SUCCESS��Ԃ�
         */
        if (enemy->moveTimer <= 0.0f)
        {
            enemy->isWanderInitialized = FALSE;
            enemy->movePointChangeCount = 1;
            return SUCCESS; // Idle�I��
        }
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// ���E���Ƀv���C���[�����邩�m�F����^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief �G�̎����������Ƀv���C���[�����邩���肷��
 * 
 * CollisionSector() ���g���āA�uforward�v�����̐�`����
 * �v���C���[�����邩���`�F�b�N���A��������noticeFlg��TRUE�ɂ���B
 *
 * @retval SUCCESS �v���C���[�𔭌�
 * @retval FAILURE ���E�O
 */
class CheckPlayerInSightTask : public BTNode
{
    ENEMY* enemy;
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy �Ώۂ̃G�l�~�[
     */
    CheckPlayerInSightTask(ENEMY* enemy) : enemy(enemy) {}

    // �ǐՍs�������s����
    BTStatus Tick() override
    {
        PLAYER* player = GetPlayer();
        enemy->isRotLock = FALSE; // �ړ����͌������b�N����

        // �G���v���C���[�̃x�N�g���𐳋K����������
        float rad = enemy->rot.y;
        XMFLOAT3 forward = {
            sinf(rad),   // X ����
            0.0f,        // Y ����
            cosf(rad)    // Z ����
        };

        if (CollisionSector(
                player->pos, 
                enemy->pos, 
                forward,
                ENEMY_NOTICE_LENGTH,
                RADIAN * 90.0f
            ))
        {
            enemy->noticeFlg = TRUE;  // �����t���O�𗧂Ă�
            return SUCCESS;
        }
        else
        {
            enemy->noticeFlg = FALSE;
            return FAILURE;
        }
    }
};

//-----------------------------------------------------------------------------
// �ҋ@�iIdle�j�Ɯp�j�iWander�j�̂ǂ��炩�������_���ɑI��Ŏ��s����^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief IdleTask�^WanderTask �̂����ꂩ�������_���I�����Ď��s����
 * 
 * �R���X�g���N�^�� IdleTask, WanderTask �𐶐����ATick �Ăяo�����ɂ�
 * selectedIndex == -1 �ł���΃����_���ɂǂ��炩�̃^�X�N��I������B
 * �I�����ꂽ�^�X�N�� SUCCESS ��Ԃ����ۂɂ͍ēx���I����Ԃɖ߂�A
 * ���̃T�C�N���ōĂу����_���ɑI�ђ����B
 */
class RandomActionTask : public BTNode
{
    ENEMY* enemy;                   // ����Ώۂ̃G�l�~�[
    vector<BTNode*> actionTasks;    // Idle/Wander �^�X�N��ێ�
    int selectedIndex;              // �I�𒆃^�X�N�̃C���f�b�N�X�i-1: ���I���j

public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy ���̃^�X�N�����삷��G�l�~�[�ւ̃|�C���^
     *
     * Initialize() �͌Ă΂���IdleTask��WanderTask�𐶐�����
     * actionTasks �Ɋi�[���AselectedIndex��-1�ɐݒ肷��B
     */
    RandomActionTask(ENEMY* enemy) : enemy(enemy), selectedIndex(-1)
    {
        // Idle/Wander���Ǘ�����iTODO:����A�N�V�����𑝂₷�j
        actionTasks.push_back(new EnemyIdleTask(enemy));
        actionTasks.push_back(new WanderTask(enemy));
    }
    /**
     * @brief �^�X�N��i�s������
     *
     *  selectedIndex == -1 �̏ꍇ�AGetRand �� 0 �� 1 ��I�����A
     *  enemy �̏������t���O�iisIdleInitialized/isWanderInitialized�j�����Z�b�g�B
     *  �I�����ꂽ�^�X�N�� Tick() ���Ăяo���A���̃X�e�[�^�X��Ԃ��B
     *  STATUS == SUCCESS �̏ꍇ�AselectedIndex��-1�ɖ߂����T�C�N���ōđI���B
     *
     * @return SUCCESS �I�𒆃^�X�N�����������ꍇ
     * @return RUNNING �I�𒆃^�X�N���܂����s���̏ꍇ
     */
    BTStatus Tick() override
    {
        if (selectedIndex == -1)
        {
            selectedIndex = GetRand(0, (int)(actionTasks.size()) - 1);

            // �e�^�X�N�̏������t���O���N���A
            enemy->isIdleInitialized = FALSE;
            enemy->isWanderInitialized = FALSE;
        }

        // �I�𒆂̃^�X�N��Tick
        BTStatus status = actionTasks[selectedIndex]->Tick();

        // �^�X�N���������玟�T�C�N���p�ɖ��I����Ԃɂ���
        if (status == SUCCESS)
        {
            selectedIndex = -1;
        }
        return status;
    }

    // ���������q�^�X�N���������i�f�X�g���N�^�j
    ~RandomActionTask()
    {
        for (auto task : actionTasks) delete task;
    }
};

//-----------------------------------------------------------------------------
// �v���C���[��ǐՂ�������^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief �v���C���[��ǂ�������s������Ɏ��s����^�X�N
 * 
 * Tick() �̂��т� EnemyChase() ���Ăяo���A��]�ƈړ����s���B
 * ��� RUNNING ��Ԃ��A���������͕ʃ^�X�N�ŊǗ�����z��B
 */
class ChaseActionTask : public BTNode
{
    ENEMY* enemy;
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] enemy �ǐՑΏۂ̃G�l�~�[
     */
    ChaseActionTask(ENEMY* enemy) : enemy(enemy) {}

    /**
     * @brief �ǐՏ��������s���A��� RUNNING ��Ԃ�
     * @return RUNNING �ǐՌp����
     */
    BTStatus Tick() override
    {
        enemy->isRotLock = FALSE;
        EnemyChase(*enemy);
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// noticeFlg �ƃv���C���[�Ƃ̋������`�F�b�N���A�t���O�����Z�b�g����^�X�N
//-----------------------------------------------------------------------------
/**
 * @brief �v���C���[�����苗�����ꂽ�� noticeFlg ���N���A����
 *
 *   �v���C���[�Ƃ̐������� > ENEMY_VIGILANCE_LENGTH �̏ꍇ�A
 *   noticeFlg �� FALSE �ɐݒ肵 FAILURE ��Ԃ��B
 *   ����ȊO�� noticeFlg �� TRUE �̏ꍇ�� SUCCESS�A
 *   ����ȊO�� FAILURE ��Ԃ��B
 */
class CheckNoticeTask : public BTNode 
{
    ENEMY* m_enemy;
public:
    /**
     * @brief �R���X�g���N�^
     * @param[in] e �����`�F�b�N�Ώۂ̃G�l�~�[
     */
    CheckNoticeTask(ENEMY* e) : m_enemy(e) {}

    /**
     * @brief �v���C���[�Ƃ̋����E�t���O��]�����ăX�e�[�^�X��Ԃ�
     * @return SUCCESS noticeFlg��TRUE�̏ꍇ
     * @return FAILURE noticeFlg��FALSE�ւ̃N���A���A�܂��͔͈͊O
     */
    BTStatus Tick() override 
    {
        PLAYER* player = GetPlayer();
        float dx = player->pos.x - m_enemy->pos.x;
        float dz = player->pos.z - m_enemy->pos.z;
        float distSq = dx * dx + dz * dz;
        static const float vigilSq = ENEMY_VIGILANCE_LENGTH_SQ;
        // ��苗���ȏ㗣�ꂽ��t���O��؂���FAILURE��Ԃ�
        if (distSq > vigilSq)
        {
            m_enemy->noticeFlg = FALSE;
            return FAILURE;
        }
        if (m_enemy->noticeFlg) 
        {
            return SUCCESS;
        }
        return FAILURE;
    }
};


