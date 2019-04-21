#include <graphics.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <io.h>
#include <dir.h>
#include <dos.h>
#include <string.h>		/* szîvegkezelÇshez kell */
#include <limits.h>		/* strtonum fÅggvÇny haszn†lja */
#include <ctype.h>		/* karakter†talak°t†sokhoz szÅksÇges */
#include <fcntl.h>		/* a f†jlok bin†ris megnyit†s†hoz kell */
#include <alloc.h>		/* kÇpernyîterÅletek mentÇsÇhez */
#include <errno.h>		/* hibaÅzenetek megjelen°tÇsÇhez */

#include "defs.h"
int x, y;
int eltereles, aktsorszam;
char akt[MAXPATH], temp[MAXPATH], ch;
/*extern menutetel fomenu[], fajl[], szerk[], help[];
extern menutipus menuk[];*/
extern int ABLAKMIN, MENUSOR, STX, STY, CIMSOR, MENUSOR, FO;
extern int *fokoord;

void memfree (int panelszam)
{
	struct bejegyzes *aktualis, *kovetkezo;

	if (panel[panelszam].start != NULL)
	{
		aktualis = panel[panelszam].start;
		do
		{
			kovetkezo = aktualis->kovetkezo;
			free(aktualis);
			aktualis = kovetkezo;
		} while (kovetkezo != NULL);
		panel[panelszam].start = NULL;
	}
}

/* ch az aktu†lis meghajt¢ betÅjelÇt tartalmazza */
int info (struct ffblk *f, char ch)
{
	int i;
	char meghajto[7] = "C:\\ ";
	long int meret;
	struct dfree free;
	static struct bejegyzes *elozo, *aktualis;
	setcolor(BLACK);


	meghajto[0]=toupper(ch);
	/* mem¢riafoglal†s ellenîrzÇssel */
	aktualis = (struct bejegyzes *) malloc (sizeof(struct bejegyzes));
	if (!aktualis)
	{
		outtextxy(x, y, "Nincs elÇg mem¢ria!");
		for (i=1; i<3; i++) if (panel[i].aktiv) memfree(i);
		return -1;
	}

	/* az aktualis listaelem adattagjainak feltîltÇse,
		l†col†si Çs lÇptetÇsi mÅveletek */

	if (aktsorszam == 0) 							/* elsî elem */
	{
		for (i=1; i<3; i++) if (panel[i].aktiv)
		{
			aktualis->kovetkezo = NULL;
			panel[i].start = elozo = aktualis;
			panel[i].start->kovetkezo = NULL;
			strcpy(panel[i].start->fajlnev, f->ff_name);
		}
	}
	else													/* tîbbi elem */
	{
		aktualis->kovetkezo = NULL;
		strcpy(aktualis->fajlnev, f->ff_name);
		/* az £j elem l†ncol†sa az elîzî elemhez */
		elozo->kovetkezo = aktualis;
		/* tov†bblÇpÇs a list†ban */
		elozo = aktualis;
	}
	aktsorszam++;

/*
	for (i=1; i<3; i++)
	if ((panel[i].aktiv) && (panel[i].jelolt)) setcolor(SARGA);
	else setcolor(FEHER);
*/

	moveto(x,y);
	if (f->ff_attrib!=8)									/* ha nem meghajt¢nÇv */
	{
		/* F†jlnÇv kiir†sa */
			outtextxy(x, y, f->ff_name);
			outtext("   ");

		/* D†tum kiir†sa */
			moveto(getx()+75, gety());
			szamSzovir((f->ff_fdate>>9) + 1980, ".");
			if (((f->ff_fdate & 0x1e0)>>5)<10) outtext("0");
			szamSzovir(((f->ff_fdate & 0x1e0)>>5), ".");
			if ((f->ff_fdate & 0x1F)<10) outtext("0");
			szamSzovir((f->ff_fdate & 0x1F), "  ");

		/* Idã kiir†sa */
			if ((f->ff_ftime>>11)<10) outtext("0");
			szamSzovir((f->ff_ftime>>11), ":");

			if (((f->ff_ftime & 0x7E0)>>5)<10) outtext("0");
			szamSzovir((f->ff_ftime & 0x7E0)>>5, ":");

			if (((f->ff_ftime & 0x1F)<<1)<10) outtext("0");
			szamSzovir((f->ff_ftime & 0x1F)<<1, "  ");

		/* MÇret kiir†sa */
			if (f->ff_attrib!=FA_DIREC)					/* ha nem kînyvt†r*/
			{
				meret = f->ff_fsize;
				meretir(meret);
			}
			else outtext("<DIR>");
			moveto(x+KOZEP-35, gety());

		/* Attrib£tum kiir†sa */
			if (f->ff_attrib & FA_RDONLY) outtext("R");
			else outtext("-");
			if (f->ff_attrib & FA_ARCH) outtext("A");
			else outtext("-");
			if (f->ff_attrib & FA_HIDDEN) outtext("H");
			else outtext("-");
			if (f->ff_attrib & FA_SYSTEM) outtext("S");
			else outtext("-");
			ujsor(&y);
		}
		else												/* ha meghajt¢nÇv */
		{
			moveto(x,y+4);
			setcolor(9);
			outtext(meghajto);
			setcolor(1);
			outtext("NÇv: ");
			setcolor(9);
			outtext(f->ff_name);
			getdfree(meghajto[0]-'A'+1, &free);
			if (free.df_sclus != 0xFFFF)
			{
				setcolor(1);
				outtext(" Teljes mÇret: ");
				setcolor(9);
				meret = (long)free.df_total * (long)free.df_bsec * (long)free.df_sclus;
				meretir(meret);
				setcolor(1);
				outtext(" Szabad: ");
				setcolor(9);
				meret = (long)free.df_avail * (long)free.df_bsec * (long)free.df_sclus;
				meretir(meret);
			}
			setcolor(FEHER);
			ujsor(&y);
		}
		return 0;
}



/* kînyvt†rlista megjelen°tÇse, hanyadik elemtîl */
/* a fÅggvÇny a kînyvt†rban levî bejegyzÇsek sz†m†t adja vissza */
int dirinfo(char s[], int hanyadik)
{
	int i, k;
	char temp[MAXPATH];
	struct ffblk f;

	k = 1;
	if (panel[1].aktiv) memfree(1);
	if (panel[2].aktiv) memfree(2);
	strcpy(temp,s);
	strcat(temp,"*.*");
	aktsorszam = 0;
	y = ABLAKMIN;
	if (panel[1].aktiv) x = 6;
	if (panel[2].aktiv) x = KOZEP+6;

	/* inform†ci¢ az aktu†lis meghajt¢r¢l */
	if (findfirst(temp, &f, FA_LABEL)<0)
	{
		setcolor(1);
		outtextxy(x, y+4, "Hib†s, nem lÇtezî, vagy nem kÇsz a meghajt¢.");
		setcolor(FEHER);
		ujsor(&y);
		return -1;
	}
	else info(&f, temp[0]);

	if (findfirst(temp, &f, 0x003F)<0)
	{
		setcolor(3);
		outtextxy(x, y+4, "Nincsenek f†jlok a kînyvt†rban, vagy az elÇrÇsi £t hib†s.");
		setcolor(FEHER);
		k=0;
		return -1;
	}
	else
	{
		/* ha a gyîkÇrkînyvt†rat kÇrjÅk le, m†r ki°rtuk azt, ezÇrt tov†bblÇpÅnk */
		if (f.ff_attrib == FA_LABEL) findnext(&f);
		for (i=0; i<hanyadik; i++) findnext(&f);
		if (y<MAXY-STATUSZ-SORTAV+3) info(&f, temp[0]);
	}
	while (findnext(&f)==0)
	{
		k++;
		if (y<MAXY-STATUSZ-SORTAV+3) info(&f, temp[0]);
	}
	k+=hanyadik;

	/* jelolo elhelyezese */
	if (panel[1].aktiv)
		jelolo(x-2, ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 1);
	if (panel[2].aktiv)
		jelolo(x-2, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 1);


	return k;
}

