
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

  L->First = NULL;    // Inicializujeme First pointer listu L na NULL
  L->Act = NULL;      // Inicializujeme Act pointer listu L na NULL
  L->Last = NULL;     // Inicializujeme Last pointer listu L na NULL
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
  DLFirst(L); // Nastavi aktivitu zoznamu na prvy prvok

  while (L->Act != NULL)    // Pokial mame itemy na mazanie tak mazeme
  {
    if (L->Act->rptr == NULL) // Ked najdeme posledny prvok zoznamu vymazeme aktivny prvok
    {
      free(L->Act);
      break;
    }
    L->Act = L->Act->rptr;  // Posunieme Aktivny prvok na dalsi v zozname
    free(L->Act->lptr);     // Uvolnime ten prvok ktory bol aktivny pred tym nez sme ho posunuly
  }
  DLInitList(L);  // Vratime list na prvotny stav
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
  tDLElemPtr item = malloc(sizeof(struct tDLElem)); // Alokujeme strukturu a pripevnime ju na pointer item
  if (!item)// Ak item nebol dobre allokovany vratime error
  {
    DLError();
    return;
  }
  item->data = val; // Nastavime data itemu na hodnotu val
  item->rptr = L->First;  // Pravy pointer itemu nastavime na First prvok
  item->lptr = NULL;  // Lavy pointer itemu nastavime na NULL
  if (L->First != NULL) // Ak nie je ukazovatel na First item NULL
  {
    L->First->lptr = item;  // Tak mozme nastavit lavy pointer First itemu na tento item
  }
  if (L->Last == NULL)  // Ak Last pointer je NULL
  {
    L->Last = item; // Tak nastavime tento item ako NULL
  }
  L->First = item;  // Ukazovatel na prvy item v liste nastavime na tento lebo tento item instertujeme na prve miesto
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
  tDLElemPtr item = malloc(sizeof(struct tDLElem)); // Alokujeme novu strukturu a pointer item nasmerujeme na tuto strukturu
  if (!item)  // Ak sa nam nepodarilo alokovat item tak vratime error
  {
    DLError();
    return;
  }
  item->data = val; // Nastavime data itemu na hodnotu val
  item->lptr = L->Last; // Lavy ukazovatel nasho itemu nastavime na Last item zoznamu L
  item->rptr = NULL;  // Nastavime Pravy ukazovatel nasho itemu na NULL
  if (L->Last != NULL)  // Ak nie je posledny item NULL
  {
    L->Last->rptr = item; // Mozme nastavit dalsi item Last itemu nastavit na nas item
  }
  if (L->First == NULL) // Ak je First item NULL
  {
    L->First = item;  // Nastavime ho na tento item
  }
  L->Last = item; // Nastavime Last pointer na nas item lebo insertujeme last
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->First;  // Nastavime aktivny prvok na Prvy prvok zoznamu
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->Last; // Nastavime aktivny prvok na posledny prvok zoznamu
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  if (L->First == NULL) // Ak je ukazovatel na prvy prvok zoznamu prazdny
  {
    DLError();  // Vraciame error
    return;
  }
  *val = L->First->data;  // Inac vraciame data prveho itemu
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  if (L->Last == NULL)  // Ak je ukazovatel na posledny prvok listu prazdny
  {
    DLError();  // Vraciame error
    return;
  }
  *val = L->Last->data; // Inac vraciame data posledneho itemu
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
  if (L->First == NULL) // Ak je ukazovatel na prvy prvok zoznamu prazdny nerobime nic
  {
    return;
  }
  if (L->First == L->Act) // Ak aktivny prvok ukazuje na prvy prvok
  {
    L->Act = NULL;  // Tak vynulujeme aktivny prvok
  }
  if (L->First->rptr == NULL) // Ak je posledny item v zozname
  {
    free(L->First); // Uvolnime prvy prvok
    L->First = NULL;  // Ukazovatel na prvy prvok vynulujeme
    L->Last = NULL;   // Ukazovatel na posledny prvok vynulujeme
    return;
  }
  // Ak nie je posledny item v zozname
  L->First = L->First->rptr;  // Posunieme ukazovatel na prvy item v zozname o jedno do prava
  free(L->First->lptr); // Uvolnime ten prvy item
  L->First->lptr = NULL;  // Nakolko je to prvy item tak jeho lavy pointer ma byt NULL
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
  if (L->Last == NULL)  // Ak ukazovatel na posledny item je NULL tak nemusime nic robit
  {
    return;
  }
  if (L->Last == L->Act)  // Ak aktivny item je ten isty ako posledny
  {
    L->Act = NULL;  // Tak aktivny prvok vynulujeme
  }
  if (L->Last->lptr == NULL)  // Ak je toto posledny item
  {
    free(L->Last);  // Uvolnime ho
    L->First = NULL;  // Ukazovatel na prvy item vynulujeme
    L->Last = NULL;   // Ukazovatel na posledny item vynulujeme
    return;
  }
  // Ak item nie je posledny
  L->Last = L->Last->lptr;  // Posunieme ukazovatel na posledny item o jedno dolava
  free(L->Last->rptr);    // Uvolnime posledny item
  L->Last->rptr = NULL; // Ukazovatel posledneho itemu na pravy item vynulujeme
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
  if (DLActive(L) == 0 || L->Act->rptr == NULL) // Ak nemame ziadny aktivny prvok alebo ak je toto posledny item tak nerobime nic
  {
    return;
  }
  if (L->Act->rptr == L->Last)  // Ak je aktivny prvok hned pred poslednym
  {
    L->Last = L->Act; // Tak nastavime posledny aby sa rovnal aktivnemu
  }

  tDLElemPtr tmp = L->Act->rptr;  // dame si do docasnej premennej nasledujuci item za aktivnym prvkom
  L->Act->rptr = tmp->rptr; // nasledujuci prvok nasledujuceho prvku dame na nasledujuci aktivny prvok

  if (L->Act->rptr != NULL) // Ak nasledujuci prvok aktivneho prvku nie je NULL
  {
    L->Act->rptr->lptr = L->Act;  // Tak lava strana itemu nasledujuceho po aktivnom prvku nastavime na aktivny prvok
  }
  free(tmp);  // Uvolnime item za aktivnym prvkom
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
  if (DLActive(L) == 0 || L->Act->lptr == NULL) // Ak je prvok neaktivny alebo je prvy v zozname nerobime nic
  {
    return;
  }
  if (L->Act->lptr == L->First) // Ak item pred aktivnym prvok je prvy item v zozname
  {
    L->First = L->Act;  // Posunieme aktivny prvok aby bol prvy
  }
  tDLElemPtr tmp = L->Act->lptr;  // Predchadzajuci item aktivneho itemu si ulozime
  L->Act->lptr = tmp->lptr; // lavy item od aktivneho itemu posunieme na lavejsi item
  if (L->Act->lptr != NULL) // Ak lavy item aktivneho itemu nie je prazdny
  {
    L->Act->lptr->rptr = L->Act;  // Linkneme vedlajsi item na aktivny prvok
  }
  free(tmp);  // Uvolnime prvok
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
  if (L->Act != NULL) // Ak mame aktivny item mozme pracovat
  {
    tDLElemPtr item = malloc(sizeof(struct tDLElem)); // Allokujeme novy item
    if (!item)  // Ak bola chyba alokovania
    {
      DLError();  // Vratime error
      return;
    }
    item->data = val; // Data itemu nastavime na hodnotu val
    if (L->Act->rptr == NULL) // Ak je aktivny item posledny
    {
      L->Last = item; // Tak nastavime nas pridany item na koniec
    }
    item->lptr = L->Act;  // Lava strana nasho itemu bude odkazovat na Aktivny item
    item->rptr = L->Act->rptr;  // Prava strana nasho itemu bude odkazovat na pravy item aktivneho itemu
    L->Act->rptr = item;  // Prava strana aktivneho itemu bude odkazovat na nas vlozeny item
    if (item->rptr != NULL) // Ak prava strana nasho vlozeneho itemu nie je NULL
    {
      item->rptr->lptr = item;  // Pravemu itemu od nasho vlozeneho itemu nastavime lavu stranu na nas item
    }
  }
  // Ak nie nerobime nic
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
  if (L->Act != NULL) // Pracujeme iba ak je aktivny prvok
  {
    tDLElemPtr item = malloc(sizeof(struct tDLElem)); // Allokujeme strukturu
    if (!item)  // Ak bol problem pri allokovani pamate
    {
      DLError();  // Vratime error
      return;
    }
    item->data = val; // Data itemu nastavime na hodnotu val
    item->lptr = L->Act->lptr;  // Lavu stranu itemu nastavime na lavy item aktivneho itemu
    item->rptr = L->Act;  // Pravu stranu itemu nastavime na aktivny item
    if (L->Act->lptr == NULL) // Ak lava strana aktivneho itemu je NULL aktivny item je prvy v liste
    {
      L->First = item;  // Nastavime ukazovatel na zaciatok zaznamu na nas item
    }
    if (item->lptr != NULL) // Ak lava strana itemu nie je prazdna
    {
      item->lptr->rptr = item;  // Nastavime pravu stranu laveho itemu od nasho vkladaneho itemu na nas item
    }
    L->Act->lptr = item;  // Nastavime lavu stranu aktivneho itemu na nas vkladany item
  }
  // Ak nie nerobime nic
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
  if (DLActive(L) == 0) // Ak nemame aktivny prvok
  {
    DLError();  // Vraciame error
    return;
  }
  *val = L->Act->data;  // Inac vraciame hodnotu itemu
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
  if (DLActive(L) != 0) // Ak mame aktivny item
  {
    L->Act->data = val; // Aktualizujeme data aktivneho itemu
  }
  // Inac nerobime nic
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/

  if (DLActive(L) != 0) // Ak mame aktivny item
  {
    L->Act = L->Act->rptr;  // Posunieme aktivitu na item dalsi v poradi
  }
  // Inac nerobime nic
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
  if (DLActive(L) != 0) // Ak je aktivny item
  {
    L->Act = L->Act->lptr;  // Posunieme aktivitu na predchadzajuci item
  }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/

  return (L->Act == NULL) ? 0 : 1;  // Vratime 0 pri Neaktivnom zazname a 1 pri aktivnom zazname 
}

/* Konec c206.c*/
