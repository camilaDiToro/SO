/* Standard library */
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Local headers */
#include <graphics.h>
#include <fonts.h>
#include <lib.h>
#include <process.h>

// Retrieved from https://wiki.osdev.org/VESA_Video_Modes
struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
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

    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed));

const TColor RED = {0xFF, 0x00, 0x00};
const TColor ORANGE = {0xFF, 0x66, 0x18};
const TColor GREEN = {0x00, 0xFF, 0x00};
const TColor BLUE = {0x00, 0x00, 0xFF};
const TColor WHITE = {0xFF, 0xFF, 0xFF};
const TColor GRAY = {0xAA, 0xAA, 0xAA};
const TColor BLACK = {0x00, 0x00, 0x00};

static uint8_t current_i, current_j;
static uint8_t width, height;

static char buffer[64] = {'0'};
static const struct vbe_mode_info_structure* graphicModeInfo = (struct vbe_mode_info_structure*)0x5C00;

static void getNextPosition();
static void checkSpace();
static void scrollUp();

static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count);
static int fdDupHandler(TPid pidFrom, TPid pidTo, int fdFrom, int fdTo, void* resource);

static void* getPixelAddress(int i, int j) {
    return (void*)((size_t)graphicModeInfo->framebuffer + 3 * (graphicModeInfo->width * i + j));
}

static void drawPixel(int i, int j, const TColor* color) {
    uint8_t* pixel = getPixelAddress(i, j);
    pixel[0] = color->B;
    pixel[1] = color->G;
    pixel[2] = color->R;
}

// Default screen
void scr_init() {
    current_i = 0;
    current_j = 0;
    width = graphicModeInfo->width / CHAR_WIDTH;
    height = graphicModeInfo->height / CHAR_HEIGHT;
    scr_clear();
}

void scr_printCharFormat(char c, const TColor* charColor, const TColor* bgColor) {

    // Backspace
    if (c == '\b') {
        if (current_j == 0) {
            current_i -= 1;
            current_j = width - 1;
            scr_printCharFormat(' ', charColor, bgColor);
            current_i -= 1;
            current_j = width - 1;
        } else {
            current_j = (current_j - 1) % width;
            scr_printCharFormat(' ', charColor, bgColor);
            current_j = (current_j - 1) % width;
        }
        return;
    }

    checkSpace();

    // scr_printLine
    if (c == '\n') {
        scr_printLine();
        return;
    }

    uint8_t* character = getCharMapping(c);

    // Upper left pixel of the current character
    uint16_t write_i = (current_i)*CHAR_HEIGHT;
    uint16_t write_j = (current_j)*CHAR_WIDTH;

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

void scr_printChar(char c) {
    scr_printCharFormat(c, &WHITE, &BLACK);
}

void scr_print(const char* string) {
    for (int i = 0; string[i] != 0; ++i) {
        scr_printChar(string[i]);
    }
}

void scr_printLine() {
    current_j = 0;
    current_i += 1;
}

void scr_restartCursor() {
    current_i = 0;
    current_j = 0;
}

void scr_clear() {
    void* p = (void*)(size_t)graphicModeInfo->framebuffer;
    memset(p, 0, (size_t)graphicModeInfo->width * graphicModeInfo->height * 3);
    current_i = 0;
    current_j = 0;
}

void scr_printDec(uint64_t value) {
    scr_printBase(value, 10);
}

void scr_printHex(uint64_t value) {
    scr_printBase(value, 16);
}

void scr_printBin(uint64_t value) {
    scr_printBase(value, 2);
}

void scr_printBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    scr_print(buffer);
}

// Function to print in register format
void scr_printRegisterFormat(uint64_t reg) {

    uint64_t aux = reg;
    uint64_t count = 16;

    while (aux) {
        aux = aux >> 4;
        --count;
    }

    for (int i = 0; i < count; i++) {
        scr_printChar('0');
    }

    if (reg) {
        scr_printHex(reg);
    }
}

static void getNextPosition() {
    current_i += ((current_j + 1) == width) ? 1 : 0;
    current_j = (current_j + 1) % width;
}

static void checkSpace() {
    if (current_i == height) {
        scrollUp();
    }
}

static void scrollUp() {
    for (int i = 1; i < height * CHAR_HEIGHT; ++i) {

        uint8_t* start = getPixelAddress(i, 0);
        uint8_t* next = getPixelAddress(CHAR_HEIGHT + i, 0);

        for (int j = 0; j < width * CHAR_WIDTH * 3; ++j) {
            start[j] = next[j];
        }
    }
    current_i -= 1;
}

int scr_mapToProcessFd(TPid pid, int fd, const TColor* color) {
    // Use the color as resource. Convert it to an uint64_t and put the last bit as 1
    // so the color black doesn't get seen as NULL.
    uint64_t col = color->R | (color->G << 8) | (color->B << 16) | (1 << sizeof(TColor));
    return prc_mapFd(pid, fd, (void*)col, NULL, &fdWriteHandler, NULL, &fdDupHandler);
}

static ssize_t fdWriteHandler(TPid pid, int fd, void* resource, const char* buf, size_t count) {
    // Only foreground processes are allowed to write to the screen.
    if (!prc_isForeground(pid))
        return -1;

    for (size_t i = 0; i < count; i++)
        scr_printCharFormat(buf[i], (const TColor*)&resource, &BLACK);

    return count;
}

static int fdDupHandler(TPid pidFrom, TPid pidTo, int fdFrom, int fdTo, void* resource) {
    return scr_mapToProcessFd(pidTo, fdTo, (const TColor*)&resource);
}