#include <fs.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
	//size_t open_offset;
  ReadFn read;
  WriteFn write;
//	size_t open_offset;
} Finfo;


enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};
enum {FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END};

size_t *open_offsets;

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb

	// initialize open_offsets to 0
	
	int files_num = sizeof(file_table) / sizeof(Finfo);
	open_offsets = (size_t *) malloc(files_num * sizeof(size_t));
	int i;
	for(i=0; i<files_num; i++){
		open_offsets[i] = 0;
	}
}

int fs_open(const char *pathname, int flags, int mode) {
	int i;
	int files_num = sizeof(file_table) / sizeof(Finfo);
	for (i=0; i<files_num; i++){
		//printf("%s vs %s\n", file_table[i].name, pathname);
		if ( strcmp(file_table[i].name, pathname) == 0 ){
			printf("open file fd=%d, filename=%s\n", i, pathname);
			return i;
		}
	}
	// file not found
	assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
	// exceeds file size
	//if(open_offsets[fd] >= file_table[fd].size){
	//	return -1;
	//}

	if(fd==FD_STDOUT){
		return -1;
	}

	if(fd==FD_STDIN || fd==FD_STDERR){
		return 0;
	}

	size_t offset = file_table[fd].disk_offset + open_offsets[fd];
	size_t remain_len = file_table[fd].size - open_offsets[fd];

	// if len to read will exceed EOF
	if(len > remain_len){
		// only read the remaining part
		open_offsets[fd] += remain_len;
		return ramdisk_read(buf, offset, remain_len);
	}

	// if the remaining part is enough for len
	open_offsets[fd] += len;
	return ramdisk_read(buf, offset, len); 
	//open_offsets[fd] += len;
	//assert(open_offsets[fd] <= file_table[fd].size);
	//return ramdisk_read(buf, offset, len);
}

size_t fs_write(int fd, const void *buf, size_t len) {
	//if(open_offsets[fd] >= file_table[fd].size){
	//	return -1;
	//}
	printf("fs_write called, fd=%d, len=%d\n",fd,len);
	
	if(fd==FD_STDOUT || fd==FD_STDERR){
		printf("using write to print to stdout, len=%d\n",len);
		int i;
		char *chars = (char *)buf;
		for(i=0; i<len; i++){
			putch(chars[i]);
		}
		open_offsets[fd] += len;
		return len;
	}
	
	if(fd==FD_STDIN){
		return -1;
	}

	size_t offset = file_table[fd].disk_offset + open_offsets[fd];
	open_offsets[fd] += len;
	assert(open_offsets[fd] <= file_table[fd].size);
	return ramdisk_write(buf, offset, len);
}

size_t fs_lseek(int fd, size_t offset, int whence) {
	printf("before lseek: open_offsets[%d] = %d\n", fd, open_offsets[fd]);
	switch (whence) {
		case FS_SEEK_SET:
			open_offsets[fd] = offset;
			break;
		case FS_SEEK_CUR:
			open_offsets[fd] += offset;
			break;
		case FS_SEEK_END:
			open_offsets[fd] = file_table[fd].size - offset;
			break;
		default:
			return -1;
	}
	printf("after lseek: open_offsets[%d] = %d\n", fd, open_offsets[fd]);
	return open_offsets[fd];
}

int fs_close(int fd){
	return 0;
}
