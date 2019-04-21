#include <dos.h>
#include <dir.h>
#include <conio.h>
#include <errno.h>		/* hibaÅzenetek megjelen°tÇsÇhez */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <limits.h>

#include "defs.h"

extern char akt[], temp[], ch;
extern int CIMSOR,  /*cimsor magassaga*/
    MENUSOR, /*menusor magassaga*/
    KERET,   /* 3dkeret fuggoleges tavolsaga a menusor szeletol*/
    ALM_SORMAG,   /*sor magassaga az almenupontokon*/
    ALM_OLDAL,    /* a kijelolt almenupont koruli sav tavolsaga az ablak oldalatol*/
    axf,ayf,axa,aya,          /*Az eppen aktiv ablak koordinatai*/
    nyitva = 0,		/* AlmenÏ kinyitva? */
    ABLAKMIN,
    STX, STY,
    *fokoord,
    FO,AL,ALF,ALS, ALH;

menutetel fomenu[] = {
{"Fajl",		NULL,     0},
{"Szerkesztes",	NULL,     0},
{"Sugo",		NULL,     0}, /* Nincs almenu */
{"Kilepes",		fugg,     9} };

menutetel fajl[] = {
{"Attributum",	fugg,	  1},
{"Nyomtatas",	fugg,	  2}};

menutetel szerk[] = {
{"Masolas",		fugg,     3},
{"Torles",		fugg,     4},
{"Athelyezes",	fugg,     5},
{"Futtatas",	fugg,     6}};

menutetel help[] = {
{"Sugo",		fugg,	7,},
{"Nevjegy",		fugg,	  8}};


menutipus menuk[] = {
/* x	y   xm	   ym t-telsz-m menÏt-telek utsÌt-tel */
{  0,	0,  0, 0,    0,  fajl,	    0},
{  0,	0,  0, 0,    0,  szerk,	    0},
{  0,	0,  0, 0,    0,  help,	    0} };

int CIMSOR,  /*cimsor magassaga*/
    MENUSOR, /*menusor magassaga*/
    KERET,   /* 3dkeret fuggoleges tavolsaga a menusor szeletol*/
    ALM_SORMAG,   /*sor magassaga az almenupontokon*/
    ALM_OLDAL,    /* a kijelolt almenupont koruli sav tavolsaga az ablak oldalatol*/
    axf,ayf,axa,aya,          /*Az eppen aktiv ablak koordinatai*/
    ABLAKMIN,
    STX, STY,
    *fokoord,
    FO,AL,ALF,ALS,ALH;


/* s karakterl†nc megford°t†sa */
void reverse (char s[])
{
	unsigned short int i, j;

	j=strlen(s);
	for(i=0; i<j/2; i++)
	{
		s[i]^=s[j-i-1];
		s[j-i-1]^=s[i];
		s[i]^=s[j-i-1];
	}
}

/* szam-b¢l csin†l sztringet a szamszov-be */
void numtostr (long int szam, char szamszov[])
{
	unsigned short int i=0, neg=FALSE, minlong=FALSE;

	if (szam<0)
	{
		neg=TRUE;
		if (szam == LONG_MIN)
		{
			minlong=TRUE;
			szam++;
		}
		szam*=-1;
	}
	do
	{
		szamszov[i++]=szam%10+'0';
		szam/=10;
	} while (szam!=0);
	if (neg==TRUE) szamszov[i++]='-';
	szamszov[i]=0;
	reverse(szamszov);
	if (minlong) szamszov[strlen(szamszov)-1]++;
}

/* grafikus kÇpernyîn £j sorba kezd °rni */
void ujsor (int *sor)
{
	*sor += SORTAV;
	moveto(0, *sor);
}

/* szam Çs szoveg paramÇterek ki°r†sa a grafikus kÇpernyîre */
void szamSzovir (long int szam, char szoveg[])
{
	char s[30];

	numtostr(szam, s);
	strcat(s, szoveg);
	outtext(s);
}

void meretir (long int meret)
{
	if (meret > (long)KILOBAJT*(long)KILOBAJT*(long)KILOBAJT)
	{
		szamSzovir(meret/(long)KILOBAJT/(long)KILOBAJT/(long)KILOBAJT, " GB");
	}
	else if (meret > (long)KILOBAJT*(long)KILOBAJT)
	{
		szamSzovir(meret/(long)KILOBAJT/(long)KILOBAJT, " MB");
	}
	else if (meret > (long)KILOBAJT)
	{
		szamSzovir(meret/(long)KILOBAJT, " kB");
	}
	else szamSzovir(meret, " b†jt");
}

