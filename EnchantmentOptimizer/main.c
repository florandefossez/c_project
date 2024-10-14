#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct ench {
    unsigned char book_id;
    unsigned char ench_id;
    unsigned char level;
    unsigned char repair_cost;
} ench_t;

typedef struct node {
    unsigned char n_ench;
    unsigned char f_cost;
    unsigned char h_cost;
    unsigned char flags;  // is_close(1) / is_merge12(1) / free(2) / cost(4)
    ench_t enchs[16]; // ie 64 bytes
    struct node* parent;
    struct node* next;
} node_t;


ench_t input_enchs[16];
node_t* opened_nodes;
node_t* final;

void set_input(int id, unsigned char book_id, unsigned char ench_id, unsigned char level, unsigned char repair_cost) {
    input_enchs[id].book_id = book_id;
    input_enchs[id].ench_id = ench_id;
    input_enchs[id].level = level;
    input_enchs[id].repair_cost = repair_cost;
}

void clean() {
    node_t* node = opened_nodes;
    node_t* next;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }
    final = NULL;
    memset(input_enchs, 0, 16*sizeof(ench_t));
}

unsigned char* get_result() {
    static unsigned char result[512];
    if (!final) {
        return NULL;
    }
    result[0] = final->n_ench;
    result[1] = final->flags;
    memcpy(result + 2, final->enchs, 16 * sizeof(ench_t));
    final = final->parent;
    return result;
}

unsigned char max_ench(unsigned char ench_id) {
    switch (ench_id) {
    case 0: // mending
        return 1;
    case 1: // unbreaking
        return 3;
    case 2: // sharpness
        return 5;
    case 3: // fortune
        return 3;
    case 4: // efficiency
        return 5;
    case 5: // protection
        return 4;
    case 6: // fire_protection
        return 4;
    case 7: // feather_falling
        return 4;
    case 8: // blast_protection
        return 4;
    case 9: // projectile_protection
        return 4;
    case 10: // thorns
        return 3;
    case 11: // respiration
        return 3;
    case 12: // depth_strider
        return 3;
    case 13: // aqua_affinity
        return 1;
    case 14: // smite
        return 5;
    case 15: // bane_of_arthropods
        return 5;
    case 16: // knockback
        return 2;
    case 17: // fire_aspect
        return 2;
    case 18: // looting
        return 3;
    case 19: // silk_touch
        return 1;
    case 20: // power
        return 5;
    case 21: // punch
        return 2;
    case 22: // flame
        return 1;
    case 23: // infinity
        return 1;
    case 24: // luck_of_the_sea
        return 3;
    case 25: // lure
        return 3;
    case 26: // frost_walker
        return 2;
    case 27: // curse_of_binding
        return 1;
    case 28: // curse_of_vanishing
        return 1;
    case 29: // impaling
        return 5;
    case 30: // riptide
        return 3;
    case 31: // loyalty
        return 3;
    case 32: // channeling
        return 1;
    case 33: // multishot
        return 1;
    case 34: // piercing
        return 4;
    case 35: // quick_charge
        return 3;
    case 36: // soul_speed
        return 3;
    case 37: // swift_sneak
        return 3;
    case 38: // sweeping_edge
        return 3;
    case 39: // density
        return 5;
    case 40: // wind_burst
        return 3;
    case 41: // breach
        return 4;
    }
    return 0;
}

