#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <map>
#include "transaction.h"

using namespace std;

class mempool {
private:
    priority_queue<pair<double, transaction>> transactions; 
    set<pair<string, int>> spent_utxos;
    int max_size;

public:
    mempool(int size = 50) {
        max_size = size;
    }

    pair<bool, string> add_transaction(const transaction& tx, double fee) {
        if (transactions.size() >= (size_t)max_size) {
            return {false, "Mempool is full"};
        }

        transactions.push(make_pair(fee, tx));

        for (const tx_input& in : tx.getinputs()) {
            pair<string, int> key = {in.getprev_tx_id(), in.getoutput_index()};
            spent_utxos.insert(key);
        }

        return {true, "Transaction added to mempool"};
    }

    bool is_spent(pair<string, int> key) {
        return spent_utxos.count(key) > 0;
    }

    vector<transaction> get_top_transactions(int n) {
        vector<transaction> top_txs;
        priority_queue<pair<double, transaction>> temp = transactions;

        for (int i = 0; i < n && !temp.empty(); i++) {
            top_txs.push_back(temp.top().second);
            temp.pop();
        }
        return top_txs;
    }

    void clear() {
        while (!transactions.empty()) {
            transactions.pop();
        }
        spent_utxos.clear();
    }
    
    int get_current_size() {
        return transactions.size();
    }
};

#endif