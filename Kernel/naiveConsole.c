#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
static void check();
static void scrollUp();

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;


void ncPrintFormat(const char * string, uint8_t format){
	for (int i = 0; string[i]; i++){
		ncPrintCharFormat(string[i], format);
	}
}


void ncPrintCharFormat(char character, uint8_t format){
	check();
	*currentVideo++ = character;
	*currentVideo++ = format;
}

static void scrollUp(){
	for(int i = 0; i < height - 1; i++){
		for(int j = 0; j < width * 2; j++){
			video[j + i * width * 2] = video[j + (i + 1) * width * 2];
		}
	}
	for(int k = 0; k < width * 2; k++)
		video[(height - 1) * width * 2+ k] = '\0';
	currentVideo = video + (height - 1) * width * 2;
}

static void check(){
	if(currentVideo - video >= width * height * 2)
		scrollUp();
}


void ncPrint(const char * string){
	for (int i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintChar(char character){
	check();
	*currentVideo = character;
	currentVideo += 2;
}

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
