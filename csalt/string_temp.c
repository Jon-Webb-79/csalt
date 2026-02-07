// ================================================================================
// ================================================================================
// - File:    c_string.c
// - purpose: this file contains the implementation for functions used in the 
//            c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_string.h"

#include <errno.h>  // For errno and strerror 
#include <stdlib.h> // For size_t, malloc, and realloc
#include <string.h> // For strerror
#include <limits.h> // For INT_MIN
#include <ctype.h>  // For isspace
#include <stdint.h> // For SIZE_MAX

#if defined(__AVX512BW__)
  #include <immintrin.h>
  #include "simd_avx512_char.inl"
#elif defined(__AVX2__)
  #include <immintrin.h>
  #include "simd_avx2_char.inl"
#elif defined(__AVX__)
  #include <immintrin.h>
  #include "simd_avx_char.inl"
#elif defined(__SSE4_1__)
  #include <smmintrin.h>
  #include "simd_sse41_char.inl"
#elif defined(__SSE3__)
  #include <pmmintrin.h>
  #include "simd_sse3_char.inl"
#elif defined(__SSE2__)
  #include <emmintrin.h>
  #include "simd_sse2_char.inl"
#elif defined(__ARM_FEATURE_SVE2)
  #include <arm_sve.h>
  #include "simd_sve2_char.inl"
#elif defined(__ARM_FEATURE_SVE)
  #include <arm_sve.h>
  #include "simd_sve_char.inl"
#elif defined(__aarch64__) && defined(__ARM_NEON)
  #include <arm_neon.h>
  #include "simd_neon_char.inl"
#endif

// ================================================================================ 
// ================================================================================ 

static inline size_t simd_last_u8_index(const unsigned char* s, size_t n, unsigned char c) {
    #if defined(__AVX512BW__)
        return simd_last_index_u8_avx512bw(s, n, c);
    #elif defined(__AVX2__)
        return simd_last_index_u8_avx2(s, n, c);
    #elif defined(__AVX__)
        return simd_last_index_u8_avx_fallback_sse2(s, n, c);
    #elif defined(__SSE4_1__)
        return simd_last_index_u8_sse41(s, n, c);
    #elif defined(__SSE3__)
        return simd_last_index_u8_sse3(s, n, c);
    #elif defined(__SSE2__)
        return simd_last_index_u8_sse2(s, n, c);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_last_index_u8_sve2(s, n, c);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_last_index_u8_sve(s, n, c);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_last_index_u8_neon(s, n, c);
    #else
        return simd_last_index_u8_scalar(s, n, c);
    #endif
}
static inline size_t simd_first_substr_index(const unsigned char* s, size_t n,
                                             const unsigned char* pat, size_t m) {
#if defined(__AVX512BW__)
    return simd_first_substr_index_avx512bw(s, n, pat, m);
#elif defined(__AVX2__)
    return simd_first_substr_index_avx2(s, n, pat, m);
#elif defined(__AVX__)
    return simd_first_substr_index_avx(s, n, pat, m);
#elif defined(__SSE4_1__)
    return simd_first_substr_index_sse41(s, n, pat, m);
#elif defined(__SSE3__)
    return simd_first_substr_index_sse3(s, n, pat, m);
#elif defined(__SSE2__)
    return simd_first_substr_index_sse2(s, n, pat, m);
#elif defined(__ARM_FEATURE_SVE2)
    return simd_first_substr_index_sve2(s, n, pat, m);
#elif defined(__ARM_FEATURE_SVE)
    return simd_first_substr_index_sve(s, n, pat, m);
#elif defined(__aarch64__) && defined(__ARM_NEON)
    return simd_first_substr_index_neon(s, n, pat, m);
#else
    /* scalar fallback */
    if (m == 0) return 0;
    if (m == 1) {
        const void* p = memchr(s, pat[0], n);
        return p ? (size_t)((const unsigned char*)p - s) : SIZE_MAX;
    }
    for (size_t i = 0; i + m <= n; ++i)
        if (s[i] == pat[0] && memcmp(s + i, pat, m) == 0) return i;
    return SIZE_MAX;
#endif
}
static inline size_t simd_last_substr_index(const unsigned char* s, size_t n,
                                            const unsigned char* pat, size_t m) {
#if defined(__AVX512BW__)
    return simd_last_substr_index_avx512bw(s, n, pat, m);
#elif defined(__AVX2__)
    return simd_last_substr_index_avx2(s, n, pat, m);
#elif defined(__AVX__)
    return simd_last_substr_index_avx(s, n, pat, m);
#elif defined(__SSE4_1__)
    return simd_last_substr_index_sse41(s, n, pat, m);
#elif defined(__SSE3__)
    return simd_last_substr_index_sse3(s, n, pat, m);
#elif defined(__SSE2__)
    return simd_last_substr_index_sse2(s, n, pat, m);
#elif defined(__ARM_FEATURE_SVE2)
    return simd_last_substr_index_sve2(s, n, pat, m);
#elif defined(__ARM_FEATURE_SVE)
    return simd_last_substr_index_sve(s, n, pat, m);
#elif defined(__aarch64__) && defined(__ARM_NEON)
    return simd_last_substr_index_neon(s, n, pat, m);
#else
    /* scalar fallback: search in reverse */
    if (m == 0) return n;                       /* empty needle matches at end */
    if (m == 1) {                               /* last byte */
        for (size_t i = n; i-- > 0; )
            if (s[i] == pat[0]) return i;
        return SIZE_MAX;
    }
    if (n < m) return SIZE_MAX;

    /* check windows [i .. i+m) from right to left */
    for (size_t i = n - m + 1; i-- > 0; ) {
        if (s[i] == pat[0] && memcmp(s + i, pat, m) == 0) return i;
    }
    return SIZE_MAX;
#endif
}
static inline size_t token_count_simd(const char* s, size_t n,
                                      const char* delim, size_t dlen) {
#if defined(__AVX512BW__)
    return simd_token_count_avx512bw(s, n, delim, dlen);
#elif defined(__AVX2__)
    return simd_token_count_avx2(s, n, delim, dlen);
#elif defined(__AVX__)
    return simd_token_count_avx(s, n, delim, dlen);
#elif defined(__SSE4_1__)
    return simd_token_count_sse41(s, n, delim, dlen);
#elif defined(__SSE3__)
    return simd_token_count_sse3(s, n, delim, dlen);
#elif defined(__SSE2__)
    return simd_token_count_sse2(s, n, delim, dlen);
#elif defined(__ARM_FEATURE_SVE2)
    return simd_token_count_sve2(s, n, delim, dlen);
#elif defined(__ARM_FEATURE_SVE)
    return simd_token_count_sve(s, n, delim, dlen);
#elif defined(__aarch64__) && defined(__ARM_NEON)
    return simd_token_count_neon(s, n, delim, dlen);
#else
    /* Scalar fallback copied from earlier */
    uint8_t lut[256]; memset(lut, 0, sizeof(lut));
    for (const unsigned char* p = (const unsigned char*)delim; *p; ++p) lut[*p] = 1;

    size_t count = 0;
    bool in_token = false;
    for (size_t i = 0; i < n; ++i) {
        const bool is_delim = lut[(unsigned char)s[i]] != 0;
        if (!is_delim) { if (!in_token) { ++count; in_token = true; } }
        else in_token = false;
    }
    return count;
#endif
}

// ================================================================================ 
// ================================================================================

static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================ 
// ================================================================================ 
// STRING_T DATA TYPE 

struct string_t {
    char* str;
    size_t len;
    size_t alloc;
    ErrorCode error;
};
// ================================================================================ 
// ================================================================================ 
// PRIVATE FUNCTIONS

static inline char*
_last_string_between_ptrs_simd(const string_t* hay,
                               const string_t* needle,
                               const char* lo,
                               const char* hi_inclusive) {
    /* Fast preconditions expected to be enforced by the caller: */
    /* - hay && hay->str, needle && needle->str
       - lo, hi_inclusive inside payload (NUL excluded), and hi_inclusive >= lo */

    const size_t n = hay->len;
    const size_t m = needle->len;

    /* Empty needle: caller decides policy; we return no match. */
    if (m == 0) return NULL;

    /* Map window to [off_lo, end_excl) in 0..n */
    size_t off_lo   = (size_t)(lo - hay->str);
    size_t end_excl = (size_t)(hi_inclusive - hay->str) + 1;   /* inclusive → exclusive */

    /* Clamp right edge to payload end (defensive) */
    if (end_excl > n) end_excl = n;
    if (off_lo >= end_excl) return NULL;

    const size_t span = end_excl - off_lo;
    if (m > span) return NULL;

    /* SIMD-dispatched last-substring search on the slice */
    const size_t idx = simd_last_substr_index(
        (const unsigned char*)(hay->str + off_lo), span,
        (const unsigned char*)needle->str,        m
    );

    return (idx == SIZE_MAX) ? NULL : (char*)(hay->str + off_lo + idx);
}
// ================================================================================ 
// ================================================================================ 

