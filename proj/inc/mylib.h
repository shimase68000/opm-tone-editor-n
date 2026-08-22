//
// mylib.h
//

extern void *my_memmove(void*,const void*,int);
extern char *my_strstr(const char*,const char*);
extern int   my_stricmp(const char*,const char*);
extern int   my_strncnt(const char*,const int,int);
extern int   my_isspace(char);
extern char  my_tolower(char);
extern void  my_strupr(char*);
extern int   my_str2ver(char*);
extern char *my_ver2str(int);
extern int   my_ver2int(int);
extern int   my_isdigit(int);
extern void  rtrim(char*);

