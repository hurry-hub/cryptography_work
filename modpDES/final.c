#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h> 
#include <time.h>
#define BIT 5                                                                      // 加密轮数
#define CONST_NUM 3                                                                 // f函数的算子
typedef unsigned long ll;

const unsigned long P = 257;                                                      // P取值
const unsigned long p_2 = 66049;                                               // P平方取值 
unsigned long key = 0;                                                              // 密钥
unsigned long sub_key[BIT] = {0};                                                   // 轮密钥

ll Inv_R(ll num);                                                                   // 求逆元
void subkey();                                                                      // 轮密钥生成
ll function(ll w_R, ll k);                                                          // function加密
ll modpDES(ll message, ll flag);                                                    // modpDES主函数


int main(void) {
    // 初始化参数
    ll origin = 0;                                                                      // 明文
    ll cipher = 0;                                                                      // 密文
    ll result = 0;                                                                      // 解密的结果
    ll flag = 1;                                                                        // 加密 or 解密标志
    clock_t Enc_start_time, Enc_end_time, Dec_start_time, Dec_end_time;                 // 时间参数
    int Enc_time_all = 0, Dec_time_all = 0;
    FILE *fp = NULL;
    fp = fopen("result.txt", "w+");
    
    // 读取明文
    printf("P = %u\nplease input the message (message < p^2 = %u)\n", P, p_2);
    while (flag) {
        scanf("%u", &origin);
        if (origin < P * P) {
            flag = 0;
        } else {
            printf("please input the correct message\n");
        }
    }

    // 密钥读取
    flag = 1;
    printf("please input key:");
    while (flag) {
        scanf("%d", &key);
        if (key < P) {
            flag = 0;
        } else {
            printf("please input the correct key\n");
        }
    }
    subkey();

    for (ll i = 0; i < p_2; i++) {
        // 加密
        //printf("begin encrypt\n");
        Enc_start_time = clock();
        cipher = modpDES(i, 0);                                        // 加密过程
        Enc_end_time = clock();
        Enc_time_all += Enc_end_time - Enc_start_time;
        fprintf(fp, "%u\n", cipher);
        //printf("the cipher is ");
        //printf("%u", cipher);

        // 解密
        //printf("\nbegin decrypt\n");
        Dec_start_time = clock();
        result = modpDES(i, 1);                                        // 解密过程
        Dec_end_time = clock();
        Dec_time_all += Dec_end_time - Dec_start_time;
        //printf("the message is ");
        //printf("%u", result);
    }

    printf("1000000 times Enc costs %d ms\n", Enc_time_all);
    printf("1000000 times Dec costs %d ms\n", Dec_time_all);
    fclose(fp);

    return 0;
}

/* 求逆元 */
ll Inv_R(ll num) {
    long pre_ctr1 = P, ctr1 = num;
    long pre_ctr2 = 1, ctr2 = 0;
    long pre_ctr3 = 0, ctr3 = 1;
    long opt = 0;

    while (ctr1 > 0) {
        long x = P / ctr1;
        opt = pre_ctr1;
        pre_ctr1 = ctr1;
        ctr1 = opt - x * ctr1;
        
        opt = pre_ctr2;
        pre_ctr2 = ctr2;
        ctr2 = opt - x * ctr2;

        opt = pre_ctr3;
        pre_ctr3 = ctr3;
        ctr3 = opt - x * ctr3;
    }  

    return (pre_ctr3 + P) % P;
}

/* 轮密钥生成 */
void subkey() {
    ll temp = key;
    for (int i = 0; i < BIT; i++) {
       sub_key[i] = ((temp << 1) % (ll)pow(2, BIT) + (temp >> (BIT - 1))) % P;
       temp = sub_key[i];
       //printf("%u ", sub_key[i]);
    }
    //printf("\n");

    return;
}

/* function结构加密 */
ll function(ll w_R, ll k) {
    w_R = Inv_R(w_R) % P;
    w_R = (w_R * CONST_NUM + k) % P; 

    return w_R;
}

/* modpDES主函数 */
ll modpDES(ll message, ll flag) {
    ll w_L = (message / P) % P;
    ll w_R = message % P;
    ll temp = 0;

    if (flag == 0) {
        //printf("w_L w_R: %u %u\n", w_L, w_R);
        for (int j = 0; j < BIT; j++) {
            temp = w_R;
            w_R = function(w_R, sub_key[j]);
            w_R = (w_R + w_L) % P;
            w_L = temp;
            //printf("w_L w_R: %u %u\n", w_L, w_R);
        }
        message = w_R * P + w_L;
    } else {
        //printf("w_L w_R: %u %u\n", w_L, w_R);
        for (int j = BIT - 1; j >= 0; j--) {
            temp = w_R;
            w_R = function(w_R, sub_key[j]);
            if (w_R > w_L) {
                w_R = (w_L - w_R + P) % P;
            } else {
                w_R = (w_L - w_R) % P;
            }
            w_L = temp;
            //printf("w_L w_R: %u %u\n", w_L, w_R);
        }
        message = w_R * P + w_L;
    }

    return message % p_2;
}
