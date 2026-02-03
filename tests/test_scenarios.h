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

void print_test_res(string label, string flow, pair<bool, string> res) {
    cout << label << " : " << flow << " - " << (res.first ? "VALID" : "REJECTED") << endl;
    if (!res.first) {
        cout << "Error : " << res.second << endl;
    }
}

void run_double_spend_test(utxo_manager &mgr, mempool &mp)
{
    reset_state(mgr, mp);
    cout << "Running test..." << endl;
    cout << "\nTest : Alice tries to spend same UTXO twice" << endl;

    transaction tx1("tx_alice_bob_001");
    tx1.inputs = {{"genesis", 0, "Alice"}};
    tx1.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    
    auto res1 = mp.add_transaction(tx1, mgr);
    print_test_res("TX1", "Alice -> Bob (10 BTC )", res1);

    transaction tx2("tx_alice_charlie_002");
    tx2.inputs = {{"genesis", 0, "Alice"}};
    tx2.outputs = {{10.0, "Charlie"}, {39.999, "Alice"}};
    
    auto res2 = mp.add_transaction(tx2, mgr);
    print_test_res("TX2", "Alice -> Charlie (10 BTC )", res2);
}

void test_1_basic_valid(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 1 : Alice sends 10 BTC to Bob" << endl;
    transaction tx1("tx1");
    tx1.inputs = {{"genesis", 0, "Alice"}};
    tx1.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    auto res = mp.add_transaction(tx1, mgr);
    print_test_res("TX1", "Alice -> Bob (10 BTC )", res);
    if (res.first) cout << "Fee calculated : " << tx1.fee << " BTC" << endl;
}

void test_2_multiple_inputs(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    mgr.add_utxo("genesis", 2, 20.0, "Alice");
    cout << "Test 2 : Alice spends 50 + 20 BTC to send 60 to Bob" << endl;
    transaction tx2("tx2");
    tx2.inputs = {{"genesis", 0, "Alice"}, {"genesis", 2, "Alice"}};
    tx2.outputs = {{60.0, "Bob"}, {9.999, "Alice"}};
    auto res = mp.add_transaction(tx2, mgr);
    print_test_res("TX2", "Alice -> Bob (60 BTC )", res);
}

void test_3_double_spend_same_tx(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 3 : Double-Spend in Same Transaction" << endl;
    transaction tx3("tx3");
    tx3.inputs = {{"genesis", 0, "Alice"}, {"genesis", 0, "Alice"}};
    tx3.outputs = {{10.0, "Bob"}};
    auto res = mp.add_transaction(tx3, mgr);
    print_test_res("TX3", "Alice tries same UTXO twice", res);
}

void test_4_mempool_double_spend(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 4 : Mempool Double-Spend" << endl;
    transaction tx4a("tx4a");
    tx4a.inputs = {{"genesis", 1, "Bob"}};
    tx4a.outputs = {{10.0, "Alice"}, {19.999, "Bob"}};
    auto res1 = mp.add_transaction(tx4a, mgr);
    print_test_res("TX1", "Bob -> Alice (10 BTC )", res1);

    transaction tx4b("tx4b");
    tx4b.inputs = {{"genesis", 1, "Bob"}};
    tx4b.outputs = {{5.0, "Charlie"}, {24.999, "Bob"}};
    auto res2 = mp.add_transaction(tx4b, mgr);
    print_test_res("TX2", "Bob -> Charlie (5 BTC )", res2);
}

void test_5_insufficient_funds(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 5 : Insufficient Funds" << endl;
    transaction tx5("tx5");
    tx5.inputs = {{"genesis", 1, "Bob"}};
    tx5.outputs = {{35.0, "Alice"}};
    auto res = mp.add_transaction(tx5, mgr);
    print_test_res("TX5", "Bob tries to send 35 BTC (has 30)", res);
}

void test_6_negative_amount(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 6 : Negative Amount" << endl;
    transaction tx6("tx6");
    tx6.inputs = {{"genesis", 3, "David"}};
    tx6.outputs = {{-5.0, "Eve"}};
    auto res = mp.add_transaction(tx6, mgr);
    print_test_res("TX6", "Negative amount", res);
}

void test_7_zero_fee(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 7 : Zero Fee Transaction" << endl;
    transaction tx7("tx7");
    tx7.inputs = {{"genesis", 4, "Eve"}};
    tx7.outputs = {{5.0, "Alice"}};
    auto res = mp.add_transaction(tx7, mgr);
    print_test_res("TX7", "5 BTC input, 5 BTC output", res);
}

