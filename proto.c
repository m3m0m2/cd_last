#include <stdio.h>
#include <stdlib.h>
#include <termcap.h>
#include <termios.h>
#include <sys/ioctl.h>


#define BUF_SIZE 2048

struct term_t
{
  int init;
  char* name;
  char* buffer;
  char term_buffer[BUF_SIZE];
  char term_string_buffer[BUF_SIZE];
  int lines,cols;
  const char* bc, // backspace
            * cl, // clear screen
            * cm, // cursor move
            * so, // start standout
            * se; // end standout
  struct termios cur_termios, orig_termios;
};

void set_raw_tty(struct term_t* term)
{
  ioctl(0, TCGETS, &term->orig_termios);
  term->cur_termios=term->orig_termios;
  term->cur_termios.c_lflag &= ~ICANON;
  term->cur_termios.c_cc[VMIN] = 1;
  term->cur_termios.c_cc[VTIME] = 0;
  ioctl(0, TCSETS, &term->cur_termios);
}

void restore_orig_tty(struct term_t* term)
{
  ioctl(0, TCSETS, &term->orig_termios);
}

void init_term(struct term_t* term)
{
  term->init = 0;
  term->buffer = term->term_string_buffer;
  term->name = getenv("TERM");
  if(!term->name)
    term->name = "dumb";

  if (tgetent (term->term_buffer, term->name) < 0)
    {
      term->lines = 24;
      term->cols = 80;
      return;
    }

  term->bc = tgetstr("bc", &term->buffer);

  term->lines = tgetnum("li");
  term->cols = tgetnum("co");

  term->cl = tgetstr("cl", &term->buffer);
  term->cm = tgetstr("cm", &term->buffer);
  term->so = tgetstr("so", &term->buffer);
  term->se = tgetstr("se", &term->buffer);
  term->init = 1;
}

int read_loop(struct term_t* term)
{
  fputs(term->cl, stdout);

  printf("win %d x %d\n", term->lines, term->cols);
  fputs(tgoto(term->cm, 20, 10), stdout);
  printf("Hello, ");
  fputs(term->so, stdout);
  printf("world");
  fputs(term->se, stdout);
  putchar('!');

}

int main()
{
  struct term_t term;
  init_term(&term);

  read_loop(&term);

  return 0;
}
