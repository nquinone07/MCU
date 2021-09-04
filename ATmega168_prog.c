#include <avr/io.h>
int global_var;
int main(void)
{
    return global_var + SP;
}
