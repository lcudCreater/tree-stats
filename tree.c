#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* 简单且安全的字符串复制（避免平台 strdup 不一致） */
static char* strdup_s(const char* s)
{
    if (!s)
    {
        return NULL;
    }

    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (!dup)
    {
        return NULL;
    }

    memcpy(dup, s, len);
    return dup;
}

/* 创建节点 */
TreeNode* tree_create_node(const char* data)
{
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node)
    {
        return NULL;
    }

    node->data = strdup_s(data);
    node->first_child = NULL;
    node->next_sibling = NULL;
    return node;
}

/* 释放整棵树（传入的 node 可能是一个兄弟链的起点） */
void tree_free(TreeNode* root)
{
    if (!root)
    {
        return;
    }

    /* 先释放孩子链 */
    if (root->first_child)
    {
        tree_free(root->first_child);
    }

    /* 再释放兄弟链 */
    if (root->next_sibling)
    {
        tree_free(root->next_sibling);
    }

    free(root->data);
    free(root);
}

/* 从控制台创建树（简化交互：只创建单个根节点；可按需扩展） */
static TreeNode* tree_create_from_console_internal(const char* prompt)
{
    char buf[256];

    if (!prompt)
    {
        prompt = "输入节点数据（输入#表示空）：";
    }

    /* 提示并读取一行输入 */
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin))
    {
        /* EOF 或读取错误，按空处理 */
        return NULL;
    }

    /* 去掉末尾换行（处理 LF 或 CRLF） */
    size_t n = strlen(buf);
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
    {
        buf[--n] = '\0';
    }

    /* 输入 '#' 表示此位置为空 */
    if (buf[0] == '#' && buf[1] == '\0')
    {
        return NULL;
    }

    /* 创建节点 */
    TreeNode* node = tree_create_node(buf);
    if (!node)
    {
        return NULL;
    }

    /* 为第一个孩子构造提示并递归创建 */
    char child_prompt[256];
    if (snprintf(child_prompt, sizeof(child_prompt), "请创建【%s】的第一个孩子节点：", node->data) >= (int)sizeof(child_prompt))
    {
        /* 若提示被截断也继续，但提示仍可用 */
        child_prompt[sizeof(child_prompt) - 1] = '\0';
    }
    node->first_child = tree_create_from_console_internal(child_prompt);

    /* 为下一个兄弟构造提示并递归创建 */
    char sibling_prompt[256];
    if (snprintf(sibling_prompt, sizeof(sibling_prompt), "请创建【%s】的下一个兄弟节点：", node->data) >= (int)sizeof(sibling_prompt))
    {
        sibling_prompt[sizeof(sibling_prompt) - 1] = '\0';
    }
    node->next_sibling = tree_create_from_console_internal(sibling_prompt);

    return node;
}

/* 对外接口：从控制台创建一棵树（以先根顺序交互） */
TreeNode* tree_create_from_console(void)
{
    return tree_create_from_console_internal("输入节点数据（输入#表示空）：");
}

/* 节点计数（包含传入节点及其所有子孙和兄弟） */
size_t tree_count_nodes(const TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    return 1 + tree_count_nodes(root->first_child) + tree_count_nodes(root->next_sibling);
}

/* 叶节点计数（没有孩子的节点） */
size_t tree_count_leaves(const TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    size_t count = 0;
    if (!root->first_child)
    {
        count = 1;
    }

    count += tree_count_leaves(root->first_child);
    count += tree_count_leaves(root->next_sibling);
    return count;
}

/* 非叶节点计数（有至少一个孩子的节点） */
size_t tree_count_non_leaves(const TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    size_t count = 0;
    if (root->first_child)
    {
        count = 1;
    }

    count += tree_count_non_leaves(root->first_child);
    count += tree_count_non_leaves(root->next_sibling);
    return count;
}

/* 单个节点的度（孩子数） */
size_t tree_node_degree(const TreeNode* node)
{
    if (!node || !node->first_child)
    {
        return 0;
    }

    size_t deg = 0;
    const TreeNode* c = node->first_child;
    while (c)
    {
        deg++;
        c = c->next_sibling;
    }

    return deg;
}

