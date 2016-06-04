/*
 * Soubor:  proj3.c
 * Datum:   9.12.2007
 * Autor:   Radim Kubis, xkubis03@stud.fit.vutbr.cz
 * Projekt: Operace s maticemi
 * Popis:   Program dela soucet matic, soucin matic, test reseni sudoku
 *          a test na submatici.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Kody chyb a stavu programu */
enum errors
{
  OK = 0,  /* Bez chyby */
  F_OPEN,  /* Nepodarilo se otevrit soubor */
  F_CLOSE, /* Nepodarilo se uzavrit soubor */
  NO_MEM,  /* Malo pameti */
  BAD_MAT, /* Spatny rozmer matice */
  BAD_FMAT,/* Spatna matice v souboru */
  NO_MAT,  /* Nejedna se o matici */
  Y_SUD,   /* Je reseni sudoku */
  N_SUD,   /* Nejedna se o reseni sudoku */
  NO_ALLOC,/* Nepodarilo se alokovat matici */
  N_SUB,   /* NO Sub */
  Y_SUB,   /* YES Sub */
  BAD_CMD, /* Spatny prepinac */
  FALSE,   /* Nelegalni vysledek */
  OTHER,   /* Neznama chyba */
  HELP,    /* Napoveda */
  END,     /* Zarazka */
};
/* Konec seznamu s chybami ... */

/* Pole textovych retezcu vypisovanych funkci printErr. */
const char *ERRMSG[] =
{
  /* OK */
  "Vse je v poradku.\n",
  /* F_OPEN */
  "Nepodarilo se otevrit soubor!!!\n",
  /* F_CLOSE */
  "Nepodarilo se uzavrit soubor!!!\n",
  /* NO_MEM */
  "Malo pameti!!!\n",
  /* BAD_MAT */
  "Spatny rozmer matice!!!\n",
  /* BAD_FMAT */
  "Spatna matice v souboru!!!\n",
  /* NO_MAT */
  "Nejedna se o matici!!!\n",
  /* Y_SUD */
  "true\n",
  /* N_SUD */
  "false\n",
  /* NO_ALLOC */
  "Nepodarilo se alokovat matici!!!\n",
  /* N_SUB */
  "false\n",
  /* Y_SUB */
  "true\n",
  /* BAD_CMD */
  "Spatny prepinac\n",
  /* FALSE */
  "false\n",
  /* OTHER */
  "Neznama chyba!!!\n",
  /* HELP */
  "\nProgram pro praci s maticemi\n"
  "Autor: Radim Kubis (c) 2007\n"
  "Program dela soucet matic, soucin matic, test sudoku a test na submatici\n\n"
  "Pouziti:\n"
  "  -h             - vypise napovedu\n"
  "  -add x y       - soucet matic v souborech x a y\n"
  "  -mult x y      - soucin matic v souborech x a y\n"
  "  -sudoku x      - test sudoku v souboru x\n"
  "  -submatrix x y - hleda v souboru x submatici v souboru y\n\n"
  "Vystup:\n"
  "Pokud je vysledkem operace matice, vypise ji, ale pokud nema operace\n"
  "legalni vysledek, vypise false.\n"
  "Pokud neni vysledkem operace matice, vypise true, pokud operace skoncila\n"
  "dobre, kdyz operace skoncila spatne, vypise false.\n"
};
/* Konec pole textovych retezcu ... */

/**
 * Vytiskne kód chyby nebo text popisující stav programu.
 * @param code - kód chyby nebo stavu programu.
 * Tato funkce nic nevraci.
 */
void printErr(int code)
{
  FILE *outstream = stdout;  //Ukazatel na vystup funkce - standardni vystup

  if(code < OK || code >= END)  //Pokud nastane neocekavana chyba
  {
    code = OTHER;  //Nastavi kod chyby na OTHER - neznama chyba
  }

  if(code <= OTHER)  //Pokud nastala znama chyba
  {
    outstream = stderr;  //Bude vystup nastaven na standardni chybovy vystup
  }

  fprintf(outstream, "%s", ERRMSG[code]);  //Vypis stavu programu
}
/* Konec funkce printErr */

