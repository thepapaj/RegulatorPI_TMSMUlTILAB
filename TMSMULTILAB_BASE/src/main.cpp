////////////////////////////////////////////////////////
///////      Podstawowa konfiguracja
////////////////////////////////////////////////////////



/****************************************************/
/*					DEFINES START					*/
#define BUFFERSYNC
#define WIN_PLOT
#define NazwaPlikuDanych "Data/TMSLABoutputData.csv"
#define CSV_NAGLOWEK "Wsp. x,Wsp. y1,Wsp. y2,Wsp. y3,Wsp. y4,Wsp. y5\n"
#define CSV_DANE "%i,%i,%i,%i,%i,%i\n",Tim, y[0], y[1], y[2], y[3], y[4]
/*					DEFINES END					*/
/************************************************/



/****************************************************/
/*					INCLUDES START					*/
#include "main.h"

#ifdef TMSLAB_WIN
#include "stdio.h"
#endif
/*					INCLUDES END					*/
/****************************************************/



/************************************************************/
/*					VARIABLES SLOT 1 START					*/
unsigned long *ekran; // Adres obszaru graficznego LCD [8*128*2]
#ifdef TMSLAB_C2000
unsigned char *textEkran; // Adres obszaru tekstowego [40*16/2]
#endif

#ifdef TMSLAB_WIN
unsigned short int *textEkran; // Adres obszaru tekstowego [40*16/2]
extern int (*PartialRefresh)();
char credits[43]="-               DEMO DISC                -";  // Tekst wyswietlany w naglowku symulatora
long Timer2IsrPeriod=1; // okres pracy symulowanego licznika Timer2 podany w przybli�eniu w ms
#ifdef WIN_PLOT
FILE* outputCSV=0;
#endif
#endif
/*					VARIABLES SLOT 1 END					*/
/************************************************************/



/************************************************************/
/*					VARIABLES SLOT 2 START					*/
int y[5];
int Tim = 0;                // Licznik uzytkownika
int Tim1 = 0;
int Tim2 = 0;
int Tim3 = 0;
unsigned int predkosc = 0;
unsigned int preScale = 0;  // Preskaler licznika uzytkownika
unsigned int Skaler = 0;	// Skaler potrzebny do przeliczania konkretnych Timow
bool st_zaklo = false;	// zmienna start/stop zaklocenia
volatile char EnableRefresh = 0;    //Zezwolenie na odswiezenie zawartosci pamieci graficznej

unsigned char Bufor1[] = "Temperatura";
unsigned char Bufor2[] = "-----------";
char Bufor3[5] ;
unsigned char Bufor4[] = "Wartosc zadana:";
unsigned char Bufor5[] = "--------------";
char Bufor6[5];

float wart_zad = 30;
float wyj_ukl = 20;
float uchyb = 0;
float syg_ster = 0;
float zaklo = 10;
float pole = 2;
unsigned char Key = 0;
/*					VARIABLES SLOT 2 END					*/
/************************************************************/



/****************************************************/
/*					OBJECTS START					*/
PID Regul(0.8, 2, 1, -50, 50);
CalkModel Model;
InerModel Pomieszczenie(wyj_ukl, pole);

R_P_LCD_TMSLAB LCD;             // Obiekt obslugujacy LCD
R_P_KEYBOARD_TMSLAB KEYBOARD;   // Obiekt obslugujacy klawiature
R_P_LEDBAR_TMSLAB LEDBAR;       // Obiekt obslugujacy diody LED
/*					OBJECTS END					*/
/************************************************/



