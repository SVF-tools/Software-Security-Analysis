#include "stdbool.h"
extern void svf_assert(bool);

int main(int x){
	int y;
	if(x > 10)
		y = x + 1;
	else
		y = 10;
	svf_assert(y>=x+1);
}
