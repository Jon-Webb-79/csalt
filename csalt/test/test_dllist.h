// ================================================================================
// ================================================================================
// - File:    test_dllist.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_dlist_H
#define test_dlist_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/dlist.h"
#include "../include/print.h"
// ================================================================================
// ================================================================================

void test_push_front_char_dllist(void **state);

#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
