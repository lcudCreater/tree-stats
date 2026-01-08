#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* 简单打印回调示例 */
static void print_node(const TreeNode* node)
{
    if (node && node->data)
    {
        printf("%s ", node->data);
    }
}

int main(void)
{
    TreeNode* root = NULL;
    char choice_buf[16];
    char filename[256]; /* 输入缓冲区：文件名 */
    int choice = 0;

    for (;;)
    {
        printf("\n======== 树型结构信息统计 ========\n");
        printf("0. 退出\n");
        printf("1. 从控制台创建树\n");
        printf("2. 从文件加载树\n");
        printf("3. 显示节点总数\n");
        printf("4. 显示叶节点个数\n");
        printf("5. 显示非叶节点个数\n");
        printf("6. 查询节点的度 / 树的最大度\n");
        printf("7. 显示树的深度\n");
        printf("8. 显示树形结构\n");
        printf("9. 层次遍历\n");
        printf("10. 先根遍历\n");
        printf("11. 后根遍历\n");
        printf("12. 释放当前树\n");
        printf("请选择（数字）: ");

        if (!fgets(choice_buf, sizeof(choice_buf), stdin))
        {
            clearerr(stdin);
            continue;
        }

        choice = atoi(choice_buf);

        switch (choice)
        {
        case 1:
            if (root) /* 先释放原有树 */
            {
                tree_free(root);
                root = NULL;
            }
            root = tree_create_from_console();
            break;

        case 2:
            if (root) /* 先释放原有树 */
            {
                tree_free(root);
                root = NULL;
            }
            printf("请输入文件名（包括路径）: ");
            if (!fgets(filename, sizeof(filename), stdin)) /* 读取文件名 */
            {
                clearerr(stdin);
                continue;
            }
            filename[strcspn(filename, "\n")] = 0; /* 去除换行符 */
            if (strlen(filename) == 0) /* 空文件名无效 */
            {
                printf("无效的文件名。\n");
                continue;
            }
            root = buildTreeFromFile(filename); /* 从文件构建树 */
            if (!root) { printf("树加载失败，请检查文件格式或路径。\n"); }
            else { printf("树加载成功。\n"); }
            break;

        case 3:
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("节点总数: %zu\n", tree_count_nodes(root));
            break;

        case 4:
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("叶节点个数: %zu\n", tree_count_leaves(root));
            break;

        case 5:
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("非叶节点个数: %zu\n", tree_count_non_leaves(root));
            break;

        case 6:
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("树的最大度: %zu\n", tree_max_degree(root));
            /* 若要查询指定节点的度，可先用 tree_find_by_data 找到节点再调用 tree_node_degree */
            break;

        case 7:
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("树的深度: %zu\n", tree_depth(root));
            break;

        case 8: /* 显示树形结构 */
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            tree_print_shape(root);
            break;

        case 9: /* 层次遍历 */
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            tree_level_order(root, print_node);
            printf("\n");
            break;

        case 10: /* 先根遍历（原 8） */
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("先根遍历: ");
            tree_preorder(root, print_node);
            printf("\n");
            break;

        case 11: /* 后根遍历（原 9） */
            if (!root) { printf("请先创建或加载一棵树！\n"); break; }
            printf("后根遍历: ");
            tree_postorder(root, print_node);
            printf("\n");
            break;

        case 12: /* 释放当前树（原 10） */
            if (root)
            {
                tree_free(root);
                root = NULL;
                printf("已释放当前树。\n");
            }
            else
            {
                printf("当前无树可释放。\n");
            }
            break;

        case 0:
            if (root) tree_free(root);
            printf("退出程序。\n");
            return 0;

        default:
            printf("无效选择，请重试。\n");
            break;
        }
    }

    return 0;
}