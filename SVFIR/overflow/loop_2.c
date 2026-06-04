#include <stdio.h>
void process_input(char input[5][10]) {
	char buffer[50];
	int i, j, k = 0;
	for (i = 0; i < 5; i++) {
		for (j = 0; j <= 10; j++) {
			buffer[k++] = input[i][j];
		}
	}
	buffer[49] = '\0';
}
int main() {
	char input[5][10] = {
	    "1234567890",
	    "abcdefghij",
	    "ABCDEFGHIJ",
	    "0987654321",
	    "ZYXWVUTSRQ" };
	process_input(input);
	return 0;
}