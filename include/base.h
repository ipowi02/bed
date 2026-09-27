#ifndef BASE_H_
#define BASE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
typedef int64_t ssize_t;
void copy(void *dst, void *src, ssize_t n) {
     if (n < 0) {
	  // strcpy
	  char *sdst = (char*)dst;
	  char *ssrc = (char*)src;

	  for (;;) {
	       uint64_t chunk = *(uint64_t*)ssrc;
	       uint64_t mask = chunk - 0x0101010101010101ULL
		    & ~chunk
		    & 0x8080808080808080ULL;

	       if (mask) {
		    size_t idx = 0;

		    while (!(mask & 0x80ULL)) {
			 mask >>= 8;
			 idx++;
		    }

		    for (size_t i = 0; i <= idx; i++)
			 sdst[i] = ssrc[i];
		    
		    return;
		    
	       }

	       *(uint64_t *)sdst = chunk;

	       ssrc += 8;
	       sdst += 8;
	  }
	  
     } else {
	  // memcpy
	  char *sdst = (char*)dst;
	  char *ssrc = (char*)src;
	  int64_t i = 0;
	  while (i < n)
	       sdst[i] = ssrc[i], i++;
     }
}

int fileinfo(FILE *f, // file to get the info about
	     char *buf, // (OPTIONAL) Buffer to read the file into, ignored if NULL
	     size_t buf_size, // Buffer size, if buf == NULL this is ignored
	     size_t *size)  // (OPTIONAL) File size, ignored if NULL
{
     buf_size -= 1;
     if (!f)
	  return -1;
     ssize_t sz = -2;
     if (buf) {  
	  if (fseek(f, 0, SEEK_END) != 0)
	       return -1;
	  sz = ftell(f);
	  if (sz == -1)
	       return -1;
	  
	  if (fseek(f, 0, SEEK_SET) != 0) {
	       return -1;
	  }
	  
	  size_t n = fread(buf, 1, buf_size, f);
	  if (n != buf_size)
	       return -1;
	  buf[n] = '\0';
     }
     if (size) {
	  if (sz != -2)
	       *size = sz;
	  else {
	       if (fseek(f, 0, SEEK_END) != 0) {
		    return -1;
	       }
	       ssize_t tmp = ftell(f);
	       if (tmp == -1) {
		    return -1;
	       }
	       *size = tmp;
	  
	       if (fseek(f, 0, SEEK_SET) != 0) {
		    return -1;
	       }
	  }
     }
}
#endif // BASE_H_