/* 树的最大度（遍历所有节点取最大孩子数） */
static size_t max_size_t(size_t a, size_t b)
{
    return (a > b) ? a : b;
}

size_t tree_max_degree(const TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    size_t deg = tree_node_degree(root);
    size_t left_max = tree_max_degree(root->first_child);
    size_t right_max = tree_max_degree(root->next_sibling);
    return max_size_t(deg, max_size_t(left_max, right_max));
}

/* 树的深度（根到最远叶子的层数，空树为 0，只有根为 1） */
size_t tree_depth(const TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    size_t child_depth = tree_depth(root->first_child);
    size_t sibling_depth = tree_depth(root->next_sibling);

    /* 深度相对于当前节点：子树深度 + 1（当前节点算一层） */
    return max_size_t(child_depth + 1, sibling_depth);
}

/* 先根（前序）遍历：访问节点 -> 遍历孩子链 -> 遍历兄弟链 */
void tree_preorder(const TreeNode* root, void (*visit)(const TreeNode*))
{
    if (!root)
    {
        return;
    }

    if (visit)
    {
        visit(root);
    }

    tree_preorder(root->first_child, visit);
    tree_preorder(root->next_sibling, visit);
}

/* 后根（后序）遍历：遍历孩子链 -> 访问节点 -> 遍历兄弟链 */
void tree_postorder(const TreeNode* root, void (*visit)(const TreeNode*))
{
    if (!root)
    {
        return;
    }

    tree_postorder(root->first_child, visit);

    if (visit)
    {
        visit(root);
    }

    tree_postorder(root->next_sibling, visit);
}

/* 层次遍历（广度优先），将同一级的兄弟按输入顺序访问 */
void tree_level_order(const TreeNode* root, void (*visit)(const TreeNode*))
{
    if (!root)
    {
        return;
    }

    /* 动态队列（指针数组），按需扩容 */
    size_t cap = 128;
    size_t head = 0;
    size_t tail = 0;
    TreeNode** queue = (TreeNode**)malloc(sizeof(TreeNode*) * cap);
    if (!queue)
    {
        return;
    }

    /* 将传入节点及其兄弟链作为起始层 */
    const TreeNode* p = root;
    while (p)
    {
        if (tail >= cap)
        {
            cap *= 2;
            queue = (TreeNode**)realloc(queue, sizeof(TreeNode*) * cap);
            if (!queue)
            {
                return;
            }
        }
        queue[tail++] = (TreeNode*)p; /* 只在此处临时移除 const */
        p = p->next_sibling;
    }

    while (head < tail)
    {
        TreeNode* cur = queue[head++];
        if (visit)
        {
            visit(cur);
        }

        /* 将当前节点的所有孩子加入队列（孩子按 next_sibling 链顺序） */
        TreeNode* child = cur->first_child;
        while (child)
        {
            if (tail >= cap)
            {
                cap *= 2;
                queue = (TreeNode**)realloc(queue, sizeof(TreeNode*) * cap);
                if (!queue)
                {
                    return;
                }
            }
            queue[tail++] = child;
            child = child->next_sibling;
        }
    }

    free(queue);
}

/* 按 data 字符串查找节点（返回首个匹配项） */
const TreeNode* tree_find_by_data(const TreeNode* root, const char* data)
{
    if (!root || !data)
    {
        return NULL;
    }

    if (root->data && strcmp(root->data, data) == 0)
    {
        return root;
    }

    const TreeNode* res = tree_find_by_data(root->first_child, data);
    if (res)
    {
        return res;
    }

    return tree_find_by_data(root->next_sibling, data);
}

