#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <time.h>
#include <moduleLoader.h>
#include <systemCalls.h>
#include <naiveConsole.h>
#include <keyboard.h>
#include <RTClock.h>
#include <idtLoader.h>
#include <graphicMode.h>
#include <exceptions.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize){
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];
	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{
	load_idt();
	initUniqueWindow();

	//print_registers();
	//divideByZero();
	ncNewline();

	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint(" Calling the sample code module returned: ");
	ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	ncPrint("[Finished]");

	ncNewline();
	ncPrintFormat("Hora: ", 0xF3);
	char date[11];
	get_time(date);
	ncPrint(date);
	ncNewline();
	ncPrintFormat("Fecha: ", 0xF3);
	get_date(date);
	ncPrint(date);
  	ncNewline();

	//print_5secs();


	// int c;
	// while((c=getScanCode())!=0x1E){
	// 	ncNewline();
	// 	ncPrintHex(c);
	// 	ncPrintChar('-');
	// 	ncPrintChar(ctoi(c));
	// }

	//sys_write(1,"Hola",5);
	//ncNewline();

	printChar('A');
	newLine(0);
	printChar('B');
		newLine(0);
	printChar('C');
		newLine(0);
	printChar('D');
		newLine(0);
	printChar('E');
		newLine(0);
	printChar('F');
		newLine(0);
	printChar('G');
		newLine(0);
	printChar('H');
		newLine(0);
	printChar('I');
		newLine(0);
	printChar('J');
		newLine(0);
	printChar('K');
		newLine(0);
	printChar('L');
		newLine(0);
	printChar('M');	
		newLine(0);
	printChar('N');
		newLine(0);
	printChar('O');
		newLine(0);
	printChar('P');
	newLine(0);
	printChar('Q');
	newLine(0);
	printChar('R');
	newLine(0);
	printChar('S');
	newLine(0);
	printChar('T');
	newLine(0);
	printChar('U');
	newLine(0);
	printChar('V');
	newLine(0);
	printChar('W');
	newLine(0);
	printChar('X');
	newLine(0);
	printChar('Y');
	newLine(0);
	printChar('Z');
	newLine(0);
	printChar('A');
	newLine(0);
	printChar('B');
		newLine(0);
	printChar('C');
		newLine(0);
	printChar('D');
		newLine(0);
	printChar('E');
		newLine(0);
	printChar('F');
		newLine(0);
	printChar('G');
		newLine(0);
	printChar('H');
		newLine(0);
	printChar('I');
		newLine(0);
	printChar('J');
		newLine(0);
	printChar('K');
		newLine(0);
	printChar('L');
		newLine(0);
	printChar('M');	
		newLine(0);
	printChar('N');
		newLine(0);
	printChar('O');
		newLine(0);
	printChar('P');
	newLine(0);
	printChar('Q');
	newLine(0);
	printChar('R');
	newLine(0);
	printChar('S');
	newLine(0);
	printChar('T');
	newLine(0);
	printChar('U');
	newLine(0);
	printChar('V');
	newLine(0);
	printChar('W');
	newLine(0);
	printChar('X');

	
	return 0;
}
