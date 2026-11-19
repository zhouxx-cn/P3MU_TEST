#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "utils.h"

typedef struct tag_MMAP_Node
{
	unsigned long Start_P;
	unsigned long Start_V;
	unsigned long length;
	unsigned long refcount;
	struct tag_MMAP_Node * next;
}TMMAP_Node_t;

TMMAP_Node_t * pTMMAPNode = NULL;

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK (~(0xfff))

static int fd = -1;
static const char dev[]="/dev/mem";

/* no need considering page_size of 4K */
void * memmap(unsigned long phy_addr, unsigned long size)
{
	unsigned long phy_addr_in_page;
	unsigned long page_diff;

	unsigned long size_in_page;

	TMMAP_Node_t * pTmp;
	TMMAP_Node_t * pNew;

	void *addr=NULL;

	if(size == 0)
	{
		printf("memmap():size can't be zero!\n");
		return NULL;
	}

	/* check if the physical memory space have been mmaped */
	pTmp = pTMMAPNode;
	while(pTmp != NULL)
	{
		if( (phy_addr >= pTmp->Start_P) && 
				( (phy_addr + size) <= (pTmp->Start_P + pTmp->length) ) )
		{
			pTmp->refcount++;   /* referrence count increase by 1  */
			return (void *)(pTmp->Start_V + phy_addr - pTmp->Start_P);
		}

		pTmp = pTmp->next;
	}

	/* not mmaped yet */
	if(fd < 0)
	{
		/* dev not opened yet, so open it */
		fd = open (dev, O_RDWR | O_SYNC);
		if (fd < 0)
		{
			printf("memmap():open %s error!\n", dev);
			return NULL;
		}
	}

	/* addr align in page_size(4K) */
	phy_addr_in_page = phy_addr & PAGE_SIZE_MASK;
	page_diff = phy_addr - phy_addr_in_page;

	/* size in page_size */
	size_in_page =((size + page_diff - 1) & PAGE_SIZE_MASK) + PAGE_SIZE;

	addr = mmap((void *)0, size_in_page, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr_in_page);
	if (addr == MAP_FAILED)
	{
		printf("memmap():mmap @ 0x%lx error!\n", phy_addr_in_page);
		close(fd);
		return NULL;
	}

	/* add this mmap to MMAP Node */
	pNew = (TMMAP_Node_t *)malloc(sizeof(TMMAP_Node_t));
	if(NULL == pNew)
	{
		printf("memmap():malloc new node failed!\n");
		munmap((void *)addr, size_in_page);
		close(fd);
		return NULL;
	}
	pNew->Start_P = phy_addr_in_page;
	pNew->Start_V = (unsigned long)addr;
	pNew->length = size_in_page;
	pNew->refcount = 1;
	pNew->next = NULL;

	if(pTMMAPNode == NULL)
	{
		pTMMAPNode = pNew;
	}
	else
	{
		pTmp = pTMMAPNode;
		while(pTmp->next != NULL)
		{
			pTmp = pTmp->next;
		}

		pTmp->next = pNew;
	}

	return (void *)((char *)addr+page_diff);
}

int memunmap(void * addr_mapped)
{
	TMMAP_Node_t * pPre;
	TMMAP_Node_t * pTmp;

	if(pTMMAPNode == NULL)
	{
		printf("memunmap(): address have not been mmaped!\n");
		return -1;
	}

	/* check if the physical memory space have been mmaped */
	pTmp = pTMMAPNode;
	pPre = pTMMAPNode;

	do
	{
		if( ((unsigned long)addr_mapped >= pTmp->Start_V) && 
				((unsigned long)addr_mapped <= (pTmp->Start_V + pTmp->length)) )
		{
			pTmp->refcount--;   /* referrence count decrease by 1  */
			if(0 == pTmp->refcount)
			{

				//printf("memunmap(): map node will be remove!\n");

				/* delete this map node from pMMAPNode */
				if(pTmp == pTMMAPNode)
				{
					pTMMAPNode = NULL;
				}
				else
				{
					pPre->next = pTmp->next;
				}

				/* munmap */
				if(munmap((void *)pTmp->Start_V, pTmp->length) != 0 )
				{
					printf("memunmap(): munmap failed!\n");
				}

				free(pTmp);
			}

			return 0;
		}

		pPre = pTmp;
		pTmp = pTmp->next;
	}while(pTmp != NULL);

	printf("memunmap(): address have not been mmaped!\n");
	return -1;
}