void jelolo (int tol_x, int tol_y, int ig_x, int ig_y, int onoff)
{
	if (onoff==0)
	{
		setcolor(WHITE);
	}
	else
	{
		setcolor(BLACK);
		setlinestyle(DOTTED_LINE,0,1);
	}
	rectangle(tol_x,tol_y,ig_x,ig_y);
	setlinestyle(SOLID_LINE,0,1);
}


void felulet (void)
{
	int i;

	/*grafikus felulet kezdoertekeinek bealliasa*/
	CIMSOR = textheight("hello")+6;				/* FûmenÏ adatok. */
	MENUSOR= CIMSOR +5;
	KERET=3;                     /* a 3d keret oldalanak es tetejenek tavolsaga a cimsor tetejetol es aljatol*/
	ALM_SORMAG=textheight("hello")+7;
	ALM_OLDAL=3;
	ABLAKMIN=CIMSOR+MENUSOR+3;
	STX=52; STY=MAXY-MENUSOR-MENUSOR/2-4;
	FO=sizeof(fomenu)/sizeof(menutetel);
    ALF=sizeof(fajl)/sizeof(menutetel);
    ALS=sizeof(szerk)/sizeof(menutetel);
    ALH=sizeof(help)/sizeof(menutetel);
    AL=sizeof(menuk)/sizeof(menutipus);


	fokoord=(int *)malloc(FO*2*sizeof(int));


	/*fokoord feltoltese - fomennupontok vizszintes koordinatainak es nagysaganak szamitasa
	  az elso oszlop tartalmazza a menupont szovege koruli keret bal oldalanak x koordinatajat,
	  a masodik pedig a keret szelesseget*/
	*(fokoord)=10; *(fokoord+1)=textwidth(fomenu[0].szoveg)+14;
	for(i=1; i<FO; ++i) {
	  *(fokoord+i*2)=*(fokoord+(i-1)*2)+ *(fokoord+(i-1)*2+(1));
	  *(fokoord+i*2+(1))=textwidth(fomenu[i].szoveg)+14;
	}

	/*almenu-ablakok helyenek es meretenek beallitasa*/
	menuk[0].x=*(fokoord);
	menuk[1].x=*(fokoord+1*2);
	menuk[2].x=*(fokoord+2*2);				/* AûmenÏ adatok. */
	menuk[0].y = menuk[1].y = menuk[2].y= CIMSOR+MENUSOR+1;
	menuk[0].xm = menuk[1].xm = menuk[2].xm = 16*textwidth("m");
	menuk[0].ym=4+ALF*(ALM_SORMAG)+4;
	menuk[1].ym=4+ALS*(ALM_SORMAG)+4;
	menuk[2].ym=4+ALH*(ALM_SORMAG)+4;
	menuk[0].tetelszam=ALF;
	menuk[1].tetelszam=ALS;
	menuk[2].tetelszam=ALH;

	/*grafikus felulet kirajzolasa*/
	setcolor(BLACK);
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	floodfill(10,10,15);
	setfillstyle(SOLID_FILL,BLUE);
	bar(0,0,getmaxx(),CIMSOR);
	settextstyle(0,0,0);
	settextjustify(LEFT_TEXT,CENTER_TEXT);
	setcolor(WHITE);
	outtextxy(10,CIMSOR/2,"Conqueror Commander");
	doboz(MIN,CIMSOR+1,getmaxx(),CIMSOR+MENUSOR);
	settextstyle(2,0,0);
	/*EgerReset();					/* EG-R inicializ-l-sa */
	setcolor(BLACK);
	settextjustify(LEFT_TEXT,CENTER_TEXT);
	for(i=0; i<FO; ++i) outtextxy( *(fokoord+i*2+(0))+8,CIMSOR+(MENUSOR/2),fomenu[i].szoveg);
	doboz(0,MAXY-MENUSOR,MAXX-1,MAXY-1);

	/*A panelek kirajzolasa*/
	setfillstyle(SOLID_FILL,WHITE);
	bar(4,CIMSOR+MENUSOR+4,KOZEP-4,MAXY-2*MENUSOR-4);
	bar(KOZEP+4,CIMSOR+MENUSOR+4,MAXX-5,MAXY-2*MENUSOR-4);
	setcolor(DARKGRAY);
	rectangle(3,CIMSOR+MENUSOR+3,KOZEP-3,MAXY-2*MENUSOR-3);
	rectangle(KOZEP+3,CIMSOR+MENUSOR+3,MAXX-4,MAXY-2*MENUSOR-3);
	/*setcolor(WHITE);
	line(2,MAXY-2*MENUSOR-4,KOZEP-2,MAXY-2*MENUSOR-4);
	line(KOZEP-2,MAXY-2*MENUSOR-4,KOZEP-2,CIMSOR+MENUSOR+2);
	setcolor(BLACK);
	line(KOZEP-2,CIMSOR+MENUSOR+2,2,CIMSOR+MENUSOR+2);
	line(2,CIMSOR+MENUSOR+2,2,MAXY-2*MENUSOR-4);*/

	/*szovegmezo kiirasa*/
	setfillstyle(1,WHITE);
	bar(50, MAXY-2*MENUSOR+1, MAXX-5, MAXY-MENUSOR-5);
	setcolor(DARKGRAY);
	rectangle(49, MAXY-2*MENUSOR, MAXX-4, MAXY-MENUSOR-4);


	/* kiirja a stﬂtuszsor pontjait */
	setcolor(BLACK);
	outtextxy(2, MAXY-MENUSOR/2-3, "F1 Sugo");
	outtextxy(55, MAXY-MENUSOR/2-3, "F2 Menu");
	outtextxy(108, MAXY-MENUSOR/2-3, "F3 Nezoke");
	outtextxy(173, MAXY-MENUSOR/2-3, "F4 Szerkeszt");
	outtextxy(254, MAXY-MENUSOR/2-3, "F5 Masol");
	outtextxy(311, MAXY-MENUSOR/2-3, "F6 Athelyez");
	outtextxy(385, MAXY-MENUSOR/2-3, "F7 Uj mappa");
	outtextxy(462, MAXY-MENUSOR/2-3, "F8 Torol");
	outtextxy(518, MAXY-MENUSOR/2-3, "F9 Atnevez");
	outtextxy(586, MAXY-MENUSOR/2-3, "F10 Kilep");


}

