#include <stdio.h>
#include <termios.h>
#include <unistd.h>


#define NUM_STATES 50
#define MAX_LENGTH 80 // Max length of a state name, including null terminator
#define B_BLUE 44
#define T_WHITE 97
#define B_CYAN 46
#define T_YELLOW 93


#define DISPLAYLIMIT 7

unsigned int displayLimit = DISPLAYLIMIT;
/********************************************************************/
/* LINUX MINI - CONIO/CRT UNIT */
// Clears the screen

void clrscr() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

// Moves the cursor to the specified position (1-based index)
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
}


void outputcolor(int foreground, int background) {
  printf("%c[%d;%dm", 0x1b, foreground, background);
    fflush(stdout);
}

// Function to reset text color and background to defaults
void resetcolor() {
    printf("\033[0m");
    fflush(stdout);
}

// Reads a single character, including arrow keys
char readchar() {
    struct termios oldt, newt;
    char ch, seq[3];

    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read the first character
    read(STDIN_FILENO, &ch, 1);

    if (ch == '\033') { // Escape sequence
        // Read the next two characters
        if (read(STDIN_FILENO, &seq[0], 1) == 0 || read(STDIN_FILENO, &seq[1], 1) == 0) {
            ch = '\033'; // Treat as ESC if sequence is incomplete
        } else if (seq[0] == '[') {
            // Interpret arrow keys
            switch (seq[1]) {
                case 'A': ch = -1; break; // Up arrow
                case 'B': ch = -2; break; // Down arrow
                case 'C': ch = -3; break; // Right arrow
                case 'D': ch = -4; break; // Left arrow
                default: ch = '\033'; // Unknown sequence
            }
        } else {
            ch = '\033'; // Unknown sequence
        }
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
/********************************************************************/

void scrollDemo2(unsigned int X, unsigned int Y, char states[NUM_STATES][MAX_LENGTH], unsigned int displayLimit, unsigned int *startFrom, unsigned int *currentIndex, unsigned int padding){
	unsigned int dummyI = 0;
	size_t i=0;
	for (i = *startFrom; i < *startFrom+displayLimit && i<NUM_STATES; i++) {
	    outputcolor(T_WHITE,B_BLUE);
	    gotoxy(X,Y+dummyI);
	    dummyI++;
	    if (i == *currentIndex) {outputcolor(T_YELLOW,B_CYAN);}
            printf("%-*s", padding, states[i]);
            outputcolor(T_WHITE,B_BLUE);
	}

}

int main() {
   char ch=0;
   unsigned int currentIndex=0;
   unsigned int lastDisplayIndex = MAX_LENGTH - displayLimit;
   unsigned int startFrom = 0;
  char states[NUM_STATES][MAX_LENGTH] = {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut",
        "Delaware", "Florida", "Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa",
        "Kansas", "Kentucky", "Louisiana", "Maine", "Maryland", "Massachusetts", "Michigan",
        "Minnesota", "Mississippi", "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire",
        "New Jersey", "New Mexico", "New York", "North Carolina", "North Dakota", "Ohio",
        "Oklahoma", "Oregon", "Pennsylvania", "Rhode Island", "South Carolina", "South Dakota",
        "Tennessee", "Texas", "Utah", "Vermont", "Virginia", "Washington", "West Virginia",
        "Wisconsin", "Wyoming"
    };


   outputcolor(T_WHITE,B_BLUE);
   clrscr(); // Clear the screen with current background color

    //Main program loop (q:exits the program)
    do {
        gotoxy(3, 2); // Move cursor to the top-left corner
	printf("Continuous scroll demo. Press 'q' to exit.");
        gotoxy(3, 3); // Move cursor to the top-left corner
	printf("==========================================");
        //Print the states

	scrollDemo2(10,5,states, displayLimit, &startFrom, &currentIndex, 15);

	fflush(stdout);
        //Program pauses to read a char from keyboard
        ch = readchar(); // Read a single character
        // Exit if the ESC key (ASCII 27) is pressed
        if (ch == -1) {
            if (currentIndex>0) {
                    if ((currentIndex==startFrom) && (startFrom >0)) {startFrom--;}
                    currentIndex--;
            }
        }
	if (ch == -2){
	    if (currentIndex<NUM_STATES-1) {
                if (currentIndex==startFrom+displayLimit-1 && startFrom < lastDisplayIndex) {startFrom++;}
                currentIndex++;
	    }
	}
	if (ch == 13 || ch == 10) {
	    gotoxy(1,18);
	    outputcolor(T_YELLOW, B_BLUE);
            printf("You have selected : %-25s" , states[currentIndex]);
            outputcolor(T_WHITE,B_BLUE);
	}
	gotoxy(1,16);
    printf("Current Index[%d]|displayLimit [%d]|MODULO[%d]|DIVISON[%d]|UPDATED INDEX[%d]",currentIndex, displayLimit, currentIndex%displayLimit,currentIndex/displayLimit,(currentIndex/displayLimit)*displayLimit);
    } while (ch != 'q'); // Continue until q is pressed
    resetcolor();
    clrscr(); // Clear the screen before exiting

    return 0;
}

