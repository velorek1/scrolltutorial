#include <stdio.h>
#include <windows.h>

#define NUM_STATES 50
#define MAX_LENGTH 80 // Max length of a state name, including null terminator

#define B_BLUE 1
#define T_WHITE 15
#define B_CYAN 11
#define T_YELLOW 14
#define DISPLAYLIMIT 7

/********************************************************************/
/* WINDOWS MINI - CONIO/CRT UNIT */
// Clears the screen
// Clears the screen
int displayLimit  = DISPLAYLIMIT;
void clrscr() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;

    // Get the console screen buffer info
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the console with spaces
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, coord, &written);

    // Reset console text attributes
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, coord, &written);

    // Move the cursor to the top-left corner
    SetConsoleCursorPosition(hConsole, coord);
}

// Moves the cursor to the specified position (1-based index)
void gotoxy(int x, int y) {
    COORD coord = {(SHORT)(x - 1), (SHORT)(y - 1)};
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, coord);
}

// Reads a single character, including arrow keys
char readchar() {
    DWORD mode, cc;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    if (h == NULL) return 0; // Failed to get handle

    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    INPUT_RECORD ir;
    char ch = 0;
    while (1) {
        ReadConsoleInput(h, &ir, 1, &cc);
        if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            switch (ir.Event.KeyEvent.wVirtualKeyCode) {
                case VK_UP:    return -1; // Up arrow
                case VK_DOWN:  return -2; // Down arrow
                case VK_RIGHT: return -3; // Right arrow
                case VK_LEFT:  return -4; // Left arrow
                case VK_ESCAPE: return 27; // ESC key
                default: return ir.Event.KeyEvent.uChar.AsciiChar;
            }
        }
    }
    SetConsoleMode(h, mode); // Restore original mode
    return ch;
}

// Sets the text and background color
void outputcolor(int foreground, int background) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Combine foreground and background colors
    int colorAttribute = (background << 4) | (foreground & 0x0F);

    SetConsoleTextAttribute(hConsole, colorAttribute);
}

// Resets text color and background to defaults
void resetcolor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7); // Default: light gray on black
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
	printf("Scroll by page demo. Press 'q' to exit.");
        gotoxy(3, 3); // Move cursor to the top-left corner
	printf("=======================================");
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