/*Kirajzol egy dobozt*/
void doboz(int xf,int yf,int xa,int ya){
	int tmp_col;
	/*doboztorles(xf,yf,xa,ya);*/
	tmp_col=getcolor();
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(xf,yf,xa,ya);
	setcolor(DARKGRAY);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	setcolor(WHITE);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp_col);
}

/*===================================================================*/

void almenuki(int index){	/* A megadott indexÏ almenÏ	megjelentet-se. */
	int xf=menuk[index].x,	/* Az almenÏ sarokkoordin-t-i. */
		yf=menuk[index].y,i, tmp;;
	tmp=getcolor();
	setcolor(BLACK);
	/*if(EgerMukodik()) EgerKi();/*Rajzolas alatt nincs kurzor*/
	doboz(xf,yf,xf+menuk[index].xm,yf+menuk[index].ym);	/* A doboz ki. */
	settextjustify(LEFT_TEXT,CENTER_TEXT);
	for(yf+=4,i=0;i<menuk[index].tetelszam;i++,yf+=ALM_SORMAG)
	if(i==menuk[index].utsotetel) {
		setfillstyle(SOLID_FILL,BLUE);
	    setcolor(WHITE);
	    bar(xf+ALM_OLDAL,yf,xf+menuk[index].xm-ALM_OLDAL,yf+ALM_SORMAG);
	    outtextxy(xf+10,yf+(ALM_SORMAG/2)-1,menuk[index].tetelek[i].szoveg);
	    setcolor(BLACK);
	}
	else 
		outtextxy(xf+10,yf+(ALM_SORMAG/2)-1,menuk[index].tetelek[i].szoveg);
	/*if(EgerMukodik()) EgerBe();*/
	setcolor(tmp);}

/*===============================================================*/


void _3dkeret_ki(int i) {
int tmp, xf, yf, xa, ya; 

  tmp=getcolor();
  xf=*(fokoord+i*2+(0));
  yf=CIMSOR+KERET;
  xa=*(fokoord+i*2+(0))+*(fokoord+i*2+(1));
  ya=CIMSOR+MENUSOR-KERET;
  setcolor(DARKGRAY);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	setcolor(WHITE);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp);
}