string_t* init_string(const char* str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }

    // Typically SIMD-optimized in libc
    size_t len = strlen(str);

    // Guard against pathological overflow on len+1
    if (len == SIZE_MAX) { errno = EOVERFLOW; return NULL; }

    string_t* ptr = malloc(sizeof(string_t));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }

    char* ptr2 = malloc(len + 1);
    if (!ptr2) {
        errno = ENOMEM;
        free(ptr);
        return NULL;
    }

    memcpy(ptr2, str, len);
    ptr2[len] = '\0';
    
    ptr->str = ptr2;
    ptr->len = len;
    ptr->alloc = len + 1;
    ptr->error = NO_ERROR;
    return ptr;
}
// -------------------------------------------------------------------------------- 

ErrorCode get_string_error(const string_t* str) {
    if (!str) {
        errno = EINVAL;
        return INVALID_ERROR;
    }
    return str->error;
}
// -------------------------------------------------------------------------------- 

bool is_string_valid(const string_t* str) {
    if (str && str->str) return true;
    errno = EINVAL;
    return false;
}
// --------------------------------------------------------------------------------

void free_string(string_t* str) {
    if (!str) {
        errno = EINVAL;
        return;
    }
    if (str->str) {
        free(str->str);
    }
    free(str);
}
// --------------------------------------------------------------------------------

void _free_string(string_t** str) {
    if (str && *str) {
        free_string(*str);
        *str = NULL;
    }
}
// --------------------------------------------------------------------------------

const char* get_string(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    return str->str;
}
// --------------------------------------------------------------------------------

size_t string_size(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return str->len;
}
// --------------------------------------------------------------------------------

size_t string_alloc(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return str->alloc;
}
// --------------------------------------------------------------------------------

bool string_string_concat(string_t* str1, const string_t* str2) {
    if (!str1 || !str2) {
        if (str1) { 
            str1->error = INVALID_ARG;
            errno = set_errno_from_error(str1->error);
        }
        else errno = EINVAL;
        return false;
    }
    // Reset error code 
    str1->error = NO_ERROR;
    if (!str1->str) {
        str1->error = NULL_POINTER;
        errno = set_errno_from_error(str1->error);
        return false;
    }
    if (!str2->str) {
        str1->error = NULL_POINTER;
        errno = set_errno_from_error(str1->error);
        /* str2 is const; don't mutate it */
        return false;
    }

    const size_t len1 = str1->len;
    const size_t len2 = str2->len;

    /* size_t overflow guard for len1 + len2 + 1 */
    if (len2 > SIZE_MAX - 1 - len1) {
        str1->error = NUMERIC_OVERFLOW;
        errno = set_errno_from_error(str1->error);
        return false;
    }
    const size_t needed = len1 + len2 + 1;

    /* Detect if src lies inside str1->str (overlap before a potential realloc) */
    const char* src_initial = str2->str;
    const char* s1_begin    = str1->str;
    const char* s1_end_used = str1->str + len1 + 1; /* include NUL */

    const bool src_overlaps_dest = (src_initial >= s1_begin && src_initial < s1_end_used);
    char* temp = NULL;

    /* If we must grow AND the source overlaps str1's buffer, stash a temp copy first */
    if (needed > str1->alloc && src_overlaps_dest) {
        temp = (char*)malloc(len2);
        if (!temp) {
            str1->error = BAD_ALLOC;
            errno = set_errno_from_error(str1->error);
            return false;
        }
        memcpy(temp, src_initial, len2);
        src_initial = temp; /* use temp as the copy source after realloc */
    }

    /* Ensure capacity */
    if (needed > str1->alloc) {
        char* newbuf = (char*)realloc(str1->str, needed);
        if (!newbuf) {
            free(temp);
            str1->error = REALLOC_FAIL;
            errno = set_errno_from_error(str1->error);
            return false;
        }
        str1->str   = newbuf;
        str1->alloc = needed;
    }

    /* If str1 == str2, the source is the start of the (possibly moved) buffer */
    if (str1 == str2) {
        src_initial = str1->str;
    }

    /* Append safely; memmove handles any overlap */
    memmove(str1->str + len1, src_initial, len2);
    str1->str[len1 + len2] = '\0';

    str1->len   = len1 + len2;
    str1->error = NO_ERROR;

    free(temp);
    return true;
}
// --------------------------------------------------------------------------------

bool string_lit_concat(string_t* str1, const char* literal) {
    if (!str1 || !literal) {
        if (str1) {
            str1->error = INVALID_ARG;
            errno = set_errno_from_error(str1->error);
        }
        else errno = EINVAL;
        return false;
    }
    // Reset error code 
    str1->error = NO_ERROR;
    if (!str1->str) {
        str1->error = NULL_POINTER;
        errno = set_errno_from_error(str1->error);
        return false;
    }

    const size_t len1 = str1->len;
    const size_t len2 = strlen(literal);

    // Fast path: nothing to append
    if (len2 == 0) {
        str1->error = NO_ERROR;
        return true;
    }

    // Guard against size_t overflow in len1 + len2 + 1
    if (len2 > SIZE_MAX - 1 - len1) {
        str1->error = NUMERIC_OVERFLOW;
        errno = set_errno_from_error(str1->error);
        return false;
    }
    const size_t needed = len1 + len2 + 1;

    // Does the source pointer lie inside str1->str's *current used* region?
    const char* s1_begin    = str1->str;
    const char* s1_end_used = str1->str + len1 + 1;  // include current NUL
    const bool   overlaps   = (literal >= s1_begin && literal < s1_end_used);

    // If we must grow and the source aliases str1->str, stash a temp copy first
    const char* src = literal;
    char* temp = NULL;
    if (needed > str1->alloc && overlaps) {
        temp = (char*)malloc(len2);
        if (!temp) {
            str1->error = BAD_ALLOC;
            errno = set_errno_from_error(str1->error);
            return false;
        }
        memcpy(temp, literal, len2);
        src = temp;  // safe to use after realloc
    }

    // Ensure capacity
    if (needed > str1->alloc) {
        char* newbuf = (char*)realloc(str1->str, needed);
        if (!newbuf) {
            free(temp);
            str1->error = REALLOC_FAIL;
            errno = set_errno_from_error(str1->error);
            return false;
        }
        str1->str   = newbuf;
        str1->alloc = needed;
    }

    // Append safely; memmove handles any overlap
    memmove(str1->str + len1, src, len2);
    str1->str[len1 + len2] = '\0';

    str1->len   = len1 + len2;
    str1->error = NO_ERROR;

    free(temp);
    return true;
}
// --------------------------------------------------------------------------------

int compare_strings_lit(const string_t* str_struct, const char* string) {
    if (!str_struct || !string || !str_struct->str) {
        errno = EINVAL;
        return INT_MIN; // Or another designated error value
    }

    size_t string_len = strlen(string);
    size_t min_len = (str_struct->len < string_len) ? str_struct->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct->str[i] != string[i]) {
            return (unsigned char)str_struct->str[i] - (unsigned char)string[i];
        }
    }
    return str_struct->len - string_len;
}
// --------------------------------------------------------------------------------

int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->str || !str_struct_two->str) {
        errno = EINVAL;
        return INT_MIN; // Or another designated error value
    } 

    size_t string_len = str_struct_two->len;
    size_t min_len = (str_struct_one->len < string_len) ? str_struct_one->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct_one->str[i] != str_struct_two->str[i]) {
            return (unsigned char)str_struct_one->str[i] - (unsigned char)str_struct_two->str[i];
        }
    }
    return str_struct_one->len - string_len;
}
// --------------------------------------------------------------------------------

string_t* copy_string(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    string_t* new_str = init_string(get_string(str));
    if (new_str->alloc < str->alloc) 
        reserve_string(new_str, str->alloc);
    return new_str; 
}
// --------------------------------------------------------------------------------

bool reserve_string(string_t* str, size_t len) {
    if (!str) {
        errno = EINVAL;
        return false;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return false;
    }
    // Ensure the requested length is greater than the current allocation
    if (len <= str->alloc) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return false;
    }

    // Attempt to reallocate memory
    char* ptr = realloc(str->str, sizeof(char) * len);
    if (!ptr) {
        str->error = REALLOC_FAIL;
        errno = set_errno_from_error(str->error);
        return false;
    }

    // Update the string_t structure with the new allocation
    str->str = ptr;
    str->alloc = len;

    return true;
}
// --------------------------------------------------------------------------------

