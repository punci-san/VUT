
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/

	if (Ptr==NULL)
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;
}

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);
	}
	else {
		return (S->a[S->top--]);
	}
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;
}

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);
	}
	else {
		return(S->a[S->top--]);
	}
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat.
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/

	if ((*RootPtr) != NULL)
	{
		(*RootPtr) = NULL;
	}
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

	// Vytvorime ukazovatel na current node
	tBTNodePtr *curr = &(*RootPtr);

	// Pokial nie je curr NULL alebo neukoncime tak nepridavame
	while ((*curr) != NULL)
	{
		// Ak sa rovnaju tak koncime nerobime nic
		if ((*curr)->Cont == Content)
		{
			return;
		}
		// Ak nie tak hladame ci ideme vpravo alebo vlavo
		else if ((*curr)->Cont > Content)
		{
			// Ideme vlavo
			curr = &(*curr)->LPtr;
		}
		// Inac ideme vpravo
		else
		{
			// Ideme vpravo
			curr = &(*curr)->RPtr;
		}
	}
	// Vytvorime node
	(*curr) = malloc(sizeof(tBTNodePtr));
	(*curr)->Cont = Content;
	(*curr)->LPtr = NULL;
	(*curr)->RPtr = NULL;
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)
{
	/*   -----------------
	** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
	**
	** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
	** a ukazatele na ně is uložíme do zásobníku.
	**/

	/*
		Ideme od 0 => 9 cize od najmensieho po najvecsie
	*/
		// Hladame pokial nedojdeme na koniec
		while (ptr != NULL)
		{
			// Printneme
			BTWorkOut(ptr);
			// Pushneme na stak
			SPushP(Stack,ptr);
			// Posunieme sa dolava
			ptr = ptr->LPtr;
		}



	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTPreorder (tBTNodePtr RootPtr)
{
	/*   ----------
	** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
	** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
	** realizujte jako volání funkce BTWorkOut().
	**/
	tStackP stack;
	SInitP(&stack);

	// Zacneme od zaciatku
	tBTNodePtr tmp = RootPtr;

	// Node ktorym budeme prechadzat vpravo
	//tBTNodePtr topRight = tmp;

	// Pokial nie sme uplne vpravo na poslednom
	while (tmp != NULL)
	{
		// Prebehneme vsetko az kym nenatrafime vlavo
		Leftmost_Preorder(tmp,&stack);

		tBTNodePtr tmp_2 = NULL;
		// Zacneme prechadzat o jedno vyssie a doprava
		while (tmp_2 == NULL)
		{
			// Ak je zasobnik prazdny
			if (SEmptyP(&stack))
			{

				return;
			}
			// Inac popujeme
			tmp_2 = STopPopP(&stack);

			// Ked popneme tak dame tmp_2 hodnotu praveho nodu
			tmp_2 = tmp_2->RPtr;
		}
		tmp = tmp_2;
	}


	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}


/*                                  INORDER                                   */

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku.
**/

		// Hladame pokial nedojdeme na koniec
		while (ptr != NULL)
		{
			// Pushneme na stak
			SPushP(Stack,ptr);
			// Posunieme sa dolava
			ptr = ptr->LPtr;
		}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}

void BTInorder (tBTNodePtr RootPtr)	{
	/*   ---------
	** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
	** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
	** realizujte jako volání funkce BTWorkOut().
	**/

	// Pokial nie je strom prazdny mozme realizovat
	if (RootPtr != NULL)
	{
		// Vytvorime stack
		tStackP stack;
		// Inicializujeme ho
		SInitP(&stack);
		// Vytvorime ukazovatel ktory budeme pouzivat na presun cez cely strom
		tBTNodePtr tmp = RootPtr;
		// Prechadzame pokial nie sme na konci
		while (tmp != NULL)
		{
			// Prejdeme cely strom a pridame ho do stacku
			Leftmost_Inorder(tmp,&stack);
			// Temporaly ukazovatel cislo 2
			tBTNodePtr tmp_2 = NULL;
			while (tmp_2 == NULL)
			{
				// Skontrolujeme stack
				if (SEmptyP(&stack))
				{
					// Ak je prazdny koncime
					return;
				}
				// Ak nie popneme
				tmp_2 = STopPopP(&stack);
				// Pritneme co sme popli
				BTWorkOut(tmp_2);
				// Posunieme sa napravo
				tmp_2 = tmp_2->RPtr;
			}
			tmp = tmp_2;
		}
	}


	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/

		// Hladame pokial nedojdeme na koniec
		while (ptr != NULL)
		{
			// Pushneme na stak
			SPushP(StackP,ptr);
			// Pushneme na stack BOOL
			SPushB(StackB,FALSE);
			// Posunieme sa dolava
			ptr = ptr->LPtr;
		}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/

		// Vytvorime stacky
		tStackP stackP;
		tStackB stackB;
		// Inicializujeme ich
		SInitP(&stackP);
		SInitB(&stackB);
		// Vytvorime temporaly pointer cez ktory sa budeme presuvat strukturou
		tBTNodePtr tmp = RootPtr;


		// Prejdeme celu lavu stranu
		Leftmost_Postorder(tmp,&stackP,&stackB);

		// Oba nesmu byt prazdne
		while (!SEmptyP(&stackP) && !SEmptyB(&stackB))
		{
			// Vytvorime 2 variable na uvolnovanie
			tBTNodePtr tmp_2 = STopPopP(&stackP);
			bool seen = STopPopB(&stackB);
			// Ak to bolo prezrete
			if (seen == TRUE)
			{
				// Znamena ze uz sme tam boli a presli sme to cele a mozme to printnut
				BTWorkOut(tmp_2);
			}
			// Znamena ze sme to este cele nepresli
			else
			{
				// A ak je na pravej strane node tak chceme prechadzat ten node
				if (tmp_2->RPtr != NULL)
				{
					// Ale predtim nez zacneme prechadzat musime si tam dat tento node ako seen
					SPushP(&stackP,tmp_2);
					SPushB(&stackB,TRUE);
					Leftmost_Postorder(tmp_2->RPtr,&stackP,&stackB);
				}
				else
				{
					BTWorkOut(tmp_2);
				}
			}

		}




	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	// Temporaly node bude pouzity na uvolnovanie
	tBTNodePtr free_node = NULL;
	// Vytvorime stack
	tStackP stack;
	// Inicializujeme stack
	SInitP(&stack);
	// Pushneme top node
	SPushP(&stack,*RootPtr);
	// Pokial nie je stack prazdny
	while (SEmptyP(&stack) == 0)
	{
		free_node = STopPopP(&stack);
		// Uvolnime tento node iba ak tento node nie je prazdny
		if (free_node != NULL)
		{
			// Pushneme lavy node
			SPushP(&stack,free_node->LPtr);
			// Pushneme pravy node
			SPushP(&stack,free_node->RPtr);
			// Uvolnime
			free(free_node);
		}
	}
	(*RootPtr) = NULL;


	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/* konec c402.c */
