//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     MATÚŠ ŠKUTA <xskuta04@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author MATÚŠ ŠKUTA
 *
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class EmptyTree : public ::testing::Test
{
  protected:
    BinaryTree binaryTree;
};

TEST_F(EmptyTree,InsertNode)
{
  // Insert 10
  std::pair<bool, BinaryTree::Node_t *> pair = binaryTree.InsertNode(10);
  BinaryTree::Node_t *node = NULL;

  // We inserted 10 should be root
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);

  // The first inserted item must be root item
  EXPECT_EQ(pair.second->key,10);
  EXPECT_EQ(pair.second->color,BinaryTree::BLACK);
  EXPECT_TRUE(pair.second->pParent == NULL);
  EXPECT_TRUE(binaryTree.GetRoot() != NULL);

  node = pair.second->pLeft;

  // Left node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,10);

  node = pair.second->pRight;

  // Right node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,10);

  // Insert node 5
  pair = binaryTree.InsertNode(5);

  // Check insert
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,5);
  // Check if it is red and its parent is 10
  EXPECT_EQ(pair.second->color,BinaryTree::RED);
  EXPECT_TRUE(pair.second->pParent != NULL);
  EXPECT_EQ(pair.second->pParent->key,10);

  node = pair.second->pLeft;

  // Left node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,5);

  node = pair.second->pRight;

  // Right node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,5);

  // Insert node 15
  pair = binaryTree.InsertNode(15);

  // Check insert
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,15);
  // Check if it is red and its parent is 10
  EXPECT_EQ(pair.second->color,BinaryTree::RED);
  EXPECT_TRUE(pair.second->pParent != NULL);
  EXPECT_EQ(pair.second->pParent->key,10);

  node = pair.second->pLeft;

  // Left node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,15);

  node = pair.second->pRight;

  // Right node
  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(node->color,BinaryTree::BLACK);
  // Leaf node
  EXPECT_TRUE(node->pLeft == NULL);
  EXPECT_TRUE(node->pRight == NULL);
  // Parent
  EXPECT_TRUE(node->pParent != NULL);
  EXPECT_EQ(node->pParent->key,15);

  // Try to insert already inserted value
  pair = binaryTree.InsertNode(10);

  EXPECT_FALSE(pair.first);
  EXPECT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,10);

  // Try inserting negative number
  pair = binaryTree.InsertNode(-5);

  // Check insert
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,-5);
  // Check if it is red and its parent is 10
  EXPECT_EQ(pair.second->color,BinaryTree::RED);
  EXPECT_TRUE(pair.second->pParent != NULL);
  EXPECT_EQ(pair.second->pParent->key,5);

  // the parent node should change to black
  EXPECT_EQ(pair.second->pParent->color,BinaryTree::BLACK);
}

TEST_F(EmptyTree,DeleteNode)
{
  EXPECT_FALSE(binaryTree.DeleteNode(0));
  EXPECT_FALSE(binaryTree.DeleteNode(1000));
  EXPECT_FALSE(binaryTree.DeleteNode(-1000));
  EXPECT_FALSE(binaryTree.DeleteNode(25));
  EXPECT_FALSE(binaryTree.DeleteNode(-25));
  EXPECT_FALSE(binaryTree.DeleteNode(1000000000));
  EXPECT_FALSE(binaryTree.DeleteNode(-1000000000));
  EXPECT_FALSE(binaryTree.DeleteNode(123456789));
}

TEST_F(EmptyTree,FindNode)
{
  EXPECT_TRUE(binaryTree.FindNode(0) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(1000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-1000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(25) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-25) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(1000000000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-1000000000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(123456789) == NULL);
}

class NonEmptyTree : public ::testing::Test
{
  protected:
    virtual void SetUp()
    {
      int values[] = { 10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55 };

      for(int i = 0; i < 14; ++i)
        binaryTree.InsertNode(values[i]);
    }

    BinaryTree binaryTree;
};

TEST_F(NonEmptyTree,InsertNode)
{
  // Try inserting alredy inserted value 30
  std::pair<bool, BinaryTree::Node_t *> pair = binaryTree.InsertNode(30);

  EXPECT_FALSE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,30);
  EXPECT_EQ(pair.second->color,BinaryTree::BLACK);
  EXPECT_TRUE(pair.second->pLeft != NULL);
  EXPECT_TRUE(pair.second->pRight != NULL);
  EXPECT_TRUE(pair.second->pParent == NULL);

  // 30 should be root
  EXPECT_EQ(binaryTree.GetRoot()->key,30);

  // Insert in the most rightest space
  pair = binaryTree.InsertNode(99);
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,99);
  EXPECT_EQ(pair.second->color,BinaryTree::RED);
  EXPECT_TRUE(pair.second->pLeft != NULL);
  EXPECT_TRUE(pair.second->pRight != NULL);
  EXPECT_TRUE(pair.second->pParent != NULL);
  EXPECT_EQ(pair.second->pParent->key,90);

  // Insert in negative number
  pair = binaryTree.InsertNode(-5);
  EXPECT_TRUE(pair.first);
  ASSERT_TRUE(pair.second != NULL);
  EXPECT_EQ(pair.second->key,-5);
  EXPECT_EQ(pair.second->color,BinaryTree::RED);
  EXPECT_TRUE(pair.second->pLeft != NULL);
  EXPECT_TRUE(pair.second->pRight != NULL);
  EXPECT_TRUE(pair.second->pParent != NULL);
  EXPECT_EQ(pair.second->pParent->key,5);
}