bool trim_string(string_t* str) {
    if (!str) {
        errno = EINVAL;
        return false;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return false;
    }
    // RESET ERROR VALUE 
    str->error = NO_ERROR;
    // If already at minimum size, nothing to do
    if (str->len + 1 == str->alloc) {
        return true;
    }
    
    // Sanity check for corrupted string_t
    if (str->len + 1 > str->alloc) {
        str->error = SIZE_MISMATCH;
        errno = set_errno_from_error(str->error);
        return false;
    }

    char *ptr = realloc(str->str, str->len + 1);
    if (ptr == NULL) {
        str->error = REALLOC_FAIL;
        errno = set_errno_from_error(str->error);
        return false;
    }
    
    str->str = ptr;
    str->alloc = str->len + 1;
    return true;
}
// -------------------------------------------------------------------------------- 

char* first_char_occurrance(string_t* str, char value) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return NULL;
    }
    void* p = memchr(str->str, (unsigned char)value, str->len);
    if (!p) {
        str->error = NOT_FOUND;
        errno = set_errno_from_error(str->error);
        return NULL;
    }
    str->error = NO_ERROR;
    return (char*)p; 
}
// -------------------------------------------------------------------------------- 

char* last_char_occurrance(string_t* str, char value) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return NULL;
    }
    if (str->len == 0) {               /* nothing to search */
        str->error = NOT_FOUND;
        errno = set_errno_from_error(str->error);
        return NULL;
    }

    const unsigned char* s = (const unsigned char*)str->str;
    const size_t n        = str->len;
    const unsigned char c = (unsigned char)value;

    const size_t idx = simd_last_u8_index(s, n, c);  /* returns SIZE_MAX if absent */

    if (idx == SIZE_MAX) {
        str->error = LENGTH_OVERFLOW;
        errno = set_errno_from_error(str->error);
        return NULL;
    }

    str->error = NO_ERROR;
    return (char*)(str->str + idx);
}
// --------------------------------------------------------------------------------

