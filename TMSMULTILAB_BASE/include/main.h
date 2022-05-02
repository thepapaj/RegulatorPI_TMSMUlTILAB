
#ifndef main_CL
#define main_CL


#include "setup.h"
#include "stdlib.h"
#include "stdint.h"
#include "R_P_LCD_TMSLAB.h"
#include "R_P_KEYBOARD_TMSLAB.h"
#include "R_P_LEDBAR_TMSLAB.h"
#include "square.h"
#include "bitmapaZad0.h"
#include "bitmapaZad30.h"
#include "bitmapaZad60.h"
#include "moje_klasy.h"
#include "math.h"

#ifdef TMSLAB_C2000
#include "F2837xD_device.h"
#include "systemInit.h"
#endif


void SetUpPeripherials();
void EnableInterrupts();
void ClearScreen();
void DrawPixels();

void kropki(float);
void wskaznik(void);


#endif
