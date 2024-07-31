#include <stdio.h>
#include <stdlib.h>

#define NFT_LEN 16
typedef struct {
	char buffer[8];
} nft_set_elem;

void nft_set_elem_init(nft_set_elem *elem,
                       int len) {
	// Some initialization code is omitted here
	elem->buffer[len - 1] = '\0';
}

int main() {
	// Call the initialization function
	nft_set_elem elem;
	nft_set_elem_init(&elem, NFT_LEN);
	return 0;
}