char* first_lit_substr_occurrence(string_t* str, const char* sub_str) {
    if (!str || !sub_str) {
        if (str) {
            str->error = INVALID_ARG;
            errno = set_errno_from_error(str->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return NULL;
    }

    const size_t n = str->len;
    const size_t m = strlen(sub_str);

    /* Policy: empty needle matches at start (like strstr). */
    if (m == 0) {
        str->error = NOT_FOUND;
        return NULL;
    }
    if (m > n) {
        str->error = NOT_FOUND; 
        errno = set_errno_from_error(str->error);
        return NULL;
    }

    /* SIMD-accelerated index lookup (dispatcher you wired up per ISA) */
    size_t idx = simd_first_substr_index(
        (const unsigned char*)str->str, n,
        (const unsigned char*)sub_str,  m
    );
    if (idx == SIZE_MAX) {
        str->error = LENGTH_OVERFLOW;
        errno = set_errno_from_error(str->error);
        return NULL;
    }
    str->error = NO_ERROR;       /* valid search completed */
    return (char*)(str->str + idx);
}
// -------------------------------------------------------------------------------- 

char* first_string_substr_occurrence(string_t* hay, const string_t* needle) {
    if (!hay || !needle) {
        if (hay) {
            hay->error = INVALID_ARG;
            errno = set_errno_from_error(hay->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    if (!hay->str) {
        hay->error = NULL_POINTER;
        errno = set_errno_from_error(hay->error);
        return NULL;
    }
    if (!needle->str) {
        hay->error = NULL_POINTER;
        errno = set_errno_from_error(hay->error);
        return NULL;
    }

    const size_t n = hay->len;
    const size_t m = needle->len;

    /* Policy: empty needle matches at start (like strstr) */
    if (m == 0) { 
        hay->error = NOT_FOUND;
        errno = set_errno_from_error(hay->error);
        return NULL; 
    }
    if (m > n)   { 
        hay->error = NOT_FOUND; 
        errno = set_errno_from_error(hay->error);
        return NULL; 
    }

    /* SIMD-accelerated index lookup (you already have this dispatcher) */
    size_t idx = simd_first_substr_index(
        (const unsigned char*)hay->str, n,
        (const unsigned char*)needle->str, m
    );
    if (idx == SIZE_MAX) {
        hay->error = LENGTH_OVERFLOW;
        errno = set_errno_from_error(hay->error);
        return NULL;
    }
    hay->error = NO_ERROR;
    return (char*)(hay->str + idx);
}
// --------------------------------------------------------------------------------

char* last_lit_substr_occurrence(string_t* str, const char* sub_str) {
    if (!str || !sub_str) {
        if (str) {
            str->error = INVALID_ARG;
            errno = set_errno_from_error(str->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    if (!str->str) {
        str->error = NULL_POINTER;
        errno = set_errno_from_error(str->error);
        return NULL;
    }

    const size_t n = str->len;
    const size_t m = strlen(sub_str);

    /* Policy: empty needle matches at end (offset n). */
    if (m == 0) { str->error = NO_ERROR; return str->str + n; }
    if (m > n)  { str->error = NO_ERROR; return NULL; }

    size_t idx = simd_last_substr_index(
        (const unsigned char*)str->str, n,
        (const unsigned char*)sub_str,  m
    );
    if (idx == SIZE_MAX) {
        str->error = LENGTH_OVERFLOW;
        errno = set_errno_from_error(str->error);
        return NULL;
    }
    str->error = NO_ERROR;                 /* valid search completed */
    return (char*)(str->str + idx);
}
// -------------------------------------------------------------------------------- 

char* last_string_substr_occurrence(string_t* hay, const string_t* needle) {
    if (!hay || !needle) {
        if (hay) {
            hay->error = INVALID_ARG;
            errno = set_errno_from_error(hay->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    if (!hay->str) { 
        hay->error = NULL_POINTER; 
        errno = set_errno_from_error(hay->error); 
        return NULL; 
    }
    if (!needle->str) { errno = EINVAL; return NULL; }

    const size_t n = hay->len;
    const size_t m = needle->len;

    if (m == 0) { hay->error = NO_ERROR; return hay->str + n; }
    if (m > n)  { hay->error = NO_ERROR; return NULL; }

    size_t idx = simd_last_substr_index(
        (const unsigned char*)hay->str, n,
        (const unsigned char*)needle->str, m
    );
    if (idx == SIZE_MAX) {
        hay->error = LENGTH_OVERFLOW;
        errno = set_errno_from_error(hay->error);
        return NULL;
    }
    hay->error = NO_ERROR;
    return (char*)(hay->str + idx);
}

// --------------------------------------------------------------------------------

char* first_char(string_t* str) {
    if (!str || !str->str) {
        if (str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    str->error= NO_ERROR;
    return str->str;
}
// --------------------------------------------------------------------------------

char* last_char(string_t* str) {
    if (!str || !str->str) {
        if (str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        }
        else errno = EINVAL;
        return NULL;
    }
    str->error = NO_ERROR;
    return str->str + str->len - 1;
}
// --------------------------------------------------------------------------------

bool is_string_ptr(string_t* str, const char* ptr, bool include_terminator) {
    if (!str || !ptr) {
        if (str) {
            str->error = INVALID_ARG;  // argument itself is invalid
            errno = set_errno_from_error(str->error);
        }
        else errno = EINVAL;
        return false;
    }
    if (!str->str) {
        str->error = NULL_POINTER;          // object lacks a buffer
        errno = set_errno_from_error(str->error);
        return false;
    }

    const uintptr_t base = (uintptr_t)(const void*)str->str;
    const uintptr_t end  = base + str->len + (include_terminator ? 1u : 0u);
    const uintptr_t p    = (uintptr_t)(const void*)ptr;

    str->error = NO_ERROR;
    return (p >= base) && (p < end);
}
// -------------------------------------------------------------------------------- 

static char* _last_literal_between_ptrs_simd(
    const string_t* s, const char* needle, const char* min_ptr, const char* max_ptr)
{
    const unsigned char* base = (const unsigned char*)s->str;
    const size_t n = s->len;
    const size_t m = strlen(needle);

    size_t off_lo    = (size_t)(min_ptr - (const char*)base);
    size_t end_excl  = (size_t)(max_ptr  - (const char*)base) + 1;  /* inclusive→exclusive */
    if (end_excl > n) end_excl = n;

    if (m == 0 || off_lo >= end_excl) return NULL;
    size_t span = end_excl - off_lo;
    if (m > span) return NULL;

    size_t idx = simd_last_substr_index(base + off_lo, span,
                                        (const unsigned char*)needle, m);
    return (idx == SIZE_MAX) ? NULL : (char*)(s->str + off_lo + idx);
}
// -------------------------------------------------------------------------------- 

bool drop_lit_substr(string_t* s, const char* needle, char* min_ptr, char* max_ptr) {
    if (!s || !needle) { 
        if (s) { 
            s->error = INVALID_ARG; 
            errno = set_errno_from_error(s->error); 
        }
        else errno = EINVAL;
        return false; 
    }
    if (!s->str) { 
        s->error = NULL_POINTER; 
        errno = set_errno_from_error(s->error); 
        return false; 
    }
    if (!is_string_ptr(s, min_ptr, false) || !is_string_ptr(s, max_ptr, false)) {
        s->error = OUT_OF_BOUNDS; 
        errno = set_errno_from_error(s->error); 
        return false;
    }
    if (max_ptr < min_ptr) { 
        s->error = INVALID_ARG; 
        errno = set_errno_from_error(s->error); 
        return false; 
    }

    const size_t m = strlen(needle);
    if (m == 0) { s->error = NO_ERROR; return true; }

    for (;;) {
        /* window remaining must fit the needle */
        size_t span = (size_t)(max_ptr - min_ptr + 1);
        if (span < m) break;

        char* hit = _last_literal_between_ptrs_simd(s, needle, min_ptr, max_ptr);
        if (!hit) break;

        /* Optionally consume a trailing space if still in payload */
        size_t hit_off = (size_t)(hit - s->str);
        size_t drop_len = m;
        if (hit_off + m < s->len && hit[m] == ' ') ++drop_len;

        /* Move the entire suffix INCLUDING the '\0' left by drop_len. */
        size_t bytes_from_src = (s->len + 1) - (hit_off + drop_len); /* +1 for NUL */
        memmove(hit, hit + drop_len, bytes_from_src);

        s->len -= drop_len;            /* logical size shrinks; NUL already moved */
        /* Keep the window aligned with the new content: slide right edge left. */
        if (max_ptr >= hit) {
            ptrdiff_t d = (ptrdiff_t)drop_len;
            if (s->len == 0) {
                max_ptr = s->str;      /* harmless placeholder; loop will end */
            } else {
                max_ptr -= d;
                if (max_ptr >= s->str + s->len) max_ptr = s->str + s->len - 1;
            }
        }
        if ((size_t)(max_ptr - min_ptr + 1) < m) break;
    }

    s->error = NO_ERROR;
    return true;
}
// -------------------------------------------------------------------------------- 

bool drop_string_substr(string_t* s, const string_t* needle, char* min_ptr, char* max_ptr) {
    if (!s || !needle) { 
        if (s) {
            s->error = INVALID_ARG; 
            errno = set_errno_from_error(s->error); 
        } else errno = EINVAL;
        return false; 
    }
    if (!s->str || !needle->str) { 
        if (s && !s->str) s->error = NULL_POINTER; 
        errno = set_errno_from_error(s->error); 
        return false; 
    }
    if (!is_string_ptr(s, min_ptr, false) || !is_string_ptr(s, max_ptr, false)) {
        s->error = OUT_OF_BOUNDS; 
        errno = set_errno_from_error(s->error); 
        return false;
    }
    if (max_ptr < min_ptr) { 
        s->error = INVALID_ARG; 
        errno = set_errno_from_error(s->error); 
        return false; 
    }

    const size_t m = needle->len;
    if (m == 0) { 
        s->error = NO_ERROR; 
        return true; 
    }

    for (;;) {
        size_t span = (size_t)(max_ptr - min_ptr + 1);
        if (span < m) break;

        /* reuse the literal helper with known length by passing needle->str */
        char* hit = _last_literal_between_ptrs_simd(s, needle->str, min_ptr, max_ptr);
        if (!hit) break;

        size_t hit_off = (size_t)(hit - s->str);
        size_t drop_len = m;
        if (hit_off + m < s->len && hit[m] == ' ') ++drop_len;

        size_t bytes_from_src = (s->len + 1) - (hit_off + drop_len);
        memmove(hit, hit + drop_len, bytes_from_src);

        s->len -= drop_len;
        if (max_ptr >= hit) {
            ptrdiff_t d = (ptrdiff_t)drop_len;
            if (s->len == 0) {
                max_ptr = s->str;
            } else {
                max_ptr -= d;
                if (max_ptr >= s->str + s->len) max_ptr = s->str + s->len - 1;
            }
        }
        if ((size_t)(max_ptr - min_ptr + 1) < m) break;
    }

    s->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_lit_substr(string_t* string,
                        const char* pattern,
                        const char* repl,
                        char* min_ptr,
                        char* max_ptr) {
    /* ---- validation ---- */
    if (!string || !pattern || !repl) {
        if (string) {
            string->error = INVALID_ARG;
            errno = set_errno_from_error(string->error); 
        } else errno = EINVAL;
        return false;
    }
    if (!string->str) { 
        string->error = NULL_POINTER; 
        errno = set_errno_from_error(string->error); 
        return false; 
    }
    if (!is_string_ptr(string, min_ptr, false) ||
        !is_string_ptr(string, max_ptr, false)) {
        string->error = OUT_OF_BOUNDS; 
        errno = set_errno_from_error(string->error); 
        return false;
    }
    if (max_ptr < min_ptr) { 
        string->error = INVALID_ARG; 
        errno = set_errno_from_error(string->error); 
        return false; 
    }

    const size_t pat_len = strlen(pattern);
    const size_t rep_len = strlen(repl);

    /* Empty pattern → no-op */
    if (pat_len == 0) { 
        string->error = NOT_FOUND;
        errno = set_errno_from_error(string->error);
        return true; 
    }

    /* Window too small to contain pattern → nothing to do */
    if ((size_t)(max_ptr - min_ptr + 1) < pat_len) { 
        string->error = NOT_FOUND; 
        errno = set_errno_from_error(string->error);
        return true; 
    }

    /* If replacement text aliases the same buffer, stash a safe copy once. */
    const char* rsrc = repl;
    char* rtmp = NULL;
    {
        const char* base = string->str;
        const char* end  = base + string->len + 1;  /* include NUL */
        if (rsrc >= base && rsrc < end) {
            rtmp = (char*)malloc(rep_len ? rep_len : 1);
            if (!rtmp && rep_len) { 
                string->error = BAD_ALLOC; 
                errno = set_errno_from_error(string->error); 
                return false; 
            }
            if (rep_len) memcpy(rtmp, rsrc, rep_len);
            rsrc = rtmp;  /* safe stable source for all edits */
        }
    }

    /* ---- growth pre-pass (if replacement longer) ---- */
    const ptrdiff_t delta = (ptrdiff_t)rep_len - (ptrdiff_t)pat_len;
    if (delta > 0) {
        size_t count = 0;
        char* end = max_ptr;
        for (;;) {
            char* hit = _last_literal_between_ptrs_simd(string, pattern, min_ptr, end);
            if (!hit) break;
            ++count;
            if (hit <= min_ptr) break;
            end = hit - 1;  /* ensure non-overlapping counting */
        }

        if (count) {
            /* overflow-safe compute: len + delta*count + 1 */
            if ((size_t)delta > 0 &&
                count > (SIZE_MAX - (size_t)string->len - 1) / (size_t)delta) {
                free(rtmp);
                errno = EOVERFLOW; return false;
            }
            size_t add = (size_t)delta * count;
            size_t new_size = string->len + add + 1;

            if (new_size > string->alloc) {
                size_t off_min = (size_t)(min_ptr - string->str);
                size_t off_max = (size_t)(max_ptr - string->str);

                char* newbuf = (char*)realloc(string->str, new_size);
                if (!newbuf) { free(rtmp); string->error = REALLOC_FAIL; errno = ENOMEM; return false; }

                string->str   = newbuf;
                string->alloc = new_size;

                /* Rebase window to new buffer address */
                min_ptr = string->str + off_min;
                max_ptr = string->str + off_max;
            }
        }
    }

    /* ---- right-to-left replace loop ---- */
    for (;;) {
        size_t span = (size_t)(max_ptr - min_ptr + 1);
        if (span < pat_len) break;

        char* hit = _last_literal_between_ptrs_simd(string, pattern, min_ptr, max_ptr);
        if (!hit) break;

        const size_t hit_off = (size_t)(hit - string->str);
        const size_t suffix_src_off = hit_off + pat_len;
        const size_t suffix_bytes   = (string->len + 1) - suffix_src_off;  /* includes NUL */

        if (delta == 0) {
            /* same length: overwrite */
            if (rep_len) memcpy(hit, rsrc, rep_len);
        } else if (delta < 0) {
            /* shrink: write replacement, then close the gap (moves NUL too) */
            if (rep_len) memcpy(hit, rsrc, rep_len);
            memmove(string->str + hit_off + rep_len,
                    string->str + suffix_src_off,
                    suffix_bytes);
            string->len = (size_t)((ptrdiff_t)string->len + delta);
        } else {
            /* grow: open space (moves NUL), then write replacement */
            memmove(string->str + hit_off + rep_len,
                    string->str + suffix_src_off,
                    suffix_bytes);
            if (rep_len) memcpy(hit, rsrc, rep_len);
            string->len = (size_t)((ptrdiff_t)string->len + delta);
        }

        /* Keep searching earlier: slide right edge left by delta if it was at/after hit */
        if (max_ptr >= hit) {
            max_ptr -= delta;
            if (string->len) {
                if ((size_t)(max_ptr - string->str) >= string->len)
                    max_ptr = string->str + string->len - 1;
            } else {
                max_ptr = string->str; /* harmless placeholder */
            }
        }

        if (min_ptr >= max_ptr) break;
    }

    /* Ensure NUL at logical end (already moved by memmove, but explicit is fine) */
    string->str[string->len] = '\0';
    string->error = NO_ERROR;
    free(rtmp);
    return true;
}
// -------------------------------------------------------------------------------- 

bool replace_string_substr(string_t* string,
                           const string_t* pattern,
                           const string_t* repl,
                           char* min_ptr,
                           char* max_ptr) {
    /* ---- basic validation ---- */
    if (!string || !pattern || !repl) {
        if (string) {
            string->error = INVALID_ARG;
            errno = set_errno_from_error(string->error); 
        } else errno = EINVAL;
        return false;
    }
    if (!string->str) { 
        string->error = NULL_POINTER; 
        errno = set_errno_from_error(string->error); 
        return false; 
    }
    if (!pattern->str || !repl->str) { 
        string->error = NULL_POINTER;
        errno = set_errno_from_error(string->error); 
        return false; 
    }

    if (!is_string_ptr(string, min_ptr, false) ||
        !is_string_ptr(string, max_ptr, false)) {
        string->error = OUT_OF_BOUNDS; 
        errno = set_errno_from_error(string->error); 
        return false;
    }
    if (max_ptr < min_ptr) { 
        string->error = INVALID_ARG; 
        errno = set_errno_from_error(string->error); 
        return false; 
    }

    const size_t pat_len = pattern->len;
    const size_t rep_len = repl->len;

    /* Policy: empty pattern → no-op */
    if (pat_len == 0) { 
        string->error = NOT_FOUND;
        errno = set_errno_from_error(string->error);
        return true; 
    }

    /* Window too small to contain pattern → nothing to do */
    if ((size_t)(max_ptr - min_ptr + 1) < pat_len) { 
        string->error = NOT_FOUND;
        errno = set_errno_from_error(string->error);
        return true; 
    }

    /* ---- growth pre-pass (if replacement longer) ---- */
    const ptrdiff_t delta = (ptrdiff_t)rep_len - (ptrdiff_t)pat_len;
    if (delta > 0) {
        size_t count = 0;
        char* end = max_ptr;
        for (;;) {
            char* hit = _last_string_between_ptrs_simd(string, pattern, min_ptr, end);
            if (!hit) break;
            ++count;
            if (hit <= min_ptr) break;     /* no more room to the left */
            end = hit - 1;                 /* keep matches non-overlapping */
        }

        if (count) {
            /* overflow-safe size calc: len + delta*count + 1 */
            size_t add;
            if ((size_t)delta > 0 &&
                count > (SIZE_MAX - (size_t)string->len - 1) / (size_t)delta) {
                errno = EOVERFLOW; return false;
            }
            add = (size_t)delta * count;
            size_t new_size = string->len + add + 1;

            if (new_size > string->alloc) {
                size_t off_min = (size_t)(min_ptr - string->str);
                size_t off_max = (size_t)(max_ptr - string->str);

                char* newbuf = (char*)realloc(string->str, new_size);
                if (!newbuf) { string->error = REALLOC_FAIL; errno = ENOMEM; return false; }

                string->str   = newbuf;
                string->alloc = new_size;

                /* Rebase window to the (potentially moved) buffer */
                min_ptr = string->str + off_min;
                max_ptr = string->str + off_max;
            }
        }
    }

    /* ---- right-to-left replacement loop ---- */
    for (;;) {
        /* If the remaining window can’t fit the pattern, we’re done */
        size_t span = (size_t)(max_ptr - min_ptr + 1);
        if (span < pat_len) break;

        char* hit = _last_string_between_ptrs_simd(string, pattern, min_ptr, max_ptr);
        if (!hit) break;

        const size_t hit_off = (size_t)(hit - string->str);
        /* suffix (including NUL) starts after the matched pattern */
        const size_t suffix_src_off = hit_off + pat_len;
        const size_t suffix_bytes   = (string->len + 1) - suffix_src_off; /* +1 for NUL */

        if (delta == 0) {
            /* same length: overwrite in place */
            memcpy(hit, repl->str, rep_len);
        } else if (delta < 0) {
            /* shrink: write replacement, then close the gap (moves NUL too) */
            memcpy(hit, repl->str, rep_len);
            memmove(string->str + hit_off + rep_len,
                    string->str + suffix_src_off,
                    suffix_bytes);
            string->len = (size_t)((ptrdiff_t)string->len + delta);
        } else {
            /* grow: open space then write replacement */
            memmove(string->str + hit_off + rep_len,
                    string->str + suffix_src_off,
                    suffix_bytes);
            memcpy(hit, repl->str, rep_len);
            string->len = (size_t)((ptrdiff_t)string->len + delta);
        }

        /* slide right edge left to keep searching earlier positions */
        if (max_ptr >= hit) {
            ptrdiff_t d = delta;
            /* After shrink/grow, max_ptr refers to the same logical character earlier */
            max_ptr -= d;
            if (string->len) {
                if ((size_t)(max_ptr - string->str) >= string->len)
                    max_ptr = string->str + string->len - 1;
            } else {
                max_ptr = string->str; /* harmless placeholder */
            }
        }

        /* If we’ve collapsed the window or can’t fit another pattern, stop */
        if (min_ptr >= max_ptr) break;
    }

    /* Ensure NUL is at the logical end (already moved with memmove, but belt & suspenders) */
    string->str[string->len] = '\0';
    string->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

void to_upper_char(char* val) {
    if (!val) {
        errno = EINVAL;
        return;
    }
    if (*val >= 'a' && *val <= 'z') *val -= 32; 
}
// --------------------------------------------------------------------------------

void to_lower_char(char* val) {
    if (!val) {
        errno = EINVAL;
        return;
    }
    if (*val >= 'A' && *val <= 'Z') *val += 32; 
}
// --------------------------------------------------------------------------------

void to_uppercase(string_t *s) {
    if(!s || !s->str) {
        if (s) {
            s->error = NULL_POINTER;
            errno = set_errno_from_error(s->error);
        } else errno = EINVAL;
        return;
    }
    char* begin = s->str;
    char* end = s->str + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'a' && *i <= 'z') *i -= 32;
    }
    s->error = NO_ERROR;
}
// --------------------------------------------------------------------------------

void to_lowercase(string_t *s) {
    if(!s || !s->str) {
        if (s) {
            s->error = NULL_POINTER;
            errno = set_errno_from_error(s->error);
        } else errno = EINVAL;
        return;
    }
    char* begin = s->str;
    char* end = s->str + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'A' && *i <= 'Z') *i += 32;
    }
    s->error = NO_ERROR;
}
// --------------------------------------------------------------------------------

string_t* pop_string_token(string_t* s, char token) {
    if (!s || !s->str) {
        if (s) {
            s->error = NULL_POINTER;
            errno = set_errno_from_error(s->error);
        } else errno = EINVAL;
        return NULL;
    }
    if (s->len == 0) {
        s->error = INVALID_ARG;         /* “empty input” rather than UNINITIALIZED */
        errno = set_errno_from_error(s->error);
        return NULL;
    }
    if ((unsigned char)token == '\0') { /* NUL never appears in payload */
        s->error = INVALID_ARG;
        errno = set_errno_from_error(s->error);
        return NULL;
    }

    /* Search from end without signed underflow */
    for (size_t i = s->len; i-- > 0; ) {
        if (s->str[i] == token) {
            /* Allocate the suffix *before* mutating the source */
            const char* suffix = s->str + i + 1;   /* points to '\0' if token is last */
            string_t* out = init_string(suffix);
            if (!out) {
                /* init_string set errno (likely ENOMEM); reflect library code too */
                s->error = BAD_ALLOC;
                errno = set_errno_from_error(s->error);
                return NULL;
            }
            /* Now safely truncate the source at the token */
            s->str[i] = '\0';
            s->len    = i;
            s->error  = NO_ERROR;
            return out;
        }
    }

    /* Not found */
    s->error = NOT_FOUND;
    errno = set_errno_from_error(s->error);
    return NULL;
}
// --------------------------------------------------------------------------------

size_t token_count(const string_t* str, const char* delim) {
    if (!str || !str->str || !delim) { 
        errno = EINVAL; 
        return 0; 
    }
    const size_t n = str->len;
    const size_t dlen = strlen(delim);
    if (n == 0 || dlen == 0) return 0;
    return token_count_simd(str->str, n, delim, dlen);
}
// --------------------------------------------------------------------------------

char get_char(string_t* str, size_t index) {
    if (!str || !str->str) {
        if (str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        } else errno = EINVAL;
        return 0;
    }
    if (index > str->len - 1) {
        str->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(str->error);
        return 0;
    }
    if (str->len == 0) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return 0;
    }
    str->error = NO_ERROR;
    return str->str[index];
}
// --------------------------------------------------------------------------------

void replace_char(string_t* str, size_t index, char value) {
    if (!str || !str->str) {
        if(str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        } else errno = EINVAL;
        return;
    }
    if (index > str->len - 1) {
        str->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(str->error);
        return;
    }
    if (str->len == 0) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return;
    }
    str->error = NO_ERROR;
    str->str[index] = value;
}
// --------------------------------------------------------------------------------

void trim_leading_whitespace(string_t* str) {
    if (!str || !str->str) {
        if (str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        } else errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return;
    }
    
    // Find first non-whitespace character
    char* ptr = str->str;
    while (*ptr && isspace(*ptr)) {
        ptr++;
    }
    
    // If no leading whitespace found, return
    if (ptr == str->str) {
        return;
    }
    
    // Calculate number of whitespace characters
    size_t whitespace_count = ptr - str->str;
    
    // Move remaining string to front
    memmove(str->str, ptr, str->len - whitespace_count + 1);  // +1 for null terminator
    
    // Update length
    str->len -= whitespace_count;
    str->error = NO_ERROR;
    return;
}
// -------------------------------------------------------------------------------- 

void trim_trailing_whitespace(string_t* str) {
    if (!str || !str->str) {
        if(str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        } else errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return;
    }
    
    // Start from last character (before null terminator)
    char* ptr = str->str + str->len - 1;
    
    // Move backwards while finding whitespace
    while (ptr >= str->str && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')) {
        ptr--;
    }
    
    // Set new end of string
    ptr++;  // Move to one after last non-whitespace char
    *ptr = '\0';
    
    // Update length (ptr - str->str gives new length)
    str->error = NO_ERROR;
    str->len = ptr - str->str;
}
// --------------------------------------------------------------------------------

void trim_all_whitespace(string_t* str) {
    if (!str || !str->str) {
        if (str) {
            str->error = NULL_POINTER;
            errno = set_errno_from_error(str->error);
        } else errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        str->error = INVALID_ARG;
        errno = set_errno_from_error(str->error);
        return;
    }
    
    char* read = str->str;
    char* write = str->str;
    
    while (*read) {
        if (*read != ' ' && *read != '\t' && *read != '\n') {
            *write = *read;
            write++;
        }
        read++;
    }
    
    // Null terminate the string at the new position
    *write = '\0';
    
    // Update length (write - str->str gives new length)
    str->len = write - str->str;
    str->error = NO_ERROR; 
    return;
}
// ================================================================================ 
// ================================================================================ 

struct str_iter {
    string_t *owner;
    char *begin;
    char *end;
    char *cur;
};
// -------------------------------------------------------------------------------- 

struct cstr_iter {
    const string_t *owner;
    const char *begin;
    const char *end;
    const char *cur;
};
// -------------------------------------------------------------------------------- 

/* ---------- helpers ---------- */
static inline bool _valid_begin_end(const char *begin, const char *end) {
    return (begin != NULL) && (end != NULL) && (begin <= end);
}

/* ---------- constructors ---------- */
str_iter str_iter_make(string_t *s) {
    str_iter it = {0};
    if (!s || !s->str) {
        if (s) { 
            s->error = NULL_POINTER;
            errno = set_errno_from_error(s->error);
        } else errno = EINVAL;
        return it;
    }
    it.owner = s;
    it.begin = s->str;
    it.end   = s->str + s->len; /* one past last, not the NUL */
    it.cur   = it.begin;
    return it;
}

cstr_iter cstr_iter_make(const string_t *s) {
    cstr_iter it = {0};
    if (!s || !s->str) {
        errno = EINVAL;
        return it;
    }
    it.owner = s;
    it.begin = s->str;
    it.end   = s->str + s->len;
    it.cur   = it.begin;
    return it;
}

/* ---------- predicates ---------- */
bool str_iter_valid(const str_iter *it) {
    return it && _valid_begin_end(it->begin, it->end) && it->cur && (it->cur < it->end);
}
bool cstr_iter_valid(const cstr_iter *it) {
    return it && _valid_begin_end(it->begin, it->end) && it->cur && (it->cur < it->end);
}
bool str_iter_at_end (const str_iter  *it) { return !str_iter_valid(it); }
bool cstr_iter_at_end(const cstr_iter *it) { return !cstr_iter_valid(it); }

/* ---------- accessors ---------- */
char str_iter_get(const str_iter *it) {
    return str_iter_valid(it) ? *it->cur : 0;
}
char cstr_iter_get(const cstr_iter *it) {
    return cstr_iter_valid(it) ? *it->cur : 0;
}
char* str_iter_ptr(const str_iter *it) {
    return str_iter_valid(it) ? it->cur : NULL;
}
const char* cstr_iter_ptr(const cstr_iter *it) {
    return cstr_iter_valid(it) ? it->cur : NULL;
}
size_t str_iter_pos(const str_iter *it) {
    if (!it || !_valid_begin_end(it->begin, it->end) || !it->cur) { errno = EINVAL; return (size_t)-1; }
    return (size_t)(it->cur - it->begin);
}
size_t cstr_iter_pos(const cstr_iter *it) {
    if (!it || !_valid_begin_end(it->begin, it->end) || !it->cur) { errno = EINVAL; return (size_t)-1; }
    return (size_t)(it->cur - it->begin);
}

/* ---------- movement ---------- */
static inline bool _advance_core(char **pcur, const char *begin, const char *end, ptrdiff_t delta) {
    if (!pcur || !*pcur || !begin || !end) return false;
    const char *cur = *pcur;
    const char *target = cur + delta;
    if (delta > 0) {
        if (target > end) target = end;
    } else if (delta < 0) {
        if (target < begin) target = begin;
    }
    *pcur = (char*)target;
    return (target < end);
}
static inline bool _advance_core_const(const char **pcur, const char *begin, const char *end, ptrdiff_t delta) {
    if (!pcur || !*pcur || !begin || !end) return false;
    const char *cur = *pcur;
    const char *target = cur + delta;
    if (delta > 0) {
        if (target > end) target = end;
    } else if (delta < 0) {
        if (target < begin) target = begin;
    }
    *pcur = target;
    return (target < end);
}

bool str_iter_next (str_iter *it)  { return _advance_core(&it->cur, it->begin, it->end, +1); }
bool cstr_iter_next(cstr_iter *it) { return _advance_core_const(&it->cur, it->begin, it->end, +1); }
bool str_iter_prev (str_iter *it)  { return _advance_core(&it->cur, it->begin, it->end, -1); }
bool cstr_iter_prev(cstr_iter *it) { return _advance_core_const(&it->cur, it->begin, it->end, -1); }

bool str_iter_advance (str_iter *it,  ptrdiff_t delta) { return _advance_core(&it->cur,  it->begin,  it->end,  delta); }
bool cstr_iter_advance(cstr_iter *it, ptrdiff_t delta) { return _advance_core_const(&it->cur, it->begin, it->end, delta); }

bool str_iter_seek_begin (str_iter *it)  { if (!it) return false; it->cur = it->begin; return it->cur < it->end; }
bool cstr_iter_seek_begin(cstr_iter *it) { if (!it) return false; it->cur = it->begin; return it->cur < it->end; }

bool str_iter_seek_end (str_iter *it)  { if (!it) return false; it->cur = it->end; return false; }
bool cstr_iter_seek_end(cstr_iter *it) { if (!it) return false; it->cur = it->end; return false; }
// ================================================================================ 
// ================================================================================ 

struct string_v {
    string_t* data;
    size_t len;
    size_t alloc;
    ErrorCode error;
};
// -------------------------------------------------------------------------------- 

string_v* init_str_vector(size_t buff) {
    string_v* struct_ptr = malloc(sizeof(string_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    // Allocate storage for `buff` string_t elements (zero-initialized).
    string_t* data_ptr = calloc(buff, sizeof(string_t));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL;
    }

    struct_ptr->data  = data_ptr;
    struct_ptr->len   = 0;
    struct_ptr->alloc = buff;
    struct_ptr->error = NO_ERROR;
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

ErrorCode get_str_vector_error(const string_v* vec) {
    if (!vec) {
        errno = EINVAL;
        return INVALID_ERROR;
    }
    return vec->error;
}
// -------------------------------------------------------------------------------- 

bool is_str_vector_valid(const string_v* vec) {
    if (vec && vec->data) return true;
    errno = EINVAL;
    return false;
}
// --------------------------------------------------------------------------------

void free_str_vector(string_v* vec) {
   if (!vec) {
       errno = EINVAL;
       return;
   }
   
   // Free each string in the vector
   if (vec->data) {
       for (size_t i = 0; i < vec->len; i++) {
           free(vec->data[i].str);
       }
       free(vec->data);
   }
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_str_vector(string_v** vec) {
    if (vec && *vec) {
        free_str_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_str_vector(string_v* vec, const char* value) {
    if (!vec || !vec->data || !value) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
   
    // Allocate and copy the new string
    size_t str_len = strlen(value);
    vec->data[vec->len].str = malloc(str_len + 1);
    if (!vec->data[vec->len].str) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        return false;
    }
   
    strcpy(vec->data[vec->len].str, value);
    vec->data[vec->len].alloc = str_len + 1;
    vec->data[vec->len].len = str_len;
    vec->len++;
    vec->error = NO_ERROR; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_str_vector(string_v* vec, const char* value) {
    if (!vec || !vec->data || !value) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Move existing elements right
    memmove(vec->data + 1, vec->data, vec->len * sizeof(string_t));
    
    // Initialize new first element space
    memset(vec->data, 0, sizeof(string_t));
    
    // Allocate and copy the new string
    size_t str_len = strlen(value);
    vec->data[0].str = malloc(str_len + 1);
    if (!vec->data[0].str) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        memmove(vec->data, vec->data + 1, vec->len * sizeof(string_t));
        return false;
    }
    
    strcpy(vec->data[0].str, value);
    vec->data[0].alloc = str_len + 1;
    vec->data[0].len = str_len;
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_str_vector(string_v* vec, const char* str, size_t index) {
    if (!vec || !vec->data || !str) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(string_t));
    }
    
    // Initialize new element space
    memset(vec->data + index, 0, sizeof(string_t));
    
    // Allocate and copy the new string
    size_t str_len = strlen(str);
    vec->data[index].str = malloc(str_len + 1);
    if (!vec->data[index].str) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        if (index < vec->len) {  // Only restore if not appending
            memmove(vec->data + index, vec->data + index + 1, 
                    (vec->len - index) * sizeof(string_t));
        }
        return false;
    }
    
    strcpy(vec->data[index].str, str);
    vec->data[index].alloc = str_len + 1;
    vec->data[index].len = str_len;
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

string_t* pop_back_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return NULL;
    }
    
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error);
        return NULL;
    }
    
    // Create copy of last element
    string_t* temp = copy_string(&vec->data[vec->len - 1]);
    if (!temp) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        return NULL;  // copy_string will set errno
    }
    
    // Clear the popped element for future reuse
    free(vec->data[vec->len - 1].str);
    vec->data[vec->len - 1].str = NULL;
    vec->data[vec->len - 1].len = 0;
    vec->data[vec->len - 1].alloc = 0;
    
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

string_t* pop_front_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return NULL;
    }
   
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error); 
        return NULL;
    }
   
    // Create copy of first element
    string_t* temp = copy_string(&vec->data[0]);
    if (!temp) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        return NULL;  // copy_string will set errno
    }
   
    // Free the first element
    free(vec->data[0].str);
   
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
   
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

string_t* pop_any_str_vector(string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return NULL;
    }
   
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error);
        return NULL;
    }

    if (index >= vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return NULL;
    }

    // Create copy of element to pop
    string_t* temp = copy_string(&vec->data[index]);
    if (!temp) {
        vec->error = BAD_ALLOC;
        errno = set_errno_from_error(vec->error);
        return NULL;  // copy_string will set errno
    }
   
    // Free the element being removed
    free(vec->data[index].str);
    
    // Shift remaining elements left
    memmove(&vec->data[index], &vec->data[index + 1], 
            (vec->len - index - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
    
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

bool delete_back_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
    
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error);
        return false;
    }
    
    // Clear the popped element for future reuse
    free(vec->data[vec->len - 1].str);
    vec->data[vec->len - 1].str = NULL;
    vec->data[vec->len - 1].len = 0;
    vec->data[vec->len - 1].alloc = 0;
    
    vec->len--;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool delete_front_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error);
        return false;
    } 
    // Free the first element
    free(vec->data[0].str);
   
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
   
    vec->len--;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool delete_any_str_vector(string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
  
    if (vec->len == 0) {
        vec->error = UNINITIALIZED;
        errno = set_errno_from_error(vec->error);
        return false;
    }
    
    if (index >= vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return false;
    }

    // Free the element being removed
    free(vec->data[index].str);
    
    // Shift remaining elements left
    memmove(&vec->data[index], &vec->data[index + 1], 
            (vec->len - index - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
    
    vec->len--;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

const string_t* cstr_vector_index(const string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return NULL;
    }
    return &vec->data[index];
}
// -------------------------------------------------------------------------------- 

string_t* str_vector_index(string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return NULL;
    }
    return &vec->data[index];
}
// --------------------------------------------------------------------------------

size_t str_vector_size(const string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t str_vector_alloc(const string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return;
    }
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_string(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
    vec->error = NO_ERROR;
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_string(string_t* a, string_t* b) {
    if (!a || !b) {
        if (a) {
            a->error = NULL_POINTER;
            errno = set_errno_from_error(a->error);
        } 
        else if (b) {
            b->error = NULL_POINTER;
            errno = set_errno_from_error(b->error);
        } else errno = EINVAL;
        return;
    }
    string_t temp = *a;
    *a = *b;
    *b = temp;
    a->error = NO_ERROR;
    b->error = NO_ERROR;
}
// --------------------------------------------------------------------------------

static string_t* _median_of_three(string_t* a, string_t* b, string_t* c, iter_dir direction) {
    if ((direction == FORWARD && compare_strings_string(a, b) < 0) ||
        (direction == REVERSE && compare_strings_string(a, b) > 0)) {
        if ((direction == FORWARD && compare_strings_string(b, c) < 0) ||
            (direction == REVERSE && compare_strings_string(b, c) > 0))
            return b;
        if ((direction == FORWARD && compare_strings_string(a, c) < 0) ||
            (direction == REVERSE && compare_strings_string(a, c) > 0))
            return c;
        return a;
    }
    if ((direction == FORWARD && compare_strings_string(a, c) < 0) ||
        (direction == REVERSE && compare_strings_string(a, c) > 0))
        return a;
    if ((direction == FORWARD && compare_strings_string(b, c) < 0) ||
        (direction == REVERSE && compare_strings_string(b, c) > 0))
        return c;
    return b;
}
// --------------------------------------------------------------------------------

static void _insertion_sort(string_t* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        string_t key = vec[i];
        int j = i - 1;
        while (j >= low && ((direction == FORWARD && compare_strings_string(&vec[j], &key) > 0) ||
                           (direction == REVERSE && compare_strings_string(&vec[j], &key) < 0))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}
// --------------------------------------------------------------------------------

static int _partition_string(string_t* vec, int low, int high, iter_dir direction) {
    // Choose pivot using median of three
    int mid = low + (high - low) / 2;
    string_t* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    // Move pivot to end
    if (pivot_ptr != &vec[high])
        swap_string(pivot_ptr, &vec[high]);
    
    string_t pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && compare_strings_string(&vec[j], &pivot) < 0) ||
            (direction == REVERSE && compare_strings_string(&vec[j], &pivot) > 0)) {
            i++;
            swap_string(&vec[i], &vec[j]);
        }
    }
    swap_string(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_str_vector(string_t* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        // Use insertion sort for small arrays
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_string(vec, low, high, direction);
        
        // Tail recursion elimination
        if (pi - low < high - pi) {
            _quicksort_str_vector(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_str_vector(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// --------------------------------------------------------------------------------

void sort_str_vector(string_v* vec, iter_dir direction) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return;
    }
    if (vec->len < 2) {
        vec->error = NO_ERROR;
        return;
    }
    
    _quicksort_str_vector(vec->data, 0, vec->len - 1, direction);
    vec->error = NO_ERROR;
}
// -------------------------------------------------------------------------------- 

size_t binary_search_str_vector(string_v* vec, char* value, bool sort_first) {
    if (!vec || !vec->data || !value) {
        if (vec) {
            vec->error = NULL_POINTER;
            set_errno_from_error(vec->error);
        } else {
            errno = EINVAL;
        }
        return SIZE_MAX;
    }

    if (vec->len == 0) {
        vec->error = NOT_FOUND;            /* empty: nothing to find */
        set_errno_from_error(vec->error);  /* -> ENOENT */
        return SIZE_MAX;
    }

    /* Optional: sort ascending first */
    if (sort_first) {
        sort_str_vector(vec, FORWARD);
        if (vec->error != NO_ERROR) {
            /* sort_str_vector has already set errno */
            return SIZE_MAX;
        }
    }

    /* Binary search (ascending order) */
    size_t low = 0, high = vec->len - 1;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;

        /* Defensive: ensure element buffer exists */
        if (!vec->data[mid].str) {
            vec->error = STATE_CORRUPT;
            set_errno_from_error(vec->error);
            return SIZE_MAX;
        }

        int cmp = strcmp(vec->data[mid].str, value);
        if (cmp == 0) {
            vec->error = NO_ERROR;
            return mid;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            if (mid == 0) break;  /* prevent size_t underflow */
            high = mid - 1;
        }
    }

    vec->error = NOT_FOUND;
    set_errno_from_error(vec->error);      /* -> ENOENT */
    return SIZE_MAX;
}
// --------------------------------------------------------------------------------

string_v* tokenize_string(const string_t* str, const char* delim) {
    if (!str || !str->str || !delim) {
        errno = EINVAL;
        return NULL;
    }
    
    // Initialize vector to store tokens
    size_t count = token_count(str, delim);
    string_v* tokens = init_str_vector(count);
    if (!tokens) {
        errno = ENOMEM;
        return NULL;
    }
    
    const char* start = str->str;
    const char* end = str->str + str->len;
    const char* current = start;
    
    while (current < end) {
        // Skip delimiters
        while (current < end) {
            bool is_delim = false;
            for (const char* d = delim; *d; d++) {
                if (*current == *d) {
                    is_delim = true;
                    break;
                }
            }
            if (!is_delim) break;
            current++;
        }
        
        if (current >= end) break;
        
        // Find end of token
        const char* token_end = current;
        while (token_end < end) {
            bool is_delim = false;
            for (const char* d = delim; *d; d++) {
                if (*token_end == *d) {
                    is_delim = true;
                    break;
                }
            }
            if (is_delim) break;
            token_end++;
        }
        
        // Create temporary null-terminated string for token
        size_t token_len = token_end - current;
        char temp[token_len + 1];
        memcpy(temp, current, token_len);
        temp[token_len] = '\0';
        
        if (!push_back_str_vector(tokens, temp)) {
            free_str_vector(tokens);
            return NULL;
        }
        
        current = token_end;
    }
    
    return tokens;
}
// -------------------------------------------------------------------------------- 

strv_iter strv_iter_make(string_v *vec) {
    strv_iter it = {0};
    if (!vec || !vec->data) {
        if (vec) {  
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return it;
    }
    it.owner = vec;
    it.begin = vec->data;
    it.end   = vec->data + vec->len; /* one past last, not the NUL */
    it.cur   = it.begin;
    return it;
}
// -------------------------------------------------------------------------------- 

cstrv_iter cstrv_iter_make(const string_v *vec) {
    cstrv_iter it = {0};
    if (!vec || !vec->data) {
        errno = EINVAL;
        return it;
    }
    it.owner = vec;
    it.begin = vec->data;
    it.end   = vec->data + vec->len;
    it.cur   = it.begin;
    return it;
}
// -------------------------------------------------------------------------------- 

static inline bool strv_iter_well_formed(const strv_iter *it) {
    return it && it->owner && it->begin && it->end && it->cur
        && (it->begin <= it->cur) && (it->cur <= it->end);
}
// -------------------------------------------------------------------------------- 

static inline bool cstrv_iter_well_formed(const cstrv_iter *it) {
    return it && it->owner && it->begin && it->end && it->cur
        && (it->begin <= it->cur) && (it->cur <= it->end);
}
// -------------------------------------------------------------------------------- 

bool strv_iter_valid(const strv_iter *it) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return false; }
    return it->cur < it->end;
}
// -------------------------------------------------------------------------------- 

bool cstrv_iter_valid(const cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return false; }
    return it->cur < it->end;
}
// -------------------------------------------------------------------------------- 

bool strv_iter_at_end(const strv_iter *it) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return false; }
    return it->cur == it->end;
}
// -------------------------------------------------------------------------------- 

