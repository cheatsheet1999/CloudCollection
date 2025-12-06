struct TrieNode {
    // a hashmap, e.g. children['a'] = a Node point to 'a'
    unordered_map<char, TrieNode*> children;
    string endword;
};

class Solution {
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        TrieNode* root = new TrieNode();
        for(auto word : words) {
            TrieNode* node = root;
            for(char letter : word) {
                if(!node->children[letter]) node->children[letter] = new TrieNode();
                node = node->children[letter];
            }
            node->endword = word;
        }

        int m = board.size();
        int n = board[0].size();
        vector<string> res;

        function<void(int, int, TrieNode*)> backtrack = [&](int i, int j, TrieNode* node) {
            char letter = board[i][j];
            auto it = node->children.find(letter);
            if(it == node->children.end()) return;
            TrieNode* curr = it->second;

            if(!curr->endword.empty()) {
                res.push_back(curr->endword);
                curr->endword.clear();
            }

            board[i][j] = '.';
            pair<int, int> dirs[] = {{-1,0},{1,0},{0,-1},{0,1}};
            for(auto [di, dj] : dirs) {
                int x = i + di;
                int y = j + dj;
                if(x >= 0 && y >= 0 && x < m && y < n && board[x][y] != '.') {
                    backtrack(x, y, curr);
                }
            }
            board[i][j]= letter;
        };

        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                backtrack(i, j, root);
            }
        }

        return res;
    }
};
