#include "world_processor.h"
#include <pqxx/pqxx>

using namespace pqxx;
using namespace std;

WorldProcessor::WorldProcessor(
    message_queue<pair<long int, ACommands> >& s_w_q,
    message_queue<AResponses>& r_w_q,
    message_queue<pair<long int, AUCommands> >& s_u_q,
    WorldCommunicator* world_communicator, long int& wnum, long int& unum,
    mutex& mt)
    : send_world_queue(s_w_q),
      recv_world_queue(r_w_q),
      send_ups_queue(s_u_q),

      world_sender(world_communicator, s_w_q),
      world_receiver(world_communicator, r_w_q),
      world_seqnum(wnum),
      ups_seqnum(unum),
      mtx(mt),
      world_thread(thread(&WorldProcessor::world_command_process, this)) {
    cout << "world processor activated" << endl;
}

void WorldProcessor::world_command_process() {
    while (1) {
        if (!recv_world_queue.if_empty()) {
            AResponses tmp_msg;
            recv_world_queue.popfront(tmp_msg);
            ACommands ack_res;
            if (tmp_msg.acks_size() != 0) {
                for (int i = 0; i < tmp_msg.acks_size(); i++) {
                    long int ack_seq = tmp_msg.acks(i);
                    long int seq = send_world_queue.front().first;
                    cout << "ack from world:" << ack_seq << endl;
                    pair<long int, ACommands> temp;
                    while (ack_seq != seq) {
                        if (send_world_queue.next_send == 0) break;
                        if (seq == -1) {
                            send_world_queue.popfront(temp);
                            continue;
                        }
                        send_world_queue.popfront(temp);
                        send_world_queue.pushback(temp);
                        seq = send_world_queue.front().first;
                    }
                    if (send_world_queue.next_send == 0) break;
                    send_world_queue.popfront(temp);
                }
            }
            if (tmp_msg.loaded_size() != 0) {
                for (int i = 0; i < tmp_msg.loaded_size(); i++) {
                    long int ship_id = tmp_msg.loaded(i).shipid();
                    long int ship_seq = tmp_msg.loaded(i).seqnum();
                    ack_res.add_acks(ship_seq);

                    //database: if world said order is loaded, update order status to "delivering"
                    connection C("dbname = mini_amazon user = postgres password = passw0rd hostaddr = 67.159.95.41 port = 5432");
                    if (C.is_open()) {
                    } else {
                        cout << "ready = Can't open database" << endl;
                    }
                    string update_to_packed = "UPDATE orders_order SET status = delivering WHERE tracking_number= "+ to_string(ship_id) + ";";
                    work W(C);
                    W.exec(update_to_packed);
                    W.commit();

                    //ask ups to deliver
                    AUCommands ups_deliver_msg;
                    Deliver * ups_deliver = ups_deliver_msg.add_todeliver();
                    ups_deliver->set_packageid(ship_id);
                    mtx.lock();//////lock
                    ups_deliver->set_seqnum(ups_seqnum);
                    pair<long int, AUCommands> ups_deliver_pair(ups_seqnum, ups_deliver_msg);
                    ups_seqnum++;
                    mtx.unlock();/////unlock
                    send_ups_queue.pushback(ups_deliver_pair);
                }
            }
            if (tmp_msg.ready_size() != 0) {
                for (int i = 0; i < tmp_msg.ready_size(); i++) {
                    long int ship_id = tmp_msg.ready(i).shipid();
                    long int ship_seq = tmp_msg.ready(i).seqnum();
                    ack_res.add_acks(ship_seq);

                    // database: if world said the order is packed, update order status to "packed"
                    connection C("dbname = mini_amazon user = postgres password = passw0rd hostaddr = 67.159.95.41 port = 5432");
                    if (C.is_open()) {
                    } else {
                        cout << "ready = Can't open database" << endl;
                    }
                    string update_to_packed = "UPDATE orders_order SET status = packed WHERE tracking_number= "+ to_string(ship_id) + ";";
                    work W1(C);
                    W1.exec(update_to_packed);
                    W1.commit();

                    //check truck if truck_num != -1, send load message to world
                    string check_truck = "SELECT orders_order WHERE tracking_number = "+ to_string(ship_id) + " AND truck_id != -1 ;";
                    nontransaction N(C);
                    result R(N.exec(check_truck));
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
                        string update_to_loading = "UPDATE orders_order SET status = loading WHERE tracking_number= "+ to_string(ship_id) + ";";
                        work W2(C);
                        W2.exec(update_to_loading);
                        W2.commit();                        
                    }
                    C.disconnect ();
                }
            }
            if (tmp_msg.arrived_size() != 0) {
                for (int i = 0; i < tmp_msg.arrived_size(); i++) {
                    //parse the arrived message;
                    int wh_num = tmp_msg.arrived(i).whnum();
                    long int product_id = tmp_msg.arrived(i).things(0).id();
                    string product_desciption = tmp_msg.arrived(i).things(0).description();
                    int product_count = tmp_msg.arrived(i).things(0).count();
                    long int arrived_seqnum = tmp_msg.arrived(i).seqnum();
                    cout << "arrived whnum:" << wh_num <<endl;
                    cout << "arrived things -> id:" << product_id << endl;
                    cout << "arrived things -> description:" << product_desciption << endl;
                    cout << "arrived things -> count:" << product_count << endl;
                    cout << "arrived seqnum:" << arrived_seqnum << endl;
                    ack_res.add_acks(arrived_seqnum);//add ack

                    // database: If new product stock arrived at warehouse, update product table, increase its stock to 500
                    connection C("dbname = mini_amazon user = postgres password = passw0rd hostaddr = 67.159.95.41 port = 5432");
                    if (C.is_open()) {
                    } else {
                        cout << "arrived = Can't open database" << endl;
                    }
                    string increase_stock = "UPDATE orders_product SET stock = 500 WHERE product_id = "+ to_string(product_id) +" AND wh_id = "+ to_string(wh_num)+";";
                    work W(C);
                    W.exec( increase_stock );
                    W.commit();
                    C.disconnect ();
                }
            }
            if(ack_res.acks_size() != 0){
                pair<int, ACommands> r_acks(-1, ack_res);
                send_world_queue.pushback(r_acks);
            }
        }
    }
}