void statfresh (void)	/* st·tuszsor frissÌtÈse */
{
	int i;

	setfillstyle(1,WHITE);
	bar(50, MAXY-2*MENUSOR+1, MAXX-5, MAXY-MENUSOR-5);
	for (i=1; i<3; i++)
	if (panel[i].aktiv) outtextxy(STX, STY, panel[i].utvonal);
}


void panelvalt (char akt[])
{
	if (EgerMukodik()) EgerKi();
	/* l†that¢-e az adott panelen a jelîlî */
	if (panel[1].aktiv)
	{
		/* az elsî panelrîl levesszÅk a kijelîlît, a m†sodikon megjelen°tjÅk */
		jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 0);
		jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 1);
		panel[1].aktiv = FALSE;
		panel[2].aktiv = TRUE;
		strcpy(akt, panel[2].utvonal);
}
	else
	{
		/* az m†sodik panelrîl levesszÅk a kijelîlît, az elsîn megjelen°tjÅk */
		jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 0);
		jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 1);
		panel[2].aktiv = FALSE;
		panel[1].aktiv = TRUE;
		strcpy(akt, panel[1].utvonal);
	}
	statfresh();
	if (EgerMukodik()) EgerBe();
}

void paneltorles (void)
{
	if (EgerMukodik()) EgerKi();
	setfillstyle(1,WHITE);
	if (panel[1].aktiv) bar(4,CIMSOR+MENUSOR+4,KOZEP-4,MAXY-2*MENUSOR-4);
	if (panel[2].aktiv) bar(KOZEP+4,CIMSOR+MENUSOR+4,MAXX-5,MAXY-2*MENUSOR-4);
	if (EgerMukodik()) EgerBe();
}




/* az aktualis panel aktualis elemÇnek nevÇt adja vissza */
void aktneve(char s[])
{
	int i, j;
	struct bejegyzes *aktual;

	for (i=1; i<3; i++) if ((panel[i].aktiv) && (panel[i].start != NULL))
	{
		aktual = panel[i].start;	 /* legelsî ki°rt bejegyzÇs */
		for (j=0; j<panel[i].aktual; j++) aktual = aktual->kovetkezo;
		strcat(s, aktual->fajlnev);
	}
}

/* s bejegyzes szÅlîkînyvt†r†t adja vissza, ha s kînyvt†r,
egyÇbkÇnt azt a kînyvt†rat, amiben a f†jl tal†lhat¢ */
void szulokonyvtar(char s[])
{
	int i;

	if (((i=strlen(s))>4) && (s[i-1]=='\\')) s[i-1]=0;
	if (i>3)
	{
		while ((s[--i]!='\\') && (i>0)) s[i]=0;
	}
}

void allapot (char s[])
{
	if (EgerMukodik()) EgerKi();
	setfillstyle(1,WHITE);
	bar(50, MAXY-2*MENUSOR+1, MAXX-5, MAXY-MENUSOR-5);
	outtextxy(STX, STY, s);
	if (EgerMukodik()) EgerBe();
	if (getch()==0) getch();
	statfresh();
}

/* rekurz°v kînyvt†rtîrlÇs */
int dirdel(char *dir)
{
	struct ffblk ffblk;
	int kesz=0;

	chdir(dir);
	strcat(dir, "\\");
	if (findfirst("*.*", &ffblk, 0x00F3)==-1) return -1;
	/* a . Çs a .. bejegyzÇs †tlÇpÇse */
	if (ffblk.ff_name[0]=='.') kesz = findnext(&ffblk);
	if (ffblk.ff_name[0]=='.') kesz = findnext(&ffblk);

	while (!kesz)
	{
		/* belÇpÇs a kîvetkezî kînyvt†rba */
		if (ffblk.ff_attrib==FA_DIREC)
		{
			if (dir[strlen(dir)-1]!='\\') strcat(dir, "\\");
			strcat(dir, ffblk.ff_name);
			if (dirdel(dir)==-1) return -1;
		}
		else
		{
			if (remove(ffblk.ff_name)==-1)
			{
				msgbox("Nem sikerÅlt a tîrlÇs:");
				/*msgbox(ffblk.ff_name);*/
				return -1;
			}
			else { /*msgbox("F†jl tîrîlve."); msgbox(ffblk.ff_name);*/}
		}
		/* kîvetkezî bejegyzÇs keresÇse */
		kesz = findnext(&ffblk);
	}
	/* visszalÇpÇs */
	chdir("..");
	if (dir[strlen(dir)-1]=='\\') dir[strlen(dir)-1]=0;
	if (rmdir(dir)==-1)
	{
		msgbox("Nem sikerÅlt a tîrlÇs.");
		/*msgbox(ffblk.ff_name);*/
		return -1;
	}
	else { /*msgbox("Kînyvt†r tîrîlve."); msgbox(dir); */}
	szulokonyvtar(dir);
	return 0;
}

/* F7 - £j kînyvt†r lÇtrehoz†sa */
void ujkonyvtar(char akt[])
{
	int i;

	if (!mkdir(akt))
	{
		msgbox("A kînyvt†r lÇtrejîtt.");
		for (i=1; i<3; i++) if (panel[i].aktiv)
		{
			paneltorles();
			panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
		}
	}
	else switch (errno)
	{
		case EACCES: msgbox("Hozz†fÇrÇs megtagadva!"); break;
		case ENOENT: msgbox("Nem lehet lÇtrehozni a kînyvt†rat!");
	}
	szulokonyvtar(akt);
	statfresh();
}

int fajlmasol(char akt[], char uj[])
{
	char ch;
	FILE *fp, *nfp;

	if ((fp=fopen(akt, "rb")) != NULL)
	{
		if ((nfp=fopen(uj, "wb")) != NULL)
		{
			while ((ch=fgetc(fp))!=EOF) fputc(ch, nfp);
			szulokonyvtar(akt);
			if (panel[1].aktiv) strcpy(panel[1].utvonal, akt);
			if (panel[2].aktiv) strcpy(panel[2].utvonal, akt);
		}
		else
		{
			msgbox("Nem lehet lÇtrehozni az £j f†jl!");
			return -1;
		}
		fclose(fp);
		fclose(nfp);
	}
	else { msgbox("Nincs meg a f†jl!"); return -1; }
	return 0;
}

int dircopy(char *honnan, char *hova)
{
struct ffblk ffblk;
int kesz=0;

/*strcat(honnan, "\\");*/
mkdir(hova);
strcat(hova,"\\");

	chdir(honnan);
	strcat(honnan, "\\");
	if (findfirst("*.*", &ffblk, 0x00F3)==-1) return -1;
	/* a . Çs a .. bejegyzÇs †tlÇpÇse */
	if (ffblk.ff_name[0]=='.') kesz = findnext(&ffblk);
	if (ffblk.ff_name[0]=='.') kesz = findnext(&ffblk);

	while (!kesz)
	{
		/* belÇpÇs a kîvetkezî kînyvt†rba */
		if (ffblk.ff_attrib==FA_DIREC)
		{
			strcat(honnan, ffblk.ff_name);
			strcat(hova, ffblk.ff_name);
			mkdir(hova);
			if (dircopy(honnan, hova)==-1) return -1;
		}
		else
		{
			strcat(hova, ffblk.ff_name);
			strcat(honnan, ffblk.ff_name);
			if (fajlmasol(honnan, hova)==-1)
			{
				msgbox("Nem sikerÅlt a m†sol†s.");
				/*msgbox(ffblk.ff_name);*/
				return -1;
			}
			else { /*msgbox("A m†sol†s sikeres volt."); msgbox(ffblk.ff_name);*/}
			szulokonyvtar(hova);
		}
		/* kîvetkezî bejegyzÇs keresÇse */
		kesz = findnext(&ffblk);
	}
	/* visszalÇpÇs */
	szulokonyvtar(hova);
	szulokonyvtar(honnan);
	return 0;
}

