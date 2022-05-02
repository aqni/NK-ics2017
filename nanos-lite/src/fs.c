#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = _screen.width * _screen.height * sizeof(uint32_t);
  file_table[FD_FB].open_offset = 0;
}

int fs_open(const char *pathname, int flags, int mode){
  for(int fd=0;fd< NR_FILES;fd++){
    if(0==strcmp(pathname, file_table[fd].name)){
      file_table[fd].open_offset=0;
      return fd;
    }
  }
  assert(0);
  return -1;
}

#define MYMIN(a,b) ((a)<(b)?(a):(b))
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);

ssize_t fs_read(int fd, void *buf, size_t len){
  if(fd<0||fd>=NR_FILES) return -1;
  Finfo* file=&file_table[fd];
  ssize_t nread=MYMIN(len,file->size-file->open_offset);

  switch(fd){
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:
    case FD_FB:
      assert(0);
      return -1;
    case FD_EVENTS:
      return events_read(buf, len);
    case FD_DISPINFO:
      dispinfo_read(buf,file->open_offset,len);
      break;
    case FD_NORMAL:
      return -1;
    default:
      ramdisk_read(buf,file->disk_offset+file->open_offset,nread);
      break;
  }
  file->open_offset += nread;
  return nread;
}

extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);
ssize_t fs_write(int fd, uint8_t *buf, size_t len){
  if(fd<0||fd>=NR_FILES) return -1;
  Finfo* file=&file_table[fd];
  ssize_t nwrite=MYMIN(len,file->size-file->open_offset);

  switch(fd){
    case FD_STDIN:
      assert(0);
      return -1;
    case FD_STDOUT:
    case FD_STDERR:
      for(ssize_t i=0;i<len;i++) _putc(buf[i]);
      return len;
    case FD_FB:
      fb_write(buf,file->open_offset,nwrite);
      break;
    case FD_EVENTS:
    case FD_DISPINFO:
    case FD_NORMAL:
      assert(0);
      return -1;
    default:
      ramdisk_write(buf,file->disk_offset+file->open_offset,nwrite);
      break;
  }

  file->open_offset += nwrite;
  return nwrite;
}

off_t fs_lseek(int fd, off_t offset,int whence){
  if( fd<0 || fd >=NR_FILES) return -1;
  Finfo* file=&file_table[fd];

  switch(whence){
    case SEEK_SET: break;
    case SEEK_CUR: offset += file->open_offset; break;
    case SEEK_END: offset += file->size; break;
    default:return -1;
  }

  if (offset < 0||offset > file->size)
    return -1;

  return file->open_offset = offset;
}

int fs_close(int fd){
  return 0;
}

size_t fs_filesz(int fd){
  return file_table[fd].size;
}
