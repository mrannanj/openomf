#include <stdio.h>

#include "controller/net_controller.h"
#include "utils/log.h"

typedef struct wtf_t {
    ENetHost *host;
    ENetPeer *peer;
    int id;
    int last_hb;
    int last_action;
    int outstanding_hb;
    int rtt;
    int disconnected;
} wtf;

void net_controller_free(controller *ctrl) {
    wtf *data = ctrl->data;
    ENetEvent event;
    if (!data->disconnected) {
        DEBUG("closing connection");
        enet_peer_disconnect(data->peer, 0);

        while (enet_host_service(data->host, &event, 3000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    DEBUG("got disconnect notice");
                    // peer has acknowledged the disconnect
                    break;
                default:
                    break;
            }
        }
    }
    enet_host_destroy(data->host);
    free(data);
}

int net_controller_tick(controller *ctrl, int ticks, ctrl_event **ev) {
    ENetEvent event;
    wtf *data = ctrl->data;
    ENetHost *host = data->host;
    ENetPeer *peer = data->peer;
    serial *ser;
    /*int handled = 0;*/
    if (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                ser = malloc(sizeof(serial));
                serial_create(ser);
                ser->data = malloc(event.packet->dataLength);
                ser->len = event.packet->dataLength;
                memcpy(ser->data, event.packet->data, event.packet->dataLength);
                switch(serial_read_int(ser)) {
                    case EVENT_TYPE_ACTION:
                        {
                            // dispatch keypress to scene
                            int action = serial_read_int(ser);
                            controller_cmd(ctrl, action, ev);
                            /*handled = 1;*/
                            serial_free(ser);
                            free(ser);
                        }
                        break;
                    case EVENT_TYPE_HB:
                        {
                            // got a tick
                            int id = serial_read_int(ser);
                            if (id == data->id) {
                                int start = serial_read_int(ser);
                                data->rtt = abs(start - ticks);
                                data->outstanding_hb = 0;
                                data->last_hb = ticks;
                                serial_free(ser);
                            } else {
                                // a heartbeat from the peer, bounce it back
                                ENetPacket *packet;
                                packet = enet_packet_create(ser->data, ser->len, ENET_PACKET_FLAG_RELIABLE);
                                if (peer) {
                                    enet_peer_send(peer, 0, packet);
                                    enet_host_flush (host);
                                }
                            }
                            free(ser);
                        }
                        break;
                    case EVENT_TYPE_SYNC:
                        controller_sync(ctrl, ser, ev);
                        /*handled = 1;*/
                        break;
                    default:
                        serial_free(ser);
                        free(ser);
                }
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                DEBUG("peer disconnected!");
                data->disconnected = 1;
                controller_close(ctrl, ev);
                return 1; // bail the fuck out
                break;
            default:
                break;
        }
    }
    if ((data->last_hb == -1 || ticks - data->last_hb > 10) && !data->outstanding_hb) {
        data->outstanding_hb = 1;
        serial ser;
        ENetPacket *packet;
        serial_create(&ser);
        serial_write_int(&ser, EVENT_TYPE_HB);
        serial_write_int(&ser, data->id);
        serial_write_int(&ser, ticks);
        packet = enet_packet_create(ser.data, ser.len, ENET_PACKET_FLAG_RELIABLE);
        if (peer) {
            enet_peer_send(peer, 0, packet);
            enet_host_flush (host);
        } else {
            DEBUG("peer is null~");
            data->disconnected = 1;
            controller_close(ctrl, ev);
        }
    }

    /*if(!handled) {*/
        /*controller_cmd(ctrl, ACT_STOP, ev);*/
    /*}*/
    return 0;
}

int net_controller_update(controller *ctrl, serial *serial) {
    wtf *data = ctrl->data;
    ENetPeer *peer = data->peer;
    ENetHost *host = data->host;
    ENetPacket *packet;
    int et = EVENT_TYPE_SYNC;
    char *buf = malloc(serial->len+sizeof(int));

    memcpy(buf, (char*)&et, sizeof(int));
    // need to copy here because enet will free this packet
    memcpy(buf+sizeof(int), serial->data, serial->len);

    packet = enet_packet_create(buf, serial->len, ENET_PACKET_FLAG_RELIABLE);
    if (peer) {
        enet_peer_send(peer, 0, packet);
        enet_host_flush(host);
    } else {
        DEBUG("peer is null~");
    }

    return 0;
}

void har_hook(char* buf, void *userdata) {
    controller *ctrl = userdata;
    wtf *data = ctrl->data;
    ENetPeer *peer = data->peer;
    ENetHost *host = data->host;
    data->disconnected = 0;
    ENetPacket * packet;

    packet = enet_packet_create(buf, strlen (buf) + 1, ENET_PACKET_FLAG_RELIABLE);
    if (peer) {
        enet_peer_send(peer, 0, packet);
        enet_host_flush (host);
    } else {
        DEBUG("peer is null~");
    }
}

void controller_hook(controller *ctrl, int action) {
    serial ser;
    wtf *data = ctrl->data;
    ENetPeer *peer = data->peer;
    ENetHost *host = data->host;
    ENetPacket *packet;
    if (action == ACT_STOP && data->last_action == ACT_STOP) {
        data->last_action = -1;
        return;
    }
    data->last_action = action;
    serial_create(&ser);
    serial_write_int(&ser, EVENT_TYPE_ACTION);
    serial_write_int(&ser, action);
    /*DEBUG("controller hook fired with %d", action);*/
    /*sprintf(buf, "k%d", action);*/
    packet = enet_packet_create(ser.data, ser.len, ENET_PACKET_FLAG_RELIABLE);
    if (peer) {
        enet_peer_send(peer, 0, packet);
        enet_host_flush (host);
    } else {
        DEBUG("peer is null~");
    }
}

int net_controller_get_rtt(controller *ctrl) {
    wtf *data = ctrl->data;
    if (data->last_hb == -1) {
        return -1;
    }
    return data->rtt;
}

void net_controller_create(controller *ctrl, ENetHost *host, ENetPeer *peer, int id) {
    wtf *data = malloc(sizeof(wtf));
    data->id = id;
    data->host = host;
    data->peer = peer;
    data->last_hb = -1;
    data->last_action = ACT_STOP;
    data->outstanding_hb = 0;
    data->rtt = 0;
    ctrl->data = data;
    ctrl->type = CTRL_TYPE_NETWORK;
    ctrl->tick_fun = &net_controller_tick;
    ctrl->update_fun = &net_controller_update;
    ctrl->har_hook = &har_hook;
    ctrl->controller_hook = &controller_hook;
}
