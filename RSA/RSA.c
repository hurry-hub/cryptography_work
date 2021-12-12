#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h> 
#include <time.h>
#define LEN 100

typedef unsigned long ll;

typedef struct number{
    int len;
    char symbol;
    int number[LEN];
}NUM, *pNUM;

/****************************** 大数运算部分 **************************************/
void reverse(int original[], int len);                                  // 数组翻转
void print(NUM original);                                               // 打印大数
void number_trim(pNUM original);                                        // 数组翻转
void zero(pNUM original);                                               // 数组清零
int compare_abs(NUM left, NUM right);                                   // 绝对值比较
NUM number(char str[]);                                                 // 字符串转大数
NUM anti_add(NUM left, NUM right);                                      // 元加法
NUM anti_add(NUM left, NUM right);                                      // 元减法
NUM add(NUM left, NUM right);                                           // 加法
NUM sub(NUM left, NUM right);                                           // 减法
NUM mul(NUM left, NUM right);                                           // 乘法
void div_with_mod(NUM left, NUM right, pNUM result, pNUM residue);      // 带余数除法
NUM Div(NUM left, NUM right);                                           // 除法
NUM mod(NUM left, NUM right);                                           // 模

/****************************** 素数生成部分 **************************************/
ll gen_rand_num(int len);                                               // 生成大随机数
ll fast_mod(ll a, ll b, ll mod);                                        // 计算２的幂
ll fast_pow_mod(ll a, ll b, ll mod); 
ll mod_mul(ll a, ll b, ll mod);                                         // 模乘
ll mod_pow(ll a, ll n, ll mod);                                         // 模幂
bool Miller_Rabin(ll n);                                                // Miller-Rabin测试
ll prime(int len);                                                      // 生成大素数

/****************************** RSA算法主体部分 **************************************/
NUM extend(NUM e, NUM F_n, const NUM one);                              // 求逆，获得解密密钥d
NUM encrypt(NUM e, NUM n, NUM one);                                     // RSA加密
void decrypt(NUM d, NUM n, NUM cipher, NUM one);                        // RSA解密


int main(void) {
    // 一些参数的初始化
    NUM p, q, e, F_n, d, n, cipher;
    ll p_init, q_init, e_init;
    char p_input[LEN] = "", q_input[LEN] = "", e_input[LEN] = "";
    const NUM  one = number("1");
    const NUM  zero = number("0");
    time_t start_time, Gen_time, Enc_time, Dec_time;
    start_time = time(NULL);
    int flag = 0;

    // 程序生成 p q e n F_n d， 即Gen()
    int len1, len2, len3;
    printf("please input the bit of p, q, e: ");
    scanf("%d%d%d", &len1, &len2, &len3);
    srand((unsigned)time(NULL));
    
    while (1) {
        do {
            p_init = prime(len1 - 1);
            q_init = prime(len2 - 1);
        } while (p_init == q_init);                     // 使生成的p q不相等
        while (1) {
            e_init = prime(len3 - 1);
            if (e_init == 2) {
                printf("2 is not the suitable key, regenerating!\n");
            } else {
                break;
            }
        }
        ltoa(p_init, p_input, 10);       p = number(p_input);
        ltoa(q_init, q_input, 10);       q = number(q_input);
        ltoa(e_init, e_input, 10);       e = number(e_input);
        printf("p = %ld\n", p_init);    
        printf("q = %ld\n", q_init);     
        printf("e = %ld\n", e_init);     

        n = mul(p, q);
        printf("n = ");         print(n);       
        F_n = mul(sub(p, one), sub(q, one));
        printf("F_n = ");       print(F_n);
        printf("Generating the decrypt key d......\n"); 
        if (compare_abs(mod(F_n, e), zero) == 1) {
            d = extend(e, F_n, one);
            if (d.symbol != one.symbol) {
                flag = 1;
            } else {
                flag = 0;
            }
            if (flag == 0) {
                break;
            } else {
                printf("d < 0, error! recalculate.....\n");
                continue;
            }
        } else {
            printf("e is a factor of F_n, try again! \n");
            continue;
        }
    }
    printf("d = ");         print(d);      
    Gen_time = time(NULL);
    printf("Time for Gen is %lf s!\n", difftime(Gen_time, start_time));

    // 程序加密，即Enc()
    cipher = encrypt(e, n, one);
    Enc_time = time(NULL);

    // 程序解密，即Dec()
    decrypt(d, n, cipher, one);
	Dec_time = time(NULL);
    printf("Time for Dec is %lf s!\n", difftime(Dec_time, Enc_time));

	system("pause");
	return 0;
}

