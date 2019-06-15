# CMake generated Testfile for 
# Source directory: /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment
# Build directory: /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(EmptyTree.InsertNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=EmptyTree.InsertNode")
add_test(EmptyTree.DeleteNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=EmptyTree.DeleteNode")
add_test(EmptyTree.FindNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=EmptyTree.FindNode")
add_test(NonEmptyTree.InsertNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=NonEmptyTree.InsertNode")
add_test(NonEmptyTree.DeleteNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=NonEmptyTree.DeleteNode")
add_test(NonEmptyTree.FindNode "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=NonEmptyTree.FindNode")
add_test(TreeAxioms.Axiom1 "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=TreeAxioms.Axiom1")
add_test(TreeAxioms.Axiom2 "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=TreeAxioms.Axiom2")
add_test(TreeAxioms.Axiom3 "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/black_box_test" "--gtest_filter=TreeAxioms.Axiom3")
add_test(NonEmptyQueue.Insert "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=NonEmptyQueue.Insert")
add_test(NonEmptyQueue.RemoveAllForward "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=NonEmptyQueue.RemoveAllForward")
add_test(NonEmptyQueue.RemoveAllBackward "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=NonEmptyQueue.RemoveAllBackward")
add_test(NonEmptyQueue.Find "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=NonEmptyQueue.Find")
add_test(EmptyQueue.Insert "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=EmptyQueue.Insert")
add_test(EmptyQueue.Remove "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=EmptyQueue.Remove")
add_test(EmptyQueue.Find "/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/tdd_test" "--gtest_filter=EmptyQueue.Find")
subdirs("googletest-build")
