// -std=c99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>


static int swap_cnt;

static void swap(int a[], int i, int j)
{
    if (i != j)
    {
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;

        swap_cnt++;
    }
}


/*
 * 对于有大量重复元素的数组，如果等于pivot时指针也移动，会出现最坏情况O(n2)，
   所以通常快排都只判大于小于，等于时停下进行交换，用多余的交换来避免每次只切分一个元素
 * 开始时，i、j所指元素与pivot的大小关系未知，无法用作哨兵，此时如果pivot以外的其它元素
   都小于或者都大于pivot，会导致一侧指针动不了，另一侧指针一直移动直到越过另一个指针，
   并最终越界，所以，指针循环中需要加入判断来防止越界。
   （注意：实现中通常选最左侧的a[l]作pivot，所以j是不可能越界的，j指针循环中的i<=j（或者
   其它边界检查）完全是多余的，同样，如果选了a[r]作pivot，则i不可能越界）
 * 一旦i、j移动过，i左边的元素一定<=pivot，j右边的元素一定>=pivot，这些已经扫描过的元素，
   正好变成哨兵，来阻止另一侧的指针越界，所以，当i、j都移动过以后，指针循环中的i<=j
   就变成完全无用的了。
 * 有的写法会省略掉指针循环中的i<=j，改为判边界l、r（即i<=r,j>l），比较一下会发现，
   无论是判边界l、r还是判i<=j，指针i、j移动的轨迹都是一样的，i、j交叉后最多只能越过1，
   两侧的哨兵会阻止i、j交叉后拉开距离，效果等同于i<=j。
 * 进一步，指针循环中之所以要去比较i、j或判边界，完全只是为了防备其它元素都小于或都大于
   pivot这两种特殊情况。去掉这两种特殊情况，i、j的指针循环完全不需要判边界，因为只要有
   一个元素的大小关系不一样，就会使那个停不下来的指针停下，无法越界，然后，要么是跟
   另一个指针交换（交换过后，两个指针都会移动，经过的元素成为隐藏的哨兵，之后就再也
   不可能越界了），要么是i、j已经重合交叉，退出主循环。
 * 注意：以上都是建立在元素等于pivot时指针会停的前提下，如果等于时也移动，那么两个指针
   循环中必须都加上i<=j或者边界判断，无法省略。
 * 两个指针循环的先后顺序是可以颠倒的。
 * 主循环可以写成死循环，也可以加上i<=j或者i<j，差别是，如果当前数组里只有两个元素，
   i<j会导致进不去主循环，后面的swap(a, l, j)就必须要额外判断大小再交换。
 * 主循环中的swap交换完后，如果i、j指针不移动，交换的两个元素又正好等于pivot，会导致死
   循环。除此以外，不移动只会带来冗余，并不会导致错误。（等于pivot时也移动指针的那种
   写法，交换后不移动同样不会导致错误。）
 * i如果停住，要么是跟j交叉了（等于j+1，极限是r+1），要么是停在了>=pivot的元素上，
   j如果停住，要么是跟i交叉了（等于i-1，极限是l），要么是停在了<=pivot的元素上，
   如果没有交叉，则i、j进行交换，如果交叉了，则退出主循环，此时，j==i-1，如果i没有移动过，
   则j==l，如果i已经移动过，则j停在一个<=pivot的元素上，所以，无论i有没有移动过，j一定
   停在一个<=pivot的元素上，此时，可以跟pivot无条件交换。
 * 因为选了一个元素做pivot，最后又跟pivot交换了，所以能以j-1、j+1为分界进行递归。有些
   写法只取pivot的值，不用pivot的具体位置，退出主循环后压根不交换，这时调用递归时，就
   只能以j、j+1分界（如上所说，j停的位置一定是<=pivot的，a[j]<=a[j+1]一定成立，但因为
   不跟pivot交换，a[j-1]不一定<=a[j]）。
 * 还有一些情况下（比如上面说的主循环加了i<j，或者指针循环中加了i<j），退出主循环后
   j所指的元素与pivot大小关系未知，需要先比较大小再决定是否交换，此时，只能以j-1、j
   进行分界（如果交换了，则a[j]==pivot。如果没交换，则a[j]>=pivot。无论交不交换，
   a[j-1]<=a[j]一定成立，而a[j]与a[j+1]大小关系未知）。
 * 同一个版本，能用j-1、j+1分界，并不意味着就能用j-1、j或j、j+1分界，虽然这样分出来的
   子数组大小关系并无问题，但有可能某个子数组长度并没有缩短，从而导致死递归。具体如何
   分界，需要看整体的逻辑，特别是用来分界的指针（这里是j）跳出主循环后的落点范围，如果
   j的取值范围是[l,r]，那么j-1、j或j、j+1分界一定死递归，如果是[l-1,r]，那么即便
   j-1、j+1分界也会死递归。
 * 递归时能不能用i来分界？完全可以。退出主循环时，j==i-1（也可能是j==i，此时a[j]==pivot，
   所以不影响统一当成j==i-1一样来处理），把这个等式代入，结果就是，用i-1跟pivot交换，
   用i-2、i作为分界进行递归，两者完全等价，但明显不如用j-1、j+1分界来得直观清楚。
 * 综上，要快速判断一个quicksort是否正确，可以依次判断以下几个方面：
   框架（参数验证、初始化pivot和指针、主循环、指针循环、跳出、交换指针、交换pivot、递归）
   主循环，如果不是永真式，检查条件对指针轨迹的影响。
   指针循环，是否做了越界保护，是否可以重合交叉，不能则退出主循环后要额外比较才能swap。
   递归分界，最容易出错的地方，切分出来的子数组很可能跟原来的一样大，导致死递归，所以要
   特别注意分界指针的停留范围，用停留范围和分界值一计算，如果能凑出[l,r]的子数组，则一定
   死循环，需要在分界之前做特殊处理。
 * i范围[l+1,r+1]，j范围[l,r]，跳出时可能交叉1（即i==j+1），也可能重合，都停在等于pivot的元素上
 */
