#pragma
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "function.h"

// 十六轮子密钥
bool SubKey[16][48] = {0};

/************************************* 各种置换表 *************************************/
// IP置换表
const char IP_Table[64] = {             
	58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4, 
	62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8, 
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3, 
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7 
};

// IP-1置换表
const char IPR_Table[64] = {              
	40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31, 
	38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29, 
	36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27, 
	34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25 
};

// E扩展表
char E_Table[48] = {
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9, 
	8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

// PC1置换表
char PC1_Table[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18, 
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36, 
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22, 
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

// pc2表     
char PC2_Table[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10, 
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2, 
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 
};

//  移位表
char Move_Table[16] = {
	1,  1,  2,  2,  2,  2,  2,  2,  1,  2,  2,  2,  2,  2,  2,  1
};

 // S盒
char S_Box[8][4][16] = {
 //S1
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7, 
	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8, 
	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0, 
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, 
 //S2
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10, 
	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5, 
	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15, 
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, 
 //S3
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8, 
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1, 
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7, 
	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12, 
 //S4
	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15, 
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9, 
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4, 
	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14, 
 //S5
	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9, 
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6, 
	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14, 
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3, 
 //S6
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11, 
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8, 
	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6, 
	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13, 
 //S7
	4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1, 
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6, 
	1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2, 
	6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12, 
 //S8
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7, 
	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2, 
	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8, 
	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

 //P置换表
char P_Table[32] = {
	16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10, 
	2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};


/*************************** 具体函数定义 *************************************/
void Bitcopy(bool * Data_out,  bool * Data_in, int number) {            // 二进制数组拷贝
	int i = 0;
	for (i = 0; i < number; i++) {  
		Data_out[i] = Data_in[i];
	}
	
    return;
}

void Swap(bool * Data_out, char * Data_in, int number) {          // 字节转位
	int i, j; 
	for (i = 0; i < number; i++) {
		Data_out[i] = (Data_in[0] >> ((number - i - 1) & 7)) & 0x01;
	}

    return;
}

void BitToHex(char * Data_out,  bool * Data_in, int number) {           // 二进制转十六进制
	int i, temp;
    temp = number >> 2;
	for (i = 0; i < temp; i++) {
		Data_out[i] = 0;
	}
	for (i = 0; i < temp; i++) {
		Data_out[i] = Data_in[i << 2] + (Data_in[(i << 2) + 1] << 1) + (Data_in[(i << 2) + 2] << 2) + (Data_in[(i << 2) + 3] << 3);
		if ((Data_out[i] & 15) > 9) {
			Data_out[i] = (Data_out[i] & 15) + '7';
		} else {
			Data_out[i] = (Data_out[i] & 15) + '0';
        }
    }

    return;
}

void HexToBit(bool * Data_out, char * Data_in, int number) {             // 十六进制转二进制
	int i;
	for (i = 0; i < number; i++) {
		if(Data_in[i >> 2] <= '9') {
			Data_out[i] = ((Data_in[i >> 2] - '0') >> (i & 3)) & 0x01;
		} else {
			Data_out[i] = ((Data_in[i >> 2] - '7') >> (i & 3)) & 0x01;
		}
	}

    return;
}

void TableReplace( bool *Data_out,  bool * Data_in, const char *Table , int number) {        // 置换算法
	int i = 0;
	static bool Temp[256] = {0};
	for (i = 0; i < number; i++) {
		Temp[i] = Data_in[Table[i] - 1];
	}
	Bitcopy(Data_out, Temp, number);

    return;
}

void Loop_bit(bool * Data_out, int movstep, int len) {
	static bool Temp[256] = {0};
	Bitcopy(Temp, Data_out, movstep);
	Bitcopy(Data_out, Data_out + movstep, len - movstep);
	Bitcopy(Data_out + len - movstep, Temp, movstep);

    return;
}

void Xor(bool * Message_out, bool * Message_in, int number) {                // 执行异或
	int i;
	for (i = 0; i < number; i++) {
		Message_out[i] = Message_out[i] ^ Message_in[i];
	}

    return;
}

void SetKey(char Enc_Key[16]) {
	int i, j;
    static bool Key_bit[64] = {0};                          // Key的二进制缓存；
    static bool *Key_bit_L, *Key_bit_R;
	HexToBit(Key_bit, Enc_Key, 64);
    TableReplace(Key_bit, Key_bit, PC1_Table, 56);          // pc-1 置换
    Key_bit_L = &Key_bit[0];                                // key的左边28位；
    Key_bit_R = &Key_bit[28];                               // key的右边28位；
    for (i = 0; i < 16; i++) {
        Loop_bit(Key_bit_L, Move_Table[i], 28);
        Loop_bit(Key_bit_R, Move_Table[i], 28);
        TableReplace(SubKey[i], Key_bit, PC2_Table, 48);    // pc-2置换
    }
    return;
}

void S_change(bool * Data_out,  bool * Data_in) {           //S盒变换
	int i;
    int r = 0, c = 0;                                       //S盒的行和列；
    for (i = 0; i < 8; i++, Data_in = Data_in + 6, Data_out = Data_out + 4) {
 	    r = (Data_in[0] << 1) + Data_in[5] * 1;
 	    c = (Data_in[1] << 3) + (Data_in[2] << 2) + (Data_in[3] << 1) + Data_in[4];
 	    Swap(Data_out, &S_Box[i][r][c], 4);
    }

    return; 
}

void F_change(bool Data_out[32], bool Data_in[48]) {      // f函数；
    static bool Message_E[48] = {0};                      // 存放E置换的结果；
    TableReplace(Message_E, Data_out, E_Table, 48);       // E表置换
    Xor(Message_E, Data_in, 48);
    S_change(Data_out, Message_E);                        // S盒变换
    TableReplace(Data_out, Data_out, P_Table, 32);        // P置换

    return;
}

void Run_DES(char message[16], char HexMessage[16], int flag) {        //DES轮加密算法, 0为加密， 1为解密
	int i; 
	static bool Message_bit[64] = {0};
	static bool *Message_bit_L = &Message_bit[0], *Message_bit_R = &Message_bit[32];
	static bool Temp[32] = {0};
	
    if (flag == 0) {										// DES加密
        HexToBit(Message_bit, message, 64);
		Message_bit[list] = Message_bit[list] ^ 0x01;
	    TableReplace(Message_bit, Message_bit, IP_Table, 64);
        for(i = 0; i < 16; i++) {
            Bitcopy(Temp, Message_bit_R, 32);
            F_change(Message_bit_R, SubKey[i]); 
            Xor(Message_bit_R, Message_bit_L, 32);
            Bitcopy(Message_bit_L, Temp, 32);
	    }
		Bitcopy(Temp, Message_bit_L, 32);
		Bitcopy(Message_bit_L, Message_bit_R, 32);
		Bitcopy(Message_bit_R, Temp, 32);
        TableReplace(Message_bit, Message_bit, IPR_Table, 64);
        BitToHex(HexMessage, Message_bit, 64);				//二进制转换成十六进制；
    } else {
        HexToBit(Message_bit, HexMessage, 64);
	    TableReplace(Message_bit, Message_bit, IP_Table, 64);
		Bitcopy(Temp, Message_bit_L, 32);
		Bitcopy(Message_bit_L, Message_bit_R, 32);
		Bitcopy(Message_bit_R, Temp, 32);
        for(i = 15; i >= 0; i--) {
            Bitcopy(Temp, Message_bit_L, 32);
            F_change(Message_bit_L, SubKey[i]); 
            Xor(Message_bit_L, Message_bit_R, 32);
            Bitcopy(Message_bit_R, Temp, 32);
	    }
        TableReplace(Message_bit, Message_bit, IPR_Table, 64);
	    BitToHex(message, Message_bit, 64);
    }

    return;
}