/* Struktura MATRIX pro uchovani poctu radku, sloupcu a prvku matice */
typedef struct
{
  int rows;    //pocet radku
  int cols;    //pocet sloupcu
  int **matrix;//ukazatel na prvky matice
}MATRIX;
/* Konec struktury MATRIX */

/**
 * Funkce printMatrix vypisuje pocet radku, sloupcu a prvky matice.
 * @param *m - ukazatel na matici, kterou budeme vypisovat.
 * Tato funkce nic nevraci.
 */
void printMatrix(MATRIX *m)
{
  int howMany = m->rows*m->cols;  //Promenna pro pocet prvku matice

  printf("%d %d\n", m->rows, m->cols);  //Vypis poctu radku a sloupcu matice

  for(int i = 0; i < howMany; i++)  //Cyklus pro vypis prvku matice po radcich
  {
    printf("%d", m->matrix[i/m->cols][i%m->cols]);  //Vypis jednoho prvku matice

    if((i%m->cols) == (m->cols - 1))  //Pokud jsem na konci radku
    {
      printf("\n");  //Vypisuji znak konce radku
    }
    else  //Kdyz nejsem na konci radku
    {
      printf(" ");  //Vypisuji mezeru mezi cisly
    }
  }
}
/* Konec funkce printMatrix */

/**
 * Funkce allocMatrix alokuje prostor pro prvky matice ve strukture MATRIX.
 * @param *m - ukazatel na matici, ve ktere budeme alokovat prostor pro prvky
 * @return - pokud nastane pri alokaci prostoru chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
*/
int allocMatrix(MATRIX *m)
{
  if(m == NULL)  //Pokud neni v parametru platna matice
  {
    return OTHER;  //Vraci chybovy kod OTHER, tato skutecnost by za normalnich
  }                //podminek nemela nastat

  if((m->matrix = (int**)malloc(m->rows*sizeof(int*))) == NULL)
  {  //Pokud alokace pro pocet radku nebyla uspesna
    return NO_MEM;  //Vraci chybovy kod NO_MEM - malo pameti
  }

  for(int i = 0; i < m->rows; i++)  //Cyklus pro alokaci sloupcu v kazdem radku
  {
    if((m->matrix[i] = (int*)malloc(m->cols*sizeof(int))) == NULL)
    {  //Pokud alokace pro pocet sloupcu nebyla uspesna
      return NO_MEM;  //Vraci chybovy kod NO_MEM - malo pameti
    }
  }

  return OK;  //Pokud se alokace zdarila, vracim kod OK - vse je v poradku
}
/* Konec funkce allocMatrix */

