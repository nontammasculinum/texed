#include <stdio.h>
#include <windows.h>
#include "./conio.h"

static HANDLE hstdout;
static int    width, height,
		      x, y,
			  buff_sz,
			  split_buff_sz,
			  input_buffer_sz,
			  scroll;
static char   *buff,
			  **split_buff,
			  c, _c,
			  *input_buffer;

int getsize(int *width, int *height);
int readfile(char **buff, int *buff_sz, char *path);
int writefile(char *buff, int buff_sz, char *path);
int splitstring(char *str, const char delim_c, char ***ret, int *sz);
int squishstring(char **strs, int sz, const char delim, char **ret, int *rsz);
int insertstr(char **sub, int *len, char insert, int pos);
int sinsertstr(char **sub, int *len, char *insert, int pos);
int appendstr(char **sub, char x);
int sappendstr(char **sub, char *x);
int sreplacestr(char **sub, char *x, int pos);
int a_insert(char ***a, int asz, char *b, int pos);
//int wputc(char c, HANDLE hstdout);
int a_remove(char ***a, int asz, int pos);
int a_append(char ***a, int asz, char *b);
int removestr(char **a, int pos);
int conxy(int x, int y);
int _print(char *x);
int _print_mode(char *mode_name);
int help_menu();
int clearln(int _y);
int refresh();

int main(int argc, char *argv[]) {
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	getsize(&width, &height);
	if(argc < 2) {
		clrscr();
		conxy(0, 0);
		_print("\nusage_error: correct usage: { <this_file> <file to edit> }\n");
		exit(0);
	}

	input_buffer = malloc(width + 1);

	help_menu();
	conxy(0, 0);

	readfile(&buff, &buff_sz, argv[1]);
	splitstring(buff, '\n', &split_buff, &split_buff_sz);
	for(int y = 0; y < split_buff_sz; y++) {
		x = 0;
		conxy(x, y);
		_print(split_buff[y + scroll]);
	}
	x = 0, y = 0;
	conxy(x, y);
	while(1) {
		if(kbhit()) {
			c = getch();
			switch(c) {
			case 'i': if(--y < 0) {
						  y = 0;
						  scroll--;
						  if(scroll < 0) scroll++;
						  refresh();
					  }
					  if(x > strlen(split_buff[y + scroll])) x = strlen(split_buff[y]);
					  break;
			case 'k': if(++y > height - 4) {
						  conxy(0, 0);
						  ++scroll;
						  y = height - 4;
						  while(scroll + y > split_buff_sz - 1) scroll--;
						  printf("%s\n", split_buff[scroll + y]);
						  refresh();
					  }
					  if(y > split_buff_sz - 1) y = split_buff_sz - 1;
					  if(x > strlen(split_buff[y + scroll])) x = strlen(split_buff[y + scroll]);
					  break;
			case 'j': if(--x < 0) x = 0; break;
			case 'l': if(++x > strlen(split_buff[y + scroll])) x = strlen(split_buff[y + scroll]); break;
			case 'Q': goto exit;
			case 'A': conxy(0, height - 3);
					  _print("what_to_append");
					  conxy(0, height - 2);
					  fgets(input_buffer, width + 1, stdin);
					  input_buffer[strlen(input_buffer) - 1] = '\0';
					  input_buffer_sz = strlen(input_buffer);
					  sappendstr(&(split_buff[y + scroll]), input_buffer);
					  conxy(0, y);
					  printf("%s", split_buff[y + scroll]);
					  conxy(x, y);
					  break;
			case 'I': conxy(0, height - 3);
					  _print("what_to_insert");
					  conxy(0, height - 2);
					  fgets(input_buffer, width + 1, stdin);
					  input_buffer[strlen(input_buffer) - 1] = '\0';
					  clearln(height - 2);
					  clearln(height - 3);
					  input_buffer_sz = strlen(input_buffer);
					  int _slen = strlen(split_buff[y + scroll]);
					  sinsertstr(&(split_buff[y + scroll]), &_slen, input_buffer, x);
					  clearln(y);
					  conxy(0, y);
					  _print(split_buff[y + scroll]);
					  conxy(x, y);
					  break;
			case 'R': conxy(0, height - 3);
					  _print("replace_with_what");
					  conxy(0, height - 2);
					  fgets(input_buffer, width + 1, stdin);
					  input_buffer[strlen(input_buffer) - 1] = '\0';
					  clearln(height - 2);
					  clearln(height - 3);
					  input_buffer_sz = strlen(input_buffer);
					  sreplacestr(&(split_buff[y + scroll]), input_buffer, x);
					  clearln(y);
					  conxy(0, y);
					  _print(split_buff[y + scroll]);
					  conxy(x, y);
					  break;
			case 'h': help_menu(); break;
			case 'N': a_insert(&split_buff, split_buff_sz, strdup(""), y);
					  split_buff_sz++;
					  refresh();
					  break;
			case 'n': a_append(&split_buff, split_buff_sz, strdup(""));
					  split_buff_sz++; 
					  break;
			case 'D': if(split_buff_sz == 0) break;
					  a_remove(&split_buff, split_buff_sz, y);
					  split_buff_sz--;
					  refresh();
					  clearln(split_buff_sz);
					  break;
			case 'd': if(strlen(split_buff[y + scroll]) == 0) break;
					  removestr(&(split_buff[y + scroll]), x);
					  clearln(y);
					  conxy(0, y);
					  _print(split_buff[y + scroll]);
					  break;
			case 'S': squishstring(split_buff, split_buff_sz, '\n', &buff, &buff_sz);
					  writefile(buff, buff_sz, argv[1]);
					  exit(0);
					  break;
			}
			conxy(x, y);
		}
	}
exit:
	clrscr();
	return 0;
}

