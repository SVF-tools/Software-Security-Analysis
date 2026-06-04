#include <stdio.h>
#include <stdbool.h>
#define BUF_LEN 20
bool continue_copying = true;
void copy_data(char *input) {
	char buffer[BUF_LEN];
	int i = 0;
	while (continue_copying) {
		buffer[i] = input[i];
		i++;
		if (input[i] == '\0') {
			continue_copying = false;
		}
	}
	buffer[BUF_LEN-1] = '\0';
	printf("Buffer content: %s\n", buffer);
}
int main() {
	char input[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123";
	copy_data(input);
	return 0;
}