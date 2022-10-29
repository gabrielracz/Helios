#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

int main(void) {
	/*struct termios tty;*/
	/*struct termios old;*/
	/*tcgetattr(STDIN_FILENO, &old);*/
	/*tty = old;*/
	/*tty.c_lflag &= ~ICANON;*/
	/*tcsetattr(STDIN_FILENO, TCSANOW, &tty);*/

	WINDOW* win = initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    scrollok(stdscr, TRUE);

	char ch;
	while(1) {
		while(kbhit()) {
			addch(getch());
			refresh();
		}
	}

	return 0;
}