void text(int also, char s[], char ch)
{
       int x = 150;

       if (EgerMukodik()) EgerKi();
       if (also) x += 30;
       setcolor(BLACK);
       if (strlen(s)>0) if (ch==8/*BACKSPACE*/)
       {
               bar(KOZEP-245, x, KOZEP+235, x+15);
               s[strlen(s)-1]=0;
               outtextxy(KOZEP-243, x+2, s);
       }
       if (strlen(s) < MAXPATH-1)
       {
               if (ch==0) getch();
               if ((ch!=13/*ENTER*/) && (ch!=27/*ESC*/) && (ch!=8/*BACKSPACE*/) && (ch!=0))
               {
                       bar(KOZEP-245, x, KOZEP+245, x+15);
                       s[strlen(s)+1] = 0;
                       s[strlen(s)] = ch;
                       outtextxy(KOZEP-243, x+2, s);
               }
       }
       if (EgerMukodik()) EgerBe();
}

int bekero2 (char felso[], char also[], char cim[])
{
       int dik=1, kilep = FALSE;
       char ch;
       void *ptr = NULL;
       unsigned int egerx, egery;

       if ((ptr=malloc(imagesize(KOZEP-260, 120, KOZEP+260, 250))) == NULL)
       {
	       allapot("Nincs elÇg mem¢ria!");
               if (getch()==0) getch();
               return -1;
       }
       else
       {
               if (EgerMukodik()) EgerKi();
               getimage(KOZEP-260, 120, KOZEP+260, 250, ptr);
               doboz(KOZEP-260, 120, KOZEP+260, 250);
               settextjustify(CENTER_TEXT, TOP_TEXT);
               outtextxy(KOZEP, 135, cim);
               setfillstyle(1,WHITE);
               bar(KOZEP-245, 150, KOZEP+245, 165);
               bar(KOZEP-245, 180, KOZEP+245, 195);
               setcolor(BLUE);
               rectangle(KOZEP-246, 149, KOZEP+246, 166);
               setcolor(DARKGRAY);
               rectangle(KOZEP-246, 179, KOZEP+246, 196);
               setcolor(BLACK);
               _3dmkeret_ki(KOZEP-100, 220, KOZEP-30, 235);
               outtextxy(KOZEP-65, 221, "OK - Enter");
	       _3dmkeret_ki(KOZEP+30, 220, KOZEP+100, 235);
	       setcolor(BLACK);
	       outtextxy(KOZEP+65, 221, "MÇgse - Esc");
               settextjustify(LEFT_TEXT, TOP_TEXT);
               outtextxy(KOZEP-243, 152, felso);
               outtextxy(KOZEP-243, 182, also);
               if (EgerMukodik()) EgerBe();
               if (EgerMukodik()) EgerBe();

               do
               {
		       kilep = FALSE;
		       while ((!kbhit()) && (!kilep)) if (EgerMukodik() && EgerBalGomb())
		       {
                               /*felso*/
                               EgerPozicio(&egerx, &egery);
                               if ((egery > 150) && (egery < 165)
                               && (egerx > KOZEP-245) && (egerx < KOZEP+245))
                                       { if (dik==2) ungetch(9); kilep=TRUE; }
                               /*also*/
                               if ((egery > 180) && (egery < 195)
                               && (egerx > KOZEP-245) && (egerx < KOZEP+245))
                                       { if (dik==1) ungetch(9); kilep=TRUE; }
                               /*OK*/
                               if ((egery > 220) && (egery < 237)
                               && (egerx > KOZEP-100) && (egerx < KOZEP-30))
                               {   if (EgerMukodik()) EgerKi();
                                       _3dmkeret_be(KOZEP-100, 220, KOZEP-30, 235);
                                       if (EgerMukodik()) EgerBe();
                                   while (EgerBalGomb());
                                       ungetch(13/*ENTER*/);
                                       kilep = TRUE;
                               }
                               /*megse*/
                               if ((egery > 220) && (egery < 237)
                               && (egerx > KOZEP+30) && (egerx < KOZEP+100))
                           {   if (EgerMukodik()) EgerKi();
                                       _3dmkeret_be(KOZEP+30, 220, KOZEP+100, 235);
                                       if (EgerMukodik()) EgerBe();
                                       while (EgerBalGomb());
                                       ungetch(27/*ESC*/);
                                       kilep = TRUE; }
                               }
			       ch=getch();
                               if (ch==9/*TAB*/)
                               {
                                       dik = 3-dik;
                                       if (EgerMukodik()) EgerKi();
                                       if (dik==1) {
                                         setcolor(BLUE);
                                         rectangle(KOZEP-246, 149, KOZEP+246, 166);
                                         setcolor(DARKGRAY);
                                         rectangle(KOZEP-246, 179, KOZEP+246, 196);
                                       }
                                       if (dik==2) {
                                         setcolor(DARKGRAY);
                                         rectangle(KOZEP-246, 149, KOZEP+246, 166);
                                         setcolor(BLUE);
                                         rectangle(KOZEP-246, 179, KOZEP+246, 196);
                                       }
                                       if (EgerMukodik()) EgerBe();
                               }
                               else
                               {
				       if (dik==1) text(FALSE, felso, ch);
				       if (dik==2) text(TRUE, also, ch);
                               }
               } while ((ch!=27/*ESC*/) && (ch!=13/*ENTER*/));
               if (EgerMukodik()) EgerKi();
               putimage(KOZEP-260, 120, ptr, COPY_PUT);
               free(ptr);
               if (EgerMukodik()) EgerBe();
       }
       settextjustify(LEFT_TEXT,CENTER_TEXT);
       if (ch == /*ESC*/27) return -1;
       return ch;
}

/* s-hez hozz†fÅzi a bekÇrt szîveget, bekero-ben van, amit ki°r */
int bekeres (char s[], char bekero[])
{
	int kilep = FALSE;
	char ch;
	void *ptr = NULL;
	unsigned int egerx, egery;

	if ((ptr=malloc(imagesize(KOZEP-260, 120, KOZEP+260, 200))) == NULL)
	{
		msgbox("Nincs elÇg mem¢ria!");
		if (getch()==0) getch();
		return -1;
	}
	else
	{
		getimage(KOZEP-260, 120, KOZEP+260, 200, ptr);
		doboz(KOZEP-260, 120, KOZEP+260, 200);
		settextjustify(CENTER_TEXT, TOP_TEXT);
		outtextxy(KOZEP, 135, bekero);
		setfillstyle(1,WHITE);
		bar(KOZEP-245, 150, KOZEP+245, 165);
		setcolor(DARKGRAY);
		rectangle(KOZEP-246,149,KOZEP+246,166);
		setcolor(BLACK);
		_3dmkeret_ki(KOZEP-100, 170, KOZEP-30, 185);
		outtextxy(KOZEP-65, 171, "OK - Enter");
		_3dmkeret_ki(KOZEP+30, 170, KOZEP+100, 185);
		outtextxy(KOZEP+65, 171, "MÇgse - Esc");
		settextjustify(LEFT_TEXT, TOP_TEXT);
		outtextxy(KOZEP-243, 152, s);


		do
		{
			if (EgerMukodik()) EgerBe();
			while ((!kbhit()) && (!kilep))
			if (EgerMukodik() && EgerBalGomb())
			{
				EgerPozicio(&egerx, &egery);
				if ((egery > 170) && (egery < 185) && (egerx > KOZEP-100) && (egerx < KOZEP-30)){
				  _3dmkeret_be(KOZEP-100, 170, KOZEP-30, 185);
				  if (EgerMukodik()) EgerBe();
				  while (EgerBalGomb());
				  if (EgerMukodik()) EgerKi();
				  ungetch(ENTER);
				  kilep = TRUE;
				}
				if ((egery > 170) && (egery < 185) && (egerx > KOZEP+30) && (egerx < KOZEP+100)) {
				  _3dmkeret_be(KOZEP+30, 170, KOZEP+100, 185);
				  if (EgerMukodik()) EgerBe();
				  while (EgerBalGomb());
				  if (EgerMukodik()) EgerKi();
				  ungetch(ESC);
				  kilep = TRUE;
				}
			}
			ch=getch();
			text(FALSE, s, ch);
		} while ((ch!=ESC) && (ch!=ENTER));
		putimage(KOZEP-260, 120, ptr, COPY_PUT);
		free(ptr);
		if (EgerMukodik()) EgerBe();

	}
	if (EgerMukodik()) EgerBe();
	settextjustify(LEFT_TEXT, CENTER_TEXT);
	if (ch == ESC) return -1;
	return ch;
}

