/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Codec {
public:

    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        ostringstream out;
        function<void(TreeNode*)> dfs = [&](TreeNode* node) {
            if(!node) {
                out << "#,";
                return;
            }
            out << node->val << ",";
            dfs(node->left);
            dfs(node->right);
        };
        dfs(root);
        return out.str();
    }

    // Decodes your encoded data to tree.
    TreeNode* deserialize(string data) {
        istringstream in(data);
        function<TreeNode*()> dfs = [&]() -> TreeNode* {
            string val;
            getline(in, val, ',');
            if(val == "#") return nullptr;

            TreeNode* node = new TreeNode(stoi(val));
            node->left = dfs();
            node->right = dfs();
            return node;
        };
        return dfs();
    };
};

// Your Codec object will be instantiated and called as such:
// Codec ser, deser;
// TreeNode* ans = deser.deserialize(ser.serialize(root));


/*
TC O(n)
SC O(n)
*/
