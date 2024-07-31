#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 10

void handle_client_request(char *input,
                           int index) {
	int buffer[BUFFER_SIZE] = { 0 };
	if (index >= 0 && index < BUFFER_SIZE)
		buffer[index] = input[index];
	else
		printf("ERR: Array index is out of bounds\n");
}
void process_socket_data(char *input,
                         int index) {
	handle_client_request(input, index);
}
int main(int index) {
	char inputBuffer[BUFFER_SIZE] = {0};
	process_socket_data(inputBuffer, index);
	return 0;
}