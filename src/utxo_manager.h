#ifndef UTXO_MANAGER_H
#define UTXO_MANAGER_H

#include <string>
#include <map>

// Structure to uniquely identify a UTXO
struct UTXOKey {
    std::string tx_id;
    int index;

    bool operator<(const UTXOKey& other) const {
        if (tx_id != other.tx_id)
            return tx_id < other.tx_id;
        return index < other.index;
    }
};

// Structure to store UTXO data
struct UTXOValue {
    double amount;
    std::string owner;
};

class UTXOManager {
private:
    std::map<UTXOKey, UTXOValue> utxo_set;

public:
    void addUTXO(const std::string &tx_id, int index, double amount,
                 const std::string &owner) {
        utxo_set[{tx_id, index}] = {amount, owner};
    }

    void removeUTXO(const std::string &tx_id, int index) {
        utxo_set.erase({tx_id, index});
    }

    bool exists(const std::string &tx_id, int index) const {
        return utxo_set.find({tx_id, index}) != utxo_set.end();
    }

    double getAmount(const std::string &tx_id, int index) const {
        auto it = utxo_set.find({tx_id, index});
        return (it != utxo_set.end()) ? it->second.amount : 0.0;
    }

    double getBalance(const std::string &owner) const {
        double total = 0.0;
        for (const auto &p : utxo_set) {
            if (p.second.owner == owner)
                total += p.second.amount;
        }
        return total;
    }
};

#endif