/**
 * Dynamicky alokuje prostor pro matici a nacte do ni data ze souboru
 * vcetne poctu radku a sloupcu.
 * @param *m - ukazatel na matici, do ktere budeme nacitat data ze souboru
 * @param file[] - pole obsahujici retezec s nazvem souboru, ze ktereho cteme
 * @return - pokud nastane pri alokaci prostoru chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int loadMatrix(MATRIX *m, char file[])
{
  FILE *f;  //Ukazatel na soubor
  int howMany;  //Promenna pro pocet prvku matice

  if((f = fopen(file, "r")) == NULL) //Otevreni souboru a test jeho otevreni
  {
    return F_OPEN;  //Pokud se nepodarilo soubor otevrit, vraci kod F_OPEN
  }

  if((fscanf(f, "%d", &m->rows)) == EOF)  //Nacteni poctu radku matice ze
  {                                       //souboru a test na konec souboru
    return BAD_FMAT;  //Pokud se nepodarilo nacist pocet radku, vraci BAD_FMAT
  }

  if(m->rows < 1)  //Pokud je pocet radku mensi jak jedna, nejedna se o matici
  {
    return NO_MAT;  //Vraci kod NO_MAT - nejedna se o matici
  }

  if((fscanf(f, "%d", &m->cols)) == EOF)  //Nacteni poctu sloupcu matice ze
  {                                       //souboru a test na konec souboru
    return BAD_FMAT;  //Pokud se nepodarilo nacist pocet sloupcu, vraci BAD_FMAT
  }

  if(m->cols < 1)  //Pokud je pocet sloupcu mensi jak jedna, nejedna se o matici
  {
    return NO_MAT;  //Vraci kod NO_MAT - nejedna se o matici
  }

  if(allocMatrix(m) != OK)  //Alokace prostoru prvku matice
  {
    return NO_MEM;  //Pokud se nepodarilo prostor alokovat, vraci NO_MEM
  }

  howMany = m->rows*m->cols;  //Pokud je doposud vse bez problemu, spocitam,
                              //kolik bude prvku v matici

  for(int i = 0; i < howMany; i++)  //Cyklus pro nacteni prvku matice ze souboru
  {
    if((fscanf(f, "%d", &m->matrix[i/m->cols][i%m->cols])) == EOF)
    {  //Pokud nastal konec souboru driv, nez se nacetli vsechny prvky,
      return BAD_FMAT;  //Vraci chybovy kod BAD_FMAT - spatne zadana matice
    }
  }

  if(fclose(f) == EOF) //Uzavreni souboru a test uzavreni
  {
    return F_CLOSE;  //Pokud se nepodarilo soubor uzavrit, vraci kod F_CLOSE
  }

  return OK;  //Pokud funkce probehla bez problemu, vraci OK
}
/* Konec funkce loadMatrix */

/**
 * Funkce addMatrix secte dve matice o stejnych rozmerech,
 * vysledek ulozi do matice treti.
 * @param *mFirst - ukazatel na 1. matici pro secteni
 * @param *mSecond - ukazatel na 2. matici pro secteni
 * @param *mResult - ukazatel na matici, ve ktere bude vysledek
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int addMatrix(MATRIX *mFirst, MATRIX *mSecond, MATRIX *mResult)
{
  int howMany;  //Promenna pro pocet prvku matice

  if(mFirst == NULL || mSecond == NULL || mResult == NULL)  //Test parametru
  {
    return OTHER;  //Pokud nektera matice neodpovida, vraci OTHER,
                   //neznama chyba - nemela by za normalnich podminek nastat
  }

  if(mFirst->cols != mSecond->cols)  //Pokud nemaji matice stejny pocet sloupcu
  {
    return BAD_MAT;  //Vraci kod BAD_MAT - spatny rozmer matice
  }
  if(mFirst->rows != mSecond->rows)  //Pokud nemaji matice stejny pocet radku
  {
    return BAD_MAT;  //Vraci kod BAD_MAT - spatny rozmer matice
  }

  mResult->rows = mFirst->rows;  //Matice mResult s vysledkem ma stejny rozmer jako obe
  mResult->cols = mFirst->cols;  //matice proto muzu nastavit parametry z matice prvni

  if(allocMatrix(mResult) != OK)  //Pokud bylo doposud vse v poradku, alokuji prostor
  {                               //pro prvky matice s vysledkem
    return NO_MEM;  //Kdyz se alokace nezdarila, vracim kod NO_MEM - malo pameti
  };

  howMany = mResult->rows*mResult->cols;  //Pokud je doposud vse bez problemu, spocitam,
                                          //kolik bude prvku v matici

  for(int i = 0; i < howMany; i++)  //Cyklus pro secitani prvku prvni matice
  {                                 //s prvky druhe matice do matice mResult
    mResult->matrix[i/mResult->cols][i%mResult->cols]=mFirst->matrix[i/mFirst->cols][i%mFirst->cols]+mSecond->matrix[i/mSecond->cols][i%mSecond->cols];
  }

  return OK;  //Pokud funkce predcasne neskoncila, vracim kod OK - bez chyby
}
/* Konec funkce addMatrix */

