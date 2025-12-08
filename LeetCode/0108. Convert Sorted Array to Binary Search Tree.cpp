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
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        function<TreeNode*(int,int)> build = [&](int l, int r) -> TreeNode* {
            if (l > r) return nullptr;

            int mid = l + (r - l) / 2;
            TreeNode* root = new TreeNode(nums[mid]);

            root->left = build(l, mid - 1);
            root->right = build(mid + 1, r);

            return root;
        };

        return build(0, nums.size() - 1);
    }
};