unsigned char rarity(unsigned char ench_id) {
    switch (ench_id) {
    case 0: // mending
        return 2;
    case 1: // unbreaking
        return 1;
    case 2: // sharpness
        return 1;
    case 3: // fortune
        return 2;
    case 4: // efficiency
        return 1;
    case 5: // protection
        return 1;
    case 6: // fire_protection
        return 1;
    case 7: // feather_falling
        return 1;
    case 8: // blast_protection
        return 2;
    case 9: // projectile_protection
        return 1;
    case 10: // thorns
        return 4;
    case 11: // respiration
        return 2;
    case 12: // depth_strider
        return 2;
    case 13: // aqua_affinity
        return 2;
    case 14: // smite
        return 1;
    case 15: // bane_of_arthropods
        return 1;
    case 16: // knockback
        return 2;
    case 17: // fire_aspect
        return 2;
    case 18: // looting
        return 2;
    case 19: // silk_touch
        return 4;
    case 20: // power
        return 1;
    case 21: // punch
        return 2;
    case 22: // flame
        return 2;
    case 23: // infinity
        return 4;
    case 24: // luck_of_the_sea
        return 4;
    case 25: // lure
        return 2;
    case 26: // frost_walker
        return 2;
    case 27: // curse_of_binding
        return 4;
    case 28: // curse_of_vanishing
        return 4;
    case 29: // impaling
        return 2;
    case 30: // riptide
        return 2;
    case 31: // loyalty
        return 1;
    case 32: // channeling
        return 4;
    case 33: // multishot
        return 2;
    case 34: // piercing
        return 1;
    case 35: // quick_charge
        return 1;
    case 36: // soul_speed
        return 4;
    case 37: // swift_sneak
        return 4;
    case 38: // sweeping_edge
        return 2;
    case 39: // density
        return 1;
    case 40: // wind_burst
        return 2;
    case 41: // breach
        return 2;
    }
    return 1;
}

