//=============================================================================
//
// エネミータスクの処理 [enemyTasks.h]
// Author : 白井陽大
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
// プレイヤーとの距離をチェックし、一定範囲内かどうかを判定するタスク
//-----------------------------------------------------------------------------
/**
 * @brief プレイヤーとの水平距離を判定する
 * @param[in] enemy    判定対象のエネミー
 * @param[in] distance プレイヤーとの判定距離（閾値）
 *
 * @retval SUCCESS プレイヤーがdistance以下の範囲内にいる
 * @retval FAILURE プレイヤーが範囲外のため、noticeFlgをFALSEにして返す
 */
class CheckPlayerDistanceTask : public BTNode 
{
    ENEMY* enemy;           // 判定対象のエネミー
    float distance;         // 判定する距離
public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy    判定対象のエネミー
     * @param[in] distance しきい値となる距離
     */
    CheckPlayerDistanceTask(ENEMY* enemy, float distance)
        : enemy(enemy), distance(distance) {}

    /**
     * @brief 距離チェックを実行
     * @return SUCCESS／FAILURE
     */
    BTStatus Tick() override 
    {
        PLAYER* player = GetPlayer();
        float dx = player->pos.x - enemy->pos.x;
        float dz = player->pos.z - enemy->pos.z;
        float dist = sqrtf(dx * dx + dz * dz);

        // 前のフラグを保存
        enemy->prevNoticeFlg = enemy->noticeFlg;

        if (dist <= distance) {
            return SUCCESS;
        }
        else {
            enemy->noticeFlg = FALSE; // 見失った場合
            return FAILURE;
        }
    }
};

//-----------------------------------------------------------------------------
// その場で一定時間待機するタスク（Idle）
//-----------------------------------------------------------------------------
/**
 * @brief エネミーをその場でアイドル状態にする
 *
 * 初回呼び出し時にタイマーをセットし、moveTimerが0以下になるまで
 * 毎フレーム EnemyIdle() を呼び出す。
 *
 * @retval RUNNING 待機中
 * @retval SUCCESS 待機終了（moveTimer <= 0）
 */
class EnemyIdleTask : public BTNode 
{
    ENEMY* enemy;
public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy 対象のエネミー
     */
    EnemyIdleTask(ENEMY* enemy) : enemy(enemy)
    {
        enemy->isIdleInitialized = FALSE; // Idle開始時に初期化
    }

