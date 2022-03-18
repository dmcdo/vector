#include "vector.h"

#define MEM(vector) ((void *)((vector) - sizeof(struct vmeta)))
#define VECTOR(mem) ((void *)((mem) + sizeof(struct vmeta)))
#define META(mem) ((struct vmeta *)((mem)))

#define ELEM(vector, meta, i) ((vector) + ((meta)->elem_size * (i)))
#define SIZE(capacity, elem_size) (sizeof(struct vmeta) + ((capacity) * (elem_size)))

#define UNSIGNED_CEIL(x) (((x) - ((size_t)(x)) == 0) ? ((size_t)(x)) : (((size_t)(x)) + 1))

#define MEMSWAP(dest, src, bytes) (_memswap(((unsigned char *)(dest)), ((unsigned char *)(src)), (bytes)))
void _memswap(unsigned char *dest, unsigned char *src, size_t bytes) {
    unsigned char *end = src + bytes;
    for (; src < end; dest++, src++) {
        *src ^= *dest;
        *dest ^= *src;
        *src ^= *dest;
    }
}

struct vmeta {
    size_t elem_size;
    size_t capacity;
    size_t len;
    double rate;
    void (*copyfunc)(void *, void *);
};

void *new_vector(size_t elem_size, size_t initial_capacity, void (*copyfunc)(void *, void *)) {
    if (initial_capacity == 0)
        initial_capacity = 1;

    void *mem = malloc(SIZE(initial_capacity, elem_size));

    struct vmeta *meta = META(mem);
    void *vector = VECTOR(mem);

    meta->elem_size = elem_size;
    meta->capacity = initial_capacity;
    meta->len = 0;
    meta->rate = V_DEFAULT_RATE;
    meta->copyfunc = copyfunc;

    return vector;
}

void delete_vector(void *vector) {
    free(MEM(vector));
}

size_t vlength(void *vector) {
    return META(MEM(vector))->len;
}

size_t vcapacity(void *vector) {
    return META(MEM(vector))->capacity;
}

size_t vsize(void *vector) {
    struct vmeta *meta = META(MEM(vector));

    return SIZE(meta->capacity, meta->elem_size);
}

void *v_set_capacity(void *vector, size_t capacity) {
    if (capacity == 0)
        capacity = 1;

    void *mem = MEM(vector);
    struct vmeta *meta = META(mem);

    if (capacity < meta->len)
        return NULL;
    
    mem = realloc(mem, SIZE(capacity, meta->elem_size));

    if (mem == NULL)
        return NULL;
    
    META(mem)->capacity = capacity;
    return VECTOR(mem);
}

void *v_shrink_to_fit(void *vector) {
    return v_set_capacity(vector, META(MEM(vector))->len);
}

void *v_set_rate(void *vector, double rate) {
    if (rate > 1) {
        META(MEM(vector))->rate = rate;
        return vector;
    }
    
    return NULL;
}

void *vpush(void *vector, void *elem) {
    return vinsert(vector, elem, META(MEM(vector))->len);
}

void *vpop(void *vector) {
    struct vmeta *meta = META(MEM(vector));

    return ELEM(vector, meta, --meta->len);
}

void *vinsert(void *vector, void *elem, size_t idx) {
    void *mem = MEM(vector);
    struct vmeta *meta = META(mem);

    if (idx > meta->len)
        return NULL;

    if (meta->len >= meta->capacity) {
        vector = v_set_capacity(vector, UNSIGNED_CEIL(meta->rate * meta->capacity));

        if (vector == NULL)
            return NULL;
        
        mem = MEM(vector);
        meta = META(mem);
    }

    void *end = ELEM(vector, meta, meta->len);

    if (meta->copyfunc) {
        meta->copyfunc(end, elem);
    } else {
        memcpy(end, elem, meta->elem_size);
    }

    for (size_t i = idx; i < meta->len; i++) {
        MEMSWAP(end, ELEM(vector, meta, i), meta->elem_size);
    }

    meta->len++;
    return vector;
}

void *vdelete(void *vector, size_t idx) {
    struct vmeta *meta = META(MEM(vector));

    if (idx >= meta->len)
        return NULL;

    meta->len--;
    for (size_t i = idx; i < meta->len; i++) {
        memcpy(ELEM(vector, meta, i), ELEM(vector, meta, i + 1), meta->elem_size);
    }

    return vector;
}