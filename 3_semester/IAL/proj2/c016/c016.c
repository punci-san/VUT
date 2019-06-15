
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht )
{
	// Ak nie je hashtable NULL
	if (ptrht != NULL)
	{
		// Tak inicializujeme kazdy item na NULL
		for (int i = 0; i < HTSIZE; i++)
		{
			// Inicializuj item I na NULL
			(*ptrht)[i] = NULL;
		}
	}
 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	// Ak pthth nie je NULL
	if (ptrht != NULL)
	{
		// Ziskame index
		tHTItem *tmp = (*ptrht)[hashCode(key)];
		// Snazime sa najst item
		while (tmp != NULL)
		{
			// Ak sa item rovna klucu
			if (strcmp(tmp->key,key) == 0)
			{
				// Vraciame item
				return tmp;
			}
			// Inac skusime dalsi item v poradi
			tmp = tmp->ptrnext;
		}
		// Ak sme dosli az na koniec a nenasli nic vraciame NULL
	}
	// List je prazdny vraciame NULL
	return NULL;

 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data )
{
	// Skusime vyhladat item
	tHTItem *tmp = htSearch(&(*ptrht),key);

	// Ak tmp ukazuje na nieco znamena to ze nasiel
	if (tmp != NULL)
	{
		// A mozme updatovat
		tmp->data = data;
	}
	// Inac nenachadza sa
	else
	{
		// Zistime index pre key
		int index = hashCode(key);
		// Allokujeme miesto
		tmp = malloc(sizeof(tHTItem));
		// Pridame hodnoty
		tmp->key = key;
		tmp->data = data;
		tmp->ptrnext = (*ptrht)[index];

		// AK je tak normalne pridavame
		(*ptrht)[index] = tmp;
	}

 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	// Pokusime sa najst cez funkciu item
	tHTItem *item = htSearch(&(*ptrht),key);
	// AK sme nasli item tak mozme vratit jeho hodnotu
	if (item != NULL)
	{
		return &item->data;
	}
	// Inac vraciame NULL
	return NULL;

 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key )
{
	// Mazeme len ak nie je table prazdny
	if ((*ptrht) != NULL)
	{
		// Ziskame index
		int index = hashCode(key);

		// Ziskame item na indexe
		tHTItem *tmp = (*ptrht)[index];

		// Ak sme nenasli nic chyba
		if (tmp == NULL)
		{
			return;
		}

		// Zistime ci item je ten co chceme zmazat
		if (strcmp(tmp->key,key) == 0)
		{
			// Nastavime pointer na nasledujuci item
			(*ptrht)[index] = tmp->ptrnext;

			// Uvolnime item
			free(tmp);

			// Vynulujeme ho
			tmp = NULL;
			return;
		}

		// Vytvorime tmp stareho
		tHTItem *prev_tmp = tmp;

		// do tmp dame dalsi
		tmp = tmp->ptrnext;

		// A hladame spravny kluc
		while (tmp != NULL)
		{
			// Hladame key
			if (strcmp(tmp->key,key) == 0)
			{
				// Nastavime pointer
				prev_tmp->ptrnext = tmp->ptrnext;

				// Uvolnime pointer
				free(tmp);

				// Vynulujeme
				tmp = NULL;
				return;
			}
			// Nastavime tmp item na prev item
			prev_tmp = tmp;
			tmp = tmp->ptrnext;
		}
	}
 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht )
{
	// Ak nie je tabulka prazdna mozme clearovat
	if ((*ptrht) != NULL)
	{
		tHTItem *tmp = NULL;
		// Kazdy list itemov uvolnime
		for (int i = 0; i < HTSIZE; i++)
		{
			tmp = (*ptrht)[i];
			while (tmp != NULL)
			{
				// Presmerujeme na dalsi
				(*ptrht)[i] = tmp->ptrnext;
				// Uvolnime tmp
				free(tmp);
				// Nastavime tmp na dalsie
				tmp = (*ptrht)[i];
			}
		}
	}
 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}
