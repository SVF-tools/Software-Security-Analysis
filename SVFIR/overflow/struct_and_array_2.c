#include <stdio.h>
struct Data {
	int value;
	char name[5];
};
void process_data_array(struct Data *data_array,
                        int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			data_array[i].name[j] = 'A';
		}
		data_array[i].name[size-1] = '\0';
	}
}
int main() {
	struct Data data_array[10];
	process_data_array(data_array, 10);
	return 0;
}