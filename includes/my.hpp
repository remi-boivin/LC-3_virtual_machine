#pragma once

#ifndef MY_HPP
#define MY_HPP

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
/* unix only */
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

#ifndef MEMORY_HPP
    #include "../includes/memory.hpp"
#endif

struct termios original_tio;

void disable_input_buffering();
void restore_input_buffering();
uint16_t check_key();
void handle_interrupt(int signal);
uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t swap16(uint16_t x);
void update_flags(uint16_t r);
void read_image_file(FILE* file);
int read_image(const char* image_path);
void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);

#endif // MY_HPP