/* a visszatÇrÇsi ÇrtÇk a meghajt¢ betÅjelÇt adja meg */
char meghajtovalasztas(void)
{
	int i, j = 0, mo = 0;
	char ch, meghajtok[27], temp[50];
	void *ptr = NULL;
	struct ffblk bejegyzes;

	for (i=0; i<26; i++)
	{
		setdisk(i);
		if (i == getdisk()) meghajtok[j++] = 'A' + i;
	}
	meghajtok[j]=0;

	if ((ptr=malloc(imagesize(70+mo, ABLAKMIN+SORTAV, 250+mo, ABLAKMIN+SORTAV+strlen(meghajtok) * SORTAV))) == NULL)
	{
		msgbox("Nincs elÇg mem¢ria! Pr¢b†lja az †llapotsort!");
		return 200;
	}
	else
	{
		if (panel[2].aktiv) mo = KOZEP;
		getimage(70+mo, ABLAKMIN+SORTAV, 250+mo, ABLAKMIN+SORTAV+strlen(meghajtok)*SORTAV, ptr);
		setfillstyle(1,3);
		bar(70+mo, ABLAKMIN+SORTAV, 250+mo, ABLAKMIN+SORTAV+strlen(meghajtok)*SORTAV);
		setcolor(FEKETE);
		settextjustify(CENTER_TEXT, TOP_TEXT);
		for (i=0; i<strlen(meghajtok); i++)
		{
			temp[0] = meghajtok[i]; temp[1] = 0;
			strcat(temp, ":\\*.*");
			if (findfirst(temp, &bejegyzes, FA_LABEL)!=-1)
			{
				temp[3]=0; strcat(temp, " - ");
				strcat(temp, bejegyzes.ff_name);
			}
			else
			{
				temp[3]=0;
				strcat(temp, " - Nincs kÇsz olvas†sra!");
			}
			outtextxy(KOZEP/2+mo, ABLAKMIN+SORTAV+i*SORTAV, temp);
		}

		do
		{
			ch = toupper(getch());
			if (ch==0) { getch(); ch = 1; }
			if (strchr(meghajtok, ch)) meghajtok[0] = ch;
		} while (!strchr(meghajtok, ch));

		putimage(70+mo, ABLAKMIN+SORTAV, ptr, COPY_PUT);
		free(ptr);
		setcolor(FEHER);
		settextjustify(LEFT_TEXT, TOP_TEXT);
	}
	return meghajtok[0];
}

/* a visszatÇrÇsi ÇrtÇk igaz vagy hamis lehet */
int eldontendo (char kerdes[])
{
	char ch;
	int kilep = FALSE;
	unsigned int egerx = 160, egery = KOZEP;
	void *ptr = NULL;

	if ((ptr=malloc(imagesize(KOZEP-160, 120, KOZEP+160, 190))) == NULL)
	{
		msgbox("Nincs elÇg mem¢ria!");
		if (getch()==0) getch();
		return FALSE;
	}
	else
	{
		getimage(KOZEP-160, 120, KOZEP+160, 190, ptr);
		setfillstyle(1,3);
		bar(KOZEP-160, 120, KOZEP+160, 190);
		setcolor(FEKETE);
		rectangle(KOZEP-155, 125, KOZEP+155, 185);
		rectangle(KOZEP-150, 130, KOZEP+150, 180);
		settextjustify(CENTER_TEXT, TOP_TEXT);
		outtextxy(KOZEP, 135, kerdes);
		rectangle(KOZEP-105, 155, KOZEP-25, 170);
		rectangle(KOZEP+25, 155, KOZEP+105, 170);
		rectangle(KOZEP+25, 155, KOZEP+105, 170);
		line(KOZEP-160, 190, KOZEP+160, 190);
		line(KOZEP+160, 120, KOZEP+160, 190);
		setcolor(FEHER);
		line(KOZEP-160, 120, KOZEP+160, 120);
		line(KOZEP-160, 120, KOZEP-160, 190);
		line(KOZEP-105, 155, KOZEP-25, 155);
		line(KOZEP-105, 155, KOZEP-105, 170);
		line(KOZEP+25, 155, KOZEP+105, 155);
		line(KOZEP+25, 155, KOZEP+25, 170);
		setcolor(FEKETE);
		outtextxy(KOZEP-65, 157, "Igen - Enter");
		outtextxy(KOZEP+65, 157, "Nem - Esc");

		if (EgerMukodik()) EgerBe();
		do
		{
			while ((!kbhit()) && (!kilep))
			{
				if (EgerMukodik() && EgerKattintas())
				{
					EgerPozicio(&egerx, &egery);
					if ((egerx < KOZEP-25) && (egerx > KOZEP-105)
					&& (egery < 170) && (egery > 155))
					{
						ungetch(ENTER);
						kilep = TRUE;
					}
					if ((egerx < KOZEP+105) && (egerx > KOZEP+25)
					&& (egery < 170) && (egery > 155))
					{
						ungetch(ESC);
						kilep = TRUE;
					}
				}
			}
			ch = getch();
			if (ch == 0) getch();
		} while ((ch!=ESC) && (ch!=ENTER));
		if (EgerMukodik()) EgerKi();

		putimage(KOZEP-160, 120, ptr, COPY_PUT);
		free(ptr);
		setcolor(FEHER);
		settextjustify(LEFT_TEXT, TOP_TEXT);
	}
	if (ch==ESC) return FALSE;
	return TRUE;
}

void restoregraphmode (char akt[])
{
	cleardevice();
	settextstyle(SMALL_FONT, HORIZ_DIR, MERET);
	felulet();
	if (panel[1].aktiv)
	{
		panel[1].maxfajl = dirinfo(panel[1].utvonal, panel[1].hanyadiktol);
		panelvalt(akt);
		panel[2].maxfajl = dirinfo(panel[2].utvonal, panel[2].hanyadiktol);
		panelvalt(akt);
	}
	if (panel[2].aktiv)
	{
		panel[2].maxfajl = dirinfo(panel[2].utvonal, panel[2].hanyadiktol);
		panelvalt(akt);
		panel[1].maxfajl = dirinfo(panel[1].utvonal, panel[1].hanyadiktol);
		panelvalt(akt);
	}
}

/**************************************************************************/
/*                         N Y O M T A T A S                              */
/**************************************************************************/
void kinyomtat(char fajlnev[])
{
	char sor[80];
	FILE *fp;

	if (stdprn == NULL)	msgbox("Ellenîrizze a nyomtat¢t.");
	else
		{
			if ((fp=fopen(fajlnev, "r")) == NULL) msgbox("Hiba lÇpett fel a f†jl megnyit†sa kîzben.");
			else
			{
				while (!feof(fp))
				{
					sor[0]=0;
					fgets(sor, 80, fp);
					fwrite(sor, strlen(sor), 1, stdprn);
				}
				fclose(fp);
			}
		}
}

