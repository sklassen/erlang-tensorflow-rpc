#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <cmath>

#include "cppflow/cppflow.h"
  
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

  fprintf(stderr,"load Model %s\n","/tmp/tf-ar-a/");
  cppflow::model model("/tmp/tf-ar-a/");

  while (read_cmd(buff) > 0) {
    fn = buff[0];

    if (fn == 0) {
      arg1 = buff[1];
      arg2 = buff[2];
      fprintf(stderr,"ACC %i calling scale_and_shift %i %i\n",ACC,arg1,arg2);
      scale_and_shift(arg1, arg2);
    } else if (fn == 1) {
      arg1 = buff[1];
      fprintf(stderr,"ACC %i calling scale %i\n",ACC,arg1);
      std::vector<float> v = {-17,14,-10,38,-16,11,-14,15};
      v[7]=(arg1-127)*1.0;
      std::transform(v.begin(), v.end(), v.begin(), [](float &f) {
          return std::tanh(f/10.0);
      });
      
      auto input= cppflow::tensor(v, {1,8});
      std::cerr << "input: " << input << std::endl;
    
      auto output = model({{"serving_default_dense_Dense1_input:0", input}}, {"StatefulPartitionedCall:0"});
  
      std::cerr << "output: " << output[0] << std::endl;
      //auto score = output[0]
      //std::cerr << "score: " << score << std::endl;
      buff[0] = 50;
      buff[1] = 25;
      buff[2] = 25;
      write_cmd(buff, 3);
      //scale(arg1);
    } else if (fn == 2) {
      arg1 = buff[1];
      fprintf(stderr,"ACC %i calling shift %i\n",ACC,arg1);
      shift(arg1);
    } else {
      // just exit on unknown function 
      exit(EXIT_FAILURE);
    }
    fprintf(stderr,"ACC=%i\n",ACC);
    //write_cmd(buff, 1);
  }
  return 0;
}