void quicksort(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int i = l + 1;
        int j = r;

        while (1)
        {
            while (i <= j && a[i] < pivot) i++;
            while (i <= j && a[j] > pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        swap(a, l, j);

        quicksort(a, l, j-1);
        quicksort(a, j+1, r);
    }
}


/*
 * 跟上面唯一的不同是，与pivot相等时也移动指针。此时，pivot已经无法拦住指针，无论
   i、j都需要加上i<=j。其它处理完全等价。
 * 指针循环中也可以判边界l、r（即i<=r,j>l），这样i、j指针越过后交叉有可能大于1，但因为
   都是相等的元素，也不影响逻辑。
 */
void quicksort0(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int i = l + 1;
        int j = r;

        while (1)
        {
            while (i <= j && a[i] <= pivot) i++;
            while (i <= j && a[j] >= pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        swap(a, l, j);

        quicksort0(a, l, j-1);
        quicksort0(a, j+1, r);
    }
}


/*
 * 这个版本的差别，是指针循环中用了i<j而不是i<=j，即左右指针在移动的过程中，允不允许重合。
   如果允许重合，则循环终止后，j所指元素一定<=pivot，直接与pivot交换即可
   如果不允许重合，则循环终止后，j所指元素与pivot的大小关系未知，需要额外比较才知道是否
   需要交换。因为j不一定会与pivot交换，所以最后递归分界的时候，不能写j+1
   另外，无论是否允许重合，循环终止后，j右边元素一定>=pivot，j左边元素一定<=pivot，差别
   只在j所指的元素。
 * 指针循环i<j条件下，i最多到r，j最多到l+1，所以跳出主循环后的else语句里可以j--再swap。
 */
void quicksort1(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int i = l + 1;
        int j = r;

        while (1) /* 也可以写成while(i < j)，等价 */
        {
            while (i < j && a[i] < pivot) i++;
            while (i < j && a[j] > pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        if (a[j] < a[l]) swap(a, l, j);
/*      else // 加上else，则递归时必须用j+1，如果还是用j，会死循环（考虑顺序的两个元素）
        {
            j--;
            swap(a, l, j);
        }
*/
        quicksort1(a, l, j-1);
        quicksort1(a, j, r);
    }
}


/*
 * 这个版本指针循环使用i<=j，而主循环用了i<j。
 * 指针循环i<=j条件下，j最多可以到l，所以跳出主循环后的else语句里不能简单地j--和swap
  （因为如果j==l，则j--后的j+1分界依旧是l，子数组长度依旧是l到r，从而死循环），需要排除
   j==l才行（可以写成j>l，也可以a[j]>a[l]）。
 */
void quicksort10(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int i = l + 1;
        int j = r;

        while (i < j)
        {
            while (i <= j && a[i] < pivot) i++;
            while (i <= j && a[j] > pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        if (a[j] < a[l]) swap(a, l, j);
        else if (j > l) /* 排除j==l，因为j最多可以到l，而quicksort1中j最多到l+1 */
        {
            j--;
            swap(a, l, j);
        }

        quicksort10(a, l, j-1);
        quicksort10(a, j+1, r);
    }
}


/*
 * 这个版本用最右的元素a[r]作pivot
 * i最右到r-1，j最左到l，i、j停住时，所指元素与pivot大小关系未知，所以需要额外判断。
 * 可以跟quicksort1对比一下分界，因为pivot位置不一样，额外判断交换后，分界也不一样。
 */
void quicksort2(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[r];
        int i = l ;
        int j = r-1;

        while (i < j) /* 也可以写成while(1)，等价 */
        {
            while (i < j && a[i] <= pivot) i++;
            while (i < j && a[j] >= pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        if (a[j] > a[r]) swap(a, r, j);

        quicksort2(a, l, j);
        quicksort2(a, j+1, r);
    }
}


/*
 * i最右可以到r，j最左可以到l-1，因为j有可能移到非法位置，用i分界更直观。
 * 当然同样可以用j分界，i==j+1代入，用j+1与pivot交换，以j、j+2分界。
 */
void quicksort20(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[r];
        int i = l ;
        int j = r-1;

        while (1)
        {
            while (i <= j && a[i] < pivot) i++;
            while (i <= j && a[j] > pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        swap(a, r, i);

        quicksort20(a, l, i-1);
        quicksort20(a, i+1, r);
    }
}


/*
 * 因为pivot用的是数组中随机一个元素的值，而且并没有把这个元素交换到固定位置，所以移动
   的过程中，并不会发生所有元素都小于或都大于pivot的情况（因为pivot就在其中），所以两个
   指针循环完全不需要比较i、j或者判断边界，i、j一定不会超出[l,r]
 */
void quicksort3(int a[], int l, int r)
{
    if (l < r)
    {
        // 使用a[l+rand()%(r-l)]就可以去掉后面的特殊处理，因为取不到a[r]
        int pivot = a[l+rand()%(r-l+1)];
        int i = l;
        int j = r;

        while (1)
        {
            /* 两个指针循环中的i<=j完全多余，加不加i<=j指针轨迹完全相同 */
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;

            if (i >= j) break;

            swap(a, i++, j--);
        }

        /*
         * 此处j==r则a[r]一定等于pivot，因为，如果a[r]>pivot，j会左移，如果a[r]<pivot，
           则i一定停在[l,r-1]的某处（因为pivot在其中），i、j会交换并移动，所以，只可能是
           a[r]==pivot，只有这种情况下j才会在退出主循环后依然停在r上，且此时i与j重合。
         * j如果停在r上，那么以j、j+1分界会导致子数组依旧是[l,r]，造成死递归，所以需要
           针对这种情况做特殊处理，j--，因为此时a[r]==pivot，所以可以用j-1、j分界。
         * 跳出主循环后j依旧停在r上，只有当用a[r]做pivot时才有可能发生，所以，如果取
           pivot时能排除掉a[r]，这里的特殊处理就可以去掉。
         */
        if (j == r) j--;

        quicksort3(a, l, j);
        quicksort3(a, j+1, r);
    }
}


/*
 * 算法4中的版本，最大差别是先++--再比较，这样++--就可以只写在一处（swap后不用再++--）。
 * 指针的轨迹唯一的不同是，i最右只会停在r。
 * if (j == l)并不会执行到，因为此时a[j] > pivot一定是不成立的。
 */
void quicksort_s(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int i = l;
        int j = r + 1;

        while (1)
        {
            while (a[++i] < pivot) if (i == r) break;
            while (a[--j] > pivot) if (j == l) break; /* redundant since a[lo] acts as sentinel */

            if (i >= j) break;

            swap(a, i, j);
        }

        swap(a, l, j);

        quicksort_s(a, l, j-1);
        quicksort_s(a, j+1, r);
    }
}


/* [l..lt-1] [lt..i-1] [i..gt] [gt..r] */
void quicksort3way(int a[], int l, int r)
{
    if (l < r)
    {
        int pivot = a[l];
        int lt = l;
        int gt = r;
        int i = l + 1;

        while (i <= gt)
        {
            if (a[i] < pivot)
            {
                swap(a, i++, lt++);
            }
            else if (a[i] > pivot)
            {
                swap(a, i, gt--);
            }
            else
            {
                i++;
            }
        }

        quicksort3way(a, l, lt-1);
        quicksort3way(a, gt+1, r);
    }
}


void insertionsort(int a[], int l, int r)
{
    for (int i = l; i <= r; i++)
    {
        for (int j = i; j > l && a[j] < a[j-1]; j--)
        {
            swap(a, j, j-1);
        }
    }
}

// return the index of the median element among a[i], a[j], and a[k]
int median3(int a[], int i, int j, int k)
{
    return (a[i] < a[j] ?
           (a[j] < a[k] ? j : a[i] < a[k] ? k : i) :
           (a[k] < a[j] ? j : a[k] < a[i] ? k : i));
}

#define INSERTION_SORT_CUTOFF   8
#define MEDIAN_OF_3_CUTOFF      40  // cutoff to median-of-3 partitioning

void quicksort3wayBM(int a[], int l, int r)
{
    int n = r - l + 1;

    if (n <= INSERTION_SORT_CUTOFF)
    {
        insertionsort(a, l, r);
        return;
    }
    // use median-of-3 as partitioning element
    else if (n <= MEDIAN_OF_3_CUTOFF)
    {
        int m = median3(a, l, l + n/2, r);
        swap(a, m, l);
    }
    // use Tukey ninther as partitioning element
    else
    {
        int eps = n/8;
        int mid = l + n/2;
        int m1 = median3(a, l, l + eps, l + eps + eps);
        int m2 = median3(a, mid - eps, mid, mid + eps);
        int m3 = median3(a, r - eps - eps, r - eps, r);
        int ninther = median3(a, m1, m2, m3);
        swap(a, ninther, l);
    }

    // Bentley-McIlroy 3-way partitioning
    int pivot = a[l];
    int i = l, j = r+1;
    int p = l, q = r+1;

    while (1)
    {
        while (a[++i] < pivot)
            if (i == r) break;
        while (pivot < a[--j])
            if (j == l) break;

        if (i == j && a[i] == pivot)
            swap(a, ++p, i);
        if (i >= j) break;

        swap(a, i, j);
        if (a[i] == pivot) swap(a, ++p, i);
        if (a[j] == pivot) swap(a, --q, j);
    }

    i = j + 1;
    for (int k = l; k <= p; k++)
        swap(a, k, j--);
    for (int k = r; k >= q; k--)
        swap(a, k, i++);

    quicksort3wayBM(a, l, j);
    quicksort3wayBM(a, i, r);
}


int partition(int a[], int l, int r)
{
    int pivot = a[l];
    int i = l;
    int j = r + 1;

    while (1)
    {
        while (a[++i] < pivot) if (i == r) break;
        while (a[--j] > pivot);

        if (i >= j) break;

        swap(a, i, j);
    }

    swap(a, l, j);

    return j;
}


/* 用快速排序的partition找出数组中第k小的元素 */
int select(int a[], int n, int k)
{
    if (k < 0 || k >= n) return -1;

    int l = 0;
    int r = n - 1;

    while (l < r)
    {
        int i = partition(a, l, r);

        if (i > k) r = i - 1;
        else if (i < k) l = i + 1;
        else return a[i];
    }

    return a[l];
}


void bubble_sort(int a[], int n)
{
    for (int i = 0; i < n - 1; i++) /* bubble count */
    {
        for (int j = 0; j < n - 1 - i; j++)
        {
            if(a[j] > a[j + 1])
            {
                swap(a, j, j+1);
            }
        }
    }
}


int is_sorted(int a[], int n)
{
    for (int i = 1; i < n; i++)
    {
        if (a[i] < a[i-1])
        {
            return 0;
        }
    }

    return 1;
}

void fill(int a[], int n)
{
    for (int i = 0; i < n; i++)
    {
        a[i] = rand() % 10000;
    }
}

void shuffle(int a[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int r = i + rand() % (n-i); // [i,n-1]
        int temp = a[i];
        a[i] = a[r];
        a[r] = temp;
    }
}

typedef void (*sort_func)(int a[], int l, int r);

void test_sort(sort_func func, int n, int trials)
{
    int *a = malloc(n * sizeof(int));
    clock_t total = 0;

    if (NULL == a) return;

    for (int i = 0; i < trials; i++)
    {
        clock_t start, now;

        for (int j = 0; j < n; j++)
        {
            a[j] = rand() % 10000;
        }

        start = clock();
        func(a, 0, n-1);
        now = clock();
        total += now - start;

        assert(is_sorted(a, n));
    }

    free(a);

    printf("time: %ldms\n", total*1000/CLOCKS_PER_SEC);
}


void print_array(int a[], int n)
{
    printf("[%d", a[0]);

    for (int i = 1; i < n; i++)
    {
        printf(",%d", a[i]);
    }

    printf("]\n");
}

#define print_array(a) print_array(a, sizeof(a)/sizeof(a[0]))


int main(int argc, char *argv[])
{
    srand(time(NULL));

    test_sort(quicksort3, 2000, 300);
    test_sort(quicksort2, 2000, 300);
    test_sort(quicksort3wayBM, 2000, 300);
    test_sort(quicksort3way, 2000, 300);

    printf("\n");
    {
        int a[] = {50, 5, 1, 98, 34, 66, 100, 5, 7, 89, 91};

        quicksort(a, 0, sizeof(a)/sizeof(int)-1);
        print_array(a);
    }

    printf("\n");
    {
        int a[] = {50, 5, 1, 98, 34, 66, 100, 5, 7, 89, 91};
        int n = sizeof(a)/sizeof(int);

        for (int i = 0; i < n; i++)
        {
            int ith = select(a, n, i);
            printf("%d ", ith);
        }

        printf("\n");
    }

    return 0;
}
