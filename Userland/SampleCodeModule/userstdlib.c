#include <stdint.h>
#include <userstdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define ULONG_MAX ((unsigned long)(~0L))      // 0xFFFFFFFF
#define LONG_MAX ((long)(ULONG_MAX >> 1))     // 0x7FFFFFFF
#define LONG_MIN ((long)(~LONG_MAX))          // 0x80000000
#define IS_SPACE(x) ((x) == ' ' || (x) == '\t')
#define IS_ALPHA(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define IS_UPPER(x) (((x) >= 'A' && (x) <= 'Z'))
#define IS_DIGIT(x) (((x) >= '0' && (x) <= '9'))

extern int  sys_write(uint64_t fd, char * buffer, uint64_t size);
extern int  sys_read(uint64_t fd, char * buffer, uint64_t size);
extern void sys_time(char * buffer);
extern int sys_tick();
extern void sys_clear();
extern void sys_restartCursor();
extern void sys_divide();
extern void sys_uniqueWindow();
extern void sys_setScreen(uint8_t id);
extern int sys_printmem(uint64_t * mem_address);
extern  void sys_date(char * buffer);

void setScreen(uint8_t id){
  sys_setScreen(id);
}

void divideWindow(){
  sys_divide();
}

void uniqueWindow(){
  sys_uniqueWindow();
}

int _strlen(const char * str){
    int i=0;
    while( *(str + (i++)) );
    return i-1;
}

int strcmp(char * s1, char * s2) {
    int i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    return s1[i] - s2[i];
}

int sprint(uint8_t fd, char * str){
  return sys_write(fd, str, _strlen(str));
}


int put_char(uint8_t fd, char c){
  return sys_write(fd, &c, 1);
}


void get_time(char * buffer){
  sys_time(buffer);
}

void get_date(char * buffer){
  sys_date(buffer);
}


int tick(){
  return sys_tick();
}

int get_char(){
    char c;
    int returnval;
    do{
      returnval = sys_read(0, &c, 1);
    }while(returnval!=1);

	  return c;
}

int read_char(){
    char c;
    sys_read(0, &c, 1);
    return c;
}

void clearScreen(){
  sys_clear();
}

void restartCursor(){
  sys_restartCursor();
}

void printMem(){

  int counter = 0; 
  char c; 
  uint64_t value = 0;
  my_printf("Ingrese una direccion de memoria en formato hexadecimal: ");

  uint8_t num[32];

  while((c=get_char()) != '\n' && counter < 32){
    if( ( c >= '0' && c <= '9') ){
       num[counter++] = (c - '0');
    } else if( ( c >= 'A' ) && ( c <= 'F' ) ){
       num[counter++] = (c - 'A' + 10);
    } else if( ( c >= 'a') && ( c <= 'f') ){
        num[counter++] = (c - 'a' + 10) ;
    } else if(c=='\b' && counter > 0){
      put_char(1,c);
      num[--counter] = 0;
    } else{
      num[counter++] = 16 + c;
    }
    if(c != '\b'){
      put_char(1,c);
    }
  }

  if(counter > 16){
    char * msg_error = "La direccion ingresada no puede tener mas de 64 bits (16 digitos hexa)";
    put_char(1,'\n');
    sys_write(2, msg_error, _strlen(msg_error));
    put_char(1,'\n');
    return;
  }

  for(int i=0 ; i < counter ; ++i){
    if(num[i] < 16){
      value = (value<<4) + num[i];
    }else{
      char * msg_error2 = "Formato hexa invalido";
      put_char(1,'\n');
      sys_write(2, msg_error2, _strlen(msg_error2));
      put_char(1,'\n');
      return;
    }
  }

  put_char(1,'\n');
  char * msg_error3 = "Acceso a memoria invalido";
  if(sys_printmem((uint64_t *)value) == -1){
    sys_write(2, msg_error3, _strlen(msg_error3));
    put_char(1,'\n');
  }
}

int atoi(char *str){
    int answer = 0;
    int neg = 1;
    if(*str == '-'){
        neg = -1;
        str++;
    }
    while(*str){
        if(!IS_DIGIT(*str)){
            return 0;
        }
        answer = 10*answer + (*str - '0');
        str++;
    }
    return neg*answer;
}

char * my_strcpy( char * destination, char * source){
	return my_strncpy(destination, source, _strlen(source) + 1);
}

char * my_strncpy( char * destination, char * source, int size){
	int i = 0;
	char * ret = destination;
	while(*source && i < size){
		*(destination++) = *(source++);
		i++;
	}
	*destination = '\0';
	return ret;
}

//----------------------------------------------------------------
// https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void my_printf(const char * frmt, ...)
{
  //Module 1: Initializing Myprintf's arguments using stdarg.h
  va_list arg;   //declares a variable which we use to manipulating the argument list contaning variable arugments
  va_start(arg, frmt);   //initialize arg with function's last fixed argument, i.e. format

  const char *aux;

  uint64_t i;
  unsigned u;
  char *s;

  for(aux=frmt; *aux != '\0'; aux++){
    while(*aux != '%'){
      if(*aux == '\0'){
        va_end(arg);
        return;
      }
      put_char(1, *aux);
      aux++;
    }
    aux++;

    //Module 2: Fetching and executing arguments
    //va_arg() fetches the next argument from the argument list, where the 2do parameter is the data type expected
    //ONLY accept char*, unsigned int, int or double
    switch(*aux){
      case 'c' :  i = va_arg(arg, int);  //Fetch char argument
                  put_char(1, i);
                  break;
      case 'd' :  i = va_arg(arg, int);   //Fetch Decimal/Integer argument
                  if(i < 0){
                    i = -i;
                    put_char(1, '-');
                  }
                  sprint(1, convert(i,10));
                  break;
      case 'o':   i = va_arg(arg, unsigned int);   //Fetch Octal representation
                  sprint(1, convert(i,8));
                  break;
      case 's':   s = va_arg(arg, char *);   //Fetch string
                  sprint(1, s);
                  break;
      case 'u':   u = va_arg(arg, unsigned int);   // Fetch Unsigned decimal integer
                  sprint(1, convert(u,10));
                  break;
      case 'x':   u = va_arg(arg, unsigned int);   //Fetch Hexadecimal representation
                  sprint(1, convert(u,16));
                  break;
      case '%':   put_char(1, '%');
                  break;
    }
  }
  //Module 3: Closing argument list to necessary clean-up
  va_end(arg);
}

char *convert(unsigned int num, int base)
{
  static char Representation[] = "0123456789ABCDEF";
  static char buff[33];
  char *ptr;

  ptr = &buff[sizeof(buff)-1];
  *ptr = '\0';

  do{
    *--ptr = Representation[num % base];
    num /= base;
  }while(num != 0);

  return(ptr);
}