/*
详细伪代码（中文）：

函数：buildTreeFromFile(const char* filename)
目标：从指定文本文件读取树的节点信息，动态创建节点数组并根据索引连接 first_child 和 next_sibling，返回根节点（索引 0）。

步骤：
1. 参数检查：如果 filename 为 NULL，返回 NULL。
2. 打开文件：使用 fopen，若失败返回 NULL。
3. 读取并解析第一行：跳过空行，读取整数 n（节点总数）。若读取失败或 n <= 0，关闭文件并返回 NULL（n==0 返回 NULL）。
4. 分配辅助数组：
   - TreeNode** nodes = calloc(n, sizeof(TreeNode*));
   - int* child_idx = malloc(n * sizeof(int));
   - int* sibling_idx = malloc(n * sizeof(int));
   若任一分配失败，释放已分配资源并返回 NULL。
5. 对 0..n-1:
   - 读取下一非空行（允许行之间有空行）。
   - 使用 sscanf 解析：一个不含空格的字符串（节点数据）和两个 int（child, sibling）。
   - 解析失败 -> 清理并返回 NULL。
   - 使用 tree_create_node 创建节点，若失败 -> 清理并返回 NULL。
   - 保存 nodes[i]、child_idx[i]、sibling_idx[i]。
6. 验证所有索引合法性（索引为 -1 或在 [0, n-1] 内），若非法 -> 清理并返回 NULL。
7. 连接指针：
   - for each i: nodes[i]->first_child = (child_idx[i] == -1) ? NULL : nodes[child_idx[i]];
                 nodes[i]->next_sibling = (sibling_idx[i] == -1) ? NULL : nodes[sibling_idx[i]];
8. 释放辅助数组 child_idx、sibling_idx、nodes（只释放数组本身，不释放节点）。
9. 关闭文件。
10. 返回 nodes[0] 作为根（若 n > 0），否则返回 NULL。

错误处理和清理策略：
- 提供内部清理函数，确保释放已创建的节点及分配的数组。
- 遵循现有代码风格，使用 tree_create_node 创建节点，使用 free 释放节点内存（释放 data 字符串并 free 节点指针）。

注意：
- 该实现假定每行的节点数据是不含空白的单词（与题目格式一致）。
- 若需要支持含空格的数据，应改为按定长字段或引号包裹的解析，这里按题目要求实现简单解析。
*/