/**
 * Funkce mulMatrix vynasobi dve matice o spravnych rozmerech a vysledek ulozi
 * do treti matice mResult
 * @param *mFirst - ukazatel na 1. matici pro nasobeni
 * @param *mSecond - ukazatel na 2. matici pro nasobeni
 * @param *mResult - ukazatel na matici s vysledkem
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int mulMatrix(MATRIX *mFirst, MATRIX *mSecond, MATRIX *mResult)
{
  int howMany;

  if(mFirst->cols != mSecond->rows)  //Pokud pocet sloupcu prvni matice neodpovidaji
  {                                  //poctu radku druhe matice
    return BAD_MAT;  //Vraci kod BAD_MAT - spatny rozmer matice
  }

  mResult->rows = mFirst->rows;   //Pokud pocet sloupcu a radku je spravny,
  mResult->cols = mSecond->cols;  //nastavim vysledne matici odpovidajici rozmery

  if(allocMatrix(mResult) != OK)  //Pokud bylo doposud vse v poradku, alokuji prostor
  {                               //pro prvky matice s vysledkem
    return NO_MEM;  //Kdyz se alokace nezdarila, vracim kod NO_MEM - malo pameti
  };

  howMany = mResult->rows * mResult->cols;

  for(int i = 0; i < howMany; i++)  //Cyklus pro prochazeni radku
  {
    mResult->matrix[i/mResult->cols][i%mResult->cols] = 0;  //Pro pricitani
                                                            //musim nastavit pocatecni nulu
    for(int j = 0; j < mFirst->cols; j++)  //Cyklus pro soucet soucinu prvku
    {
      mResult->matrix[i/mResult->cols][i%mResult->cols]+=mFirst->matrix[i/mResult->cols][j]*mSecond->matrix[j][i%mResult->cols];
    }
  }

  return OK;  //Funkce konci s navratovou hodnotou kodu OK - bez chyby
}
/* Konec funkce mulMatrix */

/*
 * Funkce pro test spravnosti submatice 3x3 v sudoku.
 * @param *m - matice obsahujici cele sudoku
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int subSudoku(MATRIX *m)
{
  int soucet;  //Promenna pro soucet vsech cisel v submatici
  int pozice;  //Promenna s cislem, ktere udava levy horni roh submatice
  int r = 0, s = 0;

  do
  {
    pozice = (r*9+s);
    soucet = 0;

    for(int i = pozice; i < (pozice+3); i++)  //Cyklus pro soucet prvku submatice
    {
      soucet+=m->matrix[i%m->cols][i/m->cols];  //Prvni radek
      soucet+=m->matrix[(i+9)%m->cols][(i+9)/m->cols];  //Druhy radek
      soucet+=m->matrix[(i+18)%m->cols][(i+18)/m->cols];  //Treti radek
    }

    if(soucet != 45)  //Pokud soucet neodpovida cislu 45
    {
      return N_SUD;  //Vraci kod N_SUD - nejedna se o reseni sudoku
    }

    if(s == 6)  //Pokud jsem na 6. sloupci
    {
      s = 0;   //Vratim se na 0. sloupec
      r += 3;  //a jdu o tri radky nize
    }
    else
    {
      s+=3;  //Jinak jdu o tri sloupce doprava
    }

  }while(r != 6 || s != 6);

  return OK;  //Funkce konci s navratovou hodnotou kodu OK - bez chyby
}
/* Konec funkce test2 */

