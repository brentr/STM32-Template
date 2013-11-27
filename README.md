STM32L-Template
===============

A Build template for projects using the stm32l1xx discovery board, 
and its Standard Peripheral Library.

To test:

   (1) install the toolchain and libarary.
   (2) change the TOOLROOT and LIBROOT paths in Makefile.common
   (3) cd to Demo
   (4) type make

To create new projects

   (1) clone the Demo directory (name the directory appropriately)
   (2) change TEMPLATEROOT in the cloned makefile to point to the 
       template directory
   (3) modify as needed, adding any needed library objects to the OBJ 
       list in the Makefile
