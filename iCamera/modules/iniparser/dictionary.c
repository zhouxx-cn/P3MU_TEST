/*-------------------------------------------------------------------------*/
/**
   @file    dictionary.c
   @author  N. Devillard
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/
#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sys_banned.h"
#ifdef TEST_MEM_INFO
#define MODULE_NAME "dictionary"
#include "MemoryInterface.h"
//#include "TCreateObject.h"

//#include "InternalCommon.h"

#endif

/** Maximum value size for integers and doubles. */
#define MAXVALSZ    (1024)

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ   (128)

/** Invalid key token */
#define DICT_INVALID_KEY    ((char*)-1)

/*---------------------------------------------------------------------------
                            Private functions
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*
  @brief    Duplicate a string
  @param    s String to duplicate
  @return   Pointer to a newly allocated string, to be freed with free()

  This is a replacement for strdup(). This implementation is provided
  for systems that do not have it.
 */
/*--------------------------------------------------------------------------*/
static char * xstrdup(const char * s)
{
    if (!s)
    {
        return NULL ;
    }

    char *t = NULL;
    size_t len = 0;

    len = strlen(s) + 1 ;
#ifndef TEST_MEM_INFO
    t = (char*) malloc(len) ;
#else
	t = (char *)MemoryMalloc(len, "dictionary", __FILE__, __FUNCTION__, __LINE__);
#endif
    if (t)
    {
        memcpy(t, s, len) ;
    }
    return t ;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    Double the size of the dictionary
  @param    d Dictionary to grow
  @return   This function returns non-zero in case of failure
 */
/*--------------------------------------------------------------------------*/
static int dictionary_grow(dictionary * d)
{
    char        ** new_val  = NULL;
    char        ** new_key  = NULL;
    unsigned     * new_hash = NULL;

    new_val  = (char**) calloc(d->size * 2, sizeof *d->val);
    new_key  = (char**) calloc(d->size * 2, sizeof *d->key);
    new_hash = (unsigned*) calloc(d->size * 2, sizeof *d->hash);
    if (!new_val || !new_key || !new_hash)
    {
        /* An allocation failed, leave the dictionary unchanged */
        if (new_val)
        {
#ifndef TEST_MEM_INFO
		free(new_val);
#else
		MemoryFree(new_val);
#endif
            new_val = NULL;
        }
        if (new_key)
        {
#ifndef TEST_MEM_INFO
			free(new_key);
#else
			MemoryFree(new_key);
#endif
            new_key = NULL;
        }
        if (new_hash)
        {
#ifndef TEST_MEM_INFO
            free(new_hash);
#else
			MemoryFree(new_hash);
#endif
            new_hash = NULL;
        }
        return -1 ;
    }
    /* Initialize the newly allocated space */
    memcpy(new_val, d->val, d->size * sizeof(char *));
    memcpy(new_key, d->key, d->size * sizeof(char *));
    memcpy(new_hash, d->hash, d->size * sizeof(unsigned));
    /* Delete previous data */
#ifndef TEST_MEM_INFO
		free(d->val);  
#else
		MemoryFree(d->val);
#endif
	d->val = NULL;
#ifndef TEST_MEM_INFO
		free(d->key); 
#else
		free(d->key); 
#endif
	d->key = NULL;
#ifndef TEST_MEM_INFO
    free(d->hash); 
#else
	MemoryFree(d->hash);
#endif
	d->hash = NULL;
    /* Actually update the dictionary */
    d->size *= 2 ;
    d->val = new_val;
    d->key = new_key;
    d->hash = new_hash;

    return 0 ;
}

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*
    @brief    Compute the hash key for a string.
    @param    key     Character string to use for key.
    @return   1 unsigned int on at least 32 bits.

    This hash function has been taken from an Article in Dr Dobbs Journal.
    This is normally a collision-free function, distributing keys evenly.
    The key is stored anyway in the struct so that collision can be avoided
    by comparing the key itself in last resort.
 */
/*--------------------------------------------------------------------------*/
unsigned dictionary_hash(const char * key)
{
    if (!key)
    {
        return 0 ;
    }
    
    size_t      len = 0;
    size_t      i = 0;
    unsigned    hash = 0;
    
    len = strlen(key);
    for (hash=0, i=0 ; i<len ; i++)
    {
        hash += (unsigned)key[i] ;
        hash += (hash<<10);
        hash ^= (hash>>6) ;
    }
    hash += (hash <<3);
    hash ^= (hash >>11);
    hash += (hash <<15);
    
    return hash ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Create a new dictionary object.
    @param    size    Optional initial size of the dictionary.
    @return   1 newly allocated dictionary objet.

    This function allocates a new dictionary object of given size and returns
    it. If you do not know in advance (roughly) the number of entries in the
    dictionary, give size=0.
 */
/*-------------------------------------------------------------------------*/
dictionary * dictionary_new(size_t size)
{
    /* If no size was specified, allocate space for DICTMINSZ */
    if (size < DICTMINSZ) 
    {
        size = DICTMINSZ ;
    }
    
    dictionary  * d = NULL;

    d = (dictionary*)calloc(1, sizeof *d);

    if (d)
    {
        d->size = size ;
        d->val  = (char**) calloc(size, sizeof *d->val);
        d->key  = (char**) calloc(size, sizeof *d->key);
        d->hash = (unsigned*) calloc(size, sizeof *d->hash);
    }
    return d ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Delete a dictionary object
    @param    d   dictionary object to deallocate.
    @return   void

    Deallocate a dictionary object and all memory associated to it.
 */
/*--------------------------------------------------------------------------*/
void dictionary_del(dictionary * d)
{
    if (d == NULL) 
    {
        return ;
    }

    ssize_t  i = 0;
    
    for (i=0 ; i<d->size ; i++)
    {
        if (d->key[i]!=NULL)
        {
#ifndef TEST_MEM_INFO
            free(d->key[i]);
#else
			MemoryFree(d->key[i]);
#endif
            d->key[i] = NULL;
        }
        if (d->val[i]!=NULL)
        {
#ifndef TEST_MEM_INFO
            free(d->val[i]);
#else
			MemoryFree(d->val[i]);
#endif
            d->val[i] = NULL;
        }
    }
    
#ifndef TEST_MEM_INFO
    free(d->val); 
#else
	MemoryFree(d->val);
#endif
	d->val = NULL;

#ifndef TEST_MEM_INFO
    free(d->key); 
#else
	MemoryFree(d->key);
#endif
	d->key = NULL;

#ifndef TEST_MEM_INFO
    free(d->hash);
#else
	MemoryFree(d->hash);
#endif
	d->hash = NULL;

#ifndef TEST_MEM_INFO
    free(d);      
#else
	MemoryFree(d);
#endif
	d = NULL;

    return;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Get a value from a dictionary.
    @param    d       dictionary object to search.
    @param    key     Key to look for in the dictionary.
    @param    def     Default value to return if key not found.
    @return   1 pointer to internally allocated character string.

    This function locates a key in a dictionary and returns a pointer to its
    value, or the passed 'def' pointer if no such key can be found in
    dictionary. The returned character pointer points to data internal to the
    dictionary object, you should not try to free it or modify it.
 */
/*--------------------------------------------------------------------------*/
const char * dictionary_get(const dictionary * d, const char * key, const char * def)
{
    unsigned    hash ;
    ssize_t      i = 0;

    hash = dictionary_hash(key);
    for (i=0 ; i<d->size ; i++)
    {
        if (d->key[i]==NULL)
            continue ;
        /* Compare hash */
        if (hash==d->hash[i])
        {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(key, d->key[i]))
            {
                return d->val[i] ;
            }
        }
    }
    return def ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Set a value in a dictionary.
    @param    d       dictionary object to modify.
    @param    key     Key to modify or add.
    @param    val     Value to add.
    @return   int     0 if Ok, anything else otherwise

    If the given key is found in the dictionary, the associated value is
    replaced by the provided one. If the key cannot be found in the
    dictionary, it is added to it.

    It is Ok to provide a NULL value for val, but NULL values for the dictionary
    or the key are considered as errors: the function will return immediately
    in such a case.

    Notice that if you dictionary_set a variable to NULL, a call to
    dictionary_get will return a NULL value: the variable will be found, and
    its value (NULL) is returned. In other words, setting the variable
    content to NULL is equivalent to deleting the variable from the
    dictionary. It is not possible (in this implementation) to have a key in
    the dictionary without value.

    This function returns non-zero in case of failure.
 */
/*--------------------------------------------------------------------------*/
int dictionary_set(dictionary * d, const char * key, const char * val)
{
    if (d==NULL || key==NULL) 
    {
        return -1 ;
    }

    ssize_t  i = 0;
    unsigned hash ;

    /* Compute hash for this key */
    hash = dictionary_hash(key) ;
    /* Find if value is already in dictionary */
    if (d->n>0)
    {
        for (i=0 ; i<d->size ; i++)
        {
            if (d->key[i]==NULL)
                continue ;
            /* Same hash value */
            if (hash==d->hash[i]) 
            {
                /* Same key */
                if (!strcmp(key, d->key[i]))
                {
                    /* Found a value: modify and return */
                    if (d->val[i]!=NULL)
                    {
#ifndef TEST_MEM_INFO
                        free(d->val[i]);
#else
						MemoryFree(d->val[i]);
#endif
                    }
                    d->val[i] = (val ? xstrdup(val) : NULL);
                    /* Value has been modified: return */
                    return 0 ;
                }
            }
        }
    }
    /* Add a new value */
    /* See if dictionary needs to grow */
    if (d->n==d->size)
    {
        /* Reached maximum size: reallocate dictionary */
        if (dictionary_grow(d) != 0)
            return -1;
    }

    /* Insert key in the first empty slot. Start at d->n and wrap at
       d->size. Because d->n < d->size this will necessarily
       terminate. */
    for (i=d->n ; d->key[i] ; )
    {
        if(++i == d->size) i = 0;
    }
    /* Copy key */
    d->key[i]  = xstrdup(key);
    d->val[i]  = (val ? xstrdup(val) : NULL) ;
    d->hash[i] = hash;
    d->n ++ ;
    return 0 ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Delete a key in a dictionary
    @param    d       dictionary object to modify.
    @param    key     Key to remove.
    @return   void

    This function deletes a key in a dictionary. Nothing is done if the
    key cannot be found.
 */
/*--------------------------------------------------------------------------*/
void dictionary_unset(dictionary * d, const char * key)
{
    if (key == NULL || d == NULL)
    {
        return;
    }

    unsigned    hash ;
    ssize_t      i = 0;

    hash = dictionary_hash(key);
    for (i=0 ; i<d->size ; i++)
    {
        if (d->key[i]==NULL)
            continue ;
        /* Compare hash */
        if (hash==d->hash[i])
        {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(key, d->key[i]))
            {
                /* Found key */
                break ;
            }
        }
    }
    
    /* Key not found */
    if (i>=d->size)
        return;

#ifndef TEST_MEM_INFO
    free(d->key[i]);
#else
	MemoryFree(d->key[i]);
#endif
    d->key[i] = NULL;
    if (d->val[i]!=NULL)
    {
#ifndef TEST_MEM_INFO
        free(d->val[i]);
#else
		MemoryFree(d->val[i]);
#endif
        d->val[i] = NULL ;
    }
    d->hash[i] = 0 ;
    d->n -- ;
    return ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    Dump a dictionary to an opened file pointer.
    @param    d   Dictionary to dump
    @param    f   Opened file pointer.
    @return   void

    Dumps a dictionary onto an opened file pointer. Key pairs are printed out
    as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
    output file pointers.
 */
/*--------------------------------------------------------------------------*/
void dictionary_dump(const dictionary * d, FILE * out)
{
    if (d==NULL || out==NULL) 
    {
        return;
    }

    ssize_t  i = 0;
    
    if (d->n < 1)
    {
        fprintf(out, "empty dictionary\n");
        return;
    }
    
    for (i=0 ; i<d->size ; i++)
    {
        if (d->key[i])
        {
            fprintf(out, "%20s\t[%s]\n", d->key[i], d->val[i] ? d->val[i] : "UNDEF");
        }
    }
    
    return;
}
