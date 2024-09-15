void str_insert_unchecked(void *buffer, const void *data, int position);
extern void OVERFLOW(void* data, int size);
int main() {
	char buffer[10] = {0};
	str_insert_unchecked(buffer, "abcdef", 5);
	OVERFLOW(buffer, 11);
}