/*
 * Funkce pro test spravnosti reseni sudoku.
 * @param *m - matice obsahujici sudoku
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int testSudoku(MATRIX *m)
{
  int soucetRadku = 0;  //Promenna pro soucet jednoho radku
  int soucetSloupce = 0;  //Promenna pro soucet jednoho sloupce

  if((m->rows != 9) || (m->cols != 9))  //Pokud neni matice 9x9
  {
    return BAD_MAT;  //Vraci kod BAD_MAT - spatny rozmer matice
  }

  if(m->matrix[0][0] == m->matrix[0][1])  //Pokud jsou prvni dve cisla na radku
  {                                       //stejna (pokud je cela z cisel 5)
    return N_SUD;  //Vraci kod N_SUD - nejedna se o reseni sudoku
  }

  for(int i = 0; i < 81; i++)  //Cyklus pro prochazeni matice sudoku
  {
    soucetRadku+=m->matrix[i/9][i%9];  //Scitani prvku jednoho radku
    soucetSloupce+=m->matrix[i%9][i/9];  //Scitani prvku jednoho sloupce

    if((i%9) == 8)  //Pokud jsem na konci radku, kontroluji soucty
    {
      if((soucetRadku != 45) || (soucetSloupce != 45)) //Pokud nejsou soucty 45
      {
        return N_SUD;  //Nejedna se o reseni sudoku - vracim N_SUD
      }
      else  //Pokud jsou soucty 45, vynuluji soucty pro dalsi radek a sloupec
      {
        soucetRadku = 0;  //Nulovani souctu prvku radku
        soucetSloupce = 0;  //Nulovani souctu prvku sloupce
      }
    }
  }

  if(subSudoku(m) != OK)  //Pokud neni nektera submatice sudoku o rozmerech 3x3
  {                       //spravna
    return N_SUD;  //vraci N_SUD - neni reseni sudoku
  }

  return Y_SUD;  //Pokud probehla funkce bez chyby, vracim Y_SUD - je reseni
}
/* Konec funkce testSudoku */

/*
 * Funkce pro hledani submatice v matici.
 * @param *m - matice, ve ktere hledame submatici
 * @param *sub - submatice, kterou hledame
 * @return - pokud nastane chyba, funkce vraci jeji kod, kdyz zadna chyba
 *           nenastane a submatice je submatici, vraci Y_SUB, pokud neni
 *           submatici, vraci N_SUB
 */
int subMatrix(MATRIX *m, MATRIX *sub)
{
  int howMany;  //Promenna pro pocet prvku puvodni matice
  int sr = 0, sc = 0;  //Indexy pozice v submatici
  int ok = 1;  //Promenna, ktera udava, zda se dva prvky rovnaji

  if(m->rows < sub->rows)  //Pokud je matice mensi jak submatice
  {
    return N_SUB;  //Vraci N_SUB
  }

  if(m->cols < sub->cols)  //Pokud je matice mensi jak submatice
  {
    return N_SUB;  //Vraci N_SUB
  }

  howMany = m->rows*m->cols;  //Spocitam pocet prvku v puvodni matici

  for(int i = 0; i < howMany; i++)  //Cyklus pro prochazeni matici
  {
    if(m->matrix[i/m->cols][i%m->cols] == sub->matrix[0][0])  //Hledam v matici
    {  //prvni cislo submatice
      if(sub->rows > (m->rows-(i/m->cols)))  //Pokud se nachazi na pozici,
      {                                      //ktera nevyhovuje pro velikost
        continue;                            //submatice, pokracuji v hledani
      }
      else if(sub->cols > (m->cols-(i%m->cols)))  //Pokud se nachazi na pozici,
      {                                           //ktera nevyhovuje pro velikost
        continue;                                 //submatice, pokracuji v hledani
      }
      else if((sub->rows > (m->rows-(i/m->cols))) && (sub->cols > (m->cols-(i%m->cols))))
      {  //Pokud dojdu na misto, kde uz nelze najit submatici
        return N_SUB;  //Vraci N_SUB
      }
      else
      {
        for(int c = (i%m->cols); c < ((i%m->cols)+sub->cols); c++)  //Sloupec
        {                                                           //matice
          for(int r = (i/m->cols); r < ((i/m->cols)+sub->rows); r++)  //Radek
          {                                                           //matice
            if(m->matrix[r][c] != sub->matrix[sr][sc])  //Pokud se nerovna
            {
              ok = 0;  //nastav ok na 0
            }
            sr++;  //Inkrementace pozice radku submatice
          }
          sr = 0;  //Nastaveni pozice radku submatice na zacatek radku
          sc++;  //Inkrementace pozice sloupce submatice
        }
        sc = 0;  //Nastaveni pozice sloupce submatice na prvni sloupec
        if(ok == 0){  //Pokud se nejmene dva prvky matice a submatice nerovnali
          ok = 1;  //nastav ok na 1 pro dalsi hledani
          continue;  //Pokracuj v hledani
        }
        else  //Pokud se ok rovna 1, tak je submatice submatici matice
        {
          return Y_SUB;  //Vracim Y_SUB - je submatici
        }
      }
    }
  }

  return N_SUB;  //Vraci N_SUB, pokud nebyla matice nalezena
}
/* Konec funkce subMatrix */

