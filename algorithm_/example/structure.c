#include <stdio.h>

typedef struct{
	int a;
} st;

int main(){
	st x;
	x.a=3;
	st y;
	y=x;
	printf("%d",y.a);
	return 0;
}
