#include <dir.h>
#include <stdio.h>

#define UTVONAL "C:\\TURBOC"
#define SUGOFAJL "C:\\CC\\HELP.TXT"

#define SUGOSOR 7
#define MIN 0
#define MAXX 639
#define MAXY 479
#define KOZEP MAXX/2
#define SORTAV 15
#define TRUE 255
#define FALSE 0
#define KILOBAJT 1024
#define STATUSZ 30
#define KFSZ (MAXY-80)/14-3		/* a kÇpernyîre k°rhat¢ f†jlok sz†ma */
#define MERET 4    			/* szîvegmÇret */

#define FEKETE 0
#define FEHER 15
#define KEK 1
#define PIROS 4
#define SARGA 14

/*	A gombok kÌdjai: */

#define BACKSPACE 8
#define FF 12
#define ENTER 13
#define ESC 27
#define HOME 327
#define END 335
#define UP 328
#define DOWN 336
#define PGUP 329
#define PGDN 337
#define LEFT 331
#define INS 338
#define RIGHT 333
#define DEL 339
#define CTRLLEFT 371
#define CTRLRIGHT 372
#define F1 315

typedef void ketstr(char[], char[]);
typedef ketstr *ssfvcim;

typedef int intfv(int);			/* Int visszatÇrÇsÅ, egy int paramÈteres
								   fÅggvÇnyt°pus	*/
typedef intfv *intfvcim;		/* Intfv t°pus£ fÅggvÇnyre mutat¢ t°pus	*/

typedef struct{					/* MenÏt-tel struktra tﬁpus */
	char *szoveg;				/* MenÏt-tel sz∏vege */
	intfvcim fv;				/* MenÏt-telhez tartozÌ fÏggv-nyre mutat.
								   Ha nincs ilyen, akkor NULL. */
	int param;					/* MenÏt-telhez tartozÌ fÏggv-ny param-- */
} menutetel;					/* tere, */

typedef struct{					/* MenÏtﬁpus struktra tﬁpus */
	int x;						/* A menÏdoboz bal fels∏ sarokkoordin-t-i */
	int y;						/*  -s a menÏdo boz x -s y ir-ny m-rete */
	int xm;
	int ym;
	int tetelszam;				/* MenÏt-telek sz-ma a menÏben */
	menutetel *tetelek;			/* MenÏt-telek list-j-ra mutat */
	int utsotetel;				/* Az utolj-ra kiv-lasztott menÏt-tel */
} menutipus;					/* indexe */

int fugg(int i); /*IDEIGLENES!!! torolni a fuggvennyel egyutt*/






static struct bejegyzes
{
	char fajlnev[15];							/* melyik bejegyzÇst °rtuk ki */
	struct bejegyzes * kovetkezo;			/* kîvetkezî bejegyzÇs mutat¢ja */
};

static struct panel
{
	int aktual;				/* melyik soron †ll a jelîlî */
	int jelolt;				/* ki van-e jelîlve az elem, ahol a jelîlî van */
	int aktiv;				/* a panel akt°v-e */
	int hanyadiktol;		/* hanyadik elemmel kezdîdik a ki°r†s */
	int maxfajl;			/* a kînyvt†r bejegyzÇseinek sz†ma */
	char utvonal[MAXPATH]; /* a panelben l†taht¢ kînyvt†r */
	struct bejegyzes *start;				/* kiirt f†jllista */
	struct bejegyzes *kijeloltek;			/* kijelîlt f†jlok list†ja */
} panel[2];

/* v200.c */
void lekerdez (void);
int dirdel(char *dir);
void ujkonyvtar(char akt[]);
int fajlmasol(char akt[], char uj[]);
void kinyomtat(char fajlnev[]);
void nyomtatas(char fajlnev[]);
long int fajlsorir(const char fajlnev[], long int sorszam);
void nezoke(char fajlnev[], char cim[]);
void atnevezes(char akt[]);
void statfresh (void);	/* st†tuszsor friss°tÇse */
void panelvalt (char akt[]);
void paneltorles (void);
void felulet (void);
void allapot (char s[]);
int bekeres (char s[], char bekero[]);
char meghajtovalasztas(void);
int eldontendo (char kerdes[]);
void restoregraphmode (char akt[]);
int info (struct ffblk *f, char ch);
int dirinfo(char s[], int hanyadik);
int dircopy(char *honnan, char *hova);
void aktneve(char s[]);
void szulokonyvtar(char s[]);
int fo_menu(int i);


/*seged_fv*/
void reverse (char s[]);
void numtostr (long int szam, char szamszov[]);
void memfree (int panelszam);
void szamSzovir (long int szam, char szoveg[]);
void ujsor (int *sor);
void meretir (long int meret);
void jelolo (int tol_x, int tol_y, int ig_x, int ig_y, int onoff);
void felulet (void);
void doboz(int xf,int yf,int xa,int ya);
void almenuki(int index);
void _3dkeret_ki(int i);
void _3dkeret_be(int i);
void _3dkeret_torl(int i);
int gombbe(void);
void _3dmkeret_ki(int xf, int yf, int xa, int ya);
void _3dmkeret_be(int xf, int yf, int xa, int ya);
int msgbox(char s[]);



/* eger.c */
/* Struktura a grafikus egerkurzor beallitasahoz */
struct TEgerKurzor{
	unsigned csucsx,csucsy;  /*egerkurzor referenciapontja*/
	unsigned kepernyomaszk[16], kurzormaszk[16];
};
int SzovegesMod(void);
int EgerReset(void);
int EgerGombSzam(void);
int EgerLathato(void);
int EgerMukodik(void);
int EgerGombLenyomva(void);
int EgerBalGomb(void);
int EgerJobbGomb(void);
int EgerKozepsoGomb(void);
int EgerKattintas(void);
void EgerPozicio(unsigned int *x,unsigned int *y);
void EgerBe(void);
void EgerKi(void);
void EgerXY(unsigned int x, unsigned int y);
void EgerAblak(unsigned x1, unsigned y1, unsigned x2, unsigned y2);
void GrafEgerKurzor(struct TEgerKurzor kurzor);
