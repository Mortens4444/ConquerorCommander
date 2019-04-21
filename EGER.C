#include <dos.h>

#define EGERINTR 0x33
#define egBalGomb 1
#define egJobbGomb 2
#define egKozepsoGomb 4

static char gombszam;
static int lathato;
static int mukodik;

/*Szoveges uzemmod eseten 1 kulonben 0*/
int SzovegesMod(void){
 int far *videomodcim=MK_FP(0x0040,0x0049);
 int videomod;

 videomod=*videomodcim<<8>>8;
 if((videomod>=0&&videomod<=3)||videomod==7)return 1;
 else return 0;
}

/*Ha az eger hasznalhato 1, kulonben nulla*/
int EgerReset(void){
	struct REGPACK reg;

  reg.r_ax=0;
	intr(EGERINTR,&reg);
	gombszam=reg.r_bx;
	lathato=0;
	if(mukodik=(reg.r_ax!=0)){
		reg.r_cx=0;
		reg.r_dx=0;
		reg.r_ax=4;
		intr(EGERINTR,&reg);
		return 1;}
	return 0;
}

/*Egergombok szama*/
int EgerGombSzam(void){
	return gombszam;
}

/*Ha lathato az eger 1, kulonben 0*/
int EgerLathato(void){
	return lathato>0;
}

/*Ha az eger hasznalhato 1, kulonben 0*/
int EgerMukodik(void){
	return mukodik;
}

/*Visszaadott ertek: egergombok allapota
	Bit0 - bal gomb
	Bit1 - jobb gomb
	Bit2 - kozepso gomb
	Bitek erteke 0 ha a gomb nem letezik, vagy a gomb nincs lenyomva*/
int EgerGombLenyomva(void){
	struct REGPACK reg;

	reg.r_ax=5;
	intr(EGERINTR,&reg);
	return reg.r_ax;
}

/*Ha az eger bal gombja le van nyomva 1, kulonben 0*/
int EgerBalGomb(void){
	return (EgerGombLenyomva() & egBalGomb);
}

/*Ha az eger jobb gombja le van nyomva 1, kulonben 0*/
int EgerJobbGomb(void){
	return (EgerGombLenyomva() & egJobbGomb);
}

/*Ha az eger kozepso gombja le van nyomva 1, kulonben 0*/
int EgerKozepsoGomb(void){
	return (EgerGombLenyomva() & egKozepsoGomb);
}

/*Visszaadott ertek: ha tortent kattintas a megfelelo egergombkonstans,
kulonben 0*/
int EgerKattintas(void){
	int lenyomva,utolso;

	lenyomva=0;
	do {
		utolso=lenyomva;
		lenyomva=EgerGombLenyomva();
	}
	while(lenyomva!=0);
	return utolso;
}

/*Visszaadja az eger oszlop es sorpoziciojat*/
void EgerPozicio(unsigned int *x,unsigned int *y){
	struct REGPACK reg;

	reg.r_ax=3;
	intr(EGERINTR,&reg);
	if(SzovegesMod()){
		reg.r_cx>>=3;
		reg.r_dx>>=3;
	}
	*x=reg.r_cx;
	*y=reg.r_dx;
}

/*Egerkurzor lathatova tetele*/
void EgerBe(void){
	struct REGPACK reg;

	reg.r_ax=1;
	intr(EGERINTR,&reg);
}

/*Egerkurzor lathatatlanna tetele*/
void EgerKi(void){
	struct REGPACK reg;

	reg.r_ax=2;
	intr(EGERINTR,&reg);
}

/*Az X oszlop Y sorba mozgatja az egeret*/
void EgerXY(unsigned int x, unsigned int y){
	struct REGPACK reg;

	if(SzovegesMod()){
		--x;x<<=3;
		--y;y<<=3;
	}
	reg.r_cx=x;
	reg.r_dx=y;
	reg.r_ax=4;
	intr(EGERINTR,&reg);
}

/*Eger mozgasteruletenek beallitasa*/
void EgerAblak(unsigned x1, unsigned y1, unsigned x2, unsigned y2){
	struct REGPACK reg;

	if(SzovegesMod()){
		--x1;x1<<=3;
		--y1;y1<<=3;
		--x2;x2<<=3;
		--y2;y2<<=3;
	}
	reg.r_cx=x1;
	reg.r_dx=x2;
	reg.r_ax=7;
	intr(EGERINTR,&reg);
	reg.r_cx=y1;
	reg.r_dx=y2;
	reg.r_ax=8;
	intr(EGERINTR,&reg);
}

/*Struktura a grafikus egerkurzor beallitasahoz*/
struct TEgerKurzor{
	unsigned csucsx,csucsy;  /*egerkurzor referenciapontja*/
	unsigned kepernyomaszk[16], kurzormaszk[16];
};

/*Grafikus egerkurzor beallitasa*/
void GrafEgerKurzor(struct TEgerKurzor kurzor){
	struct REGPACK reg;

	reg.r_bx=kurzor.csucsx;
	reg.r_cx=kurzor.csucsy;
	reg.r_es=FP_SEG(kurzor.kepernyomaszk);
	reg.r_dx=FP_OFF(kurzor.kepernyomaszk);
	reg.r_ax=9;
	intr(EGERINTR,&reg);
}
