#include <fs.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t events_read(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin",        0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout",       0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr",       0, 0, invalid_read, serial_write},
	[FD_EVENT]  = {"/dev/events",  0, 0, events_read, invalid_write},
	[FD_FB]     = {"/dev/fb",         0, 0, invalid_read, fb_write},
	[FD_DISPINFO]={"/proc/dispinfo",  0, 0, dispinfo_read, invalid_write}, 
#include "files.h"
};

enum {FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END};

size_t *open_offsets;

void init_fs() {
  // TODO: initialize the size of /dev/fb
	int w = io_read(AM_GPU_CONFIG).width;
	int h = io_read(AM_GPU_CONFIG).height;
	file_table[FD_FB].size = w * h * 4;
	

	// initialize open_offsets to 0
	int files_num = sizeof(file_table) / sizeof(Finfo);
	open_offsets = (size_t *) malloc(files_num * sizeof(size_t));
	int i;
	for(i=0; i<files_num; i++){
		open_offsets[i] = 0;
	}
}

int fs_open(const char *pathname, int flags, int mode) {
	// orinary files
	printf("fs_open: <%s>\n", pathname);
	int i;
	int files_num = sizeof(file_table) / sizeof(Finfo);
	for (i=0; i<files_num; i++){
		//printf("%s vs %s\n", file_table[i].name, pathname);
		if ( strcmp(file_table[i].name, pathname) == 0 ){
			return i;
		}
	}
	// file not found
	assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {

	// special files
	if(file_table[fd].read != NULL) {
		int cnt = file_table[fd].read(buf, open_offsets[fd], len);
		open_offsets[fd] += cnt;
		return cnt;
	}

	// ordinary files
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
	
}

size_t fs_write(int fd, const void *buf, size_t len) {
	// special files
	if(file_table[fd].write != NULL) {
		int cnt = file_table[fd].write(buf, open_offsets[fd], len);
		open_offsets[fd] += cnt;
		return cnt;
	}

	size_t offset = file_table[fd].disk_offset + open_offsets[fd];
	open_offsets[fd] += len;
	assert(open_offsets[fd] <= file_table[fd].size);
	return ramdisk_write(buf, offset, len);
}

size_t fs_lseek(int fd, size_t offset, int whence) {
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
	return open_offsets[fd];
}

int fs_close(int fd){
	return 0;
}