#define DEFAULT_MD_LEN 128
int reg_read(unsigned int phy_addr, unsigned int *value)
{
	void *pMem = NULL;
	
	if (NULL == value)
	{
		return -1;
	}
	
	pMem = memmap(phy_addr, DEFAULT_MD_LEN);
	if (pMem == NULL)
	{
		printf("memmap failed!\n");
		return -1;
	}
	
	*value = *(unsigned int*)pMem;
	memunmap((void *) pMem);
	return 0;
}

int reg_write(unsigned int phy_addr, unsigned int value)
{
	void *pMem = NULL;
	
	pMem = memmap(phy_addr, DEFAULT_MD_LEN);
	if (pMem == NULL)
	{
		printf("memmap failed!\n");
		return -1;
	}
	
	*(unsigned int*)pMem = value;
	memunmap((void *) pMem);
	return 0;
}


int i2c_write(unsigned int i2c_num, unsigned int dev_addr,
		unsigned int reg_addr, unsigned int data,
		unsigned int reg_width, unsigned int data_width)
{
	int retval = 0;
	int fd = -1;
	int index = 0;
	char file_name[0x10];
	unsigned char buf[4];

	printf("i2c_num:0x%x, dev_addr:0x%x; reg_addr:0x%x; data:0x%x; reg_width: %d; data_width: %d.\n",
			i2c_num, dev_addr << 1, reg_addr, data, reg_width, data_width);

	sprintf(file_name, "/dev/i2c-%u", i2c_num);
	fd = open(file_name, O_RDWR);
	if (fd<0) {
		printf("Open %s error!\n", file_name);
		return -1;
	}

	retval = ioctl(fd, I2C_SLAVE_FORCE, dev_addr);
	if(retval < 0) {
		printf("set i2c device address error!\n");
		retval = -1;
		goto end;
	}

	if (reg_width == 2) {
		buf[index] = (reg_addr >> 8) & 0xff;
		index++;
		buf[index] = reg_addr & 0xff;
		index++;
	} else {
		buf[index] = reg_addr & 0xff;
		index++;
	}

	if (data_width == 2) {
		buf[index] = (data >> 8) & 0xff;
		index++;
		buf[index] = data & 0xff;
		index++;
	} else {
		buf[index] = data & 0xff;
		index++;
	}

	retval = write(fd, buf, (reg_width + data_width));
	if(retval < 0) {
		printf("i2c write error!\n");
		retval = -1;
		goto end;
	}

	retval = 0;

end:
	close(fd);
	return retval;
}

int i2c_read(unsigned int i2c_num, unsigned int dev_addr, unsigned int reg_addr,
		unsigned int reg_addr_end, unsigned int reg_width,
		unsigned int data_width, unsigned int *value, int len)
{
	int retval = 0;
	int fd = -1;
	char file_name[0x10];
	unsigned char buf[4];
	int cur_addr;
	static struct i2c_rdwr_ioctl_data rdwr;
	static struct i2c_msg msg[2];
	unsigned int data;

	memset(buf, 0x0, 4);

	sprintf(file_name, "/dev/i2c-%u", i2c_num);
	fd = open(file_name, O_RDWR);
	if (fd < 0) {
		printf("Open %s error!\n",file_name);
		return -1;
	}

	retval = ioctl(fd, I2C_SLAVE_FORCE, dev_addr);
	if (retval < 0) {
		printf("CMD_SET_I2C_SLAVE error!\n");
		retval = -1;
		goto end;
	}
	msg[0].addr = dev_addr;
	msg[0].flags = 0;
	msg[0].len = reg_width;
	msg[0].buf = buf;
	msg[1].addr = dev_addr;
	msg[1].flags = 0;
	msg[1].flags |= I2C_M_RD;
	msg[1].len = data_width;
	msg[1].buf = buf;

	rdwr.msgs = &msg[0];
	rdwr.nmsgs = (__u32)2;
	for (cur_addr = reg_addr; cur_addr <= reg_addr_end; cur_addr++)
	{
		if (reg_width == 2) {
			buf[0] = (cur_addr >> 8) & 0xff;
			buf[1] = cur_addr & 0xff;
		} else
			buf[0] = cur_addr & 0xff;

		retval = ioctl(fd, I2C_RDWR, &rdwr);
		if (retval != 2) {
			printf("CMD_I2C_READ error!\n");
			retval = -1;
			goto end;
		}

		if (data_width == 2) {
			data = buf[1] | (buf[0] << 8);
		} else
			data = buf[0];

		if (cur_addr-reg_addr < len)
		{
			value[cur_addr-reg_addr] = data;
		}
		//printf("0x%x: 0x%x\n", cur_addr, data);
	}

	retval = 0;

end:
	close(fd);
	return retval;
}

