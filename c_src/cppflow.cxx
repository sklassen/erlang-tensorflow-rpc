#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <cmath>

#include "/home/simon/src/github.com/serizba/cppflow/include/cppflow/cppflow.h"
  
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

float scale(int x){
  return std::tanh((x-127.0)/10);
}

int main() {
  int fn, arg1, arg2;
  byte buff[100];
  unsigned int i=0;
  int n_input = 8;
  int n_output = 3;

  fprintf(stderr,"load Model %s\n","/tmp/tf-ar-a/");
  cppflow::model model("/tmp/tf-ar-a/");

  while (read_cmd(buff) > 0) {
    fn = buff[0];

    if (fn == 0) {
      fprintf(stderr,"Reload %s\n","xxx");
    } else if (fn == 1) {
      fprintf(stderr,"ar model %i\n",1);
      std::vector<float> v(n_input);
      for (i=0;i<n_input;i++) {
        v[i] = buff[i+1];
        fprintf(stderr,"ar model %f\n",v[i]);
      }
      std::transform(v.begin(), v.end(), v.begin(), [](float &f) {
          fprintf(stderr,"model scale %f %f\n",f,scale(f));
          return scale(f);
      });
      
      auto input= cppflow::tensor(v, {1,8});
      std::cerr << "input: " << input << std::endl;
    
      auto output = model({{"serving_default_dense_Dense1_input:0", input}}, {"StatefulPartitionedCall:0"});
  
      std::cerr << "output: " << output[0] << std::endl;
      //auto score = output[0]
      //std::cerr << "score: " << score << std::endl;
      for (i=0;i<n_output;i++) {
        buff[i] = round(output[0].get_data<float>()[i]*100);
      }
      write_cmd(buff, n_output);
    } else {
      // just exit on unknown function 
      exit(EXIT_FAILURE);
    }
    fprintf(stderr,"ACC=%i\n",ACC);
    //write_cmd(buff, 1);
  }
  return 0;
}
