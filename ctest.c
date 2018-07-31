#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wmultichar"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wformat="
#endif

#define LOG_INFO(s, ...) fprintf(stderr, "[%s:%d] " s "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define ISUNSIGNED(a) (a >= 0 && ~a >= 0)
#define ISUNSIGNEDTYPE(type) ((type)0 - 1 > 0)

#define ISSIGNED(x) ((x=-1) < 0)
//#define ISSIGNED(x) (x < 0 ? 1 : ~x)
#define ISSIGNEDTYPE(type) ((type)-1 < (type)0)

#define dprintnb(expr) printf(#expr " = %d\t", expr)
#define dprint(expr) printf(#expr " = %d\n", expr)
#define NEWLINE(s) printf("\n========= %s =========\n", #s)

#define FF(a,b)     a##b
#define GG(a)       #a
#define HH(a)       GG(a)


/*
(char p[2][3][5])等价于(char (*p)[3][5])
(char *p[3][5])等价于(char *(*p)[5])
*/
char fa1(char p[2][3][5])
{
    void *p1 = p + 1;
    void *p2 = p;

    printf("fa1: %d\t", p1-p2);
    return 1;
}

char fa2(char p[][3][5])
{
    void *p1 = p + 1;
    void *p2 = p;

    printf("fa2: %d\t", p1-p2);
    return 1;
}

char fa3(char (*p)[3][5])
{
    void *p1 = p + 1;
    void *p2 = p;

    printf("fa3: %d\n", p1-p2);
    return 1;
}


extern int fun(void)
{
    printf("unlimitted dereference func pointer\n");
    return 1;
}


