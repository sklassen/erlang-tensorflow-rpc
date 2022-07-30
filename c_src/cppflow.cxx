#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "/home/simon/src/github.com/serizba/cppflow/include/cppflow/cppflow.h"

typedef unsigned char byte;

static void show_usage(std::string name)
{
  std::cerr << "Usage: " << name << " <option(s)> SOURCES"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help message\n"
            << "\t-d,--dir DIRECTORY\tSpecify the model path (required)"
            << "\t-i,--in N\tNumber of input bytes (def:8)"
            << "\t-o,--out N\tNumber of output bytes (def:3)"
            << std::endl;
}

int read_exact(byte *buf, int len)
{
  int i, got = 0;
  do
  {
    if ((i = read(0, buf + got, len - got)) <= 0)
      return (i);
    got += i;
  } while (got < len);
  return (len);
}

int write_exact(byte *buf, int len)
{
  int i, wrote = 0;
  do
  {
    if ((i = write(1, buf + wrote, len - wrote)) <= 0)
      return (i);
    wrote += i;
  } while (wrote < len);
  return (len);
}

int read_cmd(byte *buf)
{
  int len;
  if (read_exact(buf, 2) != 2)
    return (-1);
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

float scale(int x)
{
  return std::tanh((x - 127.0) / 10);
}

int main(int argc, char *argv[])
{
  int fn, arg1, arg2;
  byte buff[100];
  unsigned int i = 0;
  int n_input = 8;
  int n_output = 3;

  if (argc < 3) {
    show_usage(argv[0]);
    return 1;
  }
  std::vector<std::string> sources;
  std::string directory;
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help"))
    {
      show_usage(argv[0]);
      return 0;
    } else if ((arg == "-d") || (arg == "--dir")) {
      if (i + 1 < argc) {
        directory = argv[++i];
      } else {
        std::cerr << "--dir option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-i") || (arg == "--in")) {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        n_input = atoi(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
      } else { // Uh-oh, there was no argument to the directory option.
        std::cerr << "--in option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-o") || (arg == "--out")) {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        n_output = atoi(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
      } else { // Uh-oh, there was no argument to the directory option.
        std::cerr << "--out option requires one argument." << std::endl;
        return 1;
      }
    } else {
      sources.push_back(argv[i]);
    }
  }

  std::cerr << "load: " << directory << std::endl;
  cppflow::model model(directory);

  while (read_cmd(buff) > 0)
  {
    fn = buff[0];

    if (fn == 0)
    {
      std::cerr << "reload: " << directory << std::endl;
      model(directory);
    }
    else if (fn == 1)
    {
      fprintf(stderr, "ar model %i\n", 1);
      std::vector<float> v(n_input);
      for (i = 0; i < n_input; i++)
      {
        v[i] = buff[i + 1];
        fprintf(stderr, "ar model %f\n", v[i]);
      }
      std::transform(v.begin(), v.end(), v.begin(), [](float &f) {
        fprintf(stderr, "model scale %f %f\n", f, scale(f));
        return scale(f);
      });

      auto input = cppflow::tensor(v, {1, n_input});
      std::cerr << "input: " << input << std::endl;

      auto output = model({{"serving_default_dense_Dense1_input:0", input}}, {"StatefulPartitionedCall:0"});

      std::cerr << "output: " << output[0] << std::endl;
      //auto score = output[0]
      //std::cerr << "score: " << score << std::endl;
      for (i = 0; i < n_output; i++)
      {
        buff[i] = round(output[0].get_data<float>()[i] * 100);
      }
      write_cmd(buff, n_output);
    } else if (fn == 1) {
      std::cerr << "got exit" << std::endl;
      return 0;
    } else {
      // just exit on unknown function
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}
