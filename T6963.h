#ifndef __T6963_H__
#define __T6963_H__
#define FS 1
#define T6963_BASE_ADDRESS 0xe020    //define the main address of T6963

#define T6963_X_MAX 128
#define T6963_Y_MAX 64


extern char T6963_putchar(char);
extern int  T6963_puts(const char *);
extern int  T6963_printf(const char *,...);

extern void T6963_draw_point(char x,char y);
extern void T6963_erase_point(char x,char y);
extern void T6963_printc(unsigned char *Tab,unsigned char X,unsigned char Y,unsigned char Size);

extern void T6963_clear_text(void);
extern void T6963_clear_graph(void);
extern void T6963_lcd_clr(void);
extern void T6963_lcd_back(void);
extern void T6963_init_lcd(void);




#endif







