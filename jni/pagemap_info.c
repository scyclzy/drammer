/*
 * pagemap_info.c
 *
 *  Created on: 2016-11-14
 *      Author: lizhongyuan
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define K(x) ((x)<<10)
#define M(x) ((x)<<20)
#define G(x) ((x)<<30)

#define MAX_CHUNKS (100)
#define SIZE_PER_CHUNK (M(4))

void * get_phys_addr(void *virt_addr) {
	static int pagemap_fd = 0;

	int bytes_read = 0;
	uint64_t value = 0;
	uint64_t pfn_num = 0;
	uint64_t offset = ((uint64_t)(virt_addr))/PAGESIZE;
	offset *= sizeof(uint64_t);

	if(pagemap_fd == -1) {
		return NULL;
	}

	if(pagemap_fd == 0) {
		pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
		if(pagemap_fd == -1) {
			perror("can not open pagemap");
			return NULL;
		}
	}

	bytes_read = pread(pagemap_fd, &value, sizeof(value), offset);
	if(bytes_read != sizeof(value)) {
		perror("read pagemap failed");
		return NULL;
	}

	if((value & (1LL<<63)) == 0) {
		printf("error:page not present\n");
		return NULL;
	}

	pfn_num = value & ((1LL<<55)-1);

	return (void*)(pfn_num*PAGESIZE + ((uint64_t)virt_addr)&(PAGE_SIZE-1));
}

int main(int argc, char * argv[]) {
	int i = 0;
	int j = 0;
	void *virt[MAX_CHUNKS] = { NULL };
	uint32_t bytes_mapped = 0;
	for(i=0; i<MAX_CHUNKS; i++) {
		virt[i] = mmap(NULL, SIZE_PER_CHUNK, PROT_READ|PROT_WRITE,
				MAP_ANONYMOUS | MAP_PRIVATE | MAP_LOCKED, -1, 0);
		if(virt[i] == MAP_FAILED) {
			perror("can not map");
			break;
		}
		bytes_mapped += SIZE_PER_CHUNK;
	}

	printf("bytes_mapped %d(MB)\n", bytes_mapped/1024/1024);

	return 0;
	printf("page size is 0x%08x\n", PAGESIZE);
	for(i=0; i<MAX_CHUNKS; i++) {
		for(j=0; j<SIZE_PER_CHUNK; j+=PAGESIZE) {
			printf("virt_addr:0x%016lX,phys_addr:0x%016lX\n",
					(uint64_t)virt[i]+j,
					(uint64_t)get_phys_addr((uint64_t)virt[i])+(uint64_t)j);
		}
	}

	return 0;
}
