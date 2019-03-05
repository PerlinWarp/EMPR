#include "stdio2.h"
#include "sys/types.h"
#include "opendir.h"

FILE *fopen(char *name, char *mode);

// Privare C function in its own file
char *number_range(char *buf,int a);
int file_flags(char *str);
void stat_flags_out(int x);

// Included for functions in libc/system
void fillbuf(unsigned char *buf,unsigned int size,unsigned char data);
void breakup_sub(char *line,char *entery_point,char c,char d);
int breakup(char *string,char *p[],char c);
void dump_breakup(int n, char *p[]);
int check_switch(char *str, char usr);
int check_cmd_line(char **str, int c, char usr, char f);
int file_flags(char *str);
void hexview(unsigned char *buffer, unsigned int size);
int htoi(char *str);
int fs_return_fs_type(char *str);
int unlink(STAT	*st);
int create(STAT	*st);
int mkdir(STAT *stat);
char *abspath(char *str,char *outstr,int len);
int setpath(char *str);
