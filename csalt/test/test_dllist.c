// ================================================================================
// ================================================================================
// - File:    test_dllist.c
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

#include "test_dllist.h"
// ================================================================================
// ================================================================================

void test_push_front_char_dllist(void **state)
{
	(void) state;
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    print(list);

    free_dllist(list);
}
// ================================================================================
// ================================================================================
// eof
