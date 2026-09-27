#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ctype.h>

#include "./include/da/da.h"
#include "./include/base.h"

#define FNAMEBUF_SIZE 64
#define TMP_SIZE 128


struct buffer {
     da_member(char*);

     size_t current;
     char *filename;
     
} buf = {0};

struct command {
     int start, end; // some sentinel will denote if it is not a range

     enum {
	  CMD_ERR = 0,

	  CMD_I,
	  CMD_C,
	  CMD_A,

	  CMD_P,
	  CMD_N,
	  CMD_Q
     } kind;
     
} cmd = {0};

void insert(struct buffer *buf, size_t after, const char *line) {
     da_insert(buf, after+1, line);
}
void delete(struct buffer *buf, size_t n) {
     da_remove(buf, n);
}

struct command parse_cmd(char *cmd) {
     struct command ret = {0};

     while (isdigit(*cmd))
	  cmd++;

     switch(*cmd) {
     case 'a':
	  ret.kind = CMD_A;
	  break;
     case 'p':
	  ret.kind = CMD_P;
	  break;
     case 'q':
	  ret.kind = CMD_Q;
	  break;
     case 'n':
	  ret.kind = CMD_N;
	  break;
     }
     return ret;
}

enum {
     INPUT_MODE,
     COMMAND_MODE,
} state = COMMAND_MODE;



int main(int argc, char **argv) {
     const char *program_name = *argv++;
     const char *input_file = NULL;
     char line[TMP_SIZE] = {0};

     da_reserve(&buf, 1024);
     
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



     
     
     int len;
     do {
	  len = strlen(fgets(line, TMP_SIZE, stdin));
	  line[--len] = '\0';

	  if (state == INPUT_MODE) {
	       if (strcmp(line, ".") == 0) {
		    state = COMMAND_MODE;
		    continue;
	       }

	       da_push(&buf, line);
	       
	  } else if (state == COMMAND_MODE) {
	       if (!(strcmp(line, "c") || strcmp(line, "a") || strcmp(line, "i"))) {
		    state = INPUT_MODE;
		    continue;
	       } else {
		    // execute command
	       }
	  }
	  
     } while (1);
     
}
