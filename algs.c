#include <string.h>
#include <stdio.h>


#define dprint(expr) printf(#expr " = %d\n", expr)

int fibonacci(int n)
{
    if (n <= 1)
    {
        return n;
    }

    return fibonacci(n-1) + fibonacci(n-2);
}

int fibonacci2(int n)
{
    int a[2] = {0, 1};
    int i;

    for (i = 2; i <= n; i++)
    {
        a[i%2] = a[0] + a[1];
    }

    return a[n%2];
}


// 平方根倒数速算法
float InvSqrt (float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i>>1);
//  i = 0x5f37642f - (i>>1);
//  i = 0x5f375a86 - (i>>1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    return x;
}


void reverse(char *pstr, int from, int to)
{
    while (from < to)
    {
        char tmp = pstr[from];
        pstr[from++] = pstr[to];
        pstr[to--] = tmp;
    }
}

void reverse_word(char *str)
{
    int len = strlen(str);
    int wordbegin = -1;

    for (int i = 0; i <= len; i++)
    {
        if ((str[i] == ' ' || str[i] == '\0') && wordbegin != -1)
        {
            reverse(str, wordbegin, i-1);
            wordbegin = -1;
        }
        else if (str[i] != ' '&& wordbegin == -1)
        {
            wordbegin = i;
        }
    }

    reverse(str, 0, len-1);
}


// v中值为1的bit个数是奇数还是偶数
int parity(unsigned int v)
{
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    v &= 0xf;
    return (0x6996 >> v) & 1;
}


int main(int argc, const char *argv[])
{
    dprint(fibonacci(10));

    printf("\n");
    {
        printf("InvSqrt = %f\n", InvSqrt(5.0f));
    }

    printf("\n");
    {
        char str[] = "I am a student.";
        reverse_word(str);

        printf("%s\n", str);
    }

    printf("\n");
    {
        int val = 0;
        int t = 1 << 10;

        for (int i = 0; i < t; i++)
        {
            val += parity(i);
        }

        printf("%d\n", val);
    }

    return 0;
}
