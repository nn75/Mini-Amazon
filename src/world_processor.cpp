#include "world_processor.h"

using namespace std;
WorldProcessor::WorldProcessor(message_queue<pair<long int, ACommands> >& mq1,
                               message_queue<AResponses>& mq2,
                               message_queue<AUCommands>& mq3,
                               WorldCommunicator* world_communicator,
                               long int& wnum, long int& unum, mutex& mt)
    : send_world_queue(mq1),
      recv_world_queue(mq2),
      send_ups_queue(mq3),

      world_sender(world_communicator, mq1),
      world_receiver(world_communicator, mq2),
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
                    if (send_world_queue.next_send == 0) {
                        break;
                    }
                    send_world_queue.popfront(temp);
                }
            }
            if (tmp_msg.loaded_size() != 0) {
                for (int i = 0; i < tmp_msg.loaded_size(); i++) {
                    long int ship_id = tmp_msg.loaded(i).shipid();
                    long int ship_seq = tmp_msg.loaded(i).seqnum();
                    ack_res.add_acks(ship_seq);
                    // AUCommands ups_load_msg;
                    // Deliver * deliver = ups_load_msg.mutable_todeliver();
                    // deliver->set_packageid(ship_id);
                    // mtx.lock();
                    // deliver->set_seqnum(ups_seqnum);
                    // ups_seqnum++;
                    // mtx.unlock();
                    // send_ups_queue.pushback(ups_load_msg);
                    // also update the status of package
                }
            }
            if (tmp_msg.ready_size() != 0) {
                for (int i = 0; i < tmp_msg.ready_size(); i++) {
                    long int ship_id = tmp_msg.ready(i).shipid();
                    long int ship_seq = tmp_msg.ready(i).seqnum();
                    ack_res.add_acks(ship_seq);
                    // please fill database code here use ship_id to select
                    // truck_id and warehouse number also update the status of
                    // package if(truckid > 0){
                    //     ACommands load_msg;
                    //     APutOnTruck * load = load_msg.mutable_load();
                    //     load->set_whnum = wh_num;
                    //     load->set_truckid = truckid;
                    //     load->set_packageid = ship_id;
                    //     mtx.lock();
                    //     load->set_seqnum(world_seqnum);
                    //     world_seqnum++;
                    //     mtx.unlock();
                    // }
                }
            }
            if (tmp_msg.arrived_size() != 0) {
                for (int i = 0; i < tmp_msg.arrived_size(); i++) {
                    int wh_num = tmp_msg.arrived(i).whnum();
                    long int product_id = tmp_msg.arrived(i).things(0).id();
                    cout << "id:" << product_id << endl;
                    int count = tmp_msg.arrived(i).things(0).count();
                    cout << "count:" << count << endl;
                    long int ship_seq = tmp_msg.arrived(i).seqnum();
                    ack_res.add_acks(ship_seq);
                    // also update the status of package
                }
            }
            pair<int, ACommands> r_acks(-1, ack_res);
            send_world_queue.pushback(r_acks);
        }
    }
}