#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef unsigned char byte;

int ACC=1;

int read_exact(byte *buf, int len)
{
  int i, got=0;
  do {
    if ((i = read(0, buf+got, len-got)) <= 0)
      return(i);
    got += i;
  } while (got<len);
  return(len);
}

int write_exact(byte *buf, int len)
{
  int i, wrote = 0;
  do {
    if ((i = write(1, buf+wrote, len-wrote)) <= 0)
      return (i);
    wrote += i;
  } while (wrote<len);
  return (len);
}

int read_cmd(byte *buf)
{
  int len;
  if (read_exact(buf, 2) != 2)
    return(-1);
  len = (buf[0] << 8) | buf[1];
  return read_exact(buf, len);
}

int write_cmd(byte *buf, int len)
{
  byte li;
  li = (len >> 8) & 0xff;
  write_exact(&li, 1);
  li = len & 0xff;
  write_exact(&li, 1);
  return write_exact(buf, len);
}

void shift(int x){
  ACC+=x;
}
void scale(int x){
  ACC*=x;
}

void scale_and_shift(int x, int y){
  scale(x);
  shift(y);
}

int main() {
  int fn, arg1, arg2;
  byte buff[100];

  while (read_cmd(buff) > 0) {
    fn = buff[0];

    if (fn == 0) {
      arg1 = buff[1];
      arg2 = buff[2];
      // fprintf(stderr,"calling scale_and_shift %i %i\n",arg1,arg2);
      scale_and_shift(arg1, arg2);
    } else if (fn == 1) {
      arg1 = buff[1];
      // fprintf(stderr,"calling scale %i\n",arg1);
      scale(arg1);
    } else if (fn == 2) {
      arg1 = buff[1];
      // fprintf(stderr,"calling shift %i\n",arg1);
      shift(arg1);
    } else {
      // just exit on unknown function 
      exit(EXIT_FAILURE);
    }
    buff[0] = ACC;
    write_cmd(buff, 1);
  }
}
