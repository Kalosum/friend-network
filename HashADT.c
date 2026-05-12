/// \file HashADT.c
/// \brief Implementation of a hash table data structure.

#include "HashADT.h"

// #include <stdbool.h>    // bool
// #include <stddef.h>     // size_t
#include <assert.h>

// #include <ctype.h>
#include <stdio.h>
// #include <string.h>
#include <stdlib.h>


///
/// General Notes on hash table Operation
///
/// - The operations identified below are the public interface to HashADT.
///
/// - The client supplies functions for hashing the key, checking key
///   equality, printing key, value pairs, and deleting pairs.
///
/// - The table assumes ownership of keys and values.  The delete function
///   (by default) will free the (key,value) pair upon destruction; to
///   prevent this, the application should pass a NULL pointer for the
///   delete function, which causes the delete function to NOT free the
///   (key, value) pair.
///
/// - There is no remove functionality. Entries remain until you call destroy.
///
/// - The destroy calls a no-operation delete if the client passes NULL destroy.
///
/// - Wherever a function has a precondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///
/// - Wherever a function has a postcondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///

///
/// The HashADT data type is a pointer to an opaque structure; clients
/// cannot see the structure's content.
///
// typedef struct hashtab_s *HashADT;

struct hashtab_ele {
    void *value;
    void *key;
};

struct hashtab_s {
    struct hashtab_ele *elements;
	size_t size;
    size_t n_elements;
    size_t n_collions;
    size_t n_rehashes;

    size_t (*hash)( const void *key );
    bool (*equals)( const void *key1, const void *key2 );
    void (*print)( const void *key, const void *value );
    void (*delete)( void *key, void *value );
};

size_t key_to_index(const HashADT t, const void *key) {
    return t->hash(key) % t->size;
    // return t->size % t->hash(key);
}

///
/// Create a new hash table instance.  If delete is NULL, destroying the
/// table will NOT free the (key,value) data pairs.
///
/// @param hash The hash function for key data
/// @param equals The equal function for key comparison
/// @param print The print function for key, value pairs is used by dump().
/// @param delete The delete function for key, value pairs is used by destroy().
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre hash, equals and print are valid function pointers.
/// 
/// @return A newly created table
///
HashADT ht_create(
    size_t (*hash)( const void *key ),
    bool (*equals)( const void *key1, const void *key2 ),
    void (*print)( const void *key, const void *value ),
    void (*delete)( void *key, void *value )
) {
    assert(*hash != NULL);
    assert(*equals != NULL);
    assert(*print != NULL);

    HashADT h = (HashADT) malloc(sizeof (struct hashtab_s));

    assert(h != 0);

    h->hash = hash;
    h->equals = equals;
    h->print = print;
    h->delete = delete;

    h->n_collions = 0;
    h->n_rehashes = 0;
    h->n_elements = 0;
    
    h->size = INITIAL_CAPACITY;
    h->elements = malloc( (sizeof (struct hashtab_ele)) * h->size);

    for (int i = 0; i < (int) h->size; i++) {
        h->elements[i].key = (void *)0;
        h->elements[i].value = (void *)0;
    }
    
    return h;
}

///
/// Destroy the table instance, and call delete function on (key,value) pair.
/// 
/// @param t The table to destroy
/// 
/// @pre t is a valid instance of table.
/// 
/// @post t is not a valid instance of table.
///
void ht_destroy( HashADT t ) {
    assert(t != NULL);

    if (t->delete) {
        for (int i = 0; i < (int) t->size; i++) {
            if ((t->elements[i].key)) {
                t->delete(t->elements[i].key, t->elements[i].value);
            }
        }
    }

    free(t->elements);
    free(t);
}

///
/// Print information about hash table (size, capacity, collisions, rehashes). 
/// 
/// If contents is true, also print the entire contents of the hash table
/// using the registered print function with each non-null entry.
/// 
/// @param t The table to display
/// @param contents Do a full dump including the entire table contents
/// 
/// @pre t is a valid instance of table.
///
void ht_dump( const HashADT t, bool contents ) {
    assert(t != NULL);

    printf("Size: %zu\n", t->n_elements);
    printf("Capacity: %zu\n", t->size);
    printf("Collisions: %zu\n", t->n_collions);
    printf("Rehashes: %zu\n", t->n_rehashes);

    if (contents) {
        for (int i = 0; i < (int) t->size; i++) {
            printf("%d: ", i);
            if (!(t->elements[i].key)) {
                printf("null\n");
            } else {
                printf("(");
                t->print(t->elements[i].key, t->elements[i].value);
                printf(")\n");
            }
        }
    }
}

