#ifndef MEMPOOL_H
#define MEMPOOL_H

// Step A: UTXO Existence: Check if the input exists in the global utxo_set.
// Step B: Mempool Check: Check if that specific (tx_id, index) is already in the spent_utxos set. If it is, someone else is already trying to spend it—this is where you reject the double-spend.
// Step C: Mark as Reserved: If the transaction is valid, add all its inputs to the spent_utxos set
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <map>

using namespace std;

struct Input {
    string prev_tx_id;
    int index;
};

struct Output {
    double amount;
    string recipient;
};

struct Transaction {
    string tx_id;
    double fee;
    vector<Input> inputs;
    vector<Output> outputs;

};

// Simplified UTXO structure for the manager
struct UTXO {
    double amount;
    string owner;
};

class Mempool {
public:
    priority_queue<pair<int,Transaction> > transactions; 
    set<pair<string, int>> spent_utxos;
    int max_size;

    Mempool(int size = 50) {
        max_size = size;
    }

    pair<bool, string> add_transaction(Transaction tx, map<pair<string, int>, UTXO>& utxo_set) {
        // 1. Check Mempool Size
        if (transactions.size() >= max_size) {
            return {false, "Mempool is full"};  
        }

        double total_input = 0;
        double total_output = 0;

        // 2. Check UTXO Existence and calculate total input
        for (int i = 0; i < tx.inputs.size(); i++) {
            pair<string, int> utxo_key = {tx.inputs[i].prev_tx_id, tx.inputs[i].index};

            // Check if it exists in the global set
            if (utxo_set.find(utxo_key) == utxo_set.end()) {
                return {false, "Input UTXO does not exist"};  
            }

            // Check if it is already "reserved" in the mempool
            if (spent_utxos.count(utxo_key)) {
                return {false, "UTXO already spent in mempool"};  
            }

            total_input = total_input + utxo_set[utxo_key].amount;
        }

        // 3. Sum up the outputs
        for (int i = 0; i < tx.outputs.size(); i++) {
            if (tx.outputs[i].amount < 0) {
                return {false, "Negative output amount"};  
            }
            total_output = total_output + tx.outputs[i].amount;
        }

        // 4. Validate Amount and Calculate Fee
        if (total_output > total_input) {
            return {false, "Insufficient funds: Output > Input"};  
        }

        int fee = total_input - total_output; 

        // 5. Commit to Mempool
        transactions.push(make_pair(fee, tx));
        for (int i = 0; i < tx.inputs.size(); i++) {
            pair<string, int> utxo_key = {tx.inputs[i].prev_tx_id, tx.inputs[i].index};
            spent_utxos.insert(utxo_key);  
        }

        return {true, "Transaction accepted"};
    }

    vector<Transaction> get_top_transactions(int n) {
        vector<Transaction> top_txs;
        // Use a temporary copy to not empty the actual mempool, actual mempool would be emptied when transaction is mines
        priority_queue<pair<int,Transaction> > temp = transactions; 

        for (int i = 0; i < n; i++) {
            if (temp.empty()) break;
            pair<int, Transaction> topTransaction = temp.top();
            temp.pop();
            top_txs.push_back(topTransaction.second);  
            temp.pop();
        }
        return top_txs;
    }
};
