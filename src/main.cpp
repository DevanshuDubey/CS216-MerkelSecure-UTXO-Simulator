#include <iostream>
#include "transaction.h"
#include "validator.h"

using namespace std;

int main()
{
    UTXOManager utxomanager;
    mempool mempool;

    tx_input in1("genesis", 10, "Alice");
    tx_output out1(0.0, "Bob");
    tx_output out2(0.999, "Alice");

    transaction tx("tx1", {in1}, {out1, out2});

    string error_message;
    double fee = 0.0;

    bool ok = validator::validate_transaction(tx, utxomanager, mempool, error_message, fee);
    if (ok)
    {
        cout << "Transaction is valid fee: " << fee << endl;
    }
    else
    {
        cout << "Transaction is invalid: " << error_message << endl;
    }
    return 0;
}