/** @return true iff iterator is well-formed and exactly at end (cur == end). */
bool cstrv_iter_at_end(const cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return false; }
    return it->cur == it->end;
}
// -------------------------------------------------------------------------------- 

string_t *strv_iter_get(const strv_iter *it) {
    return (it->cur == it->end) ? NULL : (string_t*)it->cur;
}
// -------------------------------------------------------------------------------- 

/** @return current element or NULL if at end; sets errno only if malformed. */
const string_t *cstrv_iter_get(const cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return NULL; }
    return (it->cur == it->end) ? NULL : it->cur;
}
// -------------------------------------------------------------------------------- 

size_t strv_iter_pos(const strv_iter *it) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return (size_t)-1; }
    return (size_t)(it->cur - it->begin);
}
// -------------------------------------------------------------------------------- 

/** @return 0-based index (begin..size). Returns (size_t)-1 if malformed. */
size_t cstrv_iter_pos(const cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return (size_t)-1; }
    return (size_t)(it->cur - it->begin);
}
// -------------------------------------------------------------------------------- 

bool strv_iter_next(strv_iter *it)   { 
    if (strv_iter_well_formed(it) && it->cur < it->end) {
        ++it->cur; 
        return true;
    }
    return false;
}
// -------------------------------------------------------------------------------- 

