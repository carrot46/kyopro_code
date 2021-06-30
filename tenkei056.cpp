#include <bits/stdc++.h>
using namespace std;

// Hirschberg's Algorithm を用いて空間計算量を削減した解法
// w = wordsize として、O(N + (NS/w)) time, O(N + (S/w)) space

// bitset 的なもの
// n := size / 32 個の unsigned int にデータを管理
// [], set_true : O(1) time
// 各種 bit 演算, minimum_bit : O(n) = O(size / wordsize) time
struct mybts{
    unsigned int n;
    vector<unsigned int> v;
    mybts(unsigned int _n) : n(_n), v((_n + 31)>>5, 0){}
    const bool operator[](unsigned int i){
        if(i >= n) return false;
        return (v[i>>5]>>(i&31))&1;
    }
    void set_true(unsigned int i){
        assert(i < n);
        v[i>>5] |= 1<<(i&31);
    }
    mybts &operator |= (const mybts &a){
        assert(n == a.n);
        for(int i{}; i < size(a.v); ++i) v[i] |= a.v[i];
        return *this;
    }
    mybts operator | (const mybts &a){
        mybts tmp(*this);
        return tmp |= a;
    }
    mybts &operator &= (const mybts &a){
        assert(n == a.n);
        for(int i{}; i < size(a.v); ++i) v[i] &= a.v[i];
        return *this;
    }
    mybts operator & (const mybts &a){
        mybts tmp(*this);
        return tmp &= a;
    }
    mybts &operator >>= (const unsigned int &a){
        unsigned int move_index = a>>5, move_each = a&31, mask = 1, move_each_rev = 32 - move_each;
        if(move_each) {
            (mask <<= move_each) -= 1;
            v[0] >>= move_each;
            for (int i{1}; i < size(v); ++i) {
                v[i - 1] |= (v[i] & mask) << move_each_rev;
                v[i] >>= move_each;
            }
        }
        if(move_index) {
            for (int i{}; i < size(v); ++i) {
                if (i + move_index < size(v)) v[i] = v[i + move_index];
                else v[i] = 0;
            }
        }
        return *this;
    }
    mybts operator >> (const unsigned int &a){
        mybts tmp(*this);
        return tmp >>= a;
    }
    mybts &operator <<= (const unsigned int &a){
        unsigned int move_index = a>>5, move_each = a&31, move_each_rev = 32 - move_each;
        if(move_each){
            v.back() <<= move_each;
            for(int i = (int)size(v) - 2; i >= 0; --i){
                v[i + 1] |= v[i] >> move_each_rev;
                v[i] <<= move_each;
            }
        }
        if(move_index) {
            for (int i = (int) size(v) - 1; i >= 0; --i) {
                if (i >= move_index) {
                    v[i] = v[i - move_index];
                }
                else v[i] = 0;
            }
        }
        if(n&31) v.back() &= ((unsigned int)1<<(n&31)) - 1;
        return *this;
    }
    mybts operator << (const unsigned int &a){
        mybts tmp(*this);
        return tmp <<= a;
    }
    unsigned int minimum_bit(){
        for(int i{}; i < size(v); ++i){
            if(v[i]) return (i<<5) + __builtin_ctz(v[i]);
        }
        return n;
    }
};

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);
    int N, S;
    cin>>N>>S;
    vector<int> a(N), b(N);
    for(int i{}; i < N; ++i) cin>>a[i]>>b[i];
    // i in [l, r) の a[i], b[i] を用いて 0 ~ max_w の重さを作れるか
    // ok[i] = ( 重さ i を作れるか ) if ( from_low ) else ( 重さ max_w - i を作れるか )
    auto make_bts = [&](int l, int r, int max_w, bool from_low){
        mybts ok(max_w + 1);
        if(from_low) {
            ok.set_true(0);
            for(int i = l; i < r; ++i) {
                mybts tmp = ok << a[i];
                (ok <<= b[i]) |= tmp;
            }
        }else{
            ok.set_true(max_w);
            for(int i = r - 1; i >= l; --i){
                mybts tmp = ok >> a[i];
                (ok >>= b[i]) |= tmp;
            }
        }
        return ok;
    };
    if(make_bts(0, N, S, true)[S]){
        auto solve = [&](auto &&self, int l, int r, int max_w) -> string {
            if (r - l == 1) {
                if (max_w == a[l]) return "A";
                else return "B";
            }
            int c = l + (r - l) / 2;
            mybts left = make_bts(l, c, max_w, true), right = make_bts(c, r, max_w, false);
            // and 操作によって、和を max_w とできるような中間地点 cen_w を見つける
            left &= right;
            int cen_w = left.minimum_bit();
            assert(cen_w <= max_w);
            return self(self, l, c, cen_w) + self(self, c, r, max_w - cen_w);
        };
        cout<<solve(solve, 0, N, S)<<endl;
    }else{
        cout<<"Impossible"<<endl;
    }
}