void nyomtatas(char fajlnev[])
{
	void *ptr = NULL;
	char ch;
	long int kilep = FALSE;
	unsigned int egerx, egery;

	if (EgerMukodik()) EgerKi();
	if ((ptr=malloc(imagesize(KOZEP-250, 70, KOZEP+250, 70+4*SORTAV+2))) == NULL)
	{
		msgbox("Nincs elÇg mem¢ria!");
		if (getch()==0) getch();
	}
	else
	{
		getimage(KOZEP-250, 70, KOZEP+250, 70+4*SORTAV+2, ptr);
		setfillstyle(1, 3);
		setcolor(FEKETE);
		bar(KOZEP-250, 70, KOZEP+250, 70+4*SORTAV+2);
		settextjustify(CENTER_TEXT, TOP_TEXT);
		outtextxy(KOZEP, 70, "Nyomtat†s");
		outtextxy(KOZEP, 70+3*SORTAV+1, "OK");
		outtextxy(KOZEP+235, 70, "X");
		settextjustify(LEFT_TEXT, TOP_TEXT);
		setfillstyle(1, 4);
		line(KOZEP+250, 70, KOZEP+250, 70+4*SORTAV+2);
		line(KOZEP-250, 70+4*SORTAV+2, KOZEP+250, 70+4*SORTAV+2);
		outtextxy(KOZEP-245, 70+SORTAV, "Nyotat†s alatt:");
		outtextxy(KOZEP-245, 70+2*SORTAV, fajlnev);
		line(KOZEP-20, 70+3*SORTAV+1, KOZEP+20, 70+3*SORTAV+1);
		line(KOZEP-20, 70+3*SORTAV+1, KOZEP-20, 70+4*SORTAV-2);
		setcolor(FEHER);
		line(KOZEP-250, 70, KOZEP+250, 70);
		line(KOZEP-250, 70, KOZEP-250, 70+4*SORTAV);
		line(KOZEP-20, 70+4*SORTAV-2, KOZEP+20, 70+4*SORTAV-2);
		line(KOZEP+20, 70+3*SORTAV+1, KOZEP+20, 70+4*SORTAV-2);

		if (EgerMukodik()) EgerBe();

		if (EgerMukodik()) EgerBe();
		do
		{
			while ((!kbhit()) && (!kilep))
			if (EgerMukodik() && EgerKattintas())
			{
				EgerPozicio(&egerx, &egery);
				if ((egery > 70+3*SORTAV+1) && (egery < 70+4*SORTAV-2)
				&& (egerx > KOZEP-20) && (egerx < KOZEP+20))
						 kinyomtat(fajlnev);
				if ((egery > 70) && (egery < 68+SORTAV)
				&& (egerx > KOZEP+230) && (egerx < KOZEP+240))
					{ kilep = TRUE; ungetch(ESC); }
			}
			ch = getch();
			switch (ch)
			{
					case 0: getch();
					case ENTER: kinyomtat(fajlnev); break;
			}
		} while (ch != ESC);

		if (EgerMukodik()) EgerKi();
		putimage(KOZEP-250, 70, ptr, COPY_PUT);
		free(ptr);
		if (EgerMukodik()) EgerBe();
	}
}

/**************************************************************************/
/*                      N Y O M T A T A S   V E G E                       */
/**************************************************************************/

/**************************************************************************/
/*                            N E Z O K E                                 */
/**************************************************************************/

long int fajlsorir(const char fajlnev[], long int sorszam)
{
	int j = 0;
	FILE *fp=fopen(fajlnev,"r");
	char sor[80];
	signed long int i=1;

	if (EgerMukodik()) EgerKi();
	setfillstyle(1, 7);
	bar(KOZEP-242, 70+SORTAV, KOZEP+242, 70+SUGOSOR*SORTAV);
	if (fp == NULL)
	{
		setcolor(FEHER);
		msgbox("Hiba lÇpett fel a f†jl megnyit†sa kîzben.");
		return -1;
	}
	else
	{
		while ((i!=sorszam) && (!feof(fp)))
		{
			sor[0]=0;
			fgets(sor,80,fp);
			i++;
		}
		while ((!feof(fp)) && (j < SUGOSOR-1))
		{
			j++;
			if (fgets(sor, 80, fp)!=NULL) outtextxy(KOZEP-240, 70+j*SORTAV, sor);
			if (strlen(sor)==80) { j++; moveto(KOZEP-240, 70+j*SORTAV); }
		}
		fclose(fp);
	}
	if (EgerMukodik()) EgerBe();
	return 0;
}