TEST_F(NonEmptyTree,DeleteNode)
{
  EXPECT_FALSE(binaryTree.DeleteNode(0));
  EXPECT_FALSE(binaryTree.DeleteNode(1000));
  EXPECT_FALSE(binaryTree.DeleteNode(-1000));
  EXPECT_FALSE(binaryTree.DeleteNode(25));
  EXPECT_FALSE(binaryTree.DeleteNode(-25));
  EXPECT_FALSE(binaryTree.DeleteNode(1000000000));
  EXPECT_FALSE(binaryTree.DeleteNode(-1000000000));
  EXPECT_FALSE(binaryTree.DeleteNode(123456789));

  EXPECT_TRUE(binaryTree.DeleteNode(30));
  EXPECT_TRUE(binaryTree.DeleteNode(5));
  EXPECT_TRUE(binaryTree.DeleteNode(10));
  EXPECT_TRUE(binaryTree.DeleteNode(85));
  EXPECT_TRUE(binaryTree.DeleteNode(70));
  EXPECT_TRUE(binaryTree.DeleteNode(55));
  EXPECT_TRUE(binaryTree.DeleteNode(50));

  EXPECT_FALSE(binaryTree.DeleteNode(30));
  EXPECT_FALSE(binaryTree.DeleteNode(5));

}

TEST_F(NonEmptyTree,FindNode)
{

  EXPECT_TRUE(binaryTree.FindNode(0) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(1000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-1000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(25) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-25) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(1000000000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(-1000000000) == NULL);
  EXPECT_TRUE(binaryTree.FindNode(123456789) == NULL);

  BinaryTree::Node_t *node = binaryTree.FindNode(30);

  EXPECT_TRUE(node != NULL);
  EXPECT_EQ(node->key,30);

  // Lets try to find again 30
  node = binaryTree.FindNode(30);

  EXPECT_TRUE(node != NULL);
  EXPECT_EQ(node->key,30);

  // Try to find highest number
  node = binaryTree.FindNode(90);

  EXPECT_TRUE(node != NULL);
  EXPECT_EQ(node->key,90);

  // Try to find lowest number
  node = binaryTree.FindNode(5);

  EXPECT_TRUE(node != NULL);
  EXPECT_EQ(node->key,5);
}

class TreeAxioms : public ::testing::Test
{
  protected:
    virtual void SetUp()
    {
      int values[] = { 10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55 };

      for(int i = 0; i < 14; ++i)
        binaryTree.InsertNode(values[i]);
    }

    BinaryTree binaryTree;
};

// All list nodes should be black
TEST_F(TreeAxioms,Axiom1)
{
  std::vector<BinaryTree::Node_t *> leafNodes;
  binaryTree.GetLeafNodes(leafNodes);

  for (BinaryTree::Node_t *node : leafNodes)
  {
    EXPECT_EQ(node->color, BinaryTree::BLACK);
  }
}

// Every red node should have 2 black nodes under him
TEST_F(TreeAxioms,Axiom2)
{
  std::vector<BinaryTree::Node_t *> leafNodes;
  binaryTree.GetAllNodes(leafNodes);

  for (BinaryTree::Node_t *node : leafNodes)
  {
    // If the color is RED check for 2 BLACK children
    if (node->color == BinaryTree::RED)
    {
      // Both nodes under RED node should not be empty
      ASSERT_TRUE(node->pLeft != NULL);
      ASSERT_TRUE(node->pRight != NULL);

      // And both needs to be black
      EXPECT_EQ(node->pLeft->color, BinaryTree::BLACK);
      EXPECT_EQ(node->pRight->color, BinaryTree::BLACK);
    }
  }
}

// Each way should
TEST_F(TreeAxioms,Axiom3)
{
  std::vector<BinaryTree::Node_t *> leafNodes;
  BinaryTree::Node_t *tmp = NULL;
  int blackNodes = 0;
  int prevBlackNodes = -1;

  binaryTree.GetLeafNodes(leafNodes);


  // Loop through every node
  for (BinaryTree::Node_t *node : leafNodes)
  {
    tmp = node;
    blackNodes = 0;

    // For every node count BLACK parents
    while (tmp != NULL)
    {
      if (tmp->color == BinaryTree::BLACK)
      {
        blackNodes++;
      }
      tmp = tmp->pParent;
    }
    if (prevBlackNodes != -1)
    {
      EXPECT_EQ(prevBlackNodes,blackNodes);
    }

    prevBlackNodes = blackNodes;
  }
}

/*** Konec souboru black_box_tests.cpp ***/
