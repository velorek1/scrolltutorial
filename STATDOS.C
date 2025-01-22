#include <stdio.h>
#include <dos.h>

#define NUM_STATES 50
#define MAX_LENGTH 80 // Max length of a state name, including null terminator

#define B_BLUE 1
#define T_WHITE 15
#define B_CYAN 3
#define T_YELLOW 14

// Clears the screen using direct video memory writes
void clrscr() {
    asm {
	mov ax, 0B800h   // Video memory segment
	mov es, ax       // Set ES to point to video memory
	xor di, di       // Start at offset 0
	mov ax, 1f20h    // Space character (20h) with attribute 07h
	mov cx, 2000     // 80x25 = 2000 character positions
	cld              // Clear direction flag (forward)
	rep stosw        // Fill screen with space characters

	// Reset cursor position
	mov ah, 02h
	xor bh, bh
	xor dx, dx
        int 10h
    }
}

void gotoxy(int x, int y) {
    asm {
        mov ah, 02h
        xor bh, bh
        dec word ptr [bp+4]  // Adjust for 0-based coordinates
        dec word ptr [bp+6]
        mov dl, byte ptr [bp+4]
        mov dh, byte ptr [bp+6]
        int 10h
    }
}

// Fixed readchar function to properly handle extended keys
char readchar() {
    char scan;
    asm {
	mov ah, 00h     // BIOS keyboard read
	int 16h         // Call interrupt
	mov scan, ah    // Save scan code
    }

    // Check if it's an extended key
    if (scan == 72) return -1;  // Up arrow
    if (scan == 80) return -2;  // Down arrow
    if (scan == 77) return -3;  // Right arrow
    if (scan == 75) return -4;  // Left arrow
    return _AL;  // Return ASCII character
}

// Fixed color output function that properly sets text attributes
void outputcolor(int fg, int bg) {
    unsigned char attr = (bg << 4) | (fg & 0x0F);
    asm {
	mov ah, 09h      // Set current video attribute
	mov bh, 0        // Page 0
	mov bl, attr     // New attribute
	int 10h

	// Store attribute for future writes
	mov ah, 0Eh      // Get current video page
	int 10h
	mov ah, 08h      // Read attribute/char
	int 10h
	and bh, 0F0h     // Keep background
	or bh, bl        // Combine with new attribute
    }
}

void writestr(const char* str, char attr) {
    char here='a';
    int i=0;
    here = str[i];
    while (i<strlen(str)) {
	asm {
	    push ds          // Save DS
	    pop es           // ES = DS (assumes DS points to data segment)

	    // Load character and attribute to display
	    mov al, here    // Load current character
	    mov ah, 09h      // Write character and attribute
	    mov bh, 0        // Page number
	    mov bl, attr     // Attribute
	    mov cx, 1        // Number of characters
	    int 10h          // BIOS video interrupt

	    // Move cursor forward
	    mov ah, 03h      // Get cursor position
	    mov bh, 0        // Page number
	    int 10h
	    inc dl           // Increment column
	    mov ah, 02h      // Set cursor position
	    int 10h
	}
	i++; // Move to the next character
	here = str[i];
    }
}
void resetcolor() {
    outputcolor(T_WHITE, 0);
}

void scrollDemo(unsigned int X, unsigned int Y, char states[NUM_STATES][MAX_LENGTH],
	       unsigned int displayLimit, unsigned int *currentIndex, unsigned int padding) {
    unsigned int dummyI = 0;
    unsigned int displayPage = (*currentIndex / displayLimit) * displayLimit;
    size_t i = 0, k = 0;
    char blank[MAX_LENGTH] = " ";
    char formatted[MAX_LENGTH];

    for (i = displayPage; i < displayPage + displayLimit && i < NUM_STATES; i++) {
	gotoxy(X, Y + dummyI);
	if (i == *currentIndex) {
	    sprintf(formatted, "%-*s", padding, states[i]);
	    writestr(formatted, (B_CYAN << 4) | T_YELLOW);
	} else {
	    sprintf(formatted, "%-*s", padding, states[i]);
	    writestr(formatted, (B_BLUE << 4) | T_WHITE);
	}
	dummyI++;
    }

    if (dummyI < displayLimit) {
	for (k = dummyI; k < displayLimit; k++) {
	    gotoxy(X, Y + k);
	    sprintf(formatted, "%-*s", padding, blank);
	    writestr(formatted, (B_BLUE << 4) | T_WHITE);
	}
    }
}

int main() {
    char ch = 0;
    unsigned int currentIndex = 0;
    unsigned int displayLimit = 7;
    char formatted[MAX_LENGTH];

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
    clrscr();

    do {
	gotoxy(3, 2);
	writestr("Scroll by page demo. Press 'q' to exit.", (B_BLUE << 4) | T_WHITE);
	gotoxy(3, 3);
	writestr("=======================================", (B_BLUE << 4) | T_WHITE);

	scrollDemo(10, 5, states, displayLimit, &currentIndex, 15);

	ch = readchar();
	if (ch == -1 && currentIndex > 0) currentIndex--;
	if (ch == -2 && currentIndex < NUM_STATES - 1) currentIndex++;
	if (ch == 13) { // Changed to 13 for Enter key
	    gotoxy(1, 18);
	    sprintf(formatted, "You have selected: %-25s", states[currentIndex]);
	    writestr(formatted, (B_BLUE << 4) | T_YELLOW);
	}

	gotoxy(1, 16);
	sprintf(formatted, "Current Index[%d] | Display Limit[%d] | Page Start[%d]",
		currentIndex, displayLimit, currentIndex / displayLimit * displayLimit);
	writestr(formatted, (B_BLUE << 4) | T_WHITE);

    } while (ch != 'q');

    resetcolor();
    clrscr();
    return 0;
}
