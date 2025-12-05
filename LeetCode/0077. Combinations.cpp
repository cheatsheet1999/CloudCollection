class Solution {
public:
    vector<vector<int>> combine(int n, int k) {
        vector<vector<int>> res;
        vector<int> path;

        function<void(int)> backtrack = [&](int start) {
            if(path.size() == k) {
                res.push_back(path);
                return;
            }
            for(int i = start; i <= n; i++) {
                 path.push_back(i);
                 backtrack(i + 1);
                 path.pop_back();
            }
        };

        backtrack(1);
        return res;
    }
};
