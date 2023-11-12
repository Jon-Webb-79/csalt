// ================================================================================
// ================================================================================
// - File:    swap.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 11, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/swap.h"
// ================================================================================
// ================================================================================ 

void swap_uchar(unsigned char *a, unsigned char *b) {
	unsigned char temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_short(short int *a, short int *b) {
	short int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_ushort(unsigned short int *a, unsigned short int *b) {
	unsigned short int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_int(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_uint(unsigned int *a, unsigned int *b) {
	unsigned int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_long(long int *a, long int *b) {
	long int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_ulong(unsigned long int *a, unsigned long int *b) {
	unsigned long int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_llong(long long int *a, long long int *b) {
	long long int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_ullong(unsigned long long int *a, unsigned long long int *b) {
	unsigned long long int temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_float(float *a, float *b) {
	float temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_double(double *a, double *b) {
	double temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_ldouble(long double *a, long double *b) {
	long double temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_char(char *a, char *b) {
	char temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_bool(bool *a, bool *b) {
	bool temp = *a;
	*a = *b;
	*b = temp;
}
// --------------------------------------------------------------------------------

void swap_string(str* a, str* b) {
    str temp = *a;
    *a = *b;
    *b = temp;
}
// ================================================================================
// ================================================================================
// eof
