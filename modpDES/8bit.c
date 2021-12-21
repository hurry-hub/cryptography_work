#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define L 0xF0
#define R 0X0F
#define Mask_r 0x21
#define Mask_c 0x1E
typedef unsigned char Byte;

Byte S[4][16] = { {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
                 {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
                 {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
                 {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}};

Byte e(Byte a)
{
    a = a + ((a & 0x4) << 2);
    a <<= 1;
    a += (a & 0x4) >> 2;
    return a;
}

Byte s(Byte a)
{
    int r = ((a & 0x20) >> 4) + (a & 0x1);
    int c = (a & Mask_c) >> 1;
    return S[r][c];
}
Byte p(Byte a)
{
    Byte p0 = a & 0x01;
    Byte p1 = a & 0x02;
    Byte p2 = a & 0x04;
    Byte p3 = a & 0x08;
    a = (p0 << 2) + (p2 << 1) + (p1 >> 1) + (p3 >> 2);
    return a;
}
Byte f(Byte w, Byte k)
{
    w = e(w & R) ^ k;
    w = s(w);
    w = p(w);

    return w;
}

Byte Feistel(Byte w,Byte k)
{
    Byte w_L = (w & L) >> 4;
    Byte w_R = (w & R);
    w_R = f(w_R,k);
    w_R ^= w_L;
    w_L = w & R;
    w = (w_L << 4) + w_R;

    return w;
}
void Encode(Byte *message,Byte *K, int flag)
{
    for (int i = 0; message[i] != '\0'; i++)
    {
        if (flag == 0) {
            for (int j = 0; j < 4; j++)
                message[i] = Feistel(message[i], K[j]);
            message[i] = ((message[i] & R) << 4) + ((message[i] & L) >> 4);
        } else {
            for (int j = 3; j >= 0; j--)
                message[i] = Feistel(message[i], K[j]);
            message[i] = ((message[i] & R) << 4) + ((message[i] & L) >> 4);
        }
    }

    return;
}

int main()
{
    Byte K[4] = {0x5F,0x38,0x07,0x00};
    Byte message[7] = "1234";
    printf("the message is %s\n", message);
    printf("subkey is (0x)");
    for (int i = 0; i < 4; i++)
        printf("%02X ", K[i]);
    printf("\nbegin encrypt\n");
    Encode(message, K, 0);
    printf("the cipher is\n");
    for (int i = 0; message[i]!='\0'; i++)
        printf("%02X", message[i]);
    printf("\nbegin decrypt\n");
    Encode(message, K, 1);
    printf("the message is\n");
    /*for (int i = 0; message[i] != '\0'; i++)
        printf("%02X", message[i]);*/
    printf("%s", message);
}
