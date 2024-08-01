#include <stdio.h>
#include <string.h>
#define NFT_LEN 16
typedef struct {
	char buffer[8];
} nft_set_elem;
void nft_set_elem_init(nft_set_elem *elem,
                       int len) {
	// Ensure we do not overflow the buffer
	if (len > sizeof(elem->buffer))
		elem->buffer[sizeof(elem->buffer)-1] = '\0';
	else
		elem->buffer[len - 1] = '\0';
}
int main() {
	// Call the initialization function
	nft_set_elem elem;
	nft_set_elem_init(&elem, NFT_LEN);
	return 0;
}