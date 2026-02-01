#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>

using namespace std;

class tx_input
{
private:
    string prev_tx_id;
    int output_index;
    string owner;

public:
    tx_input(string prev_tx_id, int output_index, string owner)
    {
        this->prev_tx_id = prev_tx_id;
        this->output_index = output_index;
        this->owner = owner;
    }

    string getprev_tx_id() const { return prev_tx_id; }
    int getoutput_index() const { return output_index; }
    string getowner() const { return owner; }
};

class tx_output
{
private:
    double amount;
    string receiver;

public:
    tx_output(double amount, const string &receiver)
    {
        this->amount = amount;
        this->receiver = receiver;
    }

    double getamount() const { return amount; }
    string getreceiver() const { return receiver; }
};

class transaction
{
private:
    string tx_id;
    vector<tx_input> inputs;
    vector<tx_output> outputs;

public:
    transaction(string tx_id, vector<tx_input> inputs, vector<tx_output> outputs)
    {
        this->tx_id = tx_id;
        this->inputs = inputs;
        this->outputs = outputs;
    }

    string gettx_id() const { return tx_id; }
    vector<tx_input> getinputs() const { return inputs; }
    vector<tx_output> getoutputs() const { return outputs; }
};

#endif // TRANSACTION_H