int main(int argc, char *argv[])
{
    char *str = "hello";
    const char * cstr = str;
    char **pstr = &str;
//  const char ** cpstr = pstr;

    int x = 0, y = 0;
    // int z = x+++++y;

    LOG_INFO("%s %s", "Hello", "world");

    time_t biggest = -1;
    biggest &= ~(1 << (sizeof(biggest)*8 - 1));

    printf("biggest = %s \n", asctime(gmtime(&biggest)));
    printf("biggest = %s \n", ctime(&biggest) );

    // sizeof
    NEWLINE(sizeof);
    {
        unsigned char c = 5;

        dprintnb(sizeof(c));    // 1
        dprint(sizeof('a'));    // 4（作了提升，即字符常量是被当作int的）
        dprint(sizeof(short));
        dprint(sizeof(int));
        dprint(sizeof(long));
        dprint(sizeof(long long));
        dprint(sizeof(void *));
        dprint(sizeof(size_t));
        // float固定是4，double固定是8

        dprintnb(sizeof(1L));
        dprint(sizeof(1LL));
        dprint(sizeof((char)1+(char)2));
        dprint(sizeof((short)1+(short)2));
        dprint(sizeof((char)1>(char)2));
        dprint(sizeof((short)1>(short)2));

        NEWLINE(ISSIGNEDTYPE);
        dprint(ISSIGNEDTYPE(char));
        dprint(ISSIGNEDTYPE(double));
        dprint(ISSIGNED(c));
    }

    NEWLINE(printf-format);
    {
        printf("%5.2e\n", 9999999);
        printf("%.f\n", 3.1415926);
        printf("%0*.*f\n", 5, 2, 3.1415926);
        printf("%.d\n", 0);
    }

    // overflow of printf
    NEWLINE(overflow of printf);
    {
        int a = 1000000;
        int b = 1000000;
        long long c = a * b;
        long long d = (long long)a * b;
        long long e = (long long)(a * b);

        printf("c = %lld, d = %lld, e = %lld\n", c, d, e);

        printf("ld = %ld\n", 1000000000000);
        printf("ld = %ld\n", 1000000000000l);
        printf("ld = %ld\n", 1000000000000ll);
        printf("lld = %lld\n", 1000000000000ll);

        printf("%d, %d, %d\n", 1000000000000ll, 1000000000000ll, 1000000000000ll);
        printf("%d, %d, %d\n", 1ll, 2ll, 3ll); // 32位输出: 1, 0, 2。64位输出1, 2, 3
        printf("%d, %d, %d\n", (short)1, (short)2, (short)3); // output: 1, 2, 3
        // 在1、K&C，2、没有声明，3、可变参数列表中，参数会进行整形提升

        // 竟然不出错，似乎64位版本下，可变参数的整形参数都是按8字节压栈的，
        // 然后printf中，遇到%d只取4个字节，但偏移8个字节再取下个参数。
        printf("pointer: %d, %d, %d\n", &a, &b, &c);
        printf("pointer: %p, %p, %p\n", &a, &b, &c);
    }

    NEWLINE(unsigned long long);
    {
        unsigned char uc = 0x80;
        unsigned long long ull = 0;

        ull |= uc << 24;
        printf("ull = 0x%llx\n", ull); // ull = 0xffffffff80000000
    }

    NEWLINE(? :);
    {
        int i = 0;
        int a = 1;
        int b;
        b = a ? i=10, i++, i : i, 100;
        // ANSI C规定?和:之间可以是任何表达式 ，即使这个表达式中含有优先级更低的运算符

        dprint(b);
        a = 0;
        b = a ? i++, i++, i : 1, 100;
        dprint(b);
    }

    NEWLINE(void *);
    {
        int a = 0;
        void *p = &a;

        printf("p = %p, p+1=%p\n", p, p+1);
    }

    {
        int c = 'yes';
        printf("'y'=0x%x, 'e'=0x%x, 's'=0x%x, 'yes' = 0x%x\n", 'y', 'e', 's', c);
    }

    {
        int y = 07;
        int *p = &y;
        int z = y / *p;
        y >>= 1;
    }

    // 函数名只可以&一次，对函数名加不加&效果一样
    // 对函数名和函数指针都可以任意次解引用，效果一样
    NEWLINE(dereference);
    {
        int (*pf)(void) = fun;
        (************pf)();
        (************fun)();
    }

    /*
     * p0和p2里面放的都是数组的首地址，但对它们解引用，*p0获得的是第一个元素，而*p2获得
       的还是数组的首地址
     * 之所以使人费解，是因为已经习惯了指针解引用的语义，比如***p，就是以p的值作为地址，
       读取其中的内容，然后把读取的内容作为地址再去读取，每次解引用都会进行这样一个循环
       但当多维数组的语义附加到指针上以后，指针解引用的意义变得不一样了，因为不管多少维，
       C中的数组本质上都是一维的，都是数组元素的线性排列，数组逻辑上的多维需要编译器来解
       析和体现，它一定要到最后一维，才会真正的解引用为数组元素，否则一直是一个指向数组
       元素的指针，只不过在不同的维度，它的步长是不一样的，即对于一个指向多维数组的指针，
       在解到最后一维之前，解引用只是改变了指针的步长。
     * 所以当多维数组传递给函数参数时，参数声明必须表明第一维以外的大小，这样函数内外才能对指针
       以一致的步长移动和一致的含义解引用。
     */
    NEWLINE(array);
    {
        int a[2] = {91};
        int *p0 = a;
        int **p1 = a;
        int (*p2)[2] = &a; // 加不加&结果都一样，对数组名取地址只是改变了表达式的类型，值是一样的

        // a是一个不可修改的左值，可以对其取地址，但是&a已经不是左值了，无法继续取地址
        printf("array: %d, %d\n", a, &a /*, &&a*/);
        printf("1D: %d, %d, %d, %d, %d\n", a, p0[0], p1[0], p2[0], p2[0][0]);
    }

    {
        int a[2][3] = {91};
        int *p0 = a;
        int **p1 = a;
        int (*p2)[3] = a;
        int (*p3)[2][3] = a;

        printf("2D: %d, %d, %d, %d, %d, %d, %d, %d\n",
            a, p0[0], p1[0], p2[0], p2[0][0], p3[0], p3[0][0], p3[0][0][0]);
    }

    {
        int a[2][3][4] = {91};
        int *p0 = a;
        int ***p1 = a;
        int (*p2)[3][4] = a;
        int (*p3)[2][3][4] = a;


        printf("3D: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
            a, p0[0], p1[0], p2[0], p2[0][0], p2[0][0][0],
            p3[0], p3[0][0], p3[0][0][0], p3[0][0][0][0]);
        printf("3D*: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
            a, *p0, *p1, *p2, **p2, ***p2, *p3, **p3, ***p3, ****p3);

        NEWLINE("");
        // &a[0]这个表达式的精确类型是什么 int a[2][3][4] = {91};
        // 通过GDB查看，类型是(int (*)[3][4])
        dprint(&a[0]);
        dprint(sizeof(&a[0]));
        dprint(sizeof(*&a[0]));
        {
            int (*p)[3][4]= &a[0];
            dprint(sizeof(*p));
        }
    }

    NEWLINE("");
    {
        char a[2][3][5] = {99};
        char (*p)[3][5] = a;
        char (*q)[2][3][5] = &a;

        fa1(a);
        fa2(a);
        fa3(a);

        fa1(p);
        fa2(p);
        fa3(p);

        fa1(*q);
        fa2(*q);
        fa3(*q);
    }

    NEWLINE("");
    {
        int i;
        int row = 3;
        int column = 5;
        int *p = malloc(row * column * sizeof(int));
        int (*array)[column] = (int (*)[column])p;

        for (i = 0; i < row * column; i++)
        {
            p[i] = i + 1;
        }

        dprint(array[0][2]);
        dprint(array[1][2]);
        dprint(array[2][2]);
        dprint(sizeof(*array));
    }

    NEWLINE("");
    {
        int i;
        int *p = malloc(3 * 5 * 6 * sizeof(int));
        int (*array)[5][6] = (int (*)[5][6])p;

        for (i = 0; i < 3 * 5 * 6; i++)
        {
            p[i] = i + 1;
        }

        dprint(array[0][2][2]);
        dprint(array[1][2][2]);
        dprint(array[2][2][2]);
        dprint(sizeof(*array));
    }


    NEWLINE("");
    {
        char *s = "\";printf(\"char *s = \\\"%s%s\",s,s);";
    //  "printf(\"Hello World!\\n\");";
        printf("char *s = \"%s%s",s,s);
    }

    NEWLINE("");
    {
        int n = 1;
        int *p = &n;
        void *pv = p;
        int *pi = pv;

        pv++;
    }

    NEWLINE("");
    {
        enum {false, true};

        int i = 1;
        do
        {
            printf("%d\n", i);
            i++;
            if(i < 15)
                continue;
        } while(false);
    }

    NEWLINE("a##b #a");
    {
        printf("%s\n", HH(FF(1,2)));
        printf("%s\n", GG(FF(1,2)));
    }

    if (1) ({int i = 0; dprint(i);});

#ifdef __GNUC__
    dprint(__GNUC__);
#endif

    printf("\nTime = %s%%\\\n", __TIME__);

    return 0;
}


/*
 * C中全局变量只能用常量表达式初始化，无论函数内的还是函数外的，因为c编译时即把初始值
   写入生成文件的data段（非0）或者记录在bss中，程序加载时映射入内存即可，并不存在运行
   时的静态存储初始化。
 * C++中因为有构造函数等由编译器潜在调用的函数，存在真正意义上的运行时初始化动作，延伸
   到内置类型，所以可以用变量或者函数初始化静态存储。
 */
int array[10][10];
int (*parray)[] = array; // 数组名所代表的地址是编译期常量

#if 0
int x = 1;
int y = x;
static int z = x;

void fs(int x)
{
    static y = x;
    printf("y=%d\n", y);
}
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
