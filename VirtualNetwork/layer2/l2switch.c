#include <stdio.h>
#include "layer2.h"
#include <stdlib.h>
#include "../gluethread/glthread.h"
/*L2 Switch Owns Mac Table*/

typedef struct mac_table_{
    glthread_t mac_entries;
} mac_table_t;

typedef struct mac_table_entry_{
    mac_add_t mac;
    char oif_name[IF_NAME_SIZE];
    glthread_t mac_entry_glue;
} mac_table_entry_t;
GLTHREAD_TO_STRUCT(mac_entry_glue_to_mac_entry, mac_table_entry_t, mac_entry_glue);



#define IS_MAC_TABLE_ENTRY_EQUAL(mac_entry_1, mac_entry_2)   \
    (strncmp(mac_entry_1->mac.mac, mac_entry_2->mac.mac, sizeof(mac_add_t)) == 0 && \
            strncmp(mac_entry_1->oif_name, mac_entry_2->oif_name, IF_NAME_SIZE) == 0)


void init_mac_table(mac_table_t **mac_table){
    *mac_table = calloc(1, sizeof(mac_table_t));
    init_glthread(&((*mac_table)->mac_entries));
}

mac_table_entry_t* mac_table_lookup(mac_table_t *mac_table, char *mac) {
    glthread_t *curr;
    mac_table_entry_t *mac_table_entry;
    ITERATE_GLTHREAD_BEGIN(&mac_table->mac_entries, curr){
        mac_table_entry = mac_entry_glue_to_mac_entry(curr);
        if(strncmp(mac_table_entry->mac.mac, mac, sizeof(mac_add_t)) == 0){
            return mac_table_entry;
        }
    } ITERATE_GLTHREAD_END(&mac_table->mac_entries, curr);
    return NULL;
}

void delete_mac_table_entry(mac_table_t *mac_table, char *mac) {
    mac_table_entry_t *mac_table_entry = mac_table_lookup(mac_table, mac);
    if(!mac_table_entry)
        return;
    remove_glthread(&mac_table_entry->mac_entry_glue);
    free(mac_table_entry);
}

bool mac_table_entry_add(mac_table_t *mac_table, mac_table_entry_t *mac_table_entry) {
    mac_table_entry_t *mac_table_entry_old = mac_table_lookup(
        mac_table,
        mac_table_entry->mac.mac
    );
    if(mac_table_entry_old && IS_MAC_TABLE_ENTRY_EQUAL(mac_table_entry_old, mac_table_entry)){
        return false;
    }
    if(mac_table_entry_old){
        delete_mac_table_entry(mac_table, mac_table_entry_old->mac.mac);
    }
    init_glthread(&mac_table_entry->mac_entry_glue);
    glthread_add_next(&mac_table->mac_entries, &mac_table_entry->mac_entry_glue);
    return true;
}


void dump_mac_table(mac_table_t *mac_table) {
    glthread_t *curr;
    mac_table_entry_t *mac_table_entry;
    ITERATE_GLTHREAD_BEGIN(&mac_table->mac_entries, curr){
        mac_table_entry = mac_entry_glue_to_mac_entry(curr);
        printf("\tMAC : %u:%u:%u:%u:%u:%u   | Intf : %s\n", 
            mac_table_entry->mac.mac[0], 
            mac_table_entry->mac.mac[1],
            mac_table_entry->mac.mac[2],
            mac_table_entry->mac.mac[3], 
            mac_table_entry->mac.mac[4],
            mac_table_entry->mac.mac[5],
            mac_table_entry->oif_name);
    } ITERATE_GLTHREAD_END(&mac_table->mac_entries, curr);
}