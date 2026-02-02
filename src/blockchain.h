#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include "block.h"

using namespace std;

class blockchain {
private:
    block* latest_block;
    int height;

public:
    blockchain() {
        latest_block = nullptr;
        height = 0;
    }

    void add_block(block* new_mined_block) {
        latest_block = new_mined_block;
        height++;
    }

    block* get_latest_block() const {
        return latest_block;
    }

    int get_height() const {
        return height;
    }

    void display_status() {
        cout << "--- Blockchain Ledger (Linked List) ---" << endl;
        block* current = latest_block;
        
        while (current != nullptr) {
            cout << "Miner: " << current->get_miner()
                 << " | Fees: " << current->get_total_fees() 
                 << " | TX Count: " << current->get_transactions().size() << endl;
            
            current = current->get_prev_block();
        }
    }
};

#endif