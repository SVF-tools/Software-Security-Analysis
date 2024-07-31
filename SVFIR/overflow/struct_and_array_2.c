#include <stdio.h>
struct Data {
	char name[10];
	int value;
};
void process_data_array(struct Data *data_array,
                         int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 20; j++) {
			data_array[i].name[j] = 'A';
		}
		data_array[i].name[9] = '\0';
	}
}
int main() {
	struct Data data_array[5];
	process_data_array(data_array, 5);
	return 0;
}