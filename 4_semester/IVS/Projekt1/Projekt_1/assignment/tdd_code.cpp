//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     MATÚŠ ŠKUTA <xskuta04@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author MATÚŠ ŠKUTA
 *
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
  root = NULL;
}

PriorityQueue::~PriorityQueue()
{
  // Get first
  PriorityQueue::Element_t *tmp = PriorityQueue::GetHead();
  PriorityQueue::Element_t *last_item = PriorityQueue::GetHead();
  // Loop until you find the end
  while (tmp != NULL)
  {
    root = tmp->pNext;

    free(tmp);

    tmp = root;
  }
}

void PriorityQueue::Insert(int value)
{
  // Get first element
  PriorityQueue::Element_t *tmp = PriorityQueue::GetHead();
  PriorityQueue::Element_t *item = (PriorityQueue::Element_t *)malloc(sizeof(PriorityQueue::Element_t));

  // Set values
  item->value = value;
  item->pNext = NULL;
  item->pPrev = NULL;

  // Are we inserting first item ?
  if (tmp == NULL)
  {
    root = item;
    return;
  }
  // There are more items
  else
  {
    // Look for where to insert
    tmp = PriorityQueue::Find(value);

    // We are inserting already inserted value
    if (tmp != NULL)
    {
      // Insert after
      item->pPrev = tmp;
      item->pNext = tmp->pNext;

      if (tmp->pNext != NULL)
      {
        tmp->pNext->pPrev = item;
      }
      tmp->pNext = item;
    }
    // We didnt found the required value
    else
    {
      // Manual insertion
      tmp = PriorityQueue::GetHead();

      // Look for where to insert
      while (tmp != NULL)
      {
        // When we found the higher value insert before it
        if (tmp->value > value)
        {
          // Insert before
          item->pNext = tmp;

          // Just append
          if (tmp->pPrev != NULL)
          {
            item->pPrev = tmp->pPrev;
            tmp->pPrev->pNext = item;
          }
          else
          {
            root = item;
          }
          tmp->pPrev = item;
          return;
        }
        // Insert as last
        if (tmp->pNext == NULL)
        {
          tmp->pNext = item;
          item->pPrev = tmp;
          return;
        }
        // Go for the next
        tmp = tmp->pNext;
      }
    }
  }

}

bool PriorityQueue::Remove(int value)
{
  PriorityQueue::Element_t *tmp = PriorityQueue::GetHead();

  // Keep looping until you find the one
  while (tmp != NULL)
  {
    // If we found item with higher value than we are looking for
    if (tmp->value > value)
    {
      // The item we are looking for is not there
      return false;
    }
    if (tmp->value == value)
    {
      // Switch pointers
      if (tmp->pPrev != NULL)
      {
        tmp->pPrev->pNext = tmp->pNext;
      }
      else
      {
        root = tmp->pNext;
      }
      if (tmp->pNext != NULL)
      {
        tmp->pNext->pPrev = tmp->pPrev;
      }

      // Free
      free(tmp);
      tmp = NULL;
      return true;
    }
    tmp = tmp->pNext;
  }

  return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
  PriorityQueue::Element_t *tmp = PriorityQueue::GetHead();

  // Keep looping until you find the one
  while (tmp != NULL)
  {
    // If we found item with higher value than we are looking for
    if (tmp->value > value)
    {
      // The item we are looking for is not there
      return NULL;
    }
    if (tmp->value == value)
    {
      return tmp;
    }
    tmp = tmp->pNext;
  }

  return NULL;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
  // Vratime root
  return root;
}

/*** Konec souboru tdd_code.cpp ***/
