#include <time.h>
#include <naiveConsole.h>

static unsigned long ticks = 0;
static int printed = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void print_5secs(){
	while(1){
		if(!printed && seconds_elapsed()%5 == 0){
			ncPrint("hola");
			ncNewline();
			printed = 1;
		}else if (seconds_elapsed()%5 == 1){
			printed = 0;
		}
	}
}
