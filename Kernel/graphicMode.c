#include <graphicMode.h>
#include <fonts.h>

// Retrieved from https://wiki.osdev.org/VESA_Video_Modes
struct vbe_mode_info_structure {
    uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;     // deprecated
    uint8_t window_b;     // deprecated
    uint16_t granularity; // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;        // number of bytes per horizontal line
    uint16_t width;        // width in pixels
    uint16_t height;       // height in pixels
    uint8_t w_char;        // unused...
    uint8_t y_char;        // ...
    uint8_t planes;
    uint8_t bpp;   // bits per pixel in this mode
    uint8_t banks; // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__((packed));

// cw = current window
window cw[4];

const TColor RED = {0xFF, 0x00, 0x00};
const TColor WHITE = {0xFF, 0xFF, 0xFF};
const TColor BLACK = {0x00, 0x00, 0x00};

static uint8_t cw_id = 0; // current screen

static char buffer[64] = {'0'};
static const struct vbe_mode_info_structure* graphicModeInfo = (struct vbe_mode_info_structure*)0x5C00;

static void getNextPosition();
static void checkSpace();
static void scrollUp();

static void* getPixelAddress(int i, int j) {
    return (void*)((uint64_t)graphicModeInfo->framebuffer + 3 * (graphicModeInfo->width * i + j));
}

static void drawPixel(int i, int j, const TColor* color) {
    uint8_t* pixel = getPixelAddress(i, j);
    pixel[0] = color->B;
    pixel[1] = color->G;
    pixel[2] = color->R;
}

// Default window
void initUniqueWindow() {
    cw_id = 0;
    cw[0].current_i = 0;
    cw[0].current_j = 0;
    cw[0].start_i = 0;
    cw[0].start_j = 0;
    cw[0].width = graphicModeInfo->width / CHAR_WIDTH;
    cw[0].height = graphicModeInfo->height / CHAR_HEIGHT;
    clearAll();
}

// Multiple window
void initDividedWindow() {
    cw_id = 0;
    clearAll();
    for (int i = 0; i < 4; ++i) {
        cw[i].current_i = 0;
        cw[i].current_j = 0;
        cw[i].width = (graphicModeInfo->width / (CHAR_WIDTH * 2)) - 1;
        cw[i].height = (graphicModeInfo->height / (CHAR_HEIGHT * 2)) - 1;
    }
    cw[0].start_i = 0;
    cw[0].start_j = 0;
    cw[1].start_i = 0;
    cw[1].start_j = graphicModeInfo->width / (CHAR_WIDTH * 2) + 1;
    cw[2].start_i = graphicModeInfo->height / (CHAR_HEIGHT * 2) + 1;
    cw[2].start_j = 0;
    cw[3].start_i = graphicModeInfo->height / (CHAR_HEIGHT * 2) + 1;
    cw[3].start_j = graphicModeInfo->width / (CHAR_WIDTH * 2) + 1;

    int center = graphicModeInfo->height / 2;
    for (int i = 0; i < graphicModeInfo->width; ++i) {
        drawPixel(center, i, &WHITE);
        drawPixel(center + 1, i, &WHITE);
    }

    center = graphicModeInfo->width / 2;
    for (int i = 0; i < graphicModeInfo->height; ++i) {
        drawPixel(i, center, &WHITE);
        drawPixel(i, center + 1, &WHITE);
    }
}

void printCharFormat(char c, const TColor* charColor, const TColor* bgColor) {

    // Backspace
    if (c == '\b') {
        if (cw[cw_id].current_j == 0) {
            cw[cw_id].current_i -= 1;
            cw[cw_id].current_j = cw[cw_id].width - 1;
            printCharFormat(' ', charColor, bgColor);
            cw[cw_id].current_i -= 1;
            cw[cw_id].current_j = cw[cw_id].width - 1;
        } else {
            cw[cw_id].current_j = (cw[cw_id].current_j - 1) % cw[cw_id].width;
            printCharFormat(' ', charColor, bgColor);
            cw[cw_id].current_j = (cw[cw_id].current_j - 1) % cw[cw_id].width;
        }
        return;
    }

    checkSpace();

    // Newline
    if (c == '\n') {
        newLine();
        return;
    }

    uint8_t* character = getCharMapping(c);
    // Upper left pixel of the current character
    uint16_t write_i = (cw[cw_id].start_i + cw[cw_id].current_i) * CHAR_HEIGHT;
    uint16_t write_j = (cw[cw_id].start_j + cw[cw_id].current_j) * CHAR_WIDTH;

    uint8_t mask;

    for (int i = 0; i < CHAR_HEIGHT; ++i) {
        for (int j = 0; j < CHAR_WIDTH; ++j) {
            mask = 1 << (CHAR_WIDTH - j - 1);
            if (character[i] & mask) {
                drawPixel(write_i + i, write_j + j, charColor);
            } else {
                drawPixel(write_i + i, write_j + j, bgColor);
            }
        }
    }
    getNextPosition();
}

static void getNextPosition() {
    cw[cw_id].current_i += ((cw[cw_id].current_j + 1) == cw[cw_id].width) ? 1 : 0;
    cw[cw_id].current_j = (cw[cw_id].current_j + 1) % cw[cw_id].width;
}

static void checkSpace() {
    if (cw[cw_id].current_i == cw[cw_id].height) {
        scrollUp();
    }
}

static void scrollUp() {
    for (int i = 1; i < cw[cw_id].height * CHAR_HEIGHT; ++i) {

        uint8_t* start = getPixelAddress(cw[cw_id].start_i + i, cw[cw_id].start_j);
        uint8_t* next = getPixelAddress(cw[cw_id].start_i + CHAR_HEIGHT + i, cw[cw_id].start_j);

        for (int j = 0; j < cw[cw_id].width * CHAR_WIDTH * 3; ++j) {
            start[j] = next[j];
        }
    }
    cw[cw_id].current_i -= 1;
}

void printChar(char c) {
    printCharFormat(c, &WHITE, &BLACK);
}

void setScreen(uint8_t screen_id) {
    cw_id = screen_id;
}

void print(const char* string) {
    for (int i = 0; string[i] != 0; ++i) {
        printChar(string[i]);
    }
}

void newLine() {
    cw[cw_id].current_j = 0;
    cw[cw_id].current_i += 1;
}

void restartCursor() {
    cw[cw_id].current_i = 0;
    cw[cw_id].current_j = 0;
}

void clearAll() {
    cw[cw_id].current_i = 0;
    cw[cw_id].current_j = 0;
    for (int i = 0; i < cw[cw_id].height; ++i) {
        for (int j = 0; j < cw[cw_id].width; ++j) {
            printCharFormat(' ', &WHITE, &BLACK);
        }
    }
    cw[cw_id].current_i = 0;
    cw[cw_id].current_j = 0;
}

void printDec(uint64_t value) {
    printBase(value, 10);
}

void printHex(uint64_t value) {
    printBase(value, 16);
}

void printBin(uint64_t value) {
    printBase(value, 2);
}

void printBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    print(buffer);
}

uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base) {
    char* p = buffer;
    char* p1;
    char* p2;
    uint32_t digits = 0;

    // Calculate characters for each digit
    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    // Terminate string in buffer.
    *p = 0;
    // Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    return digits;
}

// Function to print in register format
void printRegisterFormat(uint64_t reg) {

    uint64_t aux = reg;
    uint64_t count = 16;

    while (aux) {
        aux = aux >> 4;
        --count;
    }

    for (int i = 0; i < count; i++) {
        printChar('0');
    }

    if (reg) {
        printHex(reg);
    }
}