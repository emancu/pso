#include <stdio.h>

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define SET_BIT(var, pos) ((var) |= (1<<(pos)))
#define UNSET_BIT(var, pos) ((var) &= (~(1<<(pos))))

int main(void) {
  char x = 0x0F, i;
  for (i = 0; i < 8; i++) {
    printf("%d : %d\n", i, CHECK_BIT(x, i) > 0);
    if (!CHECK_BIT(x, i)) SET_BIT(x, i);
    else UNSET_BIT(x, i);
  }
  printf("x: %x\n", x);
}
