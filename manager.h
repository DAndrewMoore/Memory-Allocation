/* 
 * File:   manager.h
 * Author: Andrew
 *
 * Created on May 2, 2016, 11:54 AM
 */

#ifndef MANAGER_H
#define	MANAGER_H

int* memoryScope(int);
int checkMemory(int*, int, int);
int my_malloc(int*, int, int);
void my_malloc(int*, int, int, int);
void my_free(int*, int, int);

#endif	/* MANAGER_H */

