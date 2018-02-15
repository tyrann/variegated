/*
 *Example of unaligned read/write operation on a set of 32-bit registers
 *Copyright © 2017 Thomas J. Rubeli
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#define register_number 32 //Number of registers in the array
#define register_size	4 //In bytes
#define MASK 0xFFFFFFFF
#define ROUND_DOWN(v,r)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_TESTS 4000

void hexdump_word_le(const void *ptr, size_t buflen, size_t colsize) {
	unsigned char *buf = (unsigned char*)ptr;
	int i, j;
	for (i=0; i<buflen; i+=colsize) {
		fprintf(stderr, "%06x: ", i);
		for (j=0; j<colsize; j++){
			if (i+j < buflen){
				fprintf(stderr, "%02x ", buf[i+colsize-1-j]);
			}else{
				fprintf(stderr, "   ");
			}
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

static uint32_t register_array[register_number] = {0};

/*
 * Write to a single byte to a possibly unaligned position in a register
 */

void __unaligned_rw_reg(uint32_t reg_num, uint8_t offset, uint8_t value) {

	uint32_t reg = register_array[reg_num];
	int mask = 0xFF << (offset << 3);
	int val = value << (offset << 3);

	register_array[reg_num] = (~mask & reg) | val;
}

/*
 * Read a single byte from a possibly unaligned position in a register
 */

void __unaligned_r_reg(uint32_t reg_num, uint8_t offset, uint8_t* value) {

	uint32_t reg = register_array[reg_num];
	*value = (reg >> (offset * 8)) & 0xFF;

}

/*
 * Read an array of data from a possibly unaligned offset
 * to the array of registers, byte after byte
 * Stores the result in data
 */

int read_from_reg_memory(uint8_t* data,uint32_t from, size_t size){

	if(from + size > register_number*register_size){
		return -1;
	}

	uint32_t start_reg = from / 4;
	uint8_t start_off = from % 4;

	for (int i = 0; i < size; ++i) {
		uint8_t shift = (i+start_off);
		__unaligned_r_reg(start_reg +(shift /4), shift%4, &data[i]);
	}
	return 0;

}
/*
 * Write an array of data from a possibly unaligned offset
 * to the array of registers, byte after byte
 */

int write_to_reg_memory(uint8_t* data,uint32_t from, size_t size){

	if(from + size > register_number*register_size){
		return -1;
	}

	uint32_t start_reg = from / 4;
	uint8_t start_off = from % 4;

	for (int i = 0; i < size; ++i) {
		uint8_t shift = (i+start_off);
		__unaligned_rw_reg(start_reg +(shift /4), shift%4, data[i]);
	}
	return 0;

}

/*
 * Write a value to a register, possibly unaligned
 * 
 */


void __unaligned_rw_reg_range(uint32_t reg_num, uint8_t from, uint8_t to, uint32_t value){
	if(from == to){
		return;
	}
	else if(to > 4){
		to = 4;
	}

	uint32_t reg = register_array[reg_num];
	int mask = MASK << ( (from)*8 ) & MASK >> (( 4-to )*8);

	register_array[reg_num] = (~mask & reg) | value;


}
/*
 * Write an array of data from a possibly unaligned offset
 * to the array of registers, using range access
 */

int write_to_reg_memory_range(uint8_t* data,uint32_t from, size_t size){

	if(from + size > register_number*register_size){
		return -1;
	}
	uint32_t real_off = from;
	uint32_t start_reg = from / 4;
	uint8_t start_off = from % 4;
	uint32_t value = 0;
    int j = 0;

	for (j = start_off; j < 4; ++j) {
		value |= (((uint32_t)data[j-start_off]) & 0xFF) << (8*j);
	}
	__unaligned_rw_reg_range(start_reg, start_off, (start_off+size),value);

	int i = 0;
	for (i = 4-start_off; i+4 <= size; i+=4) {
		uint8_t* head = data + i;
		value = *((uint32_t*)head);
		__unaligned_rw_reg_range(start_reg+(start_off+i)/4,0, 4,value);
	}
	int l = 0;
	value = 0;
	for (int k = i; k < size; ++k) {
		value |= (((uint32_t)data[k]) & 0xFF) << (8*l);
		l++;
	}
	if(size > i){
		__unaligned_rw_reg_range((real_off+size) / 4, 0, size-i,value);
	}

	return 0;

}

void run_tests(){
printf("%s \n","==========Unaligned write register test==========" );
	
	uint32_t full_block = register_number*register_size;

	uint8_t test[full_block];
	uint8_t test_read[full_block];

	for (int i = 0; i < NUM_TESTS; ++i) {

		for (int i = 0; i < full_block; ++i) {
			test[i]=rand() % 0xFF;		
		}
		memset(register_array,0xFF,full_block);
		memset(test_read,0,full_block);

		
		uint32_t from = rand() % register_number*register_size;
		uint32_t size = rand() % (register_number*register_size-from);

		fprintf(stderr, "Testing write methods from %d\tof size\t%d\t--->\t", from, size);
		write_to_reg_memory(test,from,size);

		memcpy(test_read,register_array,full_block);
		memset(register_array,0xFF,full_block);

		write_to_reg_memory_range(test,from,size);
		assert(!memcmp(register_array,test_read,full_block));
		fprintf(stderr, ANSI_COLOR_GREEN "[SUCCESS]" ANSI_COLOR_RESET"\n");
	}

	printf("%s \n","==========Unaligned read register test==========" );
	for (int i = 0; i < NUM_TESTS; ++i) {

		for (int i = 0; i < full_block; ++i) {
			test[i]=rand() % 0xFF;		
		}
		memset(register_array,0xFF,full_block);
		memset(test_read,0,full_block);

		
		uint32_t from = rand() % register_number*register_size;
		uint32_t size = rand() % (register_number*register_size-from);

		fprintf(stderr, "Testing random write and read from %d\tof size\t%d\t--->\t", from, size);
		write_to_reg_memory(test,from,size);
		read_from_reg_memory(test_read,from,size);
		assert(!memcmp(test_read,test,size));

		memset(register_array,0xFF,full_block);
		memset(test_read,0,full_block);
		write_to_reg_memory_range(test,from,size);
		read_from_reg_memory(test_read,from,size);

		assert(!memcmp(test_read,test_read,size));
		fprintf(stderr, ANSI_COLOR_GREEN "[SUCCESS]" ANSI_COLOR_RESET"\n");
	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	run_tests();

	return 0;
} 
