#pragma once
#ifndef TREE_H
#define TREE_H

#include <stddef.h>

typedef struct TreeNode
{
    char* data;                       /* 节点数据（可按需改为固定类型或泛型） */
    struct TreeNode* first_child;     /* 第一个孩子指针 */
    struct TreeNode* next_sibling;    /* 下一个兄弟指针 */
} TreeNode;

/* 创建与销毁 */
TreeNode* tree_create_node(const char* data);
void tree_free(TreeNode* root);

/* 构造辅助（按需实现：从控制台或文件构建） */
TreeNode* tree_create_from_console(void);
TreeNode* buildTreeFromFile(const char* filename);


/* 基本统计 */
size_t tree_count_nodes(const TreeNode* root);
size_t tree_count_leaves(const TreeNode* root);
size_t tree_count_non_leaves(const TreeNode* root);

/* 节点的度（单个节点度与树的最大度） */
size_t tree_node_degree(const TreeNode* node);
size_t tree_max_degree(const TreeNode* root);

/* 树的深度（高度） */
size_t tree_depth(const TreeNode* root);

/* 遍历（visit 回调接收 TreeNode*）*/
void tree_preorder(const TreeNode* root, void (*visit)(const TreeNode*));
void tree_postorder(const TreeNode* root, void (*visit)(const TreeNode*));
void tree_level_order(const TreeNode* root, void (*visit)(const TreeNode*));

/* 查找/辅助 */
const TreeNode* tree_find_by_data(const TreeNode* root, const char* data);
void tree_print_shape(const TreeNode* root);

#endif /* TREE_H */