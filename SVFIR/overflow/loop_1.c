#include <stdio.h>
#define BUF_LEN 20

void handle_buffer(char *input) {
	char buffer[BUF_LEN];
	int i = 0;
	while (input[i] != '\0') {
		buffer[i] = input[i];
		i++;
	}
	buffer[BUF_LEN-1] = '\0';
	printf("Buffer content: %s\n", buffer);
}

int main() {
	char input[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123";
	handle_buffer(input);
	return 0;
}