TreeNode* buildTreeFromFile(const char* filename)
{
    if (!filename)
    {
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        return NULL;
    }

    char line[512];

    /* 读取第一行（节点总数），跳过空白行 */
    int n = -1;
    while (fgets(line, sizeof(line), fp))
    {
        /* 去掉行末换行 */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[--len] = '\0';
        }

        /* 跳过空行 */
        char* p = line;
        while (*p && (*p == ' ' || *p == '\t'))
        {
            p++;
        }

        if (*p == '\0')
        {
            continue;
        }

        if (sscanf(p, "%d", &n) == 1)
        {
            break;
        }
        else
        {
            /* 第一行格式错误 */
            fclose(fp);
            return NULL;
        }
    }

    if (n <= 0)
    {
        fclose(fp);
        return NULL;
    }

    /* 分配辅助数组 */
    TreeNode** nodes = (TreeNode**)calloc((size_t)n, sizeof(TreeNode*));
    int* child_idx = (int*)malloc(sizeof(int) * (size_t)n);
    int* sibling_idx = (int*)malloc(sizeof(int) * (size_t)n);

    if (!nodes || !child_idx || !sibling_idx)
    {
        /* 释放已分配项 */
        if (nodes) free(nodes);
        if (child_idx) free(child_idx);
        if (sibling_idx) free(sibling_idx);
        fclose(fp);
        return NULL;
    }

    /* 初始化节点指针数组为 NULL（calloc 已做） */
    int read_count = 0;
    while (read_count < n && fgets(line, sizeof(line), fp))
    {
        /* 去掉行末换行 */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[--len] = '\0';
        }

        /* 跳过前导空白 */
        char* p = line;
        while (*p && (*p == ' ' || *p == '\t'))
        {
            p++;
        }

        /* 跳过空行 */
        if (*p == '\0')
        {
            continue;
        }

        char data[256];
        int ci = -1;
        int si = -1;
        /* 解析：数据（无空格） 后跟两个整数 */
        int matched = sscanf(p, "%255s %d %d", data, &ci, &si);
        if (matched != 3)
        {
            /* 解析失败，清理并返回 */
            for (int j = 0; j < read_count; ++j)
            {
                if (nodes[j])
                {
                    free(nodes[j]->data);
                    free(nodes[j]);
                }
            }
            free(nodes);
            free(child_idx);
            free(sibling_idx);
            fclose(fp);
            return NULL;
        }

        TreeNode* node = tree_create_node(data);
        if (!node)
        {
            for (int j = 0; j < read_count; ++j)
            {
                if (nodes[j])
                {
                    free(nodes[j]->data);
                    free(nodes[j]);
                }
            }
            free(nodes);
            free(child_idx);
            free(sibling_idx);
            fclose(fp);
            return NULL;
        }

        nodes[read_count] = node;
        child_idx[read_count] = ci;
        sibling_idx[read_count] = si;
        read_count++;
    }

    if (read_count != n)
    {
        /* 行数不足，清理 */
        for (int j = 0; j < read_count; ++j)
        {
            if (nodes[j])
            {
                free(nodes[j]->data);
                free(nodes[j]);
            }
        }
        free(nodes);
        free(child_idx);
        free(sibling_idx);
        fclose(fp);
        return NULL;
    }

    /* 验证索引合法性并连接指针 */
    for (int i = 0; i < n; ++i)
    {
        int ci = child_idx[i];
        int si = sibling_idx[i];

        if (ci != -1 && (ci < 0 || ci >= n))
        {
            /* 索引非法，清理 */
            for (int j = 0; j < n; ++j)
            {
                if (nodes[j])
                {
                    free(nodes[j]->data);
                    free(nodes[j]);
                }
            }
            free(nodes);
            free(child_idx);
            free(sibling_idx);
            fclose(fp);
            return NULL;
        }

        if (si != -1 && (si < 0 || si >= n))
        {
            /* 索引非法，清理 */
            for (int j = 0; j < n; ++j)
            {
                if (nodes[j])
                {
                    free(nodes[j]->data);
                    free(nodes[j]);
                }
            }
            free(nodes);
            free(child_idx);
            free(sibling_idx);
            fclose(fp);
            return NULL;
        }

        nodes[i]->first_child = (ci == -1) ? NULL : nodes[ci];
        nodes[i]->next_sibling = (si == -1) ? NULL : nodes[si];
    }

    /* 保存根节点指针 */
    TreeNode* root = nodes[0];

    /* 释放辅助数组（不释放节点） */
    free(nodes);
    free(child_idx);
    free(sibling_idx);

    fclose(fp);
    return root;
}

