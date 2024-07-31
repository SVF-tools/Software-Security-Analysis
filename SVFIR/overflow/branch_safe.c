#include "stdbool.h"
#include <stdlib.h>

int main(int argc) {
	int buf[10];
	int *loc = (int*)malloc(4);
	int i = argc % 10;
	if (argc > 0) {
		*loc = i;
	} else {
		*loc = ++i;
	}
	int idx = *loc;
	if (idx >= 0 && idx < 10) {
		buf[idx] = 1;
	}
	return 0;
}