/*
 * Funkce freeMatrix uvolnuje alokovany prostor pro matici.
 * @param *m - matice, ve ktere chceme uvolnit prostor
 * Tato funkce nic nevraci.
 */
void freeMatrix(MATRIX *m)
{
  for(int i = 0; i < m->rows; i++)  //Cyklus pro prochazeni radku matice
  {
    free(m->matrix[i]);  //Uvolneni jednoho radku matice
  }

  free(m->matrix);  //Uvolneni poctu radku matice
}
/* Konec funkce freeMatrix */

/*
 * Funkce main
 * @param argc - pocet argumentu prikazove radky
 * @param *argv[] - argumenty prikazove radky
 * @return - pokud program skoncil dobre, vraci EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
  MATRIX first, second, third;  //Tri matice, vic jich neni v programu potreba
  int Err = OK;  //Promenna pro chyby programu

  if(!strcmp("-add", argv[1]))  //Pokud budeme delat soucet
  {
    if((Err = loadMatrix(&first, argv[2])) != OK)  //Nacteni prvniho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = loadMatrix(&second, argv[3])) != OK)  //Nacteni druheho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = addMatrix(&first, &second, &third)) != OK)  //Soucet matic
    {
      printErr(FALSE);  //Vypis chyby
      return EXIT_FAILURE;
    }
    freeMatrix(&first);  //Uvolneni pameti
    freeMatrix(&second);  //Uvolneni pameti
    printMatrix(&third);  //Vypis matice
    freeMatrix(&third);  //Uvolneni pameti
  }
  else if(!strcmp("-mult", argv[1]))  //Pokud budeme delat soucin
  {
    if((Err = loadMatrix(&first, argv[2])) != OK)  //Nacteni prvniho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = loadMatrix(&second, argv[3])) != OK)  //Nacteni druheho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = mulMatrix(&first, &second, &third)) != OK)  //Soucin matic
    {
      printErr(FALSE);  //Vypis chyby
      return EXIT_FAILURE;
    }
    freeMatrix(&first);  //Uvolneni pameti
    freeMatrix(&second);  //Uvolneni pameti
    printMatrix(&third);  //Vypis matice
    freeMatrix(&third);  //Uvolneni pameti
  }
  else if(!strcmp("-sudoku", argv[1]))  //Pokud budeme delat test sudoku
  {
    if((Err = loadMatrix(&first, argv[2])) != OK)  //Nacteni prvniho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = testSudoku(&first)) != OK)  //Test sudoku
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    freeMatrix(&first);  //Uvolneni pameti
  }
  else if(!strcmp("-submatrix", argv[1]))  //Pokud budeme delat test submatice
  {
    if((Err = loadMatrix(&first, argv[2])) != OK)  //Nacteni prvniho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = loadMatrix(&second, argv[3])) != OK)  //Nacteni druheho souboru
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    if((Err = subMatrix(&first, &second)) != OK)  //Test submatice
    {
      printErr(Err);  //Vypis chyby
      return EXIT_FAILURE;
    }
    freeMatrix(&first);  //Uvolneni pameti
    freeMatrix(&second);  //Uvolneni pameti
  }
  else if(!strcmp("-h", argv[1]))  //Pokud budeme vypisovat napovedu
  {
    printErr(HELP);  //Vypis napovedy
    return EXIT_SUCCESS;
  }
  else
  {
    printErr(BAD_CMD);  //Vypis chyby
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
/* Konec funkce main */

/* Konec souboru proj3.c */
