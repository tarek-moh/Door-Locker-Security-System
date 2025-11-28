#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Keybad_set.h"

//Rows = PD0–PD3
//Cols = PE0–PE3

void keypad_init() {
    // Enable clocks
    SYSCTL_RCGCGPIO_R |= 0x18; // Port D + E

    // Rows (PD0-PD3) OUTPUT
    GPIOD_DIR_R |= 0x0F;
    GPIOD_DEN_R |= 0x0F;

    // Columns (PE0-PE3) INPUT
    GPIOE_DIR_R &= ~0x0F;
    GPIOE_DEN_R |= 0x0F;
    GPIOE_PUR_R |= 0x0F; // Pull-up
}

char keypad_getKey() {
    int row, col;
    char keys[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    while (1) {
        for (row = 0; row < 4; row++) {

            GPIOD_DATA_R = ~(1 << row);  // make one row LOW

            col = GPIOE_DATA_R & 0x0F;   // read columns

            if (col != 0x0F) {           // key pressed
                for (int c = 0; c < 4; c++) {
                    if (!(col & (1 << c))) {
                        return keys[row][c];
                    }
                }
            }
        }
    }
}
