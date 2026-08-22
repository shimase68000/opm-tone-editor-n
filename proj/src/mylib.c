//
// mylib.c
//

#include	<stdio.h>
#include    <string.h>
#include	<iocslib.h>
#include	<doslib.h>
#include    "oe.h"

void *my_memmove(void*,const void*,int);
char *my_strstr(const char*,const char*);
int   my_stricmp(const char*,const char*);
int   my_strncnt(const char*,const int,int);
int   my_isspace(char);
char  my_tolower(char);
void  my_strupr(char*);
int   my_str2ver(char*);
char *my_ver2str(int);
int   my_ver2int(int);
int   my_isdigit(int);
void  rtrim(char*);

//
// func my_isdigit
//
int my_isdigit(int c) {
	if(c >= '0' && c <= '9') return 1;
	return 0;
}

//
// is_scalekey_ver
//
static int is_scalekey_version(int v)
{
    int c0, c1, c2, c3;

    c0 = (v >> 24) & 0xff;
    c1 = (v >> 16) & 0xff;
    c2 = (v >>  8) & 0xff;
    c3 = (v >>  0) & 0xff;

    c1 |= 0x20;  // to lower

    if(!my_isdigit(c0)) return 0;
    if(!my_isdigit(c2)) return 0;
    if(!my_isdigit(c3)) return 0;

	if(c1=='.' || (c1>='a' && c1<='z')) return 1;

	return 0;
}

//
// ver to int
//
// '1.23' -> '123.'   2文字目の'.'はサフィックスが無いことを表す
// '1a23' -> '123a'
//
int my_ver2int(int v)
{
    unsigned int i;

	if(!is_scalekey_version(v)) return -1;

    i  =  (unsigned int)(v & 0xFF000000);
    i |= ((unsigned int)(v & 0x00FF0000) >> 16);
    i |= ((unsigned int)(v & 0x0000FFFF) << 8);

    return (int)i;
}

//
// ver to string
//
// '1.23' -> '1.23'    2文字目の'.'はサフィックスが無いことを表す
// '1a23' -> '1.23a'
//

char *my_ver2str(int v)
{
	static char s[8];

	if(!is_scalekey_version(v)) return NULL;

	s[0] = (v & 0xFF000000)>>24;
	s[1] = (v & 0x00FF0000)>>16;
	s[2] = (v & 0x0000FF00)>>8;
	s[3] = (v & 0x000000FF)>>0;
	s[4] = '\0';

	if( s[1] != '.' ) {
		s[4] = s[1];
		s[1] = '.';
		s[5] = '\0';
	}

	return s;
}

//
// string to ver
//
int my_str2ver(char *s)
{
	int i;
	int r;

	r = 0;
	for(i=0;i<4;i++) {   // version string is 4 chars
		r <<= 8;
		r |= (*s++ & 0xff);
	}
	return r;
}

//
// func my_strupr
//
void my_strupr(char *s)
{
    while (*s) {
        if ('a' <= *s && *s <= 'z')
            *s -= ('a'-'A');
        s++;
    }
}

//
// func my_tolower
//
char my_tolower(char c)
{
	return (c>='A'&&c<='Z') ? (c+('a'-'A')) : c;
}

//
// func my_isspace
//
int my_isspace(char c)
{
	return (c==' '||c=='\t'||c=='\r'||c=='\n'||c=='\v'||c=='\f');
}

//
// func my_memmove
//
void *my_memmove(void *dest, const void *src, int cnt)
{
    unsigned char       *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    void *ret = dest;

    if (cnt <= 0 || d == s) return ret;

    if (d < s) {
        while (cnt--) *d++ = *s++;
    } else {
        d += cnt;
        s += cnt;
        while (cnt--) *--d = *--s;
    }
    return ret;
}

//
// func my_strncnt
//
int my_strncnt(const char *str, const int c, int n)
{
	int cnt;
	int i;

	cnt = 0;
	for(i=0; i<n; i++) {
		if(*str++==c) cnt++;
	}
	return cnt;
}

//
// func my_strstr
//
char *my_strstr(const char *str1, const char *str2)
{
	const char *s1;
	const char *s2;

	if(!*str2) return (char *)str1;

	while(*str1 != '\0') {
		s1 = str1;
		s2 = str2;

		while(*s2 != '\0' && *s1 == *s2) {
			s1++;
			s2++;
		}
		if(*s2 == '\0') {
			return (char *)str1;
		}
		str1++;
	}
	return NULL;
}

//
// func my_stricmp
//
int my_stricmp(const char *s0, const char *s1)
{
	char ss0, ss1;
	int  status;

	do {
	    ss0 = my_tolower(*s0++);
	    ss1 = my_tolower(*s1++);
		status = ss0 - ss1;
	} while(!status && ss0 && ss1);

	return status;
}

//
// func rtrim
//     note: 文末から文頭に向かって空白を削除。空白は'\0'で埋める。
//
void rtrim(char *str)
{
	int  len = strlen(str);
	char *end = str+len-1;

	if(!len) return;

	while(end >= str && my_isspace(*end)) {
		*end = '\0';
		end--;
	}
}
