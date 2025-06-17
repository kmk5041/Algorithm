#include <stdio.h>

int euclid(int m, int n){
	if(n==0)
		return m;
	while(n!=0){
		int r=m%n;
		m=n;
		n=r;
	}
	return m;
}

int main(){
	int m,n;
	scanf("%d %d",&m,&n);
	printf("%d",euclid(m,n));
	return 0;
}
