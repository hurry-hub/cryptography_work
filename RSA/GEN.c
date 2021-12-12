#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

typedef unsigned long ll;
const int times = 10;

ll gen_rand_num(int len)
{
    ll temp = 0;
    int base = 0;
    
    for (int k = 0; k < len; k++) {
        base = rand() % 2;
        temp = temp * 2 + base;
    }

    return temp;
}

ll fast_mod(ll a,ll b,ll mod)//计算2^q的过程
{
    ll res = 0;
    while(b){
        if(b & 1) res = res + a;
        a <<= 1;
        if(a >= mod) a -= mod;
        if(res >= mod) res -= mod;
        b >>= 1;
    }
    return res;
}

ll fast_pow_mod(ll a,ll b,ll mod)//快速幂算出a^m
{
    ll res = 1;
    while(b){
        if(b & 1) res = (res * a) % mod;
        a = (a * a) % mod;
        b >>= 1;
    }
    return res;
}

bool check(ll a,ll m,ll p,ll n)//对于每次随机的a进行测试
{
    ll temp = fast_pow_mod(a,m,n),ret = temp;
    for(int i = 0;i < p;++i){
        ret = fast_mod(temp,temp,n);
        if(ret == 1 && temp != n - 1 && temp != 1) return true;
        temp = ret;
    }
    return ret != 1;
}

bool Miller_Pabin(ll n)//Miller测试的主体结构
{
    if(n < 2) return false;
    if(n == 2) return true;
    if(n & 1 == 0) return false;//对于偶数的优化
    ll p = 0,x = n - 1;//p为Miller测试的q，x为Miller测试的m
    while(x & 1 == 0){
        x >>= 1;
        p++;
    }
    srand(time(NULL));
    for(int i = 0;i < times;++i){
        ll o = rand() % (n - 1) + 1;//o就是Miller测试的底数a
        if(check(o,x,p,n)) return false;
    }
    return true;
}
 
ll mod_mul(ll a, ll b, ll mod)
{
    ll res = 0;
    while (b)
    {
        if (b & 1)
            res = (res + a) % mod;
        a = (a + a) % mod;
        b >>= 1;
    }
    return res;
}
 
ll mod_pow(ll a, ll n, ll mod)
{
    ll res = 1;
    while (n)
    {
        if (n & 1)
            res = mod_mul(res, a, mod);
        a = mod_mul(a, a, mod);
        n >>= 1;
    }
    return res;
}
 
// Miller-Rabin随机算法检测n是否为素数
bool Miller_Rabin(ll n)
{
    if (n == 2)
        return true;
    if (n < 2 || !(n & 1))
        return false;
    ll m = n - 1, k = 0;
    while (!(m & 1))
    {
        k++;
        m >>= 1;
    }
    for (int i = 1; i <= 25; i++)  // 20为Miller-Rabin测试的迭代次数
    {
        ll a = rand() % (n - 1) + 1;
        ll x = mod_pow(a, m, n);
        ll y;
        for (int j = 1; j <= k; j++)
        {
            y = mod_mul(x, x, n);
            if (y == 1 && x != 1 && x != n - 1)
                return false;
            x = y;
        }
        if (y != 1)
            return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    ll a = 0;
    ll res = 0;
    int flag = 2;
    int count = 0;

    int ctr = 0;
    printf("please input the bit of prime:");
    scanf("%d", &ctr);
    srand((unsigned)time(NULL));
    a = gen_rand_num(ctr);
    while (1) {
        for (; count < 20; count++) {
            flag = Miller_Rabin(a);
            if (flag == 0) {
                break;
            }
        }
        if (flag == 1) {
            res = a;
            break;
        }
        count = 0;
        a = gen_rand_num(ctr);
    }
    printf("%u is a prime number!", res);

    return 0;
}
