# 64位DES算法使用说明



## 基本原理



### 1 输入输出参数

**key**: 64bit的密钥

**message**：64bit的需要加密的信息，即明文

**cipher**：64bit的已经被加密过的信息，即密文

### 2 流程



#### 2.1 子密钥产生

​		**DES**算法由64位密钥产生16轮的48位子密钥。在每一轮的迭代过程中，使用不同的子密钥。

a、把密钥的奇偶校验位忽略不参与计算，即每个字节的第8位，将64位密钥降至56位，然后根据选择置换 **PC-1** 将这56位分成两块 **C0** (28位)和 **D0** (28位)；

```c
char PC1_Table[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18, 
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36, 
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22, 
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
```

b、将 **C0** 和 **D0** 进行循环左移变化(注：每轮循环左移的位数由轮数决定)，变换后生成 **C1** 和 **D1** ，然后 **C1** 和 **D1** 合并，并通过选择置换 **PC-2** 生成子密钥 **K1** (48位)；

```c
char Move_Table[16] = {
	1,  1,  2,  2,  2,  2,  2,  2,  1,  2,  2,  2,  2,  2,  2,  1
};
```

c、 **C1** 和 **D1** 在次经过循环左移变换，生成 **C2** 和 **D2** ，然后 **C2** 和 **D2** 合并，通过选择置换 **PC-2** 生成密钥 **K2** (48位)；

```c
char PC2_Table[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10, 
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2, 
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 
};
```

d、以此类推，得到 **K16** (48位)。但是最后一轮的左右两部分不交换，而是直接合并在一起 **R16L16** ，作为逆置换的输入块。其中循环左移的位数一共是循环左移16次，其中第一次、第二次、第九次、第十六次是循环左移一位，其他都是左移两位。



#### 2.2 初始置换 IP

​		初始置换 **IP** 是 **DES** 的第一步密码变换，作用是将64位明文打乱重排并分成左右两半，左边32位作为 **L0** ，右边32位作为 **R0** ，供之后的加密迭代使用。

```c
const char IP_Table[64] = {             
	58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4, 
	62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8, 
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3, 
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7 
};
```



#### 2.3  加密函数

​		经过初始置换后，进行16轮完全相同的运算，在运算过程中数据与秘钥结合。函数 **f** 的输出经过一个异或运算，和左半部分结合形成新的右半部分，原来的右半部分成为新的左半部分。每轮迭代的过程可以表示如下：
$$
\begin{cases}
L_i = R_{i-1}\\
R_i = L_{i-1}\oplus f(R_{i - 1}, K_i)\\
i = 1, 2, 3, ..., 16
\end{cases}
$$
​		其中 **f** 包括三部分 **扩展置换E**、 **S-盒替换** 、**P-盒置换** 。

##### 2.3.1 扩展置换E

​		通过扩展置换E，数据的右半部分 **Rn** 从32位扩展到48位。扩展置换改变了位的次序，重复了某些位。

​		扩展置换的目的：a、产生与秘钥相同长度的数据以进行异或运算， **R0** 是32位，子秘钥是48位，所以 **R0** 要先进行扩展置换之后与子秘钥进行异或运算；b、提供更长的结果，使得在替换运算时能够进行压缩。

​		扩展置换E规则如下：

```c
char E_Table[48] = {
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9, 
	8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
```

##### 2.3.2 S-盒替换

​		**Rn** 扩展置换之后与子秘钥**Kn**异或以后的结果作为输入块进行S盒替换运算功能是把48位数据变为32位数据。

​		替换运算由8个不同的替换盒(**S盒**)完成。每个 **S-盒** 有6位输入，4位输出。所以48位的输入块被分成8个6位的分组，每一个分组对应一个S-盒替换操作。经过S-盒替换，形成8个4位分组结果。

​		值得注意的是，每一个 **S-盒** 的输入数据是6位，输出数据是4位，但是每个 **S-盒** 自身是64位！！每个 **S-盒** 是4行16列的格式，因为二进制4位是0~15。8个 **S-盒** 的值如下：

```c
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
```

##### 2.3.3 P-盒替换

​		S-盒替换运算，每一盒得到4位，8盒共得到32位输出。这32位输出作为P盒置换的输入块。P盒置换将每一位输入位映射到输出位。任何一位都不能被映射两次，也不能被略去。

​		经过P-盒置换的结果与最初64位分组的左半部分异或，然后左右两部分交换，开始下一轮迭代。P-盒置换表(表示数据的位置)共32位。

```c
char P_Table[32] = {
	16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10, 
	2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};
```

#### 2.4 逆置换

​		将初始置换进行16次的迭代，即进行16层的加密变换，这个运算过程我们暂时称为函数f。得到 **L16** 和 **R16** ，将此作为输入块，进行逆置换得到最终的密文输出块。逆置换是初始置换的逆运算，逆置换规则如下:

```c
const char IPR_Table[64] = {              
	40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31, 
	38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29, 
	36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27, 
	34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25 
};
```

### 3 流程图

<img src="https://s2.loli.net/2021/12/11/OATNKltjbcJaqMn.png" style="zoom: 67%;" />



## 具体实现

自实现 **DES** 包括三个文件：**function.h**、**function.c** 以及 **main.c**.

**function.h**: 存放全局变量定义以及各实现函数声明。

```c
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
```

function.c: 存放各函数的具体实现内容。

main.c: 主函数运行程序并测速，将1000000次的密文变化数写入result.txt中。



## 运行结果

​		以下为一次运行时的截图，经统计DES加密1000000次的时间约11400ms，解密时间类似。

![结果一](https://s2.loli.net/2021/12/12/kzpP2KX8yBfujlb.png)

​		同时，明文每变化1bit，密文的变化情况如图所示(基本符合32的正态分布)：

![](https://s2.loli.net/2021/12/12/5JLhBqwcaskubZg.png)

​		0bit的出现的可能原因在于，当变化的1bit为奇偶校验位(8^n)时，密文并不会出现任何变化，因为加密时校验位被剔除。