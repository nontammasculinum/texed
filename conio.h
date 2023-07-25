#ifndef CONIOH
#define CONIOH


enum COLORS
{
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  BROWN = 6,
  LIGHTGRAY = 7,
  DARKGRAY = 8,
  LIGHTBLUE = 9,
  LIGHTGREEN = 10,
  LIGHTCYAN = 11,
  LIGHTRED = 12,
  LIGHTMAGENTA = 13,
  YELLOW = 14,
  WHITE = 15,
  BLINK = 128
};

enum CURSORTYPE
{
  _NOCURSOR,//     turns off the cursor
  _SOLIDCURSOR,//  solid block cursor
  _NORMALCURSOR // normal underscore cursor
};

struct text_info
{
  unsigned char attribute;      /* text attribute */
  unsigned char normattr;       /* normal attribute */
  int screenheight;   /* text screen's height */
  int screenwidth;    /* text screen's width */
  int curx;           /* x-coordinate in current window */
  int cury;           /* y-coordinate in current window */
};



int   c_getch(void);
int   c_getche(void);
int   c_kbhit(void);

#define getch()  c_getch()
#define getche() c_getche()
#define kbhit()  c_kbhit()

void  c_clrscr();
void  c_gotoxy(int x, int y);
void  c_setcursortype(int cur_t);
void  c_textbackground(int newcolor);
void  c_textcolor(int newcolor);
int   c_wherex(void);
int   c_wherey(void);
void  c_gettextinfo(struct text_info *r);
void  c_textattr(int newattr);

#define clrscr()                 c_clrscr()
#define gotoxy(x, y)             c_gotoxy(x, y)
#define setcursortype(cur_t)     c_setcursortype(cur_t)
#define textbackground(newcolor) c_textbackground(newcolor)
#define wherex()                 c_wherex()
#define wherey()                 c_wherey()
#define gettextinfo(r)           c_gettextinfo(r)
#define textattr(newattr)        c_textattr(newattr)
#endif