/*==================================================================*/

void _3dkeret_be(int i) {
int tmp, xf, yf, xa, ya;

  tmp=getcolor();
  xf=*(fokoord+i*2+(0));
  yf=CIMSOR+KERET;
  xa=*(fokoord+i*2+(0))+*(fokoord+i*2+(1));
  ya=CIMSOR+MENUSOR-KERET;
  setcolor(WHITE);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	setcolor(DARKGRAY);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp);
}

/*===================================================================*/

void _3dkeret_torl(int i) {
int tmp, xf, yf, xa, ya;

  tmp=getcolor();
  xf=*(fokoord+i*2+(0));
  yf=CIMSOR+KERET;
  xa=*(fokoord+i*2+(0))+*(fokoord+i*2+(1));
  ya=CIMSOR+MENUSOR-KERET;
  setcolor(LIGHTGRAY);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp);
}

/*=====================================================================*/

void _3dmkeret_ki(int xf, int yf, int xa, int ya) {
int tmp;

  tmp=getcolor();
  setcolor(DARKGRAY);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	setcolor(WHITE);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp);
}

/*========================================================================*/


void _3dmkeret_be(int xf, int yf, int xa, int ya) {
int tmp;

  tmp=getcolor();
  setcolor(WHITE);
	line(xf,ya,xa,ya);
	line(xa,ya,xa,yf);
	setcolor(DARKGRAY);
	line(xa,yf,xf,yf);
	line(xf,yf,xf,ya);
	setcolor(tmp);
}
	

/*========================================================================*/

int gombbe(void){				/* A bioskey a 0x16-os megszak›tﬂst */
int g,ab,fb;					/* haszn-lja. Funkci°ja fÿgg a bioskey*/
g=bioskey(0);					/* aktuﬂlis param⁄ter⁄t˜l(most ez 0). */
ab=g&0x00FF;					/* Ha a bioskey int visszat⁄r⁄si ⁄rt⁄-*/
fb=(g&0xFF00)>>8;				/* k⁄nek als° 8	bitje nem z⁄rus, akkor*/
return((ab==0) ? fb+256 : ab);}	/* a bioskey a BIOS puffer⁄ben sorban
 ﬂll° k˜vetkez˜-, vagy ha a puffer ÿres, akkor a billentyÿzeten bek˜vet-
 kez˜ gombnyomﬂsnak megfelel˜ ASCII karaktert adja vissza. Ha az als° 8
 bit z⁄rus, akkor a fels˜ 8 biten un. kiterjesztett billentyÿzet k°d
 van, amit gy szokﬂs hasznﬂlni, mint ahogy a gombbe rutinban meg›rtuk.*/


int msgbox(char s[])
{
	int kilep = FALSE;
	void *ptr = NULL;
	unsigned int egerx, egery;

	if ((ptr=malloc(imagesize(KOZEP-150, 130, KOZEP+150, 190))) == NULL)
	{
		allapot("Nincs elÇg mem¢ria!");
		if (getch()==0) getch();
		return -1;
	}
	else
	{
		if (EgerMukodik()) EgerKi();
		getimage(KOZEP-150, 130, KOZEP+150, 190, ptr);
		doboz(KOZEP-150, 130, KOZEP+150, 190);
		settextjustify(CENTER_TEXT, TOP_TEXT);
		outtextxy(KOZEP, 145, s);
		outtextxy(KOZEP, 170,"OK");
		_3dmkeret_ki(KOZEP-35, 168, KOZEP+35, 183);
		if (EgerMukodik()) EgerBe();
		if (EgerMukodik()) EgerBe();
		
			while ((!kbhit()) && (!kilep))
			  if (EgerMukodik() && EgerBalGomb())
			{
				EgerPozicio(&egerx, &egery);
				if ((egery > 168) && (egery < 183) && (egerx > KOZEP-35) && (egerx < KOZEP+35)){ 
				  if (EgerMukodik()) EgerKi();
				  _3dmkeret_be(KOZEP-35, 168, KOZEP+35, 183);
				  if (EgerMukodik()) EgerBe();
				  while (EgerBalGomb()); 
				  kilep = TRUE;
			    }
			}
			if (kbhit()) getch();
			if (kbhit()) getch();
			if (EgerMukodik()) EgerKi();
			putimage(KOZEP-150, 130,ptr,COPY_PUT);
			if (EgerMukodik()) EgerBe();
			free(ptr);
			settextjustify(LEFT_TEXT, CENTER_TEXT);
	        return 0;
	}
}