/* 数组翻转 */
void reverse(int original[], int len) {
    int i, t;
    for(i = 0; i < (len >> 1); i++){
        t = original[i]; original[i] = original[len - 1 - i];  original[len - 1 - i] = t;
    }
}

/* 大数输出函数*/
void print(NUM original) {
    int i;
    if(original.symbol == '-') 
        printf("%c", original.symbol);
    for(i = original.len - 1; i >= 0; i--)
        printf("%d", original.number[i]);
    printf("\n");
}

/* 整理数组 */
void number_trim(pNUM original) {
    int i;
    for(i = original->len; i >= 0; i--)
        if(original->number[i]) break;
    original->len = ++i;
}

/* 数字归零 */
void zero(pNUM original) {
    int i;
    for(i = 0; i < LEN; i++) {
        original->number[i] = 0;
    }
}

/* 比较两个数的绝对值大小,left大于返回1,小于返回-1,相等返回0 */
int compare_abs(NUM left, NUM right) {
    int i;
    if (left.len > right.len) return 1;
    else if (right.len > left.len) return -1;
    else {
        for(i = left.len; i >= 0; i--){
            if(left.number[i] > right.number[i]) return 1;
            if(left.number[i] < right.number[i]) return -1;
        }
        return 0;
    }
}

/* 将字符串str构造大数结构 */
NUM number(char str[]) {
    int i, t, pos = 0, len = 0;
    NUM target;
    if(str[0] == '+' || str[0] == '-'){
        target.symbol = str[0];
        pos++;
    }else{
        target.symbol = '+';
    }
    for(i = 0; i < LEN; i++) target.number[i] = 0; //初始化数组
    while(str[pos] != '\0') target.number[len++] = str[pos++] - '0'; //拷贝数组
    target.len = len; //设置长度
    reverse(target.number, target.len);
    return target;
}

/* 元加法, left、right 都为正数或都为0 */
NUM anti_add(NUM left, NUM right) {
    int i = 0,
    carry = 0,  /* 进位,默认为0 */
    temp;
    NUM result;
    result.symbol = left.symbol;
    result.len = (left.len > right.len ? left.len : right.len); //设置和的位数与最大加数位数相等
    zero(&result);
    for(i = 0; i < result.len; i++){
        temp = left.number[i] + right.number[i] + carry;
        result.number[i] = temp % 10;
        carry = temp / 10;
    }
    if(carry > 0){
        //任有进位, 向前进位, 位数加1;
        result.number[i++] = carry;
    }
    result.len = i;
    return result;
}

/* 元减法, left>=right 或都为0 */
NUM anti_sub(NUM left, NUM right) {
    if (compare_abs(left, right) == 0) {
        return number("0");
    }
    int borrow = 0, i, j, temp;
    NUM result;
    result.symbol = left.symbol;
    result.len = left.len;         //差最多和被减数位数相等
    zero(&result);
    for(i = 0; i < left.len; i++){
        temp = left.number[i] - right.number[i] - borrow;
        if(temp < 0){
            result.number[i] = 10 + temp;
            borrow = 1;
        }else{
            result.number[i] = temp;
            borrow = 0;
        }
    }
    /* 去除多余0, 计算长度 */
    number_trim(&result);
    return result;
}

/* 加法 */
NUM add(NUM left, NUM right) {
    NUM result;
    /* 符号相同直接加,符号相异用绝对值大的数减去绝对值小的数 */
    if(left.symbol == right.symbol) result = anti_add(left, right);  
    else{
        if(compare_abs(left, right) >= 0) result = anti_sub(left, right);
        else result = anti_sub(right, left);
    }
    return result;
}

