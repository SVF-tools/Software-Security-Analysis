#include <stdio.h>
#define BUF_LEN 20
void handle_buffer(char *input) {
	char buffer[BUF_LEN];
	for  (int i = 0; i < 30; ++i) {
		buffer[i] = input[i];
		if (input[i] == '\0')
			break;
	}
	buffer[BUF_LEN-1] = '\0';
	printf("Buffer content: %s\n", buffer);
}
int main() {
	char input[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123";
	handle_buffer(input);
	return 0;
}