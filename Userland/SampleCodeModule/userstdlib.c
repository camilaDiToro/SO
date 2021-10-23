#include <stdint.h>
#include <userstdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

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
  
  int counter = 0; char c; uint64_t value = 0; 
  char * msg_error="Formato hexa invalido"; 
  char * msg = "Ingrese una dirección de memoria en formato hexadecimal: ";
  sys_write(1, msg, _strlen(msg));

  while((c=get_char())!='\n'){
    // TO DO : fix string conversion to number. 
    if( ( c >= '0' && c <= '9') ){
       value = value * 10 + (c - '0');
    } else if( ( c >= 'A' ) && ( c <= 'F' ) ){
       value = value * 10  + (c - 'A' + 10);
    } else if( ( c >= 'a') && ( c <= 'f') ){
       value = value * 10 + (c - 'a' + 10) ;
    } else {
      counter += 17;
    }
    put_char(1,c);
    counter++;
  }

   if(counter > 16){
      put_char(1,'\n');
      sys_write(2, msg_error, _strlen(msg_error));
      put_char(1,'\n');
      return;
    }

  put_char(1,'\n');
  char * msg_error2="Acceso a memoria invalido";
  if(sys_printmem((uint64_t *)value) == -1){
    sys_write(2, msg_error2, _strlen(msg_error2));
    put_char(1,'\n');
  }
}

// TO DO: Finish functions

/*
void my_printf(const char * frmt, ...)
{
  char *p;

  int i;
  unsigned u;
  char *s;

  //Module 1: Initializing Myprintf's arguments using stdarg.h
  va_list argp;   //declares a variable which we use to manipulating the argument list contaning variable arugments
  va_start(argp, frmt);   //initialize argp with function's last fixed argument, i.e. format

  for(p=frmt; *p != '\0'; p++){
    while(*p != '%'){
      put_char(1, *p);
      p++;
    }
    p++;

    //Module 2: Fetching and executing arguments
    //va_arg() fetches the next argument from the argument list, where the 2do parameter is the data type expected
    //ONLY accept char*, unsigned int, int or double
    switch(*p){
      case 'c' :  i = va_arg(argp, int);  //Fetch char argument
                  put_char(1, i);
                  break;
      case 'd' :  i = va_arg(argp, int);   //Fetch Decimal/Integer argument
                  if(i < 0){
                    i = -i;
                    put_char(1, '-');
                  }
                  sprint(1, convert(i,10));
                  break;
      case 'o':   i = va_arg(argp, unsigned int);   //Fetch Octal representation
                  sprint(1, convert(i,8));
                  break;
      case 's':   s = va_arg(argp, char *);   //Fetch string
                  sprint(1, s);
                  break;
      case 'u':   u = va_arg(argp, unsigned int);   // Fetch Unsigned decimal integer
                  sprint(1, convert(u,10));
                  break;
      case 'x':   u = va_arg(argp, unsigned int);   //Fetch Hexadecimal representation
                  sprint(1, convert(u,16));
                  break;
      case '%':   put_char(1, '%');
                  break;
    }
  }
  //Module 3: Closing argument list to necessary clean-up
  va_end(argp);
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

//////////////////////////////////////////

// LAS "FUNCIONES" ESTAS DE stadarg.h


//    #ifndef _STDARG
//    #define _STDARG

//    /* type definitions */
//    typedef char *va_list;

//    /* macros */
//    #define va_arg(ap, T) (* (T *)(((ap) += _Bnd(T, 3U)) - _Bnd(T, 3U)))
//    #define va_end(ap) (void)0
//    #define va_start(ap, A) (void)((ap) = (char *)&(A) + _Bnd(A, 3U))
//    #define _Bnd(X, bnd) (sizeof (X) + (bnd) & ~(bnd))

//    #endif

// typedef char *va_list;
// #define va_start(ap, parmn) (void)((ap) = (char*)(&(parmn) + 1))
// #define va_end(ap) (void)((ap) = 0)
// #define va_arg(ap, type) (((type*)((ap) = ((ap) + sizeof(type))))[-1])
/*
int my_atoi (char* s)
{
    int j;
    int i = 0;
    for(j=0; s[j] != 0; j++)
    {

        i = i*10 + s[j] - '0';
    }
    return i;
}

int my_scanf(const char * frmt, ...)
{
  va_list vl;
  int i=0, j=0, ret = 0;

  char buff[100] = {0};
  char tmp[20];
  char c;

  char *out_loc;
  while(c != '\n') {
    if (fread(&c, 1, 1, stdin)) {
      buff[i] = c;
      i++;
    }
  }

  va_start( vl, str );
  i = 0;

  while (str && str[i]) {
    if (str[i] == '%') {
      i++;
      switch (str[i]) {
        case 'c': {
          * (char *)va_arg( vl, char* ) = buff[j];
          j++;
          ret ++;
          break;
        }
        case 'd': {
          * (int *)va_arg( vl, int* ) = strtol(&buff[j], &out_loc, 10);
          j += out_loc -&buff[j];
          ret++;
          break;
        }
        case 'x': {
          * (int *)va_arg( vl, int* ) = strtol(&buff[j], &out_loc, 16);
          j += out_loc -&buff[j];
          ret++;
          break;
        }
        case 'o': {
          * (int *)va_arg( vl, int* ) = strtol(&buff[j], &out_loc, 8);
          j += out_loc -&buff[j];
          ret++;
          break;
        }
        case 's': {
          out_loc = (char *)va_arg( vl, char* );
          strcpy(out_loc, &buff[j]);
          j += strlen(&buff[j]);
          ret++;
          break;
        }
      }
    }
    else {
      buff[j] =str[i];
      j++;
    }
    i++;
  }
  va_end(vl);
  return ret;
}

*/