/*========================================================================*/
/*IDEIGLENES!!! nem elfelejteni kitorolni!*/

int fugg(int fvszam)
{
	int i, j;
	struct ffblk bejegyzes;

	switch (fvszam)
	{
		case 1: /*Attributum*/ break;
		case 2: aktneve(akt); nyomtatas(akt); break;
		case 3:
		{
			i = getdisk();
			j = toupper(akt[0])-'A';
			setdisk(j);
			if (getdisk()==j)
			for (i=1; i<3; i++) if ((panel[i].aktiv) && (TRUE))
			{
				strcpy(temp, panel[3-i].utvonal);
				aktneve(temp);
				if (panel[i].start != NULL)
				{
					aktneve(akt);
					findfirst(akt, &bejegyzes, 0x003F);
					if (bejegyzes.ff_attrib == FA_DIREC) /* ha kînyvt†r */
					{
						bekero2(akt, temp, "M†sol†s");
						dircopy(akt, temp);
					}
					else /* ha f†jl az aktu†lis bejegyzÇs */
					{
						bekero2(akt, temp, "M†sol†s");
						fajlmasol(akt, temp);
					}
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					panelvalt(temp);
					paneltorles();
					panel[3-i].maxfajl=dirinfo(panel[3-i].utvonal, panel[3-i].hanyadiktol);
					panelvalt(temp);
				}
			}
			else msgbox("Hiba tîrtÇnt!");
			setdisk(i);
		}
		break;
		case 4:
		{
			aktneve(akt);
			bekeres(akt, "Biztosan tîrli a bejegyzÇst?");
			i = getdisk();
			j = toupper(akt[0])-'A';
			setdisk(j);
			if(j == getdisk())
			{
				if (findfirst(akt, &bejegyzes, 0x003F)!=-1)
				{
					/* ha kînyvt†rt tîrlÅnk */
					if (bejegyzes.ff_attrib==FA_DIREC)
					{
						if (rmdir(akt)==-1) dirdel(akt);
						else msgbox("A kînyvt†r tîrîlve.");
					}
					else /* ha f†jlt */
					{
						if (remove(akt)==-1) msgbox("Hiba tîrtÇnt a f†jl tîrlÇsekor.");
						else msgbox("A f†jl tîrîlve.");
					}
					for (i=1; i<3; i++) if (panel[i].aktiv)
					{
						paneltorles();
						panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
						if (panel[i].aktual>panel[i].maxfajl)
						{
							if (panel[1].aktiv)
								jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-7, 0);
							if (panel[2].aktiv)
								jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-7, 0);
							panel[i].aktual--;
							panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
						}
					}
				}
				else msgbox("Nem tal†lom a bejegyzÇst!");
			}
			setdisk(i);
			szulokonyvtar(akt);
		}
		break;
		case 5:
		{
			for (i=1; i<3; i++) if (panel[i].aktiv)
			{
				strcpy(temp, panel[i].utvonal);
				aktneve(temp);
				strcpy(akt, panel[3-i].utvonal);
				aktneve(akt);
				bekeres(akt, "Adja meg az £j £tvonalat f†jlnÇvvel egyÅtt!");
				if (rename(temp, akt) == 0)
				{
					msgbox("Sikeres †thelyezÇs.");
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					panelvalt(temp);
					paneltorles();
					panel[3-i].maxfajl=dirinfo(panel[3-i].utvonal, panel[3-i].hanyadiktol);
					panelvalt(temp);
					if (panel[i].aktual>panel[i].maxfajl)
					{
						if (panel[1].aktiv)
							jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-7, 0);
						if (panel[2].aktiv)
							jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-7, 0);
						panel[i].aktual--;
						panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					}
				}
				else switch (errno)
				{
					case ENOENT: msgbox("Nem lÇtezî f†jl vagy kînyvt†r!"); break;
					case EACCES: msgbox("Hozz†fÇrÇs megtagadva!"); break;
					case ENOTSAM: msgbox("Nem azonos meghajt¢k!");
				}
				strcpy(akt, panel[i].utvonal);
			}
			statfresh();
		}
		break;
		case 6: /*Futtatas*/ break;
		case 7: nezoke(SUGOFAJL, "S˙gÛ"); break;
		case 8: msgbox("Mi Ìrtuk bizony!"); break;
		case 9: if (eldontendo("Biztosan ki akar lÇpni?")) { ungetch(ESC); }
	}
}

