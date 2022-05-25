#include <stdio.h>
#include <inttypes.h>
#include "vector.h"

int main(int argc, char const *argv[])
{
    int *vector = new_vector(sizeof(int), 6, NULL);

    printf("Length: %" PRIu64 ", Capacity: %" PRIu64 ", Size: %" PRIu64 "\n",
           vlength(vector), vcapacity(vector), vsize(vector));

    for (int i = 1; i <= 10; i++)
    {
        vector = vpush(vector, &i);
    }

    printf("Length: %" PRIu64 ", Capacity: %" PRIu64 ", Size: %" PRIu64 "\n",
           vlength(vector), vcapacity(vector), vsize(vector));

    int x = 15;
    vector = vinsert(vector, &x, 0);
    vector = vdelete(vector, 0);

    while (vlength(vector))
    {
        printf("%d ", *((int *)vpop(vector)));
    }
    putchar('\n');

    printf("Length: %" PRIu64 ", Capacity: %" PRIu64 ", Size: %" PRIu64 "\n",
           vlength(vector), vcapacity(vector), vsize(vector));

    vector = v_shrink_to_fit(vector);

    printf("Length: %" PRIu64 ", Capacity: %" PRIu64 ", Size: %" PRIu64 "\n",
           vlength(vector), vcapacity(vector), vsize(vector));

    vector = v_set_capacity(vector, 100);

    printf("Length: %" PRIu64 ", Capacity: %" PRIu64 ", Size: %" PRIu64 "\n",
           vlength(vector), vcapacity(vector), vsize(vector));

    delete_vector(vector);
    return 0;
}
