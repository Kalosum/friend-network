/// \file Support.h
/// \brief Declarations for supporting functions for the Amici project.
///
/// These functions were taken from the test program for Homework 6.

#ifndef SUPPORT_H
#define SUPPORT_H

#include <sys/types.h>
#include <stdbool.h>

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
size_t str_hash( const void *element );

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
bool str_equals( const void *element1, const void *element2 );

#endif
