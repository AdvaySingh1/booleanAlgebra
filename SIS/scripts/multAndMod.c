#include <stdio.h>

void printBin(int num, int digits)
{
    char s[digits];
    for (int i = 0; i < digits; ++i)
    {
        s[digits - i - 1] = (num >> i) & 1 ? '1' : '0';
    }
    printf("%s", s);
}

int main()
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            int mult = i * j;
            int mod = mult % 13;
            printBin(i, 3);
            printBin(j, 3);
            printf(" ");
            printBin(mod, 4);
            printf("\n");
        }
    }
}