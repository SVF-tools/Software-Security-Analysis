extern void OVERFLOW(void* data, int size);
int main() {
    int arr[5];
    OVERFLOW(arr, 5*sizeof(int));
    arr[5] = 10;
}