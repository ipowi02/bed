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
     da_member(char*); // for the lines

     
     size_t current;
     char *filename;
     char *file; // buf_load creates a big heap allocated buffer of the file we dont want to lose the pointer to
} buf = {0};

enum cmd_indicator {
     SINGLE_LINE = -1,
     DOLLAR = -2,
     EMPTY = -3
};

struct command {
     union {
	  int start;
	  int line;
     };
     int end;
     // If the command is a single line, 'line' is used instead of start for it to be clearer, end is set to SINGLE_LINE
     // If any of the above arent present in the query, they are set to one of enum cmd_indicator's values to indicate


     enum {
	  CMD_ERR = 0,

	  CMD_INSERT = 'i',
	  CMD_CHANGE = 'c',
	  CMD_APPEND = 'a',

	  CMD_PRINT  = 'p',
	  CMD_NUMBER = 'n',
	  CMD_QUIT   = 'q',
     } kind;
     
} cmd = {0};


void buf_insert(struct buffer *buf, size_t after, const char *line) {
     da_insert(buf, after+1, line);
}
void buf_delete(struct buffer *buf, size_t n) {
     da_remove(buf, n);
}

char *buf_getline(struct buffer *buf, size_t n) {
     return buf->data[n];
     // not good (pointer invalidation)
}

char *buf_copyline(struct buffer *buf, size_t n) {
     return strdup(buf_getline(buf, n));
}

int buf_load(struct buffer *buf, const char *filename) {
     // Loads file from filename into *buf
     memset(buf, 0, sizeof *buf);
     
     buf->filename = strdup(filename);
     if (filename == NULL)
	  goto e0;
     
     FILE *f = fopen(filename, "rb");
     if (!f)
	  goto e1;
     
     size_t size;
     if (fileinfo(f, NULL, 0, &size) < 0)
	  goto e2;

     char *tmp = malloc(size ? size * sizeof(char) : 1);
     if (!tmp)
	  goto e2;

     if (fileinfo(f, tmp, size, NULL) < 0)
	  goto e3;

     size_t cursor = 0;
     
     for (size_t i = 0; i < size; i++) {
	  if (tmp[i] == '\n') {
	       tmp[i] = '\0';
	       da_push(buf, tmp + cursor);
	       cursor = i + 1;
	  }
     }

     if (cursor < size)
	  da_push(buf, tmp + cursor);
     
     buf->file = tmp;

     fclose(f);
     return 0;
e3: 
     free(tmp);	  
e2:
     fclose(f);
e1:
     free(buf->filename);
e0:
     return -1;
     
}

void buf_free(struct buffer *buf) {
     free(buf->filename);
     free(buf->file);
     da_free(buf);
}


struct command parse_cmd(char *cmd) {
     struct command ret = {0};

     char *endptr;
     const char *cmds = "apqn";
     
     char *comma_loc = strchr(cmd, ',');
     char *command_loc = strpbrk(cmd, cmds);

     assert(command_loc && "Invalid command");
     
     // We are making the assumption that addresses are single numbers
     if (comma_loc != NULL) {

	  int start = strtol(cmd, &endptr, 10);
	  assert(endptr == comma_loc && "Invalid address");
	  endptr = NULL; // necessary?
 
	  int end = strtol(comma_loc + 1, &endptr, 10);
	  assert(endptr == command_loc && "Invalid address");

	  ret.start = start;
	  ret.end = end;
     } else {
	  ret.end = SINGLE_LINE;

	  
	  int line = strtol(cmd, &endptr, 10);
	  assert(endptr == command_loc && "Invalid address?");

	  ret.line = line;
     }
     

     assert(strchr(cmds, *command_loc) != NULL && "Bad command");
     ret.kind = *command_loc;
     
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
     struct command cmd = {0};
     do {
	  len = strlen(fgets(line, TMP_SIZE, stdin));
	  line[--len] = '\0'; // skip '\n'

	  if (state == INPUT_MODE) {
	       if (strcmp(line, ".") == 0) {
		    state = COMMAND_MODE;
		    continue;
	       }
	       
	       da_push(&buf, line);
	       
	  } else if (state == COMMAND_MODE) {
	       cmd = parse_cmd(line);

	       switch (cmd.kind) {
	       case CMD_APPEND:
	       case CMD_INSERT:
	       case CMD_CHANGE:
		    
		    
		    
	       }
	  }
	  
     } while (1);
     
}
