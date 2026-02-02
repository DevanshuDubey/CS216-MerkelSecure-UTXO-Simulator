#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "transaction.h"
#include "utxo_manager.h"
#include <vector>
#include <string>
#include <set>
#include <algorithm>

using namespace std;

class validator;

class mempool
{
private:
    vector<transaction> transactions;
    set<utxo_key> spent_utxos;
    int max_size;

public:
    mempool(int size = 50) : max_size(size) {}

    bool is_spent(const utxo_key &key) const
    {
        return spent_utxos.find(key) != spent_utxos.end();
    }

    bool is_full() const
    {
        return transactions.size() >= (size_t)max_size;
    }

    pair<bool, string> add_transaction(transaction tx, utxo_manager &um);

    vector<transaction> mine_top_transactions(int n)
    {
        sort(transactions.begin(), transactions.end(), [](const transaction &a, const transaction &b) {
            return a.fee > b.fee;
        });

        vector<transaction> mined_txs;
        int to_mine = min((int)transactions.size(), n);
        
        for (int i = 0; i < to_mine; i++) {
            mined_txs.push_back(transactions[i]);
            for (auto const &in : transactions[i].inputs) {
                spent_utxos.erase(utxo_key(in.prev_tx, in.index));
            }
        }
        
        transactions.erase(transactions.begin(), transactions.begin() + to_mine);
        return mined_txs;
    }

    vector<transaction> get_all_transactions()
    {
        vector<transaction> all_txs = transactions;
        sort(all_txs.begin(), all_txs.end(), [](const transaction &a, const transaction &b) {
            return a.fee > b.fee;
        });
        return all_txs;
    }

    void remove_transaction(string id)
    {
        auto it = find_if(transactions.begin(), transactions.end(), [&](const transaction &tx) {
            return tx.tx_id == id;
        });
        if (it != transactions.end())
        {
            for (auto const &in : it->inputs)
            {
                spent_utxos.erase(utxo_key(in.prev_tx, in.index));
            }
            transactions.erase(it);
        }
    }

    void clear()
    {
        transactions.clear();
        spent_utxos.clear();
    }
};

#include "validator.h"

pair<bool, string> mempool::add_transaction(transaction tx, utxo_manager &um)
{
    string error_message;
    double calculated_fee = 0;
    if (!validator::validate_transaction(tx, um, *this, error_message, calculated_fee))
    {
        return {false, error_message};
    }
    tx.fee = calculated_fee;

    if (is_full()) {
        // transaction with lowest fee
        auto min_it = min_element(transactions.begin(), transactions.end(), [](const transaction &a, const transaction &b) {
            return a.fee < b.fee;
        });

        if (tx.fee > min_it->fee) {
            // remove the lowest fee transaction
            for (auto const &in : min_it->inputs) {
                spent_utxos.erase(utxo_key(in.prev_tx, in.index));
            }
            transactions.erase(min_it);
        } else {
            return {false, "Mempool is full and new transaction fee is too low for eviction"};
        }
    }

    transactions.push_back(tx);
    for (auto const &in : tx.inputs)
    {
        spent_utxos.insert(utxo_key(in.prev_tx, in.index));
    }

    return {true, "Transaction added to mempool"};
}

#endif