void compute(int initial_n_ench, int book0_is_item) {

    node_t first_node;
    first_node.next = NULL;
    first_node.flags = 0;
    first_node.n_ench = initial_n_ench;
    memcpy(first_node.enchs, input_enchs, initial_n_ench * sizeof(ench_t));

    opened_nodes = &first_node;
    while (1) {

        // Best node
        node_t* current = NULL;
        node_t* node = opened_nodes;
        unsigned char best_cost = 0xff;
        while (node != NULL) {
            if (!(node->flags >> 7) && node->f_cost + node->h_cost < best_cost) {
                best_cost = node->f_cost + node->h_cost;
                current = node;
            }
            node = node->next;
        }
        if (current == NULL) {
            return;
        }

        current->flags |= 128;

        // determine the number of books and the book_ids
        unsigned char book_ids[16];
        memset(book_ids, 0, 16);
        int n_book = 0;
        for (int i=0; i<current->n_ench; i++) {
            bool found = false;
            for (int j=0; j<n_book; j++) {
                if (book_ids[j] == current->enchs[i].book_id) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                book_ids[n_book] = current->enchs[i].book_id;
                n_book++;
            }
        }

        // if final node we're done
        if (n_book == 1) {
            final = current;
            return;
        }

        // explore the neighbours
        for (int i=0; i<n_book; i++) {
            for (int j=i+1; j<n_book; j++) {
                if (book_ids[i] == book_ids[j]) {continue;}
                ench_t enchs[16] = {0};
                unsigned char cost12 = 0;
                unsigned char cost21 = 0;
                unsigned char is_merge12 = 1;
                unsigned char h_cost = 0;
                unsigned char repair_cost = 0;
                unsigned char n_ench = 0;
                unsigned char book_id1;
                unsigned char book_id2;
                int index1 = 0; // index of enchs of book 1
                int index2 = 0; // index of enchs of book 2
                if (book_ids[i] < book_ids[j]) {
                    book_id1 = book_ids[i];
                    book_id2 = book_ids[j];
                } else {
                    book_id1 = book_ids[j];
                    book_id2 = book_ids[i];
                }


                while (current->enchs[index1].book_id < book_id1) { // stack first books not implicated in the merge + set index1
                    enchs[n_ench] = current->enchs[index1];
                    n_ench++;
                    index1++;
                }

                if (current->enchs[index1].book_id != book_id1) {
                    return;
                }
                index2 = index1;

                while (current->enchs[index2].book_id < book_id2) { // set index2
                    index2++;
                }
                if (current->enchs[index2].book_id != book_id2) {
                    return;
                }

                // set costs
                cost12 = current->enchs[index1].repair_cost + current->enchs[index2].repair_cost;
                cost21 = cost12;

                // set new repair_cost
                if (current->enchs[index1].repair_cost < current->enchs[index2].repair_cost) {
                    repair_cost = 2*current->enchs[index2].repair_cost + 1;
                } else {
                    repair_cost = 2*current->enchs[index1].repair_cost + 1;
                }

                // merge ench
                while (current->enchs[index1].book_id == book_id1 && current->enchs[index2].book_id == book_id2) {
                    if (current->enchs[index1].ench_id < current->enchs[index2].ench_id) {
                        enchs[n_ench] = current->enchs[index1];
                        enchs[n_ench].repair_cost = repair_cost; 
                        cost21 += enchs[n_ench].level * rarity(enchs[n_ench].ench_id);
                        n_ench++;
                        index1++;
                    } else if (current->enchs[index1].ench_id > current->enchs[index2].ench_id) {
                        enchs[n_ench] = current->enchs[index2];
                        enchs[n_ench].book_id = book_id1;
                        enchs[n_ench].repair_cost = repair_cost;
                        cost12 += enchs[n_ench].level * rarity(enchs[n_ench].ench_id);
                        n_ench++;
                        index2++;
                    } else { // both books have this ench_id
                        enchs[n_ench] = current->enchs[index1];
                        enchs[n_ench].repair_cost = repair_cost;
                        if (current->enchs[index1].level < current->enchs[index2].level) {
                            enchs[n_ench].level = current->enchs[index2].level;
                        } else if (current->enchs[index1].level > current->enchs[index2].level) {
                            enchs[n_ench].level = current->enchs[index1].level;
                        } else if (enchs[n_ench].level + 1 <= max_ench(enchs[n_ench].ench_id)) {
                            enchs[n_ench].level++;
                        }
                        cost12 += enchs[n_ench].level * rarity(enchs[n_ench].ench_id);
                        cost21 += enchs[n_ench].level * rarity(enchs[n_ench].ench_id);
                        n_ench++;
                        index1++;
                        index2++;
                    }
                }
                // use index1 alone from now and stack the remainings ench from book1
                while (current->enchs[index1].book_id == book_id1) {
                    enchs[n_ench] = current->enchs[index1];
                    enchs[n_ench].repair_cost = repair_cost;
                    cost21 += current->enchs[index1].level * rarity(current->enchs[index1].ench_id);
                    index1++;
                    n_ench++;
                }
                // stack the remainings ench from book2
                while (current->enchs[index2].book_id == book_id2) {
                    enchs[n_ench] = current->enchs[index2];
                    enchs[n_ench].repair_cost = repair_cost;
                    enchs[n_ench].book_id = book_id1;
                    cost12 += current->enchs[index2].level * rarity(current->enchs[index2].ench_id);
                    index2++;
                    n_ench++;
                }
                
                while (index1 < current->n_ench) {
                    if (current->enchs[index1].book_id == book_id2) {index1++; continue;}
                    enchs[n_ench] = current->enchs[index1];
                    index1++;
                    n_ench++;
                }

                // determine best combination order, keep cost12 as the best cost
                if ( !(book0_is_item && book_ids[i] == 0) && cost12 > cost21) {
                    cost12 = cost21;
                    is_merge12 = 0;
                }


                // now we have the neighbour, check if it has aleady been explored
                node = opened_nodes;
                bool found = false;
                while (node) {
                    if (!memcmp(node->enchs, enchs, 16*sizeof(ench_t))) {
                        found = true;
                        break;
                    }
                    node = node->next;
                }

                // skip to next if the neighbour is already closed or has a lower cost
                if (found && (node->flags >> 7)) {continue;}
                if (found && node->f_cost + node->h_cost < current->f_cost + cost12 + h_cost) {continue;}

                // built the node is needed
                if (!found) {
                    node_t* neighbour = (node_t*) malloc(sizeof(node_t));
                    memcpy(neighbour->enchs, enchs, 16*sizeof(ench_t));
                    neighbour->n_ench = n_ench;
                    neighbour->h_cost = h_cost;
                    neighbour->flags = 0;
                    neighbour->next = NULL;
                    neighbour->parent = current;
                    node = opened_nodes;
                    while (node->next) {
                        node = node->next;
                    }
                    node->next = neighbour;
                    node = neighbour;
                }

                node->f_cost = current->f_cost + cost12;
                node->flags = (is_merge12 << 6) + cost12;
                node->parent = current;

            }
        }
    }
    
    return;
}
