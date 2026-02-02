#ifndef TEST_SCENARIOS_H
#define TEST_SCENARIOS_H

#include <iostream>
#include <cassert>
#include <string>
#include "../src/block.h"
#include "../src/blockchain.h"
#include "../src/mempool.h"
#include "../src/utxo_manager.h"
#include "../src/validator.h"
#include "../src/transaction.h"
#include "../src/mining.h"

using namespace std;

void reset_state(utxo_manager &mgr, mempool &mp)
{
    mgr.get_utxo_set().clear();
    mp.clear();
    mgr.add_utxo("genesis", 0, 50.0, "Alice");
    mgr.add_utxo("genesis", 1, 30.0, "Bob");
    mgr.add_utxo("genesis", 2, 20.0, "Charlie");
    mgr.add_utxo("genesis", 3, 10.0, "David");
    mgr.add_utxo("genesis", 4, 5.0, "Eve");
}

void run_tests()
{
    utxo_manager mgr;
    mempool mp;
    blockchain bc;

    reset_state(mgr, mp);
    transaction tx1;
    tx1.tx_id = "tx1";
    tx1.inputs = {{"genesis", 0, "Alice"}};
    tx1.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    assert(mp.add_transaction(tx1, mgr).first == true);
    cout << "Test 1 Passed: Basic Valid transaction" << endl;

    reset_state(mgr, mp);
    transaction tx2;
    tx2.tx_id = "tx2";
    tx2.inputs = {{"genesis", 0, "Alice"}, {"genesis", 2, "Charlie"}};
    tx2.outputs = {{60.0, "Bob"}, {9.999, "Alice"}};
    assert(mp.add_transaction(tx2, mgr).first == true);
    cout << "Test 2 Passed: Multiple Inputs" << endl;

    reset_state(mgr, mp);
    transaction tx3;
    tx3.tx_id = "tx3";
    tx3.inputs = {{"genesis", 0, "Alice"}, {"genesis", 0, "Alice"}};
    tx3.outputs = {{10.0, "Bob"}};
    assert(mp.add_transaction(tx3, mgr).first == false);
    cout << "Test 3 Passed: Double-Spend in Same transaction (Rejected)" << endl;

    reset_state(mgr, mp);
    transaction tx4a;
    tx4a.tx_id = "tx4a";
    tx4a.inputs = {{"genesis", 1, "Bob"}};
    tx4a.outputs = {{10.0, "Alice"}};
    mp.add_transaction(tx4a, mgr);

    transaction tx4b;
    tx4b.tx_id = "tx4b";
    tx4b.inputs = {{"genesis", 1, "Bob"}};
    tx4b.outputs = {{5.0, "Charlie"}};
    assert(mp.add_transaction(tx4b, mgr).first == false);
    cout << "Test 4 Passed: mempool Double-Spend (Rejected)" << endl;

    reset_state(mgr, mp);
    transaction tx5;
    tx5.tx_id = "tx5";
    tx5.inputs = {{"genesis", 1, "Bob"}};
    tx5.outputs = {{35.0, "Alice"}};
    assert(mp.add_transaction(tx5, mgr).first == false);
    cout << "Test 5 Passed: Insufficient Funds (Rejected)" << endl;

    reset_state(mgr, mp);
    transaction tx6;
    tx6.tx_id = "tx6";
    tx6.inputs = {{"genesis", 3, "David"}};
    tx6.outputs = {{-5.0, "Eve"}};
    assert(mp.add_transaction(tx6, mgr).first == false);
    cout << "Test 6 Passed: Negative Amount (Rejected)" << endl;

    reset_state(mgr, mp);
    transaction tx7;
    tx7.tx_id = "tx7";
    tx7.inputs = {{"genesis", 4, "Eve"}};
    tx7.outputs = {{5.0, "Alice"}};
    assert(mp.add_transaction(tx7, mgr).first == true);
    cout << "Test 7 Passed: Zero Fee transaction (Accepted)" << endl;

    reset_state(mgr, mp);
    transaction tx8_low_fee;
    tx8_low_fee.tx_id = "tx8_low";
    tx8_low_fee.inputs = {{"genesis", 0, "Alice"}};
    tx8_low_fee.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    mp.add_transaction(tx8_low_fee, mgr);

    transaction tx8_high_fee;
    tx8_high_fee.tx_id = "tx8_high";
    tx8_high_fee.inputs = {{"genesis", 0, "Alice"}};
    tx8_high_fee.outputs = {{10.0, "Charlie"}, {39.0, "Alice"}};
    assert(mp.add_transaction(tx8_high_fee, mgr).first == false);
    cout << "Test 8 Passed: Race Attack (First-seen rule enforced)" << endl;

    reset_state(mgr, mp);
    transaction tx9;
    tx9.tx_id = "tx9";
    tx9.inputs = {{"genesis", 1, "Bob"}};
    tx9.outputs = {{10.0, "Alice"}, {19.0, "Bob"}};
    mp.add_transaction(tx9, mgr);

    mine_block("Miner1", mp, mgr, bc);
    assert(mgr.exists("tx9", 0) == true);
    assert(mgr.exists("genesis", 1) == false);
    cout << "Test 9 Passed: Complete Mining Flow" << endl;

    reset_state(mgr, mp);
    transaction tx10a;
    tx10a.tx_id = "tx10a";
    tx10a.inputs = {{"genesis", 0, "Alice"}};
    tx10a.outputs = {{10.0, "Bob"}, {39.9, "Alice"}};
    mp.add_transaction(tx10a, mgr);

    transaction tx10b;
    tx10b.tx_id = "tx10b";
    tx10b.inputs = {{"tx10a", 0, "Bob"}};
    tx10b.outputs = {{5.0, "Charlie"}, {4.9, "Bob"}};

    assert(mp.add_transaction(tx10b, mgr).first == false);
    cout << "Test 10 Passed: Unconfirmed Chain (Reject simple approach)" << endl;
}

#endif