#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define V_DEFAULT_RATE (1.5)

void *new_vector(size_t elem_size, size_t initial_capacity, void (*copyfunc)(void *, void *));

void delete_vector(void *vector);

size_t vlength(void *vector);

size_t vcapacity(void *vector);

size_t vsize(void *vector);

void *v_set_capacity(void *vector, size_t capacity);

void *v_shrink_to_fit(void *vector);

void *v_set_rate(void *vector, double rate);

void *vpush(void *vector, void *elem);

void *vpop(void *vector);

void *vinsert(void *vector, void *elem, size_t idx);

void *vdelete(void *vector, size_t idx);

#endif