void test_8_race_attack(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 8 : Race Attack (First-seen rule)" << endl;
    transaction tx8a("tx8_low");
    tx8a.inputs = {{"genesis", 0, "Alice"}};
    tx8a.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    auto res1 = mp.add_transaction(tx8a, mgr);
    print_test_res("TX1 (Low fee )", "arrives first", res1);

    transaction tx8b("tx8_high");
    tx8b.inputs = {{"genesis", 0, "Alice"}};
    tx8b.outputs = {{10.0, "Charlie"}, {39.0, "Alice"}};
    auto res2 = mp.add_transaction(tx8b, mgr);
    print_test_res("TX2 (High fee )", "arrives second", res2);
}

void test_9_mining_flow(utxo_manager &mgr, mempool &mp, blockchain &bc) {
    reset_state(mgr, mp);
    cout << "Test 9 : Complete Mining Flow" << endl;
    transaction tx9("tx9");
    tx9.inputs = {{"genesis", 1, "Bob"}};
    tx9.outputs = {{10.0, "Alice"}, {19.999, "Bob"}};
    mp.add_transaction(tx9, mgr);
    mine_block("Miner1", mp, mgr, bc);
    cout << "Verification: Bob's genesis UTXO is gone: " << (!mgr.exists("genesis", 1) ? "YES" : "NO") << endl;
    cout << "Verification: Miner1 received fee: " << (mgr.get_balance("Miner1") > 0 ? "YES" : "NO") << endl;
}

void test_10_unconfirmed_chain(utxo_manager &mgr, mempool &mp) {
    reset_state(mgr, mp);
    cout << "Test 10 : Unconfirmed Chain" << endl;
    transaction tx10a("tx10a");
    tx10a.inputs = {{"genesis", 0, "Alice"}};
    tx10a.outputs = {{10.0, "Bob"}, {39.999, "Alice"}};
    auto res1 = mp.add_transaction(tx10a, mgr);
    print_test_res("TX10a", "Alice -> Bob (10 BTC )", res1);

    transaction tx10b("tx10b");
    tx10b.inputs = {{"tx10a", 0, "Bob"}};
    tx10b.outputs = {{5.0, "Charlie"}, {4.999, "Bob"}};
    auto res2 = mp.add_transaction(tx10b, mgr);
    print_test_res("TX10b", "Bob spends unconfirmed output", res2);
    if (!res2.first) cout << " (Decision : Rejected as per design)" << endl;
}

void run_tests()
{
    utxo_manager mgr;
    mempool mp;
    blockchain bc;

    while (true) {
        cout << "\n1. Test 1 : Basic Valid Transaction" << endl;
        cout << "2. Test 2 : Double-spend" << endl;
        cout << "3. Test 3 : Double-Spend in Same Transaction" << endl;
        cout << "4. Test 4 : Mempool Double-Spend" << endl;
        cout << "5. Test 5 : Insufficient Funds" << endl;
        cout << "6. Test 6 : Negative Amount" << endl;
        cout << "7. Test 7 : Zero Fee Transaction" << endl;
        cout << "8. Test 8 : Race Attack Simulation" << endl;
        cout << "9. Test 9 : Complete Mining Flow" << endl;
        cout << "10. Test 10 : Unconfirmed Chain" << endl;
        cout << "11. Run all tests" << endl;
        cout << "12. Back up" << endl;
        cout << "Enter choice : ";
        
        int choice;
        cin >> choice;

        if (choice == 1) test_1_basic_valid(mgr, mp);
        else if (choice == 2) run_double_spend_test(mgr, mp);
        else if (choice == 3) test_3_double_spend_same_tx(mgr, mp);
        else if (choice == 4) test_4_mempool_double_spend(mgr, mp);
        else if (choice == 5) test_5_insufficient_funds(mgr, mp);
        else if (choice == 6) test_6_negative_amount(mgr, mp);
        else if (choice == 7) test_7_zero_fee(mgr, mp);
        else if (choice == 8) test_8_race_attack(mgr, mp);
        else if (choice == 9) test_9_mining_flow(mgr, mp, bc);
        else if (choice == 10) test_10_unconfirmed_chain(mgr, mp);
        else if (choice == 11) {
            test_1_basic_valid(mgr, mp);
            run_double_spend_test(mgr, mp);
            test_3_double_spend_same_tx(mgr, mp);
            test_4_mempool_double_spend(mgr, mp);
            test_5_insufficient_funds(mgr, mp);
            test_6_negative_amount(mgr, mp);
            test_7_zero_fee(mgr, mp);
            test_8_race_attack(mgr, mp);
            test_9_mining_flow(mgr, mp, bc);
            test_10_unconfirmed_chain(mgr, mp);
        }
        else if (choice == 12) break;
    }
}

#endif