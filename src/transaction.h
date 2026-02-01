#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>

using namespace std;

class tx_input
{
public:
    string prev_tx_id;
    int index;
    string owner;

    tx_input(string id = "", int idx = 0, string own = "") : prev_tx_id(id), index(idx), owner(own) {}
};

class tx_output
{
public:
    double amount;
    string recipient;

    tx_output(double amt = 0.0, string rec = "") : amount(amt), recipient(rec) {}
};

class transaction
{
public:
    string tx_id;
    vector<tx_input> inputs;
    vector<tx_output> outputs;
    double fee;

    transaction(string id = "") : tx_id(id), fee(0.0) {}

    void add_input(string prev_tx_id, int index, string owner)
    {
        inputs.push_back(tx_input(prev_tx_id, index, owner));
    }

    void add_output(double amount, string recipient)
    {
        outputs.push_back(tx_output(amount, recipient));
    }

    bool operator<(const transaction &other) const
    {
        return tx_id < other.tx_id;
    }
};

#endif