#include <stdio.h>

f();

main() {
	f();
}

f() {
	int x = 3;
	int y = 5;
	if (x < y) {
		printf("%d\n", x);
	} else {
		printf("%d\n", y);
	}
	scanf("%d",&x);
}