void nezoke(char fajlnev[], char cim[])
{
       long int tol = 1, fhossz = 0, kilep = FALSE;
       char ch, ch2;
       char puff[80];
       unsigned int egerx, egery;
       void *ptr = NULL;
       FILE *fp = fopen(fajlnev, "rb");

       if (EgerMukodik()) EgerKi();
       if ((ptr=malloc(imagesize(KOZEP-250, 70, KOZEP+250, 75+(SUGOSOR+1)*SORTAV))) == NULL)
       {
               msgbox("Nincs elÇg memcria!");
               if (getch()==0) getch();
       }
       else
       {
               strcpy(puff, cim);
               strcat(puff, " - ");
               strcat(puff, fajlnev);
               getimage(KOZEP-250, 70, KOZEP+250, 75+(SUGOSOR+1)*SORTAV, ptr);
               setcolor(FEKETE);
               doboz(KOZEP-250, 70, KOZEP+250, 75+(SUGOSOR+1)*SORTAV);
               settextjustify(CENTER_TEXT, TOP_TEXT);
               outtextxy(KOZEP, 70, puff);
               outtextxy(KOZEP+236, 69, "X");
               _3dmkeret_ki(KOZEP+232,70,KOZEP+241,80);
               settextjustify(LEFT_TEXT, TOP_TEXT);
               setfillstyle(1,WHITE);
               _3dmkeret_ki(KOZEP+250-81, 73+SUGOSOR*SORTAV, KOZEP+195, 73+(SUGOSOR+1)*SORTAV);
               _3dmkeret_ki(KOZEP+250-41, 73+SUGOSOR*SORTAV, KOZEP+235, 73+(SUGOSOR+1)*SORTAV);
               _3dmkeret_ki(KOZEP-250+8, 73+SUGOSOR*SORTAV, KOZEP-250+38, 73+(SUGOSOR+1)*SORTAV);
               _3dmkeret_ki(KOZEP-250+48, 73+SUGOSOR*SORTAV, KOZEP-250+78, 73+(SUGOSOR+1)*SORTAV);
               outtextxy(KOZEP+174, 71+SUGOSOR*SORTAV+3, "Fel");
               outtextxy(KOZEP+216, 71+SUGOSOR*SORTAV+3, "Le");
               outtextxy(KOZEP-250+12,71+SUGOSOR*SORTAV+3, "PgUP" );
               outtextxy(KOZEP-250+52,71+SUGOSOR*SORTAV+3, "PgDW" );
               setcolor(DARKGRAY);
               rectangle(KOZEP-243, 77+SUGOSOR, KOZEP+243,70+SUGOSOR*SORTAV+1);
               setcolor(FEKETE);
               if (EgerMukodik()) EgerBe();

               if (fp == NULL)
               {
                       setcolor(FEHER);
                       msgbox("Hiba lÇpett fel a fjl megnyitsa kîzben.");
               }
               else
               {
                       if (EgerMukodik()) EgerBe();
                       while (!feof(fp))
                       {
                               fgets(puff, 80, fp);
                               fhossz++;
                       }
                       fclose(fp);
                       fajlsorir(fajlnev, tol);
                       do
                       {
                               while ((!kbhit()) && (!kilep))
                               if (EgerMukodik() && EgerBalGomb())
                               {
                                       EgerPozicio(&egerx, &egery);
                                       /*fel*/
                                       if ((egery > 73+SUGOSOR*SORTAV) && (egery < 73+(SUGOSOR+1)*SORTAV)
                                       && (egerx > KOZEP+250-81) && (egerx < KOZEP+195)) {
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_be(KOZEP+250-81, 73+SUGOSOR*SORTAV, KOZEP+195, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               while (EgerBalGomb());
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_ki(KOZEP+250-81, 73+SUGOSOR*SORTAV, KOZEP+195, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               if (tol > 1) fajlsorir(fajlnev, --tol);
                                       }
                                       /*le*/
                                       if ((egery > 73+SUGOSOR*SORTAV) && (egery < 73+(SUGOSOR+1)*SORTAV)
                                       && (egerx > KOZEP+250-41) && (egerx < KOZEP+235)) {
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_be(KOZEP+250-41, 73+SUGOSOR*SORTAV, KOZEP+235, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               while (EgerBalGomb());
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_ki(KOZEP+250-41, 73+SUGOSOR*SORTAV, KOZEP+235, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               if(tol < fhossz-SUGOSOR+1) fajlsorir(fajlnev, ++tol);
                                       }
                                       /*kilep*/
                                       if ((egery > 70) && (egery < 80)
                                       && (egerx > KOZEP+232) && (egerx < KOZEP+241)) {
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_be(KOZEP+232,70,KOZEP+241,80);
                                               if (EgerMukodik()) EgerBe();
                                               while (EgerBalGomb());
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_ki(KOZEP+232,70,KOZEP+241,80);
                                               if (EgerMukodik()) EgerBe();
                                               kilep = TRUE;
                                               ungetch(ESC);
                                       }
                                       /*pgup*/
                                       if ((egery > 73+SUGOSOR*SORTAV) && (egery < 73+(SUGOSOR+1)*SORTAV)
                                       && (egerx > KOZEP-250+8) && (egerx < KOZEP-250+38)) {
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_be(KOZEP-250+8, 73+SUGOSOR*SORTAV, KOZEP-250+38, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               while (EgerBalGomb());
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_ki(KOZEP-250+8, 73+SUGOSOR*SORTAV, KOZEP-250+38, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               if (tol > SUGOSOR) fajlsorir(fajlnev, tol-=(SUGOSOR-1));
                                               else fajlsorir(fajlnev, tol=1);
                                       }
                                       /*pgdwn*/
                                       if ((egery > 73+SUGOSOR*SORTAV) && (egery < 73+(SUGOSOR+1)*SORTAV)
                                       && (egerx > KOZEP-250+48) && (egerx < KOZEP-250+78)) {
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_be(KOZEP-250+48, 73+SUGOSOR*SORTAV, KOZEP-250+78, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               while (EgerBalGomb());
                                               if (EgerMukodik()) EgerKi();
                                               _3dmkeret_ki(KOZEP-250+48, 73+SUGOSOR*SORTAV, KOZEP-250+78, 73+(SUGOSOR+1)*SORTAV);
                                               if (EgerMukodik()) EgerBe();
                                               if (tol < fhossz-SUGOSOR) fajlsorir(fajlnev, tol+=(SUGOSOR-1));
                                               else fajlsorir(fajlnev, tol=fhossz-SUGOSOR+1);
                                       }
                               }

                               ch = getch();
                               if (ch==0) ch2=getch();
                               switch (ch2)
                               {
                                       case 72: if (tol > 1) fajlsorir(fajlnev, --tol); break;
                                       case 80: if (tol < fhossz-SUGOSOR+1) fajlsorir(fajlnev, ++tol); break;
                                       case 73:
                                               if (tol > SUGOSOR) fajlsorir(fajlnev, tol-=(SUGOSOR-1));
                                               else fajlsorir(fajlnev, tol=1); break;
                                       case 81:
                                               if (tol < fhossz-SUGOSOR) fajlsorir(fajlnev, tol+=(SUGOSOR-1));
                                               else fajlsorir(fajlnev, tol=fhossz-SUGOSOR+1);

                               }
                       } while (ch != ESC);
               }
               if (EgerMukodik()) EgerKi();
               putimage(KOZEP-250, 70, ptr, COPY_PUT);
               free(ptr);
               if (EgerMukodik()) EgerBe();
               setcolor(FEHER);
       }
}


/**************************************************************************/
/*                          N E Z O K E    V E G E                        */
/**************************************************************************/

void atnevezes(char akt[])
{
	int i;
	char temp[MAXPATH];

	if (bekeres(akt, "Adja meg az £j f†jlnevet!") != -1)
		for (i=1; i<3; i++) if (panel[i].aktiv)
		{
			strcpy(temp, panel[i].utvonal);
			aktneve(temp);
			if (rename(temp, akt) == 0)
			{
				msgbox("Sikeres atnevezes");
				paneltorles();
				panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
			}
			else msgbox("Nem sikerÅlt az †tnevezÇs!");
			strcpy(akt, panel[i].utvonal);
		}
	statfresh();
}

void lekerdez (void)
{
	int i, j, kilep=FALSE, megerosit = TRUE;
	unsigned int egerx, egery;
	char ch2;
/*	struct time ttime; */
	struct ffblk bejegyzes;
	struct bejegyzes *aktual;						/* aktu†lis bejegyzÇs */

/*	int page = 0;*/
	EgerReset();
	if (EgerMukodik()) EgerXY(KOZEP, 240);

	/* kezdîÇrtÇkek be†ll°t†sa */
	panel[1].aktual = panel[2].aktual = 1;
	panel[1].jelolt = panel[2].jelolt = FALSE;
	panel[1].hanyadiktol = panel[2].hanyadiktol = 0;
	panel[1].aktiv = TRUE; panel[2].aktiv = FALSE;
	panel[1].start = panel[2].start = NULL;
	panel[1].kijeloltek = panel[2].kijeloltek = NULL;
	strcpy(panel[1].utvonal, "C:\\");
	strcpy(panel[2].utvonal, "C:\\");
	if (panel[1].aktiv) strcpy(akt, panel[1].utvonal);
	else strcpy(akt, panel[2].utvonal);

	/* panelek f†jllist†j†nak kiirat†sa - kînyvt†r neve, hanyadik elemtîl */
	panel[1].maxfajl=dirinfo(panel[1].utvonal, panel[1].hanyadiktol);
	panelvalt(akt);
	panel[2].maxfajl=dirinfo(panel[2].utvonal, panel[2].hanyadiktol);
	panelvalt(akt);

	do
	{
/*		setactivepage(page); */
		kilep = FALSE;
		if (EgerMukodik()) EgerBe();
		while ((!kbhit()) && (!kilep))
		{
			if (EgerMukodik() && EgerKattintas())
			{
				EgerPozicio(&egerx, &egery);

				/*menure kattintas*/
				if (egerx> *(fokoord) && egerx< *(fokoord+(FO-1)*2+(0))+ *(fokoord+(FO-1)*2+(1)) && egery>CIMSOR && egery<CIMSOR+MENUSOR) {
				  for(j=0; egerx> *(fokoord+j*2+(0)) && j<FO; ++j);
				  --j;
				  fo_menu(j);
				}

				else if (egery>MAXY-STATUSZ/2)
				{
					if (egerx < 44) nezoke(SUGOFAJL, "S£g¢");
					else if ((egerx > 54) && (egerx < 97)) msgbox("/* a menÅre kattintottak */");
					else if ((egerx > 107) && (egerx < 162))
					{
						for (i=1; i<3; i++)
							if (panel[i].aktiv)
							{
								if (strcmp(panel[i].utvonal, akt)==0) aktneve(akt);
								nezoke(akt, "NÇzîke");
							}
						szulokonyvtar(akt);
						statfresh();
					}
					else if ((egerx > 172) && (egerx < 243))
					{
						for (i=1; i<3; i++) if (panel[i].aktiv)
							if (strcmp(panel[i].utvonal, akt)==0) aktneve(akt);
						strcpy(temp, "edit ");
						strcat(temp, akt);
						if (system(temp)==-1) switch (errno)
						{
							case ENOENT: msgbox("Nincs ilyen f†jl vagy kînyvt†r!"); break;
							case ENOMEM: msgbox("Nincs elÇg mem¢ria!"); break;
							case E2BIG: msgbox("T£l hossz£ az argumentumlista!"); break;
							case ENOEXEC: msgbox("Exec format error!");
						}
						szulokonyvtar(akt);
						restoregraphmode(akt);
					}
					else if ((egerx > 253) && (egerx < 300)) msgbox("/* a m†sol†sra kattintottak */");
					else if ((egerx > 310) && (egerx < 374)) msgbox("/* az †thelyezÇsre kattintottak */");
					else if ((egerx > 384) && (egerx < 451))
					{
						/* F7 - Uj kînyvt†r */
						if (bekeres(akt, "Adja meg a lÇtrehozand¢ kînyvt†r nevÇt!") != -1)
						{
							ujkonyvtar(akt);
							if (panel[1].aktiv) strcpy(akt, panel[1].utvonal);
							else strcpy(akt, panel[2].utvonal);
						}
					}
					else if ((egerx > 461) && (egerx < 507)) msgbox("/* a tîrlÇsre kattintottak */");
					else if ((egerx > 517) && (egerx < 575)) atnevezes(akt);
					else if (egerx > 585)
					{
						ungetch(1);
						kilep = TRUE;
					}
				}
			}
/*			if (EgerBalDuplaKattintas()) msgbox("double click"); */
		}

		ch=getch();
		if (EgerMukodik()) EgerKi();
		switch (ch)
		{
			case 0: /* kÇtb†jtos karakter */ ch2=getch(); break;
			case 1: /* az egÇrrel a kilÇpÇsre kattintottak */
			{
				if (megerosit) { if (eldontendo("Val¢ban ki akar lÇpni a programb¢l?")) ch = ESC; }
				else ch = ESC;
				break;
			}
			case BACKSPACE:
			{
				/* backspace */
				if (strlen(akt) > 0)
				{
					setfillstyle(1,WHITE);
	                bar(50, MAXY-2*MENUSOR+1, MAXX-5, MAXY-MENUSOR-5);
					akt[strlen(akt)-1]=0;
					outtextxy(STX, STY, akt);
				}
				break;
			}
			case 9:
			{
				/* tab */
				panelvalt(akt);
				break;
			}
			case ENTER:
			{
				/* enter */
				for (i=1; i<3; i++)
				if (panel[i].aktiv)
				{
					if (strcmp(panel[i].utvonal,akt)==0)
					{
						/* ha nem °rtak a st†tuszsorba, megkeressÅk a bejegyzÇst */
						if (panel[i].start != NULL)
						{
							aktual = panel[i].start;	 /* legelsî ki°rt bejegyzÇs */
							for (j=0; j<panel[i].aktual; j++)
								aktual = aktual->kovetkezo;
							if (strcmp(aktual->fajlnev, "..")==0)
							{
								panel[i].aktual = 1;
								panel[i].jelolt = FALSE;
								panel[i].hanyadiktol = 0;
								szulokonyvtar(akt);
								strcpy(panel[i].utvonal, akt);
							}
							else if (strcmp(aktual->fajlnev, ".") != 0)
							{
								strcat(akt, aktual->fajlnev);
								findfirst(akt, &bejegyzes, 0x003F);
								if (bejegyzes.ff_attrib == FA_DIREC) /* ha kînyvt†r */
								{
									if (strlen(akt)>3) strcat(akt, "\\");
									panel[i].aktual = 1;
									panel[i].jelolt = FALSE;
									panel[i].hanyadiktol = 0;
									strcpy(panel[i].utvonal, akt);
								}
								else /* ha f†jl az aktu†lis bejegyzÇs */
								{
									/* execv(akt); */
									restorecrtmode();
									if (system(akt)==-1) switch (errno)
									{
										case ENOENT: msgbox("Nincs ilyen f†jl vagy kînyvt†r!"); break;
										case ENOMEM: msgbox("Nincs elÇg mem¢ria!"); break;
										case E2BIG: msgbox("T£l hossz£ az argumentumlista!"); break;
										case ENOEXEC: msgbox("Exec format error!");
									}
									szulokonyvtar(akt);
									setgraphmode(2);
									restoregraphmode(akt);
								}
							}
						}
					}
					else
					{
						if (((akt[1]==':') && (akt[2]=='\\') && (akt[3]==0))
						|| ((akt[1]==':') && (akt[2]==0)))
						{
							if ((akt[1]==':') && (akt[2]==0)) strcat(akt, "\\");
							if (panel[1].aktiv) strcpy(panel[1].utvonal, akt);
							else strcpy(panel[2].utvonal, akt);
							panel[i].aktual = 1;
							panel[i].jelolt = FALSE;
							panel[i].hanyadiktol = 0;
						}
						else if (findfirst(akt, &bejegyzes, 0x003F)==-1)
						{
							switch (errno)
							{
								case ENOENT: msgbox("Nem tal†lhat¢ az elÇrÇsi £t vagy a f†jl!"); break;
								case EMFILE: msgbox("Nincs tîbb f†jl!");
							}
							if (panel[1].aktiv) strcpy(akt, panel[1].utvonal);
							else strcpy(akt, panel[2].utvonal);
						}
						else
						{
							if ((bejegyzes.ff_attrib == FA_DIREC)
							|| (bejegyzes.ff_attrib == FA_LABEL))
							{
								if (strlen(akt)>3) strcat(akt, "\\");
								panel[i].aktual = 1;
								panel[i].jelolt = FALSE;
								panel[i].hanyadiktol = 0;
								strcpy(panel[i].utvonal, akt);
							}
							else
							{
								restorecrtmode();
								if (system(akt)==-1) switch (errno)
								{
									case ENOENT: msgbox("Nincs ilyen f†jl vagy kînyvt†r!"); break;
									case ENOMEM: msgbox("Nincs elÇg mem¢ria!"); break;
									case E2BIG: msgbox("T£l hossz£ az argumentumlista!"); break;
									case ENOEXEC: msgbox("Exec format error!");
								}
								szulokonyvtar(akt);
								setgraphmode(2);
								restoregraphmode(akt);
							}
						}
					}
					paneltorles();
					panel[i].maxfajl = dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					statfresh();
				}
				break;
			}
			case 15:
			{
				/* ctrl+o */
				restorecrtmode();
				if (getch()==0) getch();
				setgraphmode(2);
				restoregraphmode(akt);
				break;
			}

			/*menube lepes*/
			case 'M': {fo_menu(0); break;}

			default:
			{
				/* a st†tuszsorba °runk */
				if (strlen(akt) < MAXPATH)
				{
					setfillstyle(1,WHITE);
	                bar(50, MAXY-2*MENUSOR+1, MAXX-5, MAXY-MENUSOR-5);
					akt[strlen(akt)+1] = 0;
					akt[strlen(akt)] = ch;
					outtextxy(STX, STY, akt);
				}
			}
		}

		switch (ch2)
		{
			case 59:
			{
				/* f1 - s£g¢ */
				nezoke(SUGOFAJL, "S£g¢");
				break;
			}
			case 60:
			{
				/* f2 - menÅ */

strcpy(akt, "elso");
strcpy(temp, "masodik");
bekero2(akt, temp, "m†sol");
msgbox(akt);
msgbox(temp);


/*					for (i=1; i<3; i++)
					if (panel[i].aktiv)
					{
						if (strcmp(panel[i].utvonal, akt)==0) aktneve(akt);
						nyomtatas(akt);
					}
				szulokonyvtar(akt);
*/				statfresh();
				break;
			}
			case 61:
			{
				/* f3 - nÇzîke */
				for (i=1; i<3; i++)
					if (panel[i].aktiv)
					{
						if (strcmp(panel[i].utvonal, akt)==0) aktneve(akt);
						nezoke(akt, "NÇzîke");
					}
				szulokonyvtar(akt);
				statfresh();
				break;
			}
			case 62:
			{
				/* f4 - szerkesztÇs */
				for (i=1; i<3; i++) if (panel[i].aktiv)
					if (strcmp(panel[i].utvonal, akt)==0) aktneve(akt);
				strcpy(temp, "edit ");
				strcat(temp, akt);
				if (system(temp)==-1) switch (errno)
				{
					case ENOENT: msgbox("Nincs ilyen f†jl vagy kînyvt†r!"); break;
					case ENOMEM: msgbox("Nincs elÇg mem¢ria!"); break;
					case E2BIG: msgbox("T£l hossz£ az argumentumlista!"); break;
					case ENOEXEC: msgbox("Exec format error!");
				}
				szulokonyvtar(akt);
				restoregraphmode(akt);
				break;
			}
			case 63:
			{
				/* f5 - m†sol†s */
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
/*						paneltorles();
						panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
						panelvalt(temp);
						paneltorles();
						panel[3-i].maxfajl=dirinfo(panel[3-i].utvonal, panel[3-i].hanyadiktol);
						panelvalt(akt);
*/					} else msgbox("Hiba tîrtÇnt!");
				}
				setdisk(i);
				break;
			}
			case 64:
			{
				/* f6 - †thelyezÇs */
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
								jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 0);
							if (panel[2].aktiv)
								jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 0);
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
				break;
			}
			case 65:
			{
				/* F7 - Uj kînyvt†r */
				if (bekeres(akt, "Adja meg a lÇtrehozand¢ kînyvt†r nevÇt!") != -1)
				{
					ujkonyvtar(akt);
					if (panel[1].aktiv) strcpy(akt, panel[1].utvonal);
					else strcpy(akt, panel[2].utvonal);
				}
				break;
			}
			case 66:
			{
				/* f8 - tîrlÇs */
				/* nincs break, mert a del is tîrlÇs */
			}
			case 83:
			{
				/* del - tîrlÇs */
				aktneve(akt);
				bekeres(akt, "Biztosan tîrîl a bejegyzÇst?");
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
									jelolo(4,ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 0);
								if (panel[2].aktiv)
									jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 0);
								panel[i].aktual--;
								panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
							}
							if (strcmp(panel[i].utvonal, panel[3-i].utvonal)==0)
							{
								panelvalt(akt);
								paneltorles();
								panel[3-i].maxfajl=dirinfo(panel[3-i].utvonal, panel[3-i].hanyadiktol);
								panelvalt(akt);
							}
						}
					}
					else msgbox("Nem tal†lom a bejegyzÇst!");
				}
				setdisk(i);
				szulokonyvtar(akt);
				break;
			}
			case 67:
			{
				/* f9 - †tnevezÇs */
				atnevezes(akt);
				break;
			}
			case 68:
			{
				/* f10 - kilÇpÇs */
				if (megerosit) { if (eldontendo("Val¢ban ki akar lÇpni a programb¢l?")) ch = ESC; }
				else ch = ESC;
				break;
			}
			case 71:
			{
				/* home */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && (panel[i].aktual+panel[i].hanyadiktol != 1))
				{
					panel[i].aktual = 1;
					panel[i].hanyadiktol = 0;
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
				}
				break;
			}
			case 72:
			{
				/* fel */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && ((panel[i].hanyadiktol > 0) || (panel[i].aktual > 1)))
				{
					if (panel[i].aktual==1)
					{
						panel[i].hanyadiktol--;
						paneltorles();
						panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					}
					else /* if (panel[i].aktual>1) */
					{
						panel[i].aktual--;
						if (panel[1].aktiv)
						{
							jelolo(4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+2)*SORTAV-8, 0);
							jelolo(4, ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 1);
						}
						if (panel[2].aktiv)
						{
							jelolo(KOZEP+4, ABLAKMIN+(panel[2].aktual+1)*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+2)*SORTAV-8, 0);
							jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 1);
						}
					}
				}
				break;
			}
			case 73:
			{
				/* page up */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && (panel[i].hanyadiktol+panel[i].aktual != 1))
				{
					if (panel[i].aktual==1)
					{
						if (panel[i].hanyadiktol >= KFSZ) panel[i].hanyadiktol-=KFSZ;
						else { panel[i].hanyadiktol = 0; panel[i].aktual = 1; }
					}
					else panel[i].aktual=1;
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
				}
				break;
			}
			case 79:
			{
				/* end */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && (panel[i].aktual+panel[i].hanyadiktol != panel[i].maxfajl))
				{
					panel[i].aktual = panel[i].maxfajl%KFSZ;
					if (panel[i].maxfajl > KFSZ)
					{
						panel[i].aktual = KFSZ;
						panel[i].hanyadiktol = panel[i].maxfajl-KFSZ;
					}
					else panel[i].aktual = panel[i].maxfajl;
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
				}
				break;
			}
			case 82:
			{
				/* insert */
				for (i=1; i<3; i++)
				if (panel[i].aktiv) panel[i].jelolt=TRUE;
				/* nincs break, hogy a kijelîlÇs ut†n a kîvetkezî elem legyen akt°v */
			}
			case 80:
			{
				/* le */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && (panel[i].hanyadiktol+panel[i].aktual < panel[i].maxfajl))
				{
					if (panel[i].aktual==KFSZ)
					{
						panel[i].hanyadiktol++;
						paneltorles();
						panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
					}

					else if (panel[i].aktual < KFSZ)
					{
						panel[i].aktual++;
						if (panel[1].aktiv)
						{
							jelolo(4, ABLAKMIN+(panel[1].aktual-1)*SORTAV-5, KOZEP-4, ABLAKMIN+panel[1].aktual*SORTAV-8, 0);
							jelolo(4, ABLAKMIN+panel[1].aktual*SORTAV-5, KOZEP-4, ABLAKMIN+(panel[1].aktual+1)*SORTAV-8, 1);
						}
						if (panel[2].aktiv)
						{
							jelolo(KOZEP+4, ABLAKMIN+(panel[2].aktual-1)*SORTAV-5, MAXX-5, ABLAKMIN+panel[2].aktual*SORTAV-8, 0);
							jelolo(KOZEP+4, ABLAKMIN+panel[2].aktual*SORTAV-5, MAXX-5, ABLAKMIN+(panel[2].aktual+1)*SORTAV-8, 1);
						}
					}
				}
				break;
			}
			case 81:
			{
				/* page down */
				for (i=1; i<3; i++)
				if ((panel[i].aktiv) && (panel[i].hanyadiktol+panel[i].aktual != panel[i].maxfajl))
				{
					if (panel[i].maxfajl > KFSZ)
					{
						if (panel[i].aktual == KFSZ)
						{
							if (panel[i].hanyadiktol+KFSZ+panel[i].aktual < panel[i].maxfajl)
								panel[i].hanyadiktol+=KFSZ;
							else
							{
								panel[i].hanyadiktol = panel[i].maxfajl-KFSZ;
							}
						}
						else panel[i].aktual = KFSZ;
					}
					else panel[i].aktual = panel[i].maxfajl;
					paneltorles();
					panel[i].maxfajl=dirinfo(panel[i].utvonal, panel[i].hanyadiktol);
				}
				break;
			}
			case 104:
			{
				/* alt+f1 */
				if (panel[2].aktiv) panelvalt(panel[2].utvonal);
				ch2 = meghajtovalasztas();
				if ((ch2<='Z') && (ch2>='A'))
				{
					panel[1].utvonal[0] = ch2;
					panel[1].utvonal[1] = 0;
					strcat(panel[1].utvonal, ":\\");
					strcpy(akt, panel[1].utvonal);
					panel[1].aktual = 1;
					panel[1].hanyadiktol = 0;
					paneltorles();
					panel[1].maxfajl=dirinfo(panel[1].utvonal, panel[1].hanyadiktol);
					statfresh();
				}
				break;
			}
			case 105:
			{
				/* alt+f2 */
				if (panel[1].aktiv) panelvalt(panel[1].utvonal);
				ch2 = meghajtovalasztas();
				if ((ch2<='Z') && (ch2>='A'))
				{
					panel[2].utvonal[0] = ch2;
					panel[2].utvonal[1] = 0;
					strcat(panel[2].utvonal, ":\\");
					strcpy(akt, panel[2].utvonal);
					panel[2].aktual = 1;
					panel[2].hanyadiktol = 0;
					paneltorles();
					panel[2].maxfajl=dirinfo(panel[2].utvonal, panel[2].hanyadiktol);
					statfresh();
				}
				break;
			}
		}
/* nincs lapoz¢ technika, csak, ha setgraphmode(1)-et csin†lunk */
/*		setvisualpage(page);
		if (page==1) page=0;
		else if (page==0) page=1;*/

		ch2=0;
	} while (ch != ESC);
}

void main (void)
{
	 int gd = DETECT, gm = 0, hibakod;

	 clrscr();
	 initgraph(&gd, &gm, UTVONAL);
/*	setgraphmode(1); */
	 if ((hibakod=graphresult()) == grOk)
		{
			_fmode = O_BINARY;
			cleardevice();
			settextstyle(SMALL_FONT, HORIZ_DIR, MERET);

		  felulet();
		  lekerdez();

			memfree(1);
			memfree(2);
			free(fokoord);
		  cleardevice();
			closegraph();
			restorecrtmode();
			textmode(C80);
		}
	 else
	 {
		  printf("Error: %s",grapherrormsg(hibakod));
		  if (getch()==0) getch();
	 }
	 printf("\nA programot kÇsz°tette: Mortens <mortens@freemail.hu>\n\nA ");
	 printf("program haszn†lat†b¢l eredî k†rokÇrt a programoz¢ felelîssÇget");
	 printf(" nem v†llal!\n");
}
