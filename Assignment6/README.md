# Virtual Address Translator

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C program that translates logical to physical addresses for a virtual address space of size 216 = 65,536 bytes. The program reads from a file containing logical addresses and, using a TLB as well as a page table, will translate each logical address to it's corresponding physical address and output the value of the byte stored at the translated physical address. The goal behind this project is to simulate the steps involved in translating logical to physical addresses.

### Files
- vmmgr.c
- BACKING_STORE.bin
- ADDRESS_FILE.txt

### Instructions
- gcc vmmgr.c -o vmmgr
- ./vmmgr <backing store file> <address file>

### Notes
Along with the data found at the physical address, the program will ouptut the number of page faults, the page fault rate, TLB hits, and TLB hit rate.