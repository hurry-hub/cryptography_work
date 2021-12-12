#ifndef __VARIABLES__
#define __VARIABLES__

#include <stdbool.h>

/************************************** 全局变量 **************************************************/
extern int list;


/************************************** 自定义函数 ************************************************/
void SetKey(char Enc_Key[8]);                                                       // 生成16轮的子密钥；
void Swap(bool * Data_out, char * Data_in, int number);                             // 字节转换成位；
void TableReplace(bool *Data_out, bool *Data_in, const char *Table, int number);    // 各种表的置换算法；
void Bitcopy(bool * Data_out, bool * Data_in, int number);                          // 二进制数组的拷贝
void Loop_bit(bool * Data_out, int movstep, int len);                               // 左移位；
void Run_DES(char message[8], char HexMssage[16], int flag);                        // DES的轮加密算法
void Xor(bool * Message_out,  bool * Message_in, int number);                       // 执行异或
void S_change(bool * Data_out,  bool * Data_in);                                    // S盒变换；
void HexToBit(bool * Data_out, char * Data_in, int number);                         // 十六进制转二进制
void BitToHex(char * Data_out, bool * Data_in, int number);                         // 二进制转换成十六进制；

#endif