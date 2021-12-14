#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "function.h"

int list = 1;

/**************************************** 主函数 ***************************************************/
int main(void) {
    // 参数初始化
	int i = 0, k = 0, Enc_time_all = 0, Dec_time_all = 0;
    int count[64] = {0};
    char Enc_Key[16] = {0};                      // 记录密钥；
    char message[16] = {0};                      // 明文
    char cipher[16] = {0};                      // 16进制的密文
    bool cipher_bit[64] = {0};
    char pre_cipher[16] = {0};
    bool pre_cipher_bit[64] = {0};
    char result[16]  = {0};                      // 解密结果
    clock_t Enc_start_time, Enc_end_time, Dec_start_time, Dec_end_time;
    FILE *fp = NULL;
    fp = fopen("result.txt", "w+");
    
    printf("Please input the message(16 Hex numbers):\n");
    gets(message);
    printf("Please input the key:\n");
    gets(Enc_Key);
    
    SetKey(Enc_Key);                            //生成16轮的加密子密钥； 
    Run_DES(message, pre_cipher, 0);                //DES的轮加密过程
    HexToBit(pre_cipher_bit, pre_cipher, 64);
    // DES解密
    // SetKey(Enc_Key);                            //生成16轮的解密子密钥；
    // Run_DES(result, pre_cipher, 1);                 //解密;
    // printf("\nDecrytion:\n");
    // for (int j = 0; j < 16; j++) {
    //     printf("%c", result[j]);
    // }

    srand((unsigned)time(NULL));
    for (; i < 1000000; i++) {
        // DES加密
        list = rand() % 64;
        Enc_start_time = clock();
        Run_DES(message, cipher, 0);                //DES的轮加密过程
        Enc_end_time = clock();
        Enc_time_all += Enc_end_time - Enc_start_time;
        HexToBit(cipher_bit, cipher, 64);
        int ctr = 0;
        for (int j = 0; j < 64; j++) {
            if (pre_cipher_bit[j] != cipher_bit[j]) {
                ctr++;
            }
            //pre_cipher_bit[j] = cipher_bit[j];
        }
        fprintf(fp, "%d\n", ctr);
        //DES解密
        Dec_start_time = clock();
        Run_DES(result, cipher, 1);                 //解密;
        Dec_end_time = clock();
        Dec_time_all += Dec_end_time - Dec_start_time;
    }

    printf("\n1000000 times Enc costs %d ms\n", Enc_time_all);
    printf("\n1000000 times Dec costs %d ms\n", Dec_time_all);
    fclose(fp);

    return 0;
}