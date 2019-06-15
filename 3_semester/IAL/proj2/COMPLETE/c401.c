
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvořil: Petr Přikryl, listopad 1994
**                                         Úpravy: Andrea Němcová, prosinec 1995
**                                                      Petr Přikryl, duben 1996
**                                                   Petr Přikryl, listopad 1997
**                                  Převod do jazyka C: Martin Tuček, říjen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** Implementujte rekurzivním způsobem operace nad binárním vyhledávacím
** stromem (BVS; v angličtině BST - Binary Search Tree).
**
** Klíčem uzlu stromu je jeden znak (obecně jím může být cokoliv, podle
** čeho se vyhledává). Užitečným (vyhledávaným) obsahem je zde integer.
** Uzly s menším klíčem leží vlevo, uzly s větším klíčem leží ve stromu
** vpravo. Využijte dynamického přidělování paměti.
** Rekurzivním způsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíčem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zrušení uzlu se zadaným klíčem
**   BSTDispose ... zrušení celého stromu
**
** ADT BVS je reprezentován kořenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíč (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Přesnou definici typů
** naleznete v souboru c401.h.
**
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
**
** Ověřit, zda byl již strom předaný přes RootPtr inicializován, nelze,
** protože před první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor využívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to před vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by totiž mohlo vést ke ztrátě přístupu
** k dynamicky alokované paměti (tzv. "memory leak").
**
** Všimněte si, že se v hlavičce objevuje typ ukazatel na ukazatel.
** Proto je třeba při přiřazení přes RootPtr použít dereferenční operátor *.
** Ten bude použit i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/
		(*RootPtr) = NULL;	// Inicializujeme na NULL
		//solved = FALSE;		  /* V prípadě řešení smažte tento řádek! */

}

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)
{
	/*  ---------
	** Funkce vyhledá uzel v BVS s klíčem K.
	**
	** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
	** vrací obsah příslušného uzlu.´Pokud příslušný uzel není nalezen, vrací funkce
	** hodnotu FALSE a obsah proměnné Content není definován (nic do ní proto
	** nepřiřazujte).
	**
	** Při vyhledávání v binárním stromu bychom typicky použili cyklus ukončený
	** testem dosažení listu nebo nalezení uzlu s klíčem K. V tomto případě ale
	** problém řešte rekurzivním volání této funkce, přičemž nedeklarujte žádnou
	** pomocnou funkci.
	**/
	// Iba ak nie je zoznam prazdny mozme hladat
	if (RootPtr != NULL)
	{
		// Nasli sme
		if (RootPtr->Key == K)
		{
			// Ked sme nasli vratime kontent
			*Content = RootPtr->BSTNodeCont;
			// A true lebo sme nasli
			return TRUE;
		}
		// Ak je K co hladame mensie nez K tohto nodu hladame vlavo
		else if (RootPtr->Key > K)
		{
			// skontrolujeme ci je vobec nieco vlavo
			if (RootPtr->LPtr != NULL)
			{
				// Hladame vlavo
				return BSTSearch(RootPtr->LPtr,K,Content);
			}
			// Ak sa vlavo nic nenachadza nenasli sme nic
			return FALSE;
		}
		// Ak je K co hladame vacsie nez K tohto nodu hladame vpravo
		else if (RootPtr->Key < K)
		{
			// Skontrolujeme ci je vobec nieco vpravo
			if (RootPtr->RPtr != NULL)
			{
				// Hladame vpravo
				return BSTSearch(RootPtr->RPtr,K,Content);
			}
			// Ak sa vpravo nic nenachadza vraciame FALSE
			return FALSE;
		}
		// Nic sa nenaslo
		return FALSE;

	}
	// Nenajde sa nic ak je zoznam prazdny
	return FALSE;
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)
{
	/*   ---------
	** Vloží do stromu RootPtr hodnotu Content s klíčem K.
	**
	** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
	** s klíčem K nahrazen novou hodnotou. Pokud bude do stromu vložen nový
	** uzel, bude vložen vždy jako list stromu.
	**
	** Funkci implementujte rekurzivně. Nedeklarujte žádnou pomocnou funkci.
	**
	** Rekurzivní implementace je méně efektivní, protože se při každém
	** rekurzivním zanoření ukládá na zásobník obsah uzlu (zde integer).
	** Nerekurzivní varianta by v tomto případě byla efektivnější jak z hlediska
	** rychlosti, tak z hlediska paměťových nároků. Zde jde ale o školní
	** příklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
	**/

	// Ak sme bud uplne na zaciatku alebo uplne na konci pridavame novy note
	if ((*RootPtr) == NULL)
	{
		// Allokujeme sktrukturu
		(*RootPtr) = malloc(sizeof(struct tBSTNode));
		// Nastavime key
		(*RootPtr)->Key = K;
		// Nastavime data
		(*RootPtr)->BSTNodeCont = Content;
		// Inicializujeme Lavy node na NULL
		(*RootPtr)->LPtr = NULL;
		// Inicializujeme Pravy node na NULL
		(*RootPtr)->RPtr = NULL;
	}
	// Inac hladame kde to ulozit
	else
	{
		/*
			Do nodov sa uklada na zaklade velkosti
			Mensie => VLAVO
			Vacsie => Vpravo
			To iste => Prepise sa
		*/
		// Ak je K mensie ako current volame znova funkciu Left nodom
		if (K < (*RootPtr)->Key)
		{
			BSTInsert(&(*RootPtr)->LPtr,K,Content);
		}
		// Ak je K vacsie ako current volame znova funkciu z Right nodom
		else if ((*RootPtr)->Key < K)
		{
			BSTInsert(&(*RootPtr)->RPtr,K,Content);
		}
		// Ak je K == Key tak prepisujeme
		else if ((*RootPtr)->Key == K)
		{
			(*RootPtr)->BSTNodeCont = Content;
		}
		// Dalsia moznost sa nemoze stat
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
	/*   ------------------
	** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
	**
	** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude přesunuta hodnota
	** nejpravějšího uzlu v podstromu, který je určen ukazatelem RootPtr.
	** Předpokládá se, že hodnota ukazatele RootPtr nebude NULL (zajistěte to
	** testováním před volání této funkce). Tuto funkci implementujte rekurzivně.
	**
	** Tato pomocná funkce bude použita dále. Než ji začnete implementovat,
	** přečtěte si komentář k funkci BSTDelete().
	**/

	// Pracujeme iba ked RootPtr nie je NULL
	if ((*RootPtr) != NULL)
	{
		// Hladame ten najvacsi prvok ktory sa nachadza vpravo aby sme ho replacli
		if ((*RootPtr)->RPtr != NULL)
		{
			// Ak sme zistili ze vpravo je este vacsi prvok tak ideme tym smerom
			ReplaceByRightmost(PtrReplaced,(&(*RootPtr)->RPtr));
		}
		// Ak je toto najpravejsi prvok tak replacujeme PtrReplaced s tymto
		else
		{
			// Nahradime key
			PtrReplaced->Key = (*RootPtr)->Key;
			// Nahradime hodnotu
			PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
			// Vytvorime docasnu variable
			tBSTNodePtr tmp =  (*RootPtr);
			// Presunieme lavy NOde
			(*RootPtr) = (*RootPtr)->LPtr;
			// Mozme volne uvolnit lebo vieme ze toto je posledny prvok a jeho lavy node sme uz presunuly
			free(tmp);
			// Vynulujeme tmp
			//tmp = NULL;
		}
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zruší uzel stromu, který obsahuje klíč K.
**
** Pokud uzel se zadaným klíčem neexistuje, nedělá funkce nic.
** Pokud má rušený uzel jen jeden podstrom, pak jej zdědí otec rušeného uzlu.
** Pokud má rušený uzel oba podstromy, pak je rušený uzel nahrazen nejpravějším
** uzlem levého podstromu. Pozor! Nejpravější uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivně s využitím dříve deklarované
** pomocné funkce ReplaceByRightmost.
**/

	// Robime len ked nie je node prazdny
	if ((*RootPtr) != NULL)
	{
		// Ak je Key vacsie nez K  jedna sa o lavy NODE
		if (K < (*RootPtr)->Key)
		{
			// Hladame item na zmazanie v lavom node
			BSTDelete((&(*RootPtr)->LPtr),K);
		}
		// Ak je Key mensi nez K jedna sa o pravy NODE
		else if (K > (*RootPtr)->Key)
		{
			// Hladame item na zmazanie v pravom node
			BSTDelete((&(*RootPtr)->RPtr),K);
		}
		// Ak K neni ani vacsie ani mensia nasli sme spravny kluc
		else
		{
			// Treba zistit o aky typ nodu sa jedna

			// Ak node obsahuje oba pointere
			if (((*RootPtr)->LPtr != NULL) && ((*RootPtr)->RPtr != NULL))
			{
				// POuzijeme pomocnu funkciu ReplaceByRightmost
				ReplaceByRightmost(*RootPtr,&(*RootPtr)->LPtr);
			}
			// Inac obsahuje len jeden alebo ziadny
			else
			{
				// Vytvorime tmp variable na uvolnenie
				tBSTNodePtr tmp = *RootPtr;
				// Skontrolujeme ci node obsahuje lavy node
				if ((*RootPtr)->LPtr != NULL)
				{
					// Nastavime root na lavy node
					(*RootPtr) = (*RootPtr)->LPtr;
				}
				// Inac nastavime na pravy
				else
				{
					// Nastavime root na pravy node
					(*RootPtr) = (*RootPtr)->RPtr;
				}
				// Uvolnime pointer
				free(tmp);
				// Vynulujeme pointer
				tmp = NULL;
			}
		}
	}
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BSTDispose (tBSTNodePtr *RootPtr) {
/*   ----------
** Zruší celý binární vyhledávací strom a korektně uvolní paměť.
**
** Po zrušení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivně bez deklarování pomocné
** funkce.
**/
		/*
			Mazanie bude prebiehat vzdy tym sposobom ze najprv skontrolujeme
			pravy a lavy node a ak su tam tak najprv mazeme tie volanim znova tejto funkcie
			co znamena ze sa to bude mazat zdola hore
		*/
		// Mazeme len vtedy ked struktura nie je prazdna
		if (*RootPtr != NULL)
		{
			// Mazeme najprv lavy node
			BSTDispose(&(*RootPtr)->LPtr);
			// A potom ten pravy
			BSTDispose(&(*RootPtr)->RPtr);
			// Najprv sa vymazali prave a lave nody
			(*RootPtr)->RPtr = NULL;	// Pravy na NULL
			(*RootPtr)->LPtr = NULL;	// Lavy na NULL
			free(*RootPtr);	// Uvolnime node
			(*RootPtr) = NULL;	// Nastavime ho na NULL
		}
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}

/* konec c401.c */
