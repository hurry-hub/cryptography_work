#include <stdio.h>
#include <stdlib.h>

//交换算法
void swap(int *a, int *b) {
	*a ^= *b;       //a=a^b
	*b ^= *a;      //b=b^(a^b)=b^a^b=b^b^a=0^a=a
	*a ^= *b;     //a=(a^b)^a=a^b^a=a^a^b=0^b=b
}

//计算的是B在模A上的逆 B^(-1)modA
int mod_reverse_Stein(int b, int a)
{
	int A = a;
	int B = b;
	int x = 1;
	int y = 0;
	int x1 = 0;
	int y1 = 1;

	if (b == 0) {
		return -1;
	}

	while (1) {
		if (a % 2 == 0) {
			if (y % 2 == 0) {

			}
			else {
				if (y < 0) {
					x -= B;
					y += A;
				}
				else {
					x += B;
					y -= A;
				}
			}

			a /= 2;
			x /= 2;
			y /= 2;
		}
		else {
			if (a == 1) {
				return (y + A) % A;
			}
			else {
				if (a - b > 0) {

				}
				else {
					swap(&a, &b);
					swap(&x, &x1);
					swap(&y, &y1);
				}

				a -= b;
				x -= x1;
				y -= y1;
			}
		}
	}

	return -1;
}

int main(void) {
    int e, n;
    printf("input e,n: \n");
    scanf("%d%d", &e, &n);
    printf("%d", mod_reverse_Stein(e, n));

    return 0;
}