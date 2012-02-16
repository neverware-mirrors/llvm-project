//===-- main.cpp ------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stdio.h>

int main (int argc, char const *argv[])
{
    char my_string[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 0};
    printf("my_string=%s\n", my_string); // Set break point at this line.
    return 0;
}