///
/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
/// 
/// @param t The table
/// @param key The key
/// 
/// @pre The table must have the key, or the function will assert failure
/// @pre has( t, key) is true.
/// @pre t is a valid instance of table, and key is not NULL.
/// 
/// @return The value associated with the key
///
const void *ht_get( const HashADT t, const void *key ) {
    assert(ht_has(t, key));

    size_t index = key_to_index(t, key);
    while (t->equals(t->elements[index].key, key) == false) {
        index = (index + 1) % t->size;
    }

    return t->elements[index].value;
}

///
/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
/// 
/// @param t The table
/// @param key The key
/// 
/// @pre t is a valid instance of table, and key is not NULL.
/// 
/// @return Whether the key exists in the table.
///
bool ht_has( const HashADT t, const void *key ) {
    assert(t != NULL);

    size_t index = key_to_index(t, key);
    size_t init_i = index;

    while (t->elements[index].key != NULL) {
        if (t->equals(t->elements[index].key, key)) {
            return true; 
        }
        t->n_collions++;
        index = (index + 1) % t->size;

        if (index == init_i) {
            break;
        }
    }

    return false;
}

/// Add a key value pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
/// 
/// @param t The table
/// @param key The key
/// @param value The value
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @post if size reached the LOAD_THRESHOLD, table has grown by RESIZE_FACTOR.
/// 
/// @return The old value associated with the key, if one exists.
///
void *ht_put( HashADT t, const void *key, const void *value ) {
    assert(t != NULL);

    if (((double) t->n_elements / t->size) >= LOAD_THRESHOLD) {
        HashADT temp_table = ht_create(t->hash, t->equals, t->print, t->delete); 
        temp_table->size = t->size * RESIZE_FACTOR;
        assert(temp_table != NULL);
        temp_table->elements = malloc(sizeof(struct hashtab_s) * temp_table->size);
        assert(temp_table->elements != NULL);
        for (int i = 0; i < (int) temp_table->size; ++i) {
            temp_table->elements[i].key = NULL;
            temp_table->elements[i].value = NULL;
        }

        // re-hash
        for (int i = 0; i < (int) t->size; ++i) {
            if (t->elements[i].key) {
                ht_put(temp_table, t->elements[i].key, t->elements[i].value);
            }
        }
        
        free(t->elements);
        t->size = temp_table->size;
        t->elements = temp_table->elements;
        free(temp_table);

        t->n_rehashes++;
    }

    struct hashtab_ele e;
    e.key = key;
    e.value = value;

    int idx = t->hash(key) % (int) t->size;
    while (t->elements[idx].key) {
        if (t->equals(t->elements[idx].key, key)) {
            void *prev = t->elements[idx].value;
            t->elements[idx] = e;
            return prev;
        }
        idx = (idx + 1) % (int) t->size;
        t->n_collions++;
    }

    t->elements[idx] = e;
    t->n_elements++;

    return NULL;
}

///
/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
/// 
/// @param t The table
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre t is a valid instance of table.
/// 
/// @post client is responsible for freeing the returned array.
/// 
/// @return A dynamic array of keys
///
void **ht_keys( const HashADT t ) {
    assert(t);
    void **keys = malloc( (sizeof (void *)) * t->n_elements);
    assert(keys);

    int index = 0;
    for (int i = 0; i < (int) t->size; i++) {
        if ((t->elements[i].key)) {
            keys[index++] = t->elements[i].key;
        }
    }

    return keys;
}

///
/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
/// 
/// @param t The table
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre t is a valid instance of table.
/// 
/// @post client is responsible for freeing the returned array.
/// 
/// @return A dynamic array of values
///
void **ht_values( const HashADT t ) {
    assert(t);
    void **values = malloc( (sizeof (void *)) * t->n_elements);
    assert(values);

    int index = 0;
    for (int i = 0; i < (int) t->size; i++) {
        if ((t->elements[i].key)) {
            values[index++] = t->elements[i].value;
        }
    }

    return values;
}
