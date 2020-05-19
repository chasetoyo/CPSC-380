/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A C program that translates logical to physical addresses
for a virtual address space of size 216 = 65,536 bytes.
The program will read from a file containing logical addresses
amd using a TLB as well as a page table, will translate each
logical address to it's corresponding physical addresss and output
the value of the byte stored at that physical address.

It will also output the number of page faults, TLB hits,
and their respective rates.

Instructions:
gcc vmmgr.c -o vmmgr
./vmmgr <backing store file> <address file>

Notes:
The byte is outputed as a signed int value. There are many values
that only contain a 0, so I'm not sure if I did something
wrong or if that's how it's supposed to be.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char* to_hex(int address) {
	int size = 4;
	char* hex = (char*) malloc(size * sizeof(char));
	sprintf(hex, "%04X", address);
	return hex;
}

/*reads an inter*/
int get_page(int address) {
	int size = 4;
	char* hex = (char*) malloc(size * sizeof(char));
	char* page = (char*) malloc((size/2) * sizeof(char));
	hex = to_hex(address);

	for (int i = 0; i < size/2; ++i) {
		page[i] = hex[i];
	}

	int page_int = (int)strtol(page, NULL, 16);
	free(hex);
	free(page);
	return page_int;
}

int get_offset(int address) {
	int size = 4;
	char* hex = (char*) malloc(size * sizeof(char));
	char* offset = (char*) malloc((size/2) * sizeof(char));
	hex = to_hex(address);

	int j = size/2;
	for (int i = 0; i < size/2; ++i) {
		offset[i] = hex[j];
		++j;
	}

	int offset_int = (int)strtol(offset, NULL, 16);
	free(hex);
	free(offset);
	return offset_int;
}

typedef struct tlb_entry{
	int page;
	int frame;
} tlb_entry;

int main(int argc, char const *argv[])
{
	
	if (argc != 3) { //check if right num of args provided
	    printf("You provided %i argument(s), %i required.\n", argc, 3);
	    return -1;
	}
	 
	FILE *backing_store;
	FILE *address_file;
	/*open backing store as a binary file*/
	backing_store = fopen(argv[1],"rb");
	if (backing_store == NULL) {
		 printf("Error %i: %s\n", errno, strerror(errno));
		 return -1;
	}
	address_file = fopen(argv[2], "r");
	if (address_file == NULL) {
		 printf("Error %i: %s\n", errno, strerror(errno));
		 return -1;
	}

	/*256x256 byte array to hold read in data*/
	char* mm = (char*) malloc(256 * 256 * sizeof(char));
	tlb_entry* tlb = (tlb_entry*) malloc(16*sizeof(tlb_entry));
	int page_table[256];

	/*initialize page table with -1*/
	for (int i = 0; i < 256; ++i){
		page_table[i] = -1;
	}

	/*intialize summary values*/
	double pf = 0, hit = 0;
	double pf_rate = -1.0, hit_rate = -1.0;

	int free_page = 0, free_index = 0, num_address = 0, curr_page = -1;

	char line[32];
	/*read in logical addresses*/
	while(fgets(line, 32, address_file)) {
		/*convert string to int*/
		int address = atoi(line);

		/*extract from logical address*/
		int page = get_page(address);
		int offset = get_offset(address);
		int frame = page_table[page];

		/*consult tlb*/
		for (int i = 0; i < 16; ++i) {
			if (tlb[i].page == page) {
				frame = tlb[i].frame;
				++hit;
				break;
			}
		}

		/*page fault*/
		if (frame == -1) {
			++pf;
			frame = free_page;

			/*set file pointer offset*/
			fseek(backing_store, page, SEEK_SET);
			curr_page = frame*256;
			fread(mm+curr_page, sizeof(char), 256, backing_store);

			/*update tlb*/
			tlb_entry* t = (tlb_entry*) malloc(sizeof(tlb_entry));
			t->page = page;
			t->frame = frame;
			tlb[free_index] = *t;

			/*update page table with new physical address*/
			page_table[page] = frame;

			++free_index;
			++free_page;

			/*reset TLB using FIFO*/
			if (free_index == 15) {
				free_index = 0;
			}
		}
		++num_address;
		curr_page = frame*256;
		int physical = curr_page+offset;
		char byte = mm[physical];
		printf("Virtual Address:%-5i || Physical Address: %-5i || Data: %d\n", address, physical, byte);
	}

	pf_rate = pf/num_address;
	hit_rate = hit/num_address;

	printf("---------------------------------------------------------\n");
	printf("Page Faults: %.1f\n", pf);
	printf("Page Fault Rate: %.3f\n", pf_rate);
	printf("TLB Hits: %.1f\n", hit);
	printf("TLB Hit Rate: %.3f\n", hit_rate);

	fclose(backing_store);
	fclose(address_file);
	free(mm);
	free(tlb);
	return 0;
}