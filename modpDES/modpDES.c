#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

void generate_sub_key(uint32_t key, uint32_t subkey[], uint32_t p) {
    uint32_t c = 231;
    for (int i = 0; i < 16; i++) {
        subkey[i] = (uint32_t)(((uint64_t)key * c) % p);
        c = (c * c) % 1000;
    }
}

uint64_t inverse_R(uint64_t base, uint64_t idx, uint64_t p) {
    uint64_t ans = 1;
    while (idx) {
        if (idx & 1) {
            ans *= base;
            ans %= p;
        }
        base *= base;
        base %= p;
        idx >>= 1;
    }
    return ans;
}

uint64_t encrypt_f(uint64_t R, uint32_t p, uint32_t sub_key) {
    //求R的逆元
    uint64_t R_inv = inverse_R(R, (uint64_t)p - 2, (uint64_t)p);
	//printf("R_inv = %llu", R_inv);
    uint64_t c = 21;
    return (R_inv * c + sub_key) % (uint64_t)p;
}

uint64_t encrypt(uint64_t input, uint32_t p, uint32_t subkey[]) {
    uint64_t L = 0, R = 0;
    L = input / p;
    R = input % p;
    uint64_t f = 0;
    for (int i = 0; i < 16; i++) {
         f = encrypt_f(R, p, subkey[i]);

         uint64_t tmp = R;
         R = (L + f) % (uint64_t)p;
         L = tmp;
    }
    //printf("R = %llu, L = %llu", R, L);
    //printf("\ncipher = %llu", R * (uint64_t)p + L);
    return R * (uint64_t)p + L;
}

uint64_t decrypt(uint64_t cipherText, uint32_t p, uint32_t subkey[]) {
    uint64_t L = 0, R = 0;
    L = cipherText / p;
    R = cipherText % p;
    uint64_t f = 0;
    for (int i = 0; i < 16; i++) {
        f = encrypt_f(R, p, subkey[15 - i]);

        uint64_t tmp = R;
        if (L < f) {
            R = (L + p - f) % (uint64_t)p;
        }
        else {
            R = (L - f) % (uint64_t)p;
        }
        L = tmp;
    }
    //printf("L = %llu, R = %llu", L, R);
    //printf("\nresult = %llu", R * (uint64_t)p + L);
    return R * (uint64_t)p + L;
}

int main() {
    uint64_t input = 289673124;
    uint32_t key = 26372;
    uint32_t sub_key[16] = { 0 };
    uint32_t p = 24473;
    printf("明文：%llu", input);
    generate_sub_key(key, sub_key, p);
    //加密
	uint64_t cipherText = encrypt(input, p, sub_key);
    printf("\n加密得到的密文：%llu", cipherText);
    //解密
    uint64_t result = decrypt(cipherText, p, sub_key);
    printf("\n解密得到的明文：%llu", result);
    return 0;
}