/* 减法：减去一个数等于加上这个数的相反数 */
NUM sub(NUM left, NUM right) {
    //NUM result;
    right.symbol = (right.symbol == '+' ? '-' : '+');
    return add(left, right);
}

/* 乘法 */
NUM mul(NUM left, NUM right) {
    int i = 0,
    j = 0,
    carry = 0, /* 进位 */
    pos,  /* 计算结果所在位置 */
    temp;
    NUM result;
    result.len = left.len + right.len; // 乘法结果位数最多为两个因数位数之和
    zero(&result);
    for(i = 0; i < right.len; i++){
        carry = 0;
        for(j = 0; j < left.len; j++){
             pos = i + j; /* 计算位置 */
            temp = right.number[i] * left.number[j] + carry;
            carry = temp / 10; /* 相乘时进位 */
            temp = temp % 10;  /* 相乘时该位置累计的值 */
            result.number[pos] += temp;  /* 求和累计 */
            carry = carry + result.number[pos] / 10; /* 求和后进位 */
            result.number[pos] %= 10;  /* 求和后该位置的值 */
        }
        if(carry > 0 ){
            result.number[++pos] = carry;
            result.len = pos + 1;
        }else{
            result.len = pos + 1;
    }
    }
    result.symbol = left.symbol == right.symbol ? '+' : '-';  /* 确定符号 */
    return result;
}

/* 带余数除法 */
void div_with_mod(NUM left, NUM right, pNUM result, pNUM residue) {
    int i, j, t, k, m = 0; /* k: 商 */ /* m: 商的位数 */
    char symbol;
    zero(result);
    zero(residue);
    symbol = (result->symbol = left.symbol == right.symbol ? '+' : '-'); //确定符号
    for(i = left.len - 1; i >= 0; i--){
        residue->len = right.len + 1;  /* 设置余数位数比除数位数多1 */
        for(j = residue->len - 1; j>0; j--)
            residue->number[j] = residue->number[j - 1];  /* 移动余数 */
        residue->number[0] = left.number[i];  /* 复制除数第一位到余数最低位中 */
        number_trim(residue);        /* 整理余数 */
        k = 0;
        while(compare_abs(*residue, right) >= 0){   /* 余数比除数大, 利用减法求商 */
            *residue = sub(*residue, right);  /* 用余数减去除数，差值保存在余数中 */
            k++;    /* 商+1 */
        }
        result->number[m++] = k;
    }
    result->len = m;
    for(i = 0; i < m / 2; i++){
        t = result->number[i];
        result->number[i] = result->number[m - 1 - i];
        result->number[m - 1 - i] =t;
    }
    number_trim(result);    /* 整理商 */
    number_trim(residue);   /* 整理余数 */
    result->symbol = residue->symbol = symbol;
}

/* 整数除法 */
NUM Div(NUM left, NUM right) {
    NUM result;
    pNUM residue = (NUM *)malloc(sizeof(NUM));
    div_with_mod(left, right, &result, residue);
    free(residue);
    return result;
}

/* 整数模 */
NUM mod(NUM left, NUM right) {
    NUM residue;
    pNUM result = (NUM *)malloc(sizeof(NUM));
    div_with_mod(left, right, result, &residue);
    free(result);
    return residue;
}

/* 获得大随机数 */
ll gen_rand_num(int len) {
    ll temp = 1;
    int base = 0;
    
    for (int k = 0; k < len; k++) {         //通过控制随机数的位数进行生成
        base = rand() % 2;
        temp = temp * 2 + base;
    }

    return temp;
}

