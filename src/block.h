#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include "transaction.h"

using namespace std;

class block {
private:
    block* prevBlock;               
    string miner_address;           
    double total_fees;              
    vector<transaction> transactions; 

public:
    block(block* prev, string miner, double fees, vector<transaction> txs) {
        prevBlock = prev;
        miner_address = miner;
        total_fees = fees;
        transactions = txs;
    }

    block* get_prev_block() const { return prevBlock; }
    string get_miner() const { return miner_address; }
    double get_total_fees() const { return total_fees; }
    const vector<transaction>& get_transactions() const { return transactions; }
};

#endif