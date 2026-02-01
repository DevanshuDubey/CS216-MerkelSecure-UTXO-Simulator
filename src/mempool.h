#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "transaction.h"
#include "validator.h"
#include "utxo_manager.h"
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

class mempool
{
private:
    priority_queue<pair<double, transaction>> transactions;
    set<utxo_key> spent_utxos;
    int max_size;

public:
    mempool(int size = 50) : max_size(size) {}

    pair<bool, string> add_transaction(transaction tx, utxo_manager &um)
    {
        if (transactions.size() >= (size_t)max_size)
        {
            return {false, "Mempool is full"};
        }

        string error_message;
        if (!validator::validate(tx, um, error_message))
        {
            return {false, error_message};
        }

        for (auto const &in : tx.inputs)
        {
            utxo_key key(in.prev_tx_id, in.index);
            if (spent_utxos.find(key) != spent_utxos.end())
            {
                return {false, "UTXO already spent in mempool"};
            }
        }

        transactions.push(make_pair(tx.fee, tx));
        for (auto const &in : tx.inputs)
        {
            spent_utxos.insert(utxo_key(in.prev_tx_id, in.index));
        }

        return {true, "Transaction added to mempool"};
    }

    vector<transaction> mine_top_transactions(int n)
    {
        vector<transaction> mined_txs;
        for (int i = 0; i < n && !transactions.empty(); i++)
        {
            transaction tx = transactions.top().second;
            transactions.pop();
            mined_txs.push_back(tx);
            for (auto const &in : tx.inputs)
            {
                spent_utxos.erase(utxo_key(in.prev_tx_id, in.index));
            }
        }
        return mined_txs;
    }

    vector<transaction> get_all_transactions()
    {
        vector<transaction> all_txs;
        priority_queue<pair<double, transaction>> temp = transactions;
        while (!temp.empty())
        {
            all_txs.push_back(temp.top().second);
            temp.pop();
        }
        return all_txs;
    }

    void clear()
    {
        while (!transactions.empty())
            transactions.pop();
        spent_utxos.clear();
    }
};

#endif
