#ifndef MINING_ENGINE_H
#define MINING_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include "mempool.h"
#include "blockchain.h"
#include "block.h"

using namespace std;


void mine_new_block(mempool& mp, Blockchain& chain, string miner_name, int tx_to_mine = 3) {
    cout << "Mining block..." << endl;

   
    vector<transaction> selected_txs = mp.mine_top_transactions(tx_to_mine);

    if (selected_txs.empty()) {
        cout << "Error: No transactions available in mempool to mine!" << endl;
        return;
    }

    cout << "Selected " << selected_txs.size() << " transactions from mempool." << endl;

    double total_fees = 0;
    for (const auto& tx : selected_txs) {
        total_fees += tx.fee;
    }

    cout << "Total fees: " << total_fees << " BTC" << endl;
    cout << "Miner " << miner_name << " receives " << total_fees << " BTC" << endl;

    block* new_mined_block = new block(miner_name, total_fees, selected_txs, chain);

    chain.add_block(new_mined_block);

    cout << "Block mined successfully!" << endl;
    cout << "Removed " << selected_txs.size() << " transactions from mempool." << endl;
}

#endif