//=============================================================================
//
// ビヘイビアツリー [behaviorTree.h]
// Author : 白井陽大
//
//=============================================================================
#pragma once
#include <vector>

using std::vector;

enum BTStatus
{
    SUCCESS,        // 0：成功
    FAILURE,        // 1：失敗
    RUNNING         // 2：実行中
};

// 基底ノードクラス
class BTNode 
{
public:
    virtual BTStatus Tick() = 0;
    virtual ~BTNode() {}
};

// コンポジットノード（子ノードを持つ）
class CompositeNode : public BTNode
{
protected:
    vector<BTNode*> children;
public:
    void AddChild(BTNode* child)
    {
        children.push_back(child);
    }
    virtual ~CompositeNode() 
    {
        for (auto child : children) delete child;
    }
};

// シーケンスノード（順番に実行）
class SequenceNode : public CompositeNode 
{
public:
    BTStatus Tick() override 
    {
        for (auto child : children) 
        {
            BTStatus status = child->Tick();
            if (status != SUCCESS)
            {
                return status;
            }
        }
        return SUCCESS;
    }
};

// セレクターノード（成功するノードを探す）
class SelectorNode : public CompositeNode 
{
public:
    BTStatus Tick() override 
    {
        for (auto child : children) 
        {
            BTStatus status = child->Tick();
            if (status != FAILURE)
            {
                return status;
            }
        }
        return FAILURE;
    }
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


