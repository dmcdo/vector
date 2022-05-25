#include "vector.h"

#define VECTORPTR char *
#define METADATAPTR struct vmeta *
#define OBJECTPTR char *

#define FROM_VOID_VECTOR(ptr) ((VECTORPTR)ptr)
#define FROM_VOID_OBJECT(ptr) ((OBJECTPTR)(FROM_VOID_VECTOR(ptr) - sizeof(struct vmeta)))
#define FROM_VOID_METADATA(ptr) ((METADATAPTR)FROM_VOID_OBJECT(ptr))
#define FROM_OBJECT_METADATA(ptr) ((METADATAPTR)ptr)
#define FROM_OBJECT_VECTOR(ptr) ((VECTORPTR)(ptr + sizeof(struct vmeta)))

#define ELEM(vector, meta, i) ((vector) + ((meta)->elem_size * (i)))
#define SIZE(capacity, elem_size) (sizeof(struct vmeta) + ((capacity) * (elem_size)))

#define UNSIGNED_CEIL(x) (((x) - ((size_t)(x)) == 0) ? ((size_t)(x)) : (((size_t)(x)) + 1))

#define MEMSWAP(dest, src, bytes) (_memswap(((unsigned char *)(dest)), ((unsigned char *)(src)), (bytes)))
void _memswap(unsigned char *dest, unsigned char *src, size_t bytes)
{
    unsigned char *end = src + bytes;
    for (; src < end; dest++, src++)
    {
        *src ^= *dest;
        *dest ^= *src;
        *src ^= *dest;
    }
}

struct vmeta
{
    size_t elem_size;
    size_t capacity;
    size_t len;
    double rate;
    void (*copyfunc)(void *, void *);
};

void *new_vector(size_t elem_size, size_t initial_capacity, void (*copyfunc)(void *, void *))
{
    if (initial_capacity == 0)
        initial_capacity = 1;

    OBJECTPTR obj = (OBJECTPTR)malloc(SIZE(initial_capacity, elem_size));

    if (obj == NULL)
        return NULL;

    METADATAPTR meta = FROM_OBJECT_METADATA(obj);
    VECTORPTR vector = FROM_OBJECT_VECTOR(obj);

    meta->elem_size = elem_size;
    meta->capacity = initial_capacity;
    meta->len = 0;
    meta->rate = V_DEFAULT_RATE;
    meta->copyfunc = copyfunc;

    return (void *)vector;
}

void delete_vector(void *ptr)
{
    free(FROM_VOID_OBJECT(ptr));
}

size_t vlength(void *ptr)
{
    return FROM_VOID_METADATA(ptr)->len;
}

size_t vcapacity(void *ptr)
{
    return FROM_VOID_METADATA(ptr)->capacity;
}

size_t vsize(void *ptr)
{
    METADATAPTR meta = FROM_VOID_METADATA(ptr);
    return SIZE(meta->capacity, meta->elem_size);
}

void *v_set_capacity(void *ptr, size_t capacity)
{
    if (capacity == 0)
        capacity = 1;

    OBJECTPTR obj = FROM_VOID_OBJECT(ptr);
    METADATAPTR meta = FROM_OBJECT_METADATA(obj);

    if (capacity < meta->len)
        return NULL;

    obj = (OBJECTPTR)realloc(obj, SIZE(capacity, meta->elem_size));

    if (obj == NULL)
        return NULL;

    FROM_OBJECT_METADATA(obj)->capacity = capacity;
    return (void *)FROM_OBJECT_VECTOR(obj);
}

void *v_shrink_to_fit(void *ptr)
{
    return v_set_capacity(ptr, FROM_VOID_METADATA(ptr)->len);
}

void *v_set_rate(void *ptr, double rate)
{
    if (rate > 1)
    {
        FROM_VOID_METADATA(ptr)->rate = rate;
        return ptr;
    }

    return NULL;
}

void *vpush(void *ptr, void *elem)
{
    return vinsert(ptr, elem, FROM_VOID_METADATA(ptr)->len);
}

void *vpop(void *ptr)
{
    METADATAPTR meta = FROM_VOID_METADATA(ptr);
    VECTORPTR vec = FROM_VOID_VECTOR(ptr);
    return ELEM(vec, meta, --meta->len);
}

void *vinsert(void *ptr, void *elem, size_t idx)
{
    OBJECTPTR obj = FROM_VOID_OBJECT(ptr);
    METADATAPTR meta = FROM_OBJECT_METADATA(obj);
    VECTORPTR vec = FROM_VOID_VECTOR(ptr);

    if (idx > meta->len)
        return NULL;

    if (meta->len >= meta->capacity)
    {
        ptr = v_set_capacity(ptr, UNSIGNED_CEIL(meta->rate * meta->capacity));

        if (ptr == NULL)
            return NULL;

        vec = FROM_VOID_VECTOR(ptr);
        obj = FROM_VOID_OBJECT(ptr);
        meta = FROM_OBJECT_METADATA(obj);
    }

    void *end = ELEM(vec, meta, meta->len);

    if (meta->copyfunc)
    {
        meta->copyfunc(end, elem);
    }
    else
    {
        memcpy(end, elem, meta->elem_size);
    }

    for (size_t i = idx; i < meta->len; i++)
    {
        MEMSWAP(end, ELEM(vec, meta, i), meta->elem_size);
    }

    meta->len++;
    return ptr;
}

void *vdelete(void *ptr, size_t idx)
{
    METADATAPTR meta = FROM_VOID_METADATA(ptr);
    VECTORPTR vec = FROM_VOID_VECTOR(ptr);

    if (idx >= meta->len)
        return NULL;

    meta->len--;
    for (size_t i = idx; i < meta->len; i++)
    {
        memcpy(ELEM(vec, meta, i), ELEM(vec, meta, i + 1), meta->elem_size);
    }

    return ptr;
}