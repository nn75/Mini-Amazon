#include "ups_processor.h"
#include <pqxx/pqxx>


using namespace pqxx;
using namespace std;
UpsProcessor::UpsProcessor(message_queue<pair<long int, ACommands>>& mq1,
                           message_queue<UACommands>& mq2,
                           message_queue<pair<long int, AUCommands>>& mq3,
                           UpsCommunicator* ups_communicator, long int& wnum,
                           long int& unum, mutex& mt)
    : send_world_queue(mq1),
      recv_ups_queue(mq2),
      send_ups_queue(mq3),

      ups_sender(ups_communicator, mq3),
      ups_receiver(ups_communicator, mq2),
      world_seqnum(wnum),
      ups_seqnum(unum),
      mtx(mt),
      ups_thread(thread(&UpsProcessor::ups_command_process, this)) {
    cout << "ups processor activated" << endl;
}

void UpsProcessor::ups_command_process() {
    while (1) {
        if (!recv_ups_queue.if_empty()) {
            UACommands tmp_msg;
            recv_ups_queue.popfront(tmp_msg);
            AUCommands ack_res;
            if (tmp_msg.acks_size() != 0) {
                for (int i = 0; i < tmp_msg.acks_size(); i++) {
                    long int ack_seq = tmp_msg.acks(i);
                    long int seq = send_ups_queue.front().first;
                    cout << "ack from ups:" << ack_seq << endl;
                    pair<long int, AUCommands> temp;
                    while (ack_seq != seq) {
                        if (send_ups_queue.get_next_send() == 0) break;
                        if (seq == -1) {
                            send_ups_queue.popfront(temp);
                            continue;
                        }
                        send_ups_queue.popfront(temp);
                        send_ups_queue.pushback(temp);
                        seq = send_ups_queue.front().first;
                    }
                    if (send_ups_queue.get_next_send() == 0) break;
                    send_ups_queue.popfront(temp);
                }
            }
            if (tmp_msg.arrived_size() != 0) {
                for (int i = 0; i < tmp_msg.arrived_size(); i++) {
                    int wh_num = tmp_msg.arrived(i).whid();
                    int truck_id = tmp_msg.arrived(i).truckid();
                    long int package_id = tmp_msg.arrived(i).packageid();
                    long int arrived_seq = tmp_msg.arrived(i).seqnum();
                    ack_res.add_acks(arrived_seq);

                    //database: If the ups said the truck is arrived, update the truck_id in database from -1 to given number
                    connection C("dbname = mini_amazon user = postgres password = passw0rd hostaddr = 67.159.95.41 port = 5432");
                    if (C.is_open()) {
                    } else {
                        cout << "ready = Can't open database" << endl;
                    }
                    string update_truck = "UPDATE order_orders SET truck_id = "+ to_string(truck_id) + " WHERE tracking_number= "+ to_string(package_id) + ";";
                    work W1(C);
                    W1.exec(update_truck);
                    W1.commit();
                    
                    //check status of order, if status == packed, send load message to world
                    string check_pack = "SELECT order_orders WHERE tracking_number = "+ to_string(package_id) + " AND status = packed ;";
                    nontransaction N(C);
                    result R(N.exec(check_pack));
                    if(R.size() == 1){
                        result::const_iterator it = R.begin();
                        ACommands world_load_msg;
                        APutOnTruck * put_on_truck = world_load_msg.add_load();
                        put_on_truck->set_whnum(it[4].as<int>());
                        put_on_truck->set_truckid(it[5].as<int>());
                        put_on_truck->set_shipid(it[0].as<long int>());
                    
                        mtx.lock();//////lock
                        put_on_truck->set_seqnum(world_seqnum);
                        pair<long int, ACommands> world_load_pair(world_seqnum, world_load_msg);
                        world_seqnum++;
                        mtx.unlock();/////unlock
                        send_world_queue.pushback(world_load_pair);

                        //After packed and truck arrived, update order status to loading
                        string update_to_loading = "UPDATE order_orders SET status = loading WHERE tracking_number= "+ to_string(package_id) + ";";
                        work W2(C);
                        W2.exec(update_to_loading);
                        W2.commit();                        
                    }
                    C.disconnect ();
                }
            }
            if (tmp_msg.finish_size() != 0) {
                for (int i = 0; i < tmp_msg.finish_size(); i++) {
                    long int package_id = tmp_msg.finish(i).packageid();
                    long int finish_seq = tmp_msg.finish(i).seqnum();
                    ack_res.add_acks(finish_seq);
                    cout << "ups finish package_id" << package_id << endl;
                    cout << "ups finish finish_seq" << finish_seq << endl;
                    
                    // database: If ups said 
                    connection C("dbname = mini_amazon user = postgres password = passw0rd hostaddr = 67.159.95.41 port = 5432");
                    if (C.is_open()) {
                    } else {
                        cout << "arrived = Can't open database" << endl;
                    }
                    string update_delivered = "UPDATE orders_order SET status='delivered' WHERE tracking_number = "+ to_string(package_id) + ";";
                    work W(C);
                    W.exec(update_delivered);
                    W.commit();
                    C.disconnect ();
                }
            }
            if(ack_res.acks_size() != 0){
                pair<long int, AUCommands> r_acks(-1, ack_res);
                cout<<"stupid"<<ack_res.acks(0)<<endl;
                send_ups_queue.pushback(r_acks);
            }
        }//not if_empty
    }//While
}
