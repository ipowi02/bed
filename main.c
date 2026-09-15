#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>


#include "../tools/base.h"

#define TMPBUF_SIZE 128*1000
#define FNAMEBUF_SIZE 64
#define TMP_SIZE 128

char tmpbuf[TMPBUF_SIZE] = {0};
size_t bufcursor = 0;

size_t linecntr = 0;
char *linecursor = tmpbuf;

int advanceline() {
     char *linecursor = strchr(linecursor, '\n');
     if (linecursor == NULL) {
	  return -1;
	  // no more lines
     }

     linecntr++;
     return 0;
}

char *queryline(int lineno) {
     char *ret = tmpbuf;
     while ((ret = strchr(ret, '\n')) != NULL && lineno)
	  lineno--;
     // TODO: make it O(1) amortized somehow
     return ret;
}

char fnamebuf[FNAMEBUF_SIZE] = {0};

char input_mode = 0;



int main(int argc, char **argv) {
     const char *program_name = *argv++;
     const char *input_file = NULL;
     
     if (argv != NULL) {
	  input_file = *argv++;
     }

     if (input_file) {
	  FILE *f = fopen(input_file, "r");
	  assert(f != NULL && "Malformed input file path");
	  size_t input_size;
	  
	  assert(fileinfo(f, NULL, 0, &input_size) > 0);
	  printf("%zu\n", input_size);
     }
     
     char* ret;

     do {
	  if (!input_mode) {
	       char cmd[TMP_SIZE] = {0};
	       ret = fgets(cmd, TMP_SIZE, stdin);
	       
	       assert(ret != NULL && "failed at cmd");
	     
	       // look up command
	  } else {
	       ret = fgets(tmpbuf + bufcursor, TMP_SIZE, stdin);
	       
	       assert(ret != NULL && "i dunno what hppened");

	       size_t retsize = strlen(ret);
	       if (strcmp(ret, ".") == 0) {
		    input_mode = 0;
		    continue;
	       }
	       
	       
	       bufcursor += retsize;
	       tmpbuf[bufcursor++] = '\n';
	       
	       
	  }
     } while (1);
}
