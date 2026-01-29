#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include "transaction.h"
#include "utxo_manager.h"
#include "mempool.h"

using namespace std;

class validator
{
public:
    static bool validate_transaction(
        transaction &tx,
        UTXOManager &utxo_manager,
        mempool &mempool,
        string error_message,
        double &fee);
};

#endif // VALIDATOR_H