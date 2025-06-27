//=============================================================================
//
// �r�w�C�r�A�c���[ [behaviorTree.h]
// Author : ����z��
//
//=============================================================================
#pragma once
#include <vector>

using std::vector;

enum BTStatus
{
    SUCCESS,        // 0�F����
    FAILURE,        // 1�F���s
    RUNNING         // 2�F���s��
};

// ���m�[�h�N���X
class BTNode 
{
public:
    virtual BTStatus Tick() = 0;
    virtual ~BTNode() {}
};

// �R���|�W�b�g�m�[�h�i�q�m�[�h�����j
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

// �V�[�P���X�m�[�h�i���ԂɎ��s�j
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

// �Z���N�^�[�m�[�h�i��������m�[�h��T���j
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
// �v���g�^�C�v�錾
//*****************************************************************************


