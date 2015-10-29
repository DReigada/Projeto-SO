#include <unistd.h>
#include <stdio.h>

int main()
{
	printf("Process %d started to calculate a division by 0.\n", getpid());

  int a, b, c;

  a = 5;
  b = 0;
  c = a / b;

  printf("c=%d\n",c);

  printf("------------------Não vai dar meus friends------------------------\n");

  return 0;
}

