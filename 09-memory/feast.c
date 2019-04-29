#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** Number of dishes that fit on the table (TABLE_SIZE) */
#define TABLE_SPACE 5

/** dish id and uses */
typedef struct {
    int dish;
    int uses;
} platter;

/** array of platters */
platter* platters;

/** next int from stdin, -1 on eof */
int get_next_dish_order();

/** run this after each step of your algorithm (each dish request) */
void print_state();

/*  Serve dishes to the dwarves based on their requests. The 
    replacement strategy is LEAST FREQUENTLY USED (LFU).

    This function returns an integer count of the number of 
    times Bilbo has had to fetch a dish to put on one of the platters.
    Remember to call print_state() after each time a dish request
    is dealt with! 
*/
int serve_dishes()
{
    int fetches=0, order_id;

    /** in one loop, find the least used and determine if order_id exists */
    while ((order_id = get_next_dish_order()) != -1) {
        platter *exists = NULL;
        platter *least = &platters[0];

        for (int i=0; i<TABLE_SPACE; ++i) {
            if (platters[i].dish == order_id) {
                exists = &platters[i];
                break;
            }

            if (least->uses > platters[i].uses)
                least = &platters[i];    
        }

        if (exists) {
            ++exists->uses;
        } else {
            ++fetches;
            least->dish = order_id;
            least->uses = 0;
        }

        print_state();
    } 

    return fetches;
}

int main( int argc, char** argv )
{
    /** each platter starts with -1 uses and id */
    platters = malloc(sizeof(platter) * TABLE_SPACE);
    for (int i=0; i<TABLE_SPACE; ++i) {
        platters[i].uses = -1;
        platters[i].dish = -1;
    }

    /** serve dishes loops until eof from stdin */
    int fetches = serve_dishes(); 
    printf("Bilbo had to fetch dishes %d times using LFU.\n", fetches );
    
    free(platters);
    return 0;
}


/****** Do not change anything below this line -- Internal Functionality Only ********/ 
int get_next_dish_order( )
{
    if (feof(stdin)) {
        return -1;
    }
    int next;
    scanf("%d", &next);
    return next;
}

void print_state()
{
    printf("("); 
    for ( int i = 0; i < TABLE_SPACE - 1; i++ ) { 
        if (platters[i].uses == -1) {
            printf("- [], ");
        } else {
            printf( "%d [%d], ", platters[i].dish, platters[i].uses );
        }
    }
    if ( platters[TABLE_SPACE - 1].uses == -1 ) {
        printf("- [])\n");
    } else {
        printf("%d [%d])\n", platters[TABLE_SPACE - 1].dish, platters[TABLE_SPACE -1].uses );
    }
}