/************************************************************/
/*					FUNCTION MAIN BEGINS					*/
/************************************************************/
int main()
{
#ifdef TMSLAB_C2000
	LCD.LCD_Init(ekran, textEkran);
#endif

#ifdef TMSLAB_WIN
	LCD.LCD_Init(&ekran,&textEkran);
#ifdef WIN_PLOT
	outputCSV=fopen(NazwaPlikuDanych,"w+" );
	fprintf(outputCSV,CSV_NAGLOWEK);
#endif
#endif
	KEYBOARD.InitKB(100);
	LEDBAR.InitLedBar();

	while (1)
	{
		Key = KEYBOARD.GetKey();
		LEDBAR.SetValue(Tim); //Dzialanie linijki diodowej

		y[0]=syg_ster;
		y[1]=wyj_ukl;
		y[2]=zaklo;
		y[3]=uchyb;
		y[4]=wart_zad;

		PrintText(textEkran, Bufor1, 12, 19, 1);
		PrintText(textEkran, Bufor2, 12, 19, 2);
		PrintText(textEkran, Bufor3, 8, 19, 3);
		PrintText(textEkran, Bufor4, 14, 19, 5);
		PrintText(textEkran, Bufor5, 14, 19, 6);
		PrintText(textEkran, Bufor6, 4, 19, 7);
		ClearScreen();
		DrawPixels();

#ifdef TMSLAB_WIN
if(PartialRefresh()) return 0;
#ifdef WIN_PLOT
// Zapis danych do pliku
fprintf(outputCSV,CSV_DANE);
printf("time %d syg_ster %f wyj_ukl %f zaklo %f\n",Tim,syg_ster,wyj_ukl,zaklo);
sprintf(Bufor3, "%.1f", wyj_ukl);
sprintf(Bufor6, "%.1f", wart_zad);
fflush(outputCSV);
fflush(stdout);
#endif
#endif

	}
}

void Timer2Isr()
{
	if(++preScale==500)
	{
		preScale=0;Tim++;
		if (Key == 1) zaklo++;
		else if (Key == 2) zaklo--;
		else if (Key == 4) zaklo=zaklo+3;
		else if (Key == 5) zaklo=zaklo-3;
		else if (Key == 7) zaklo=zaklo+6;
		else if (Key == 8) zaklo=zaklo-6;

		if (Key == 12) wart_zad++;
		else if (Key == 13) wart_zad--;

		uchyb = wart_zad - wyj_ukl;
		uchyb = -uchyb;

		Regul.setInput(uchyb);
		Regul.Calculate();

		syg_ster = Regul.getOutput();

		Model.setInput(syg_ster, zaklo);
		Model.Calculate();

		wyj_ukl = Model.getOutput();
	}
	if (preScale % 80 == 0)
	{
		Tim1++;
		if (Tim1 % 2 == 0) Tim2++;
		else if (Tim2 % 2 == 0) Tim3++;
	}
}

void ClearScreen()
{

	for (int a = 0; a < (128 * 8); a++)
		ekran[a] = 0;
}
void DrawPixels()
{
	unsigned char* ekran_bajt = (unsigned char*)ekran;
	for (int i=0; i< (256*128/8); i = i+4)
	{
		if (syg_ster <= 7.5) predkosc = Tim3;
		else if (syg_ster > 7.5 && syg_ster <= 18) predkosc = Tim2;
		else if (syg_ster > 18) predkosc = Tim1;
		if (predkosc % 3 == 0)
		{
			ekran_bajt[i] = wiatraktermogrzalkaNowa0_h[i+3];
			ekran_bajt[i+1] = wiatraktermogrzalkaNowa0_h[i+2];
			ekran_bajt[i+2] = wiatraktermogrzalkaNowa0_h[i+1];
			ekran_bajt[i+3] = wiatraktermogrzalkaNowa0_h[i];
		}
		else if (predkosc % 3 == 1)
		{
			ekran_bajt[i] = wiatraktermogrzalkaNowa60_h[i+3];
			ekran_bajt[i+1] = wiatraktermogrzalkaNowa60_h[i+2];
			ekran_bajt[i+2] = wiatraktermogrzalkaNowa60_h[i+1];
			ekran_bajt[i+3] = wiatraktermogrzalkaNowa60_h[i];
		}
		else if (predkosc % 3 == 2)
		{
			ekran_bajt[i] = wiatraktermogrzalkaNowa30_h[i+3];
			ekran_bajt[i+1] = wiatraktermogrzalkaNowa30_h[i+2];
			ekran_bajt[i+2] = wiatraktermogrzalkaNowa30_h[i+1];
			ekran_bajt[i+3] = wiatraktermogrzalkaNowa30_h[i];
		}
		kropki(zaklo);
		wskaznik();
	}

}

void wskaznik()
{
	int i,j;
	int temm = wyj_ukl;
	if (temm > 45) temm = 45;
		for (i=0;i<3;i++)
		{
			for (j=75-temm;j<91;j++)
			{
				SetPixel(ekran,(224+i),j);
			}
		}
}

void kropki(float zaklo)
{
	int ilosc = zaklo;
	if (ilosc>25) ilosc=25;

	int x = 0;
	int y = 0;
	x=rand()&0xff;
	y=rand()&0x7f;
	if (x>147 && x<198 && y>78-ilosc && y<78)
	SetPixel(ekran,x,y);
}
