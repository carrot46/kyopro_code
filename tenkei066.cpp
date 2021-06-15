// マージソートをベースにした O(N log N) 解法
// 期待値が次のように言い換えられることを利用した：
// 0-based, 各区間は [L[i], R[i]) に変換済み, range[i] := R[i] - L[i] として、
// sum_{ 0 <= i < j < N, L[i] <= x < R[i], L[j] <= y < R[j], x > y} ( 1.0 / (range[i] * range[j]) )
#include <bits/stdc++.h>
using namespace std;

using P = pair<int, int>;
using T = double;
const T two_inv = 1.0 / 2.0;

int N;
vector<int> L, R;
vector<P> val;
vector<T> range_inv;
T res;

// mergesort(left_id, right_id) では、上記 sum の内、left_id <= i < center_id <= j < right_id を満たすものを計算する
// 計算方法： L, R をまとめたもの (val) を小さい順に見て、( val による区間内の y ) * ( ありうる x ) を計算している
void mergesort(int left_id, int right_id){
    if(right_id - left_id == 1) return;
    int center_id = left_id + (right_id - left_id) / 2;
    mergesort(left_id, center_id);
    mergesort(center_id, right_id);
    vector<P> lval((center_id - left_id) * 2), rval((right_id - center_id) * 2);

    copy(val.begin() + left_id * 2, val.begin() + center_id * 2, lval.begin());
    lval.emplace_back(1<<30, 1<<30);
    copy(val.begin() + center_id * 2, val.begin() + right_id * 2, rval.begin());
    rval.emplace_back(1<<30, 1<<30);

    int i{}, j{}, k = left_id * 2, last = -1;
    T left_seg{}, right_seg{}, left_sum = center_id - left_id;
    while(k < right_id * 2){
        bool use_left = lval[i] < rval[j];
        val[k] = use_left ? lval[i] : rval[j];
        int d = val[k].first - last, id = val[k].second;
        if(d > 0) {
            // 幅 d の区間内に x, y があるパターン
            res += left_seg * right_seg * d * (d - 1) * two_inv;
            left_sum -= left_seg * d;
            // 幅 d の区間内に y, それより上に x があるパターン
            res += right_seg * d * left_sum;
            last += d;
        }
        (use_left ? left_seg : right_seg) += (id&1 ? - range_inv[id>>1] : range_inv[id>>1]);
        use_left ? ++i : ++j;
        ++k;
    }
}

int main() {
    cin>>N;
    L.resize(N); R.resize(N);
    range_inv.resize(N);
    for(int i = 0; i < N; ++i) {
        cin>>L[i]>>R[i];
        --L[i];
        range_inv[i] = (T)1 / (R[i] - L[i]);
        val.emplace_back(L[i], i * 2);
        val.emplace_back(R[i], i * 2 + 1);
    }
    mergesort(0, N);
    cout<<fixed<<setprecision(10)<<res<<endl;
}