/*=================================================================================================================*/

int fo_menu(int i){
    unsigned x, y;
	int gmb=ENTER, j, k;
	void *ment;
	ment=malloc(imagesize(0,0,100,100));


	while(1){


		/*egerkezeles*/
		if(EgerMukodik()){
			if(EgerBalGomb()){
			   EgerPozicio(&x,&y);
			   if(EgerMukodik()) EgerKi();

			   /*Lenyilo ablakok kezelese*/
			   if (nyitva && axf<x && axa>x && ayf<y && aya>y){
			     for(j=0, k=ayf+ALM_SORMAG; k<y; ++j,k+=ALM_SORMAG);
			     menuk[i].utsotetel=j;
			     if(EgerMukodik()) EgerBe();
			     almenuki(i);
			     while (EgerBalGomb());
			     if(EgerMukodik()) EgerKi();
			     putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	             nyitva=0;
	             _3dkeret_torl(i);
	             if(EgerMukodik()) EgerBe();
			     (menuk[i].tetelek[menuk[i].utsotetel].fv) (menuk[i].tetelek[menuk[i].utsotetel].param);
			     free(ment);
			     return 0;
			   }

			   /*Fomenu kezelese*/
			   else if (x>10 && x<*(fokoord+(FO-1)*2+(0))+*(fokoord+(FO-1)*2+(1)) && y>CIMSOR && y<CIMSOR+MENUSOR) {
			     for(j=0; x>*(fokoord+j*2+(0)) && j<FO; ++j);
			     _3dkeret_torl(i);
			     _3dkeret_be(--j);
			     if(EgerMukodik()) EgerBe();
			     while (EgerBalGomb());
			     if(EgerMukodik()) EgerKi();
			     if (j<AL) {
			       if (nyitva) {
			         if (j==i) {
			           if (i<AL) putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	                   nyitva=0;
	                   _3dkeret_ki(i);
	                 }
	                 else {
	                   if (i<AL) putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	                   _3dkeret_torl(i);
	                   getimage(menuk[j].x,menuk[j].y,menuk[j].x+menuk[j].xm,menuk[j].y+menuk[j].ym,ment);
	                   axf=menuk[j].x; ayf=menuk[j].y; axa=menuk[j].x+menuk[j].xm; aya=menuk[j].y+menuk[j].ym;
	                   menuk[j].utsotetel=0;
	                   _3dkeret_be(j);
	                   almenuki(j);
	                 }
	               }
	               else {
	                 getimage(menuk[j].x,menuk[j].y,menuk[j].x+menuk[j].xm,menuk[j].y+menuk[j].ym,ment);
	                 axf=menuk[j].x; ayf=menuk[j].y; axa=menuk[j].x+menuk[j].xm; aya=menuk[j].y+menuk[j].ym;
	                 menuk[j].utsotetel=0;
	                 _3dkeret_be(j);
	                 almenuki(j);
	                 nyitva=1;
	               }
	             }
	             else {
	               if (nyitva && i<AL) putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	               _3dkeret_torl(j);
	               nyitva=0;
	               if(EgerMukodik()) EgerBe();
	               (fomenu[j].fv) (fomenu[j].param);
	               free(ment);
	               return 0;
			     }
			     i=j;
			   }

			   /*Kilepes a menubol*/
			   else if(y>CIMSOR+MENUSOR) {
			     if (nyitva && i<AL) putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
			     _3dkeret_torl(i);
			     nyitva=0;
			     if(EgerMukodik()) EgerBe();
			     free(ment);
			     return 0;
			   }

            if(EgerMukodik()) EgerBe();
		    }
		}


	  /*billentyukezeles*/
	  
	  switch (gmb) {
	     case LEFT:  {if(EgerMukodik()) EgerKi();
	                  if (i>0) {
						 _3dkeret_torl(i);
						 --i;
						 if (nyitva) {
						   _3dkeret_be(i);
						   j=0;
						   if (i<AL-1) putimage(menuk[i+1].x,menuk[i+1].y,ment,COPY_PUT);
						   if (i<AL) {
						     getimage(menuk[i].x,menuk[i].y,menuk[i].x+menuk[i].xm,menuk[i].y+menuk[i].ym,ment);
						     axf=menuk[i].x; ayf=menuk[i].y; axa=menuk[i].x+menuk[i].xm; aya=menuk[i].y+menuk[i].ym;
						     menuk[i].utsotetel=0;
						     almenuki(i);}}
						 else _3dkeret_ki(i);
					   }
					   if(EgerMukodik()) EgerBe();
					   break;}
		 case RIGHT: {if(EgerMukodik()) EgerKi();
		              if (i<FO-1) {
						 _3dkeret_torl(i);
						 ++i;
						 if (nyitva) {
						   _3dkeret_be(i);
						   if (i<=AL) putimage(menuk[i-1].x,menuk[i-1].y,ment,COPY_PUT);
						   if (i<AL) {
						     getimage(menuk[i].x,menuk[i].y,menuk[i].x+menuk[i].xm,menuk[i].y+menuk[i].ym,ment);
						     axf=menuk[i].x; ayf=menuk[i].y; axa=menuk[i].x+menuk[i].xm; aya=menuk[i].y+menuk[i].ym;
						     menuk[i].utsotetel=0;
						     almenuki(i);}}
						 else _3dkeret_ki(i);
					   }
					   if(EgerMukodik()) EgerBe();
					   break;}
	    case DOWN:  {if(EgerMukodik()) EgerKi();
	                 if (i<AL)
	                    if (!nyitva) {
	                      _3dkeret_be(i);
	                      getimage(menuk[i].x,menuk[i].y,menuk[i].x+menuk[i].xm,menuk[i].y+menuk[i].ym,ment);
	                      axf=menuk[i].x; ayf=menuk[i].y; axa=menuk[i].x+menuk[i].xm; aya=menuk[i].y+menuk[i].ym;
	                      menuk[i].utsotetel=0;
	                      almenuki(i);
	                      nyitva=1;}
	                    else if (menuk[i].utsotetel<menuk[i].tetelszam-1){
	                      ++(menuk[i].utsotetel);
	                      almenuki(i);}
	                  if(EgerMukodik()) EgerBe();
	                  break;}
	    case UP:    {if(EgerMukodik()) EgerKi();
	                 if (i<AL)
	                    if (menuk[i].utsotetel==0) {
	                      putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	                      _3dkeret_ki(i);
	                      nyitva=0;}
	                    else {
	                      --(menuk[i].utsotetel);
	                      almenuki(i);}
	                  if(EgerMukodik()) EgerBe();
	                  break;}
	    case ENTER: {if(EgerMukodik()) EgerKi();
	                 if (i<AL) {
	                    if (!nyitva) {
	                      getimage(menuk[i].x,menuk[i].y,menuk[i].x+menuk[i].xm,menuk[i].y+menuk[i].ym,ment);
	                      axf=menuk[i].x; ayf=menuk[i].y; axa=menuk[i].x+menuk[i].xm; aya=menuk[i].y+menuk[i].ym;
	                      menuk[i].utsotetel=0;
	                      _3dkeret_be(i);
	                      almenuki(i);
	                      nyitva=1;
	                    }
	                    else {
	                      putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	                      nyitva=0;
	                      _3dkeret_torl(i);
	                      if(EgerMukodik()) EgerBe();
	                      (menuk[i].tetelek[menuk[i].utsotetel].fv) (menuk[i].tetelek[menuk[i].utsotetel].param);
	                      free(ment);
	                      return 0;
	                    }
	                  }
	                  else {
	                    nyitva=0;
	                    _3dkeret_torl(i);
	                    if(EgerMukodik()) EgerBe();
	                    (fomenu[i].fv) (fomenu[i].param);
	                    if(EgerMukodik()) EgerBe();
	                    free(ment);
	                    return 0;}
			  if(EgerMukodik()) EgerBe();
			  if(EgerMukodik()) EgerBe();
	                  break;}
	    case ESC:   {if(EgerMukodik()) EgerKi();
	                 if (nyitva) {
	                    putimage(menuk[i].x,menuk[i].y,ment,COPY_PUT);
	                    _3dkeret_ki(i);
	                    nyitva=0;
	                  }
	                  else {
	                    _3dkeret_torl(i);
	                    if(EgerMukodik()) EgerBe();
	                    free(ment);
	                    return 0;
	                  }
	                  if(EgerMukodik()) EgerBe();
	                  break;}
	  }
	  gmb=0;
	  if(kbhit()) gmb=gombbe();
	}
}