bool cstrv_iter_next(cstrv_iter *it) { 
    if (cstrv_iter_well_formed(it) && it->cur < it->end) {
        ++it->cur;
        return true;
    }
    return false;
}
// -------------------------------------------------------------------------------- 

bool strv_iter_prev(strv_iter *it) { 
    if (strv_iter_well_formed(it) && it->cur > it->begin) {
        --it->cur;
        return true;
    }
    return false;
}
// -------------------------------------------------------------------------------- 

bool cstrv_iter_prev(cstrv_iter *it) { 
    if (cstrv_iter_well_formed(it) && it->cur > it->begin) {
        --it->cur;
        return true;
    }
    return false;
}
// -------------------------------------------------------------------------------- 

size_t strv_iter_advance(strv_iter *it, size_t n) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return 0; }
    size_t left = (size_t)(it->end - it->cur);
    size_t step = (n < left) ? n : left;
    it->cur += step;
    return step;
}
// -------------------------------------------------------------------------------- 

size_t cstrv_iter_advance(cstrv_iter *it, size_t n) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return 0; }
    size_t left = (size_t)(it->end - it->cur);
    size_t step = (n < left) ? n : left;
    it->cur += step;
    return step;
}
// -------------------------------------------------------------------------------- 

bool strv_iter_seek_begin(strv_iter *it) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return false; }
    it->cur = it->begin;
    return true;
}
// -------------------------------------------------------------------------------- 

bool cstrv_iter_seek_begin(cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return false; }
    it->cur = it->begin;
    return true;
}
// -------------------------------------------------------------------------------- 

bool strv_iter_seek_end(strv_iter *it) {
    if (!strv_iter_well_formed(it)) { errno = EINVAL; return false; }
    it->cur = it->end;
    return true;
}
// -------------------------------------------------------------------------------- 

bool cstrv_iter_seek_end(cstrv_iter *it) {
    if (!cstrv_iter_well_formed(it)) { errno = EINVAL; return false; }
    it->cur = it->end;
    return true;
}
// ================================================================================
// ================================================================================
// eof