int refresh() {
	clearln(height - 3);
	clearln(height - 2);
	for(int _y = 0; _y < height - 4; _y++) {
        clearln(_y);
		x = 0;
        conxy(x, _y);
        _print(split_buff[_y + scroll]);
    }
}

int clearln(int _y) {
	for(int i = 0; i < width - 1; i++) {
		conxy(i, _y);
		putchar(' ');
	}
}

int help_menu() {
	clrscr();
	conxy(0, 0);
	_print("\
i=========HELP=========i\n\
|i: move cursor up     |\n\
|j: move cursor left   |\n\
|k: move cursor down   |\n\
|l: move cursor right  |\n\
|I: insert             |\n\
|A: append             |\n\
|R: replace            |\n\
|N: insert new line    |\n\
|n: insert new line at |\n\
|   end of file        |\n\
|Q: quit the program   |\n\
|h: this menu :p       |\n\
|D: delete the current |\n\
|   line               |\n\
|d: delete the current |\n\
|   character          |\n\
|S: save file and exit |\n\
j(any key to continue)=j\n");
	getch();
	clrscr();
	return 0;
}

int _print(char *x) {
	for(char *y = x; *y; y++)
		putchar(*y);
}

int _print_mode(char *mode_name) {
	conxy(0, height - 2);
	_print(mode_name);
}

int insertstr(char **sub, int *len, char insert, int pos) {
	*sub = realloc(*sub, *len + 1);
	memmove((*sub)+pos+1, (*sub)+pos, (*len)-pos);
	(*sub)[pos] = insert;
}

int a_insert(char ***a, int asz, char *b, int pos) {
	*a = realloc(*a, (asz + 1) * sizeof(char**));
	memmove((*a)+(pos+1), (*a)+pos, (asz-pos)*sizeof(char**));
	(*a)[pos] = b;
}
int a_append(char ***a, int asz, char *b) {
	(*a) = realloc(*a, (asz + 1) * sizeof(char**));
	(*a)[asz] = b;
}
int a_remove(char ***a, int asz, int pos) {
	memmove((*a)+pos, (*a)+pos+1, (asz-pos-1)*sizeof(char**));
	*a = realloc(*a, (asz - 1) * sizeof(char**));
}

int removestr(char **a, int pos) {
	memmove((*a)+x, (*a)+pos+1, (strlen(*a)-pos-1));
	*a = realloc(*a, strlen(*a));
	(*a)[strlen(*a) - 1] = '\0';
}

int sinsertstr(char **sub, int *len, char *insert, int pos) {
	*sub = realloc(*sub, *len + strlen(insert));
	char *temp = (char*)(*sub + pos), *temp_2 = strdup(temp);
	*temp = '\0';
	strcat(*sub, insert);
	strcat(*sub, temp_2);
	free(temp_2);
}

int appendstr(char **sub, char x) {
	*sub = realloc(*sub, strlen(*sub) + 1);
	(*sub)[strlen(*sub) - 1] = x;
	(*sub)[strlen(*sub)] = '\0';
}

int sappendstr(char **sub, char *x) {
	*sub = realloc(*sub, strlen(*sub) + strlen(x));
	strcat(*sub, x);
	(*sub)[strlen(*sub)] = '\0';
}

int sreplacestr(char **sub, char *x, int pos) {
	int i;
	for(i = pos; i < strlen(x) + pos; i++) {
		if(i > strlen(*sub)) break;
		(*sub)[i] = x[i - pos];
	}
	(*sub)[i] = '\0';
}

int squishstring(char **strs, int sz, const char delim, char **ret, int *rsz) {
	*ret = calloc(1,1); *rsz = 1;
	for(int i = 0; i < sz; i++) {
		*ret = realloc(*ret, *rsz + strlen(strs[i]) + 1);
		*rsz += strlen(strs[i]) + 1;
		strcat(*ret, strs[i]);
		(*ret)[strlen(*ret)] = delim;
	}
	printf("\n%s\n", *ret);
	return 0;
}

int splitstring(char *str, const char delim_c, char ***ret, int *sz) {
	*ret = malloc(sizeof(char*)); *sz = 0;
	for(char *c = str, *l = str; *c; c++) {
		if(*c == delim_c) {
			*c = '\0';
			*ret = realloc(*ret, sizeof(char*) * (++(*sz)));
			(*ret)[*sz - 1] = strdup(l);
			l = (c + 1);
			*c = delim_c;
		}
	}
}

int readfile(char **buff, int *buff_sz, char *path) {
	FILE *fp = fopen(path, "r");
	*buff = malloc(*buff_sz = (fseek(fp, 0L, SEEK_END), ftell(fp)));
	rewind(fp);
	fread(*buff, 1, *buff_sz, fp);
	fclose(fp);
}

int writefile(char *_buff, int _buff_sz, char *path) {
	printf("\n%s\n", _buff);
	FILE *fp = fopen(path, "w");
	fwrite(_buff, 1, _buff_sz, fp);
	fclose(fp);
}

int getsize(int *_width, int *_height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return 0;
}

int conxy(int x, int y) {
	COORD coord = {x, y};
	SetConsoleCursorPosition(hstdout, coord);
}
