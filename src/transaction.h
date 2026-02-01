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

    string getprev_tx_id() const { return prev_tx_id; }
    int getoutput_index() const { return index; }
    string getowner() const { return owner; }
};

class tx_output
{
public:
    double amount;
    string recipient;

    tx_output(double amt = 0.0, string rec = "") : amount(amt), recipient(rec) {}

    double getamount() const { return amount; }
    string getreceiver() const { return recipient; }
};

class transaction
{
public:
    string tx_id;
    vector<tx_input> inputs;
    vector<tx_output> outputs;
    double fee;

    string gettx_id() const { return tx_id; }
    const vector<tx_input>& getinputs() const { return inputs; }
    const vector<tx_output>& getoutputs() const { return outputs; }

    transaction(string id = ""){
        this->tx_id = id;
        this->fee = 0.0;
    }

    transaction(string id, vector<tx_input> ins, vector<tx_output> outs)
        : tx_id(id), inputs(ins), outputs(outs), fee(0.0) {}

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