/* 计算2^q的过程 */
ll fast_mod(ll a, ll b, ll mod) {
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

/* 快速幂算出a^m */
ll fast_pow_mod(ll a, ll b, ll mod) {
    ll res = 1;
    while(b){
        if(b & 1) res = (res * a) % mod;
        a = (a * a) % mod;
        b >>= 1;
    }
    return res;
}
 
/* 模乘运算 */
ll mod_mul(ll a, ll b, ll mod) {
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
 
/* 模幂运算 */
ll mod_pow(ll a, ll n, ll mod) {
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
 
/* Miller-Rabin随机算法检测n是否为素数 */
bool Miller_Rabin(ll n) {
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

/* 生成大素数 */
ll prime(int len) {
    ll a = 0;
    ll res = 0;
    int flag = 2;
    int count = 0;
    a = gen_rand_num(len);
    while (1) {
        for (; count < 20; count++) {           // 询问20次MR算法的结果
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
        a = gen_rand_num(len);
    }

    return res;
}

/* 模逆运算，根据公钥e求私钥d */
NUM extend(NUM e, NUM F_n, const NUM one) {
    NUM opt0 = number("0");
    NUM opt1 = number("1");
    NUM opt = opt0;
    NUM pre_ctr1 = F_n,  ctr1 = e;
    NUM pre_ctr2 = opt1, ctr2 = opt0;
    NUM pre_ctr3 = opt0, ctr3 = opt1;

    while (compare_abs(ctr1 ,opt0) == 1) {
        NUM x = Div(pre_ctr1, ctr1);
        opt = pre_ctr1;
        pre_ctr1 = ctr1;
        ctr1 = sub(opt, mul(x, ctr1));
        
        opt = pre_ctr2;
        pre_ctr2 = ctr2;
        ctr2 = sub(opt, mul(x, ctr2));

        opt = pre_ctr3;
        pre_ctr3 = ctr3;
        ctr3 = sub(opt, mul(x, ctr3));
    }  

    return pre_ctr3;
}

/* RSA加密 */
NUM encrypt(NUM e, NUM n, NUM one) {
    // 必要参数的定义
    time_t enc1, enc2;
    unsigned long long message_init;
    char message[LEN];
    NUM opt0 = number("0");
    NUM opt1 = number("1");
    NUM opt2 = number("2");

    // 读取明文
    printf("Please input the message: \n");
    scanf("%llu", &message_init);
    enc1 = time(NULL);
    sprintf(message, "%llu", message_init);
    int len = strlen(message);
    NUM message_num = number(message);
    printf("message_num is: ");    print(message_num);     
    
    // 快速计算得到 m ^ e (n) 
    NUM cipher = message_num;
    NUM t = e;
    int tent[LEN];
    int i = 0;
    while (compare_abs(t, opt1)) {
        if (!compare_abs(mod(t, opt2), opt0)) {
            tent[i] = 0;
        } else if (!compare_abs(mod(t, opt2), opt1)){
            tent[i] = 1;
        }
        i++;
        t = Div(t, opt2);
    }
    tent[i] = 1;
    reverse(tent, i + 1);                   // 获得明文的二进制表示
    for (int j = 1; j <= i; j++) {          // 根据二进制简化模幂运算
        cipher = mod(mul(cipher, cipher), n);
        if (tent[j] == 1) {  
            cipher = mod(mul(message_num,cipher), n);
        }
    }
    
    printf("The result of Encryption is:");         print(cipher);     
    enc2 = time(NULL);
    printf("Time for Gen is %lf s\n", difftime(enc1, enc2));

    return cipher;
}

/* RSA解密 */
void decrypt(NUM d, NUM n, NUM cipher, NUM one) {
    // 必要参数的定义
    NUM opt0 = number("0");
    NUM opt1 = number("1");
    NUM opt2 = number("2");
    NUM message = cipher;
    NUM t = d;

    // 快速计算得到 c ^ d (n) 
    int tent[LEN];
    int i = 0;
    while (compare_abs(t, opt1)) {
        if (!compare_abs(mod(t, opt2), opt0)) {
            tent[i] = 0;
        } else if (!compare_abs(mod(t, opt2), opt1)){
            tent[i] = 1;
        }
        i++;
        t = Div(t, opt2);
    }
    tent[i] = 1;
    reverse(tent, i + 1);                       // 获得密文的二进制表示
    for (int j = 1; j <= i; j++) {              // 根据二进制简化模幂运算
        message = mod(mul(message,message), n);
        if (tent[j] == 1) {  
            message = mod(mul(message,cipher), n);
        }
    }
    printf("The result of Decryption is:");     print(message);     

    return;
}
