
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
  L->Act = NULL;    // Inicializujeme listovu variable Act na NULL
  L->First = NULL;  // Inicializujeme listovu variable First na NULL


 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
  First(L); // Nastavime Activitu listu L na prvy prvok listu

  while (Active(L)) // Pokial sa aktivny prvok nerovna NULL
  {
    Succ(L);          // Posunieme sa na dalsi prvok ktory sa nachadza za aktivnym prvkom
    DeleteFirst(L);   // Vymazeme prvy prvok zo zoznamu L
  }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/
  tElemPtr el = (tElemPtr)malloc(sizeof(struct tElem)); // Alokujeme strukturu tElem a pripevnime jej pointer na variable el

  el->data = val;       // nastavime data na hodnotu val
  el->ptr = L->First;   // nastavime aby pointer ptr tohto zoznamu ukazoval na prvy item z listu L

  L->First = el;        // V liste L nastavime aby prvy prvok ukazoval na nas item

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/

  L->Act = L->First;  // Nastavime aktivny prvok na prvy prvok zoznamu L
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/

  if (L->First == NULL)     // Ak je prvy prvok zoznamu prazdny tak vratime chybu
  {
    Error();
    return;
  }
  *val = L->First->data;    // Ak nie je list prazdny vratime hodnotu

// solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
  if (L->First == NULL) // Ak je zoznam prazdny nerobime nic
  {
    return;
  }

  if (L->First == L->Act) // Ak je prvy prvok ten isty ako aktivny
  {
    L->Act = NULL;        // Tak ho nastavime na NULL
  }
  tElemPtr tmp_ptr = L->First;  // Vytvorime temporaly pointer na prvu strukturu zo zoznamu
  L->First = tmp_ptr->ptr;      // Posunieme ukazovatel na vrch listu aby ukazoval na dalsi prvok
  free(tmp_ptr);                // Uvolnime prvy prvok

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void PostDelete (tList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/
  if (L->First == NULL)   // Ak je zoznam prazdny nerobime nic
  {
    return;
  }

  if (L->Act->ptr != NULL)      // Ak nasledujuci aktivny prvok nie je NULL tak mozme pokracovat
  {
    struct tElem *ptr = L->Act->ptr;  // Ulozime si ukazovatel na nasledujuci item
    L->Act->ptr = L->Act->ptr->ptr;   // Nastavime nasledujuci prvok aktivneho prvku na za nim nasledujuci prvok
    free(ptr);                        // Uvolnime nasledujuci prvok
  }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/

  if (Active(L))  // Ak nie je zvoleny aktivny prvok nerobime nic
  {
    tElemPtr el = malloc(sizeof(struct tElem)); // Allokujeme strukturu tElem a pripevnime jej pointer na el variable
    if (!el)  // Ak sa nepodarila alokacia tak vratime error
    {
      Error();
      return;
    }
    el->data = val;     // Nastavime data el na hodnotu val
    el->ptr = NULL;     // Nastavime pointer na nasledujuci prvok na NULL
    if (L->Act->ptr != NULL)  // Ak nasledujuci prvok aktivneho zoznamu nie je NULL
    {
      el->ptr = L->Act->ptr;  // Tak nastavime nasledujuci prvok itemu na nasledujuci prvok Aktivneho itemu
    }
    L->Act->ptr = el; //  Nasledujuci prvok aktivneho zoznamu nastavime na item

  }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
  if (L->Act == NULL) // Ak aktivny prvok je NULL jedna sa o chybu a vratime error
  {
    Error();
    return;
  }
  *val = L->Act->data;  // Vratime ukazovatel na integer z Aktivneho itemu na hodnotu val
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/

  if (L->Act != NULL) // Ak aktivny prvok nie je NULL
  {
    L->Act->data = val; // Tak prepiseme data aktualne aktivneho itemu na hodnotu val
  }

  // Inac nerobime nic

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
  if (Active(L) == 1) // Robime iba ak je aktivny prvok
  {
    L->Act = L->Act->ptr; // Nastavime aktivny prvok zoznamu na nasledujuci item
  }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return.
**/

  return (L->Act != NULL) ? 1 : 0;  // Vratime 0 ak Aktivny prvok zoznamu je NULL inac vratime 1
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

/* Konec c201.c */
