/// \file Support.c
/// \brief Supporting functions for the Amici project.
///
/// These functions were taken from the test program for Homework 6.

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Support.h"

/////////////////////////////////////////////////////////////////////////
///  HASH TABLE SUPPORT FUNCTIONS
/////////////////////////////////////////////////////////////////////////

///
/// Because the hash table keeps pointers to these functions, similar
/// functions must have identical signatures.
///
/// For simplicity, none of these functions do error checking on their
/// parameters, so NULL parameters may cause unexpected behavior.
///
/// Hash functions: size_t name( const void * );
///    returning the hash value
///
/// Comparison functions: bool name( const void *, const void * );
///    returning 'true' if the parameters are equal
///
/// Print functions: void name( const void *, const void * );
///

///
/// Hash functions
///

/// str_hash function returns the hash of a native C-string.
///
/// Based on djb2, A simple c-string hashing algorithm by Dan Bernstein
/// http://www.cse.yorku.ca/~oz/hash.html
///
/// @param element the c-string to hash
///
/// @return the hash value of the c-string
///
size_t str_hash( const void *element ) {
    unsigned char *str = (unsigned char *) element;
    size_t hash = 5381;
    int c;

    while( (c = *str++) ) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return (size_t)hash;
}

///
/// Comparison functions
///

/// str_equals function - compares the elements as two C-strings.
///
/// @param element1 first c-string
/// @param element2 second c-string
///
/// @return true if the two strings are equal
///
bool str_equals( const void *element1, const void *element2 ) {
    return strcmp( (const char*)element1, (const char*)element2 ) == 0;
}
