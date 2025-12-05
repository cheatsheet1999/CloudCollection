class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> res;
        vector<int> path;

        function<void(int, int)> backtrack = [&](int start, int target) {
            if(target == 0) {
                res.push_back(path);
                return;
            }
            for(int i = start; i < candidates.size(); i++) {
                if(candidates[i] > target) continue;
                path.push_back(candidates[i]);
                backtrack(i, target - candidates[i]);
                path.pop_back();
            }
        };

        backtrack(0, target);
        return res;
    }
};