    // 待機行動を実行する
    BTStatus Tick() override 
    {
        // タスクに初めて突入したらmoveTimerを初期化
        if (!enemy->isIdleInitialized)
        {
            EnemyMoveTimerUpdate(*enemy);
            enemy->isIdleInitialized = TRUE;
            enemy->isRotLock = TRUE;
        }

        EnemyIdle(*enemy);

        /**
         * @brief 待機アクションを実行
         * @return RUNNING／SUCCESS
         * @note 待機時間が終了したらSUCCESSを返す
         */
        if (enemy->moveTimer <= 0.0f)
        {
            enemy->isIdleInitialized = FALSE;
            enemy->isRotLock = FALSE;
            enemy->movePointChangeCount = 1;  // 次のWander用の移動ポイント設定準備
            return SUCCESS;
        }
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// ランダムな方向に移動するタスク
//-----------------------------------------------------------------------------
/**
 * @brief エネミーをランダムに徘徊させる
 *
 * 初回呼び出し時に移動タイマーをセットし、EnemyWander() を呼び出す。
 * moveTimer が 0 以下になると SUCCESS を返す。
 */
class WanderTask : public BTNode 
{
    ENEMY* enemy;
public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy 対象のエネミー
     */
    WanderTask(ENEMY* enemy) : enemy(enemy) {}

    // 徘徊行動を実行する
    BTStatus Tick() override 
    {
        if (!enemy->isIdleInitialized)  // IdleTask から移行してきた直後のみ
        {
            EnemyMoveTimerUpdate(*enemy);
            enemy->isIdleInitialized = TRUE;
        }

        enemy->isRotLock = FALSE; // 移動中は向きロック解除

        EnemyWander(*enemy);

        /**
         * @brief 徘徊アクションを実行
         * @return RUNNING／SUCCESS
         * @note 徘徊時間が終了したらSUCCESSを返す
         */
        if (enemy->moveTimer <= 0.0f)
        {
            enemy->isWanderInitialized = FALSE;
            enemy->movePointChangeCount = 1;
            return SUCCESS; // Idle終了
        }
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// 視界内にプレイヤーがいるか確認するタスク
//-----------------------------------------------------------------------------
/**
 * @brief 敵の視線方向内にプレイヤーがいるか判定する
 * 
 * CollisionSector() を使って、「forward」方向の扇形内に
 * プレイヤーがいるかをチェックし、見つけたらnoticeFlgをTRUEにする。
 *
 * @retval SUCCESS プレイヤーを発見
 * @retval FAILURE 視界外
 */
class CheckPlayerInSightTask : public BTNode
{
    ENEMY* enemy;
public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy 対象のエネミー
     */
    CheckPlayerInSightTask(ENEMY* enemy) : enemy(enemy) {}

    // 追跡行動を実行する
    BTStatus Tick() override
    {
        PLAYER* player = GetPlayer();
        enemy->isRotLock = FALSE; // 移動中は向きロック解除

        // 敵→プレイヤーのベクトルを正規化したもの
        float rad = enemy->rot.y;
        XMFLOAT3 forward = {
            sinf(rad),   // X 成分
            0.0f,        // Y 成分
            cosf(rad)    // Z 成分
        };

        if (CollisionSector(
                player->pos, 
                enemy->pos, 
                forward,
                ENEMY_NOTICE_LENGTH,
                RADIAN * 90.0f
            ))
        {
            enemy->noticeFlg = TRUE;  // 発見フラグを立てる
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
// 待機（Idle）と徘徊（Wander）のどちらかをランダムに選んで実行するタスク
//-----------------------------------------------------------------------------
/**
 * @brief IdleTask／WanderTask のいずれかをランダム選択して実行する
 * 
 * コンストラクタで IdleTask, WanderTask を生成し、Tick 呼び出し時には
 * selectedIndex == -1 であればランダムにどちらかのタスクを選択する。
 * 選択されたタスクが SUCCESS を返した際には再度未選択状態に戻り、
 * 次のサイクルで再びランダムに選び直す。
 */
class RandomActionTask : public BTNode
{
    ENEMY* enemy;                   // 操作対象のエネミー
    vector<BTNode*> actionTasks;    // Idle/Wander タスクを保持
    int selectedIndex;              // 選択中タスクのインデックス（-1: 未選択）

public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy このタスクが操作するエネミーへのポインタ
     *
     * Initialize() は呼ばずにIdleTaskとWanderTaskを生成して
     * actionTasks に格納し、selectedIndexを-1に設定する。
     */
    RandomActionTask(ENEMY* enemy) : enemy(enemy), selectedIndex(-1)
    {
        // Idle/Wanderを管理する（TODO:今後アクションを増やす）
        actionTasks.push_back(new EnemyIdleTask(enemy));
        actionTasks.push_back(new WanderTask(enemy));
    }
    /**
     * @brief タスクを進行させる
     *
     *  selectedIndex == -1 の場合、GetRand で 0 か 1 を選択し、
     *  enemy の初期化フラグ（isIdleInitialized/isWanderInitialized）をリセット。
     *  選択されたタスクの Tick() を呼び出し、そのステータスを返す。
     *  STATUS == SUCCESS の場合、selectedIndexを-1に戻し次サイクルで再選択。
     *
     * @return SUCCESS 選択中タスクが完了した場合
     * @return RUNNING 選択中タスクがまだ実行中の場合
     */
    BTStatus Tick() override
    {
        if (selectedIndex == -1)
        {
            selectedIndex = GetRand(0, (int)(actionTasks.size()) - 1);

            // 各タスクの初期化フラグをクリア
            enemy->isIdleInitialized = FALSE;
            enemy->isWanderInitialized = FALSE;
        }

        // 選択中のタスクをTick
        BTStatus status = actionTasks[selectedIndex]->Tick();

        // タスク完了したら次サイクル用に未選択状態にする
        if (status == SUCCESS)
        {
            selectedIndex = -1;
        }
        return status;
    }

    // 生成した子タスクを解放する（デストラクタ）
    ~RandomActionTask()
    {
        for (auto task : actionTasks) delete task;
    }
};

//-----------------------------------------------------------------------------
// プレイヤーを追跡し続けるタスク
//-----------------------------------------------------------------------------
/**
 * @brief プレイヤーを追いかける行動を常に実行するタスク
 * 
 * Tick() のたびに EnemyChase() を呼び出し、回転と移動を行う。
 * 常に RUNNING を返し、完了条件は別タスクで管理する想定。
 */
class ChaseActionTask : public BTNode
{
    ENEMY* enemy;
public:
    /**
     * @brief コンストラクタ
     * @param[in] enemy 追跡対象のエネミー
     */
    ChaseActionTask(ENEMY* enemy) : enemy(enemy) {}

    /**
     * @brief 追跡処理を実行し、常に RUNNING を返す
     * @return RUNNING 追跡継続中
     */
    BTStatus Tick() override
    {
        enemy->isRotLock = FALSE;
        EnemyChase(*enemy);
        return RUNNING;
    }
};

//-----------------------------------------------------------------------------
// noticeFlg とプレイヤーとの距離をチェックし、フラグをリセットするタスク
//-----------------------------------------------------------------------------
/**
 * @brief プレイヤーから一定距離離れたら noticeFlg をクリアする
 *
 *   プレイヤーとの水平距離 > ENEMY_VIGILANCE_LENGTH の場合、
 *   noticeFlg を FALSE に設定し FAILURE を返す。
 *   それ以外で noticeFlg が TRUE の場合は SUCCESS、
 *   それ以外は FAILURE を返す。
 */
class CheckNoticeTask : public BTNode 
{
    ENEMY* m_enemy;
public:
    /**
     * @brief コンストラクタ
     * @param[in] e 距離チェック対象のエネミー
     */
    CheckNoticeTask(ENEMY* e) : m_enemy(e) {}

    /**
     * @brief プレイヤーとの距離・フラグを評価してステータスを返す
     * @return SUCCESS noticeFlgがTRUEの場合
     * @return FAILURE noticeFlgがFALSEへのクリア時、または範囲外
     */
    BTStatus Tick() override 
    {
        PLAYER* player = GetPlayer();
        float dx = player->pos.x - m_enemy->pos.x;
        float dz = player->pos.z - m_enemy->pos.z;
        float distSq = dx * dx + dz * dz;
        static const float vigilSq = ENEMY_VIGILANCE_LENGTH_SQ;
        // 一定距離以上離れたらフラグを切ってFAILUREを返す
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