/*
详细实现计划（伪代码，中文）：

目标：实现 void tree_print_shape(const TreeNode* root)；
要求：用纯 ASCII 字符（'/', '\', '``'（两个反引号），'|'）以可视化缩进格式打印树的形状，每行显示节点 data。

思路：
1. 使用先根遍历，但在打印时构造前缀（缩进与竖线）来表示树的分支关系。
2. 维护一个布尔数组 stack_flags，stack_flags[i] 表示在第 i 层（从 0 开始计）对应的祖先节点是否存在“后续兄弟”：
   - 如果 stack_flags[i] == 1，则该列应打印 '|' 作为垂直延续线；
   - 否则打印若干空格。
3. 对于当前节点打印：
   - 若 depth == 0（根节点），直接打印节点数据（无前导分支符号）。
   - 否则：
       - 根据 stack_flags[0..depth-1] 输出前导列（'|  ' 或 '   '，保持对齐）。
       - 再输出分支符号：
           - 如果当前节点有 next_sibling（不是该层最后一个），输出 "/ "（表示后面还有兄弟）。
           - 否则输出 "`` "（两个反引号 + 空格，表示该层的最后一个分支）。
       - 然后输出节点数据并换行。
4. 递归处理孩子：
   - 在递归前设置 stack_flags[depth] = (当前节点有 next_sibling) ? 1 : 0；
   - 对当前节点的所有孩子按 next_sibling 顺序遍历并递归调用，depth + 1。
5. 顶层（传入的 root 可能是一个兄弟链的起点）：
   - 为每个顶层节点（root 及其 next_sibling）调用打印逻辑，depth 为 0，初始 stack_flags 为空或全 0。
6. 实现细节：
   - 使用动态分配的 int* 作为 stack_flags，并在需要时扩容（重用同一数组，递归安全：同一数组在递归深度上被共享，并通过 depth 索引写入）。
   - 对于节点 data 为 NULL 的情况，打印 "(null)"。
   - 所有输出使用 printf，保证只使用指定的 ASCII 分支字符。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 辅助递归函数：打印单个节点及其子树。
   参数：
     node  - 当前节点（非 NULL）
     depth - 当前深度（根深度为 0）
     pflags - 指向 int*，表示已分配的标志数组指针（可重分配）
     pcap   - 指向当前 flags 数组容量（元素个数）
   语义：
     flags[i] 表示在第 i 层（从 0 开始）对应的祖先是否存在后续兄弟。
*/
static void tree_print_shape_print_node(const TreeNode* node, int depth, int** pflags, int* pcap)
{
    if (!node)
    {
        return;
    }

    /* 保证 flags 数组足够大以容纳 depth 索引（用于子层写入 flags[depth]） */
    if (depth >= *pcap)
    {
        int newcap = (*pcap == 0) ? 16 : (*pcap * 2);
        while (depth >= newcap)
        {
            newcap *= 2;
        }
        int* newflags = (int*)realloc(*pflags, sizeof(int) * (size_t)newcap);
        if (!newflags)
        {
            /* 内存分配失败：尽量继续但不写入 flags（会假定为 0） */
            return;
        }
        /* 初始化新扩展的部分为 0 */
        for (int i = *pcap; i < newcap; ++i)
        {
            newflags[i] = 0;
        }
        *pflags = newflags;
        *pcap = newcap;
    }

    /* 打印当前节点（构造前缀） */
    if (depth == 0)
    {
        /* 根节点直接打印 */
        printf("%s\n", node->data ? node->data : "(null)");
    }
    else
    {
        /* 前缀：根据祖先层的 flags 决定是否打印 '|' 或空格，保持宽度两字符以对齐 */
        int* flags = *pflags;
        for (int i = 0; i < depth; ++i)
        {
            if (flags[i])
            {
                /* 祖先在该层之后还有兄弟：打印垂直线 */
                printf("|  ");
            }
            else
            {
                /* 没有后续兄弟：打印空白占位 */
                printf("   ");
            }
        }

        /* 当前节点的分支符号：若有 next_sibling 则表示不是最后一个，用 "/ "；否则用 "`` " 表示最后一个分支 */
        int is_last = (node->next_sibling == NULL);
        if (!is_last)
        {
            /* 不是最后一个兄弟 */
            printf("/ %s\n", node->data ? node->data : "(null)");
        }
        else
        {
            /* 最后一个兄弟，使用两个反引号表示终结分支 */
            printf("`` %s\n", node->data ? node->data : "(null)");
        }
    }

    /* 为孩子设置当前层的 flags：如果当前节点有 next_sibling，则在孩子层对应列应显示 '|' */
    (*pflags)[depth] = (node->next_sibling != NULL) ? 1 : 0;

    /* 遍历所有孩子（按 next_sibling 顺序），递归打印 */
    const TreeNode* child = node->first_child;
    while (child)
    {
        tree_print_shape_print_node(child, depth + 1, pflags, pcap);
        child = child->next_sibling;
    }

    /* 递归返回时，不需要显式清除 (*pflags)[depth]，调用者在其它分支会覆盖 */
}

/* 对外接口：打印整棵树的形状（root 可能是兄弟链的起点） */
void tree_print_shape(const TreeNode* root)
{
    if (!root)
    {
        return;
    }

    int* flags = NULL;
    int cap = 0;

    /* 顶层可能由一串兄弟组成（root 及其 next_sibling），依次打印每个顶层节点 */
    const TreeNode* p = root;
    while (p)
    {
        tree_print_shape_print_node(p, 0, &flags, &cap);
        p = p->next_sibling;
    }

    free(flags);
}