#ifndef UTXO_MANAGER_H
#define UTXO_MANAGER_H

#include <string>
using namespace std;

// to be done by pratyush
class UTXOManager
{
public:
    bool exists(const string &tx_id, int index) const
    {
        return true;
    }

    double getAmount(const string &tx_id, int index) const
    {
        return 1.0;
    }
};

#endif
