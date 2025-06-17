#include <stdio.h>
int ss(int m, int n){
	
	int min=m;
	if(n<min)
		min=n;
	for(int i=min; i>1; i--){
		if(m%i==0){
			if(n%i==0)
				return i;
		}
	}
	return 1;
}
int main(){
	int m,n;
	scanf("%d %d", &m,&n);
	printf("%d",ss(m,n));
	return 0;
}
