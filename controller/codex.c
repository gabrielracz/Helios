//	Gabriel Racz
//	11-24-2021
//	v1.2
//
// char sample[] = "And as he rose from the black of night his cold smile pierced like light";
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<unistd.h>

#define MAX_WORD_LEN 12
#define NUM_WORDS 1000

//Scoring
//	read numwords from file
// Testing git changes	

int GenerateSample(char* sample, int len);
char* GenerateGoodbye(void);
int FormattedPrint(WINDOW* win, char ch, int width);

enum COLORS {
	WHITE = 1,
	GREEN,
	RED,
	BLUE
};

int main(int argc, char** argv){

	FILE* scribe = fopen("./manuscript", "a");
	
	time_t time;
	srand((long) &time);
	long wrds = 10;

	if(argc >=  2){
		sscanf(argv[1], "%ld", &wrds);
	}
	if(wrds < 1 || argc < 2){ 
		wrds = rand() % 10 + 2;
	}
    
	WINDOW* win = initscr();
    noecho();		//disables displaying garbage characters 

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    // wbkgd(win, COLOR_PAIR(5));
    
	clear();
   	refresh(); 
	int w, h;
    getmaxyx(win, h, w);

	char* sample = malloc(sizeof(char) * wrds * MAX_WORD_LEN);
    GenerateSample(sample, wrds);
    int len = strlen(sample);

		
	//Maximum lines
	int ends[10];
	int n = 0;
	char rc;
   	for(int i = 0; i < len; i++){
		rc = FormattedPrint(win, sample[i], w - 11);
		if(rc > 0){
			ends[n] = rc;
			n++;
		}
    }
    refresh();

    int input_y = len / w + 1; 
    int index = 0;
    int wordcount = 0;
    char ch;

    struct timespec start, finish; 
    char* userinput = malloc(sizeof(char) * len);
	
	short perfectword = 1;

	int combo = 1;
	int topcombo = 1;
    int errcount = 0;

	short error = 0;
    short first = 1;
	int ix = 0;
	move(0, 0);
    while(index < len){
        ch = getch();

		int x, y;
		getyx(win, y, x);
        
        if(first){	//Start clock
            clock_gettime(CLOCK_MONOTONIC, &start);
            first = 0;
        }
	
        if(ch == 127){    //Backspace
            if(wordcount > 0){
                wordcount--;
                index--;
            }
            attron(COLOR_PAIR(WHITE));
			int srows;
			int scols;
			if(x == 0){
				ix--;
				srows = ix;
				scols = ends[ix];
				move(n, ends[ix]);
            	FormattedPrint(win, sample[index], 1000);
				move(srows, scols);
			}else{
				move(y, x-1);
            	FormattedPrint(win, sample[index], 1000);
				move(y, x-1);
			}
            continue;
        }
		else if(ch == sample[index]){	//Match
			if(ch == ' '){
				if(perfectword == 1){
					combo++;
					if(combo > topcombo)
						topcombo = combo;
				}
			}
			perfectword = 1;
			attron(COLOR_PAIR(GREEN));
            //mvaddch(index/w, index%w, ch);
        }
		else{								//Error
			errcount++;
            error = 1;
           	perfectword = 0;
			combo = 0;
            attron(COLOR_PAIR(RED));
			if(sample[index] == ' ')
                ch = '_';
            else
                ch = sample[index];

           // mvaddch(index/w, index%w, ch);
        }
		
		if(FormattedPrint(win, ch, w - 11)){
			ix++;
		}	

        wordcount++;
        index++;
        refresh();
    }

    endwin();
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double delta = (finish.tv_sec - start.tv_sec);
    delta += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Delta:       %.2fs\n", delta);
    printf("WPM:         %d\n", (int) round(wrds/delta*60));
	printf("Top:         %d\n", topcombo);
	if(!error){	
		printf("%s\n", GenerateGoodbye());
		fprintf(scribe, "%s\n", sample);
		if(rand()%6 == 0)
			fprintf(scribe, "\n");
		fclose(scribe);
	}
    return 0;
}

int FormattedPrint(WINDOW* win, char ch, int width){
	int cursorx;
	int cursory;
	int savex;
	getyx(win, cursory, cursorx);
	//Adding a test comment
	addch(ch);
	if(ch == ' ' && cursorx > width - 4){
		savex = cursorx;
		addch('\n');
		return savex;
	}
	return 0;
}

int GenerateSample(char* sample, int len){
    char words[NUM_WORDS][MAX_WORD_LEN];
    
    FILE* file = fopen("dictionary", "r");
    for(int i = 0; i < NUM_WORDS; i++){
        fscanf(file, "%[^\n] ", words[i]);
    }
    time_t t;
    srand((unsigned) time(&t));

    int ix;
    for(int j = 0; j < len; j++){
        char* randomstring = words[rand() % NUM_WORDS - 1];
        strcat(sample, randomstring);
        if(j != len - 1)
            strcat(sample, " ");
    }
}

char* GenerateGoodbye(){
	char* sayings[] = {
	"The scribe will make note of it.", 
	"It will be dealt with.",
	"The records will be updated.",
	};
	int l = sizeof(sayings)/ sizeof(char*);
	return sayings[rand() % l];

}
