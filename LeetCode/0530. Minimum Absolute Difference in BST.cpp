/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    int getMinimumDifference(TreeNode* root) {
        int res = INT_MAX;
        TreeNode* prev = nullptr;

        function<void(TreeNode*)> dfs = [&](TreeNode* node) {
            if(!node) return;
            dfs(node->left);
            if(prev) {
                res = min(res, node->val - prev->val);
            }
            prev = node;
            dfs(node->right);
        };

        dfs(root);
        return res;
    }
};


/*
Time Complexity：O(n) 
Space Complexity：O(h)
*/
