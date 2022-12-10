#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <cassert>
#include <map>


typedef struct ClientInfo {
    char buffer[4096];
    int socketfd;
    bool hasMSG;
    int msgLength;
} ClientInfo;

class SocketConn {
    private: 
        int port;
        std::string ip_addr;
        int socketfd;
        // int new_socket;
        int addrlen;
        struct sockaddr_in address;
        
        /*
            Base 64 Encoding copied from https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
        */

        const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::string base64_encode(const ::std::string &bindata)
        {
            using ::std::string;
            //using ::std::numeric_limits;

            // if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
            //     throw ::std::length_error("Converting too large a string to base64.");
            // }

            const ::std::size_t binlen = bindata.size();
            // Use = signs so the end is properly padded.
            string retval((((binlen + 2) / 3) * 4), '=');
            ::std::size_t outpos = 0;
            int bits_collected = 0;
            unsigned int accumulator = 0;
            const string::const_iterator binend = bindata.end();

            for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
                accumulator = (accumulator << 8) | (*i & 0xffu);
                bits_collected += 8;
                while (bits_collected >= 6) {
                    bits_collected -= 6;
                    retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
                }
            }
            if (bits_collected > 0) { // Any trailing bits that are missing.
                assert(bits_collected < 6);
                accumulator <<= 6 - bits_collected;
                retval[outpos++] = b64_table[accumulator & 0x3fu];
            }
            assert(outpos >= (retval.size() - 2));
            assert(outpos <= retval.size());
            return retval;
        }
        /* End Base 64 Encoding */


    public:
        SocketConn(std::string ip, int p) { 
            this->port = p;
            this->ip_addr = ip;
        }


        int acceptNewConn() {
            int new_socket = accept(this->socketfd, (struct sockaddr*) &this->address, (socklen_t*) &this->addrlen);
            //this->recv(new_socket, 1);
            return new_socket;
        }

        int connect() {
            int opt = 1;
            this->addrlen = sizeof(this->address);
            this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
            if (this->socketfd < 0) {
                perror("socket failed");
                return -1;
            }
            if (setsockopt(this->socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
                perror("setsockop");
                return -1;
            }
            this->address.sin_family = AF_INET;
            this->address.sin_addr.s_addr = INADDR_ANY;
            this->address.sin_port = htons(port);

            if (bind(this->socketfd, (struct sockaddr*) &this->address, sizeof(this->address)) < 0) {
                perror("bind");
                return -1;
            }
            if (listen(this->socketfd, 64) < 0) {
                perror("listen");
                return -1;
            }
            int flags  = fcntl(this->socketfd, F_GETFL, 0);
            flags |= SOCK_NONBLOCK;
            flags = fcntl(this->socketfd, F_SETFL, flags);
            return 0;
        }

        int sendMsg(ClientInfo *client, int fin=1, int mask=0, int mask_key=0) {
            int socket = client->socketfd;
            int p_len = client->msgLength;
            char * packet = (char *) malloc(p_len + 14);
            int op = 1;
            char f_op = fin << 7;
            f_op += op;
            packet[0] = f_op; // Append FIN bit and opcode
            // Get mask bit and length of payload
            char mask_len = mask << 7;
            int cur_index = 2;
            if (p_len < 126) {
                mask_len += (char) p_len;
                // packet[2] = 0;
                // packet[3] = 0;
                // packet[4] = 0;
                // packet[5] = 0;
                // packet[6] = 0;
                // packet[7] = 0;
                // packet[8] = 0;
                // packet[9] = 0;
            }
            else if (p_len < pow(2, 16)) {
                mask_len += (char) 126;
                packet[2] = (char) (p_len >> 8); // Append MSByte of length
                packet[3] = (char) p_len;  // Append LSByte of length
                // packet[4] = 0;
                // packet[5] = 0;
                // packet[6] = 0;
                // packet[7] = 0;
                // packet[8] = 0;
                // packet[9] = 0;
                cur_index = 4;
            }
            else {
                mask_len += (char) 127;
                // Get individual bytes of the payload length
                packet[2] = (char) (p_len >> 56);
                packet[3] = (char) (p_len >> 48);
                packet[4] = (char) (p_len >> 40);
                packet[5] = (char) (p_len >> 32);
                packet[6] = (char) (p_len >> 24);
                packet[7] = (char) (p_len >> 16);
                packet[8] = (char) (p_len >> 8);
                packet[9] = (char) p_len;
                cur_index = 10;
            }
            packet[1] = mask_len;  // Add mask bit and length to packet
            if (mask) {
                // Append mask key
                packet[cur_index++] = (char) (mask_key >> 24);
                packet[cur_index++] = (char) (mask_key >> 16);
                packet[cur_index++] = (char) (mask_key >> 8);
                packet[cur_index++] = (char) mask_key;
            } // else {
            //     packet[cur_index++] = 0;
            //     packet[cur_index++] = 0;
            //     packet[cur_index++] = 0;
            //     packet[cur_index++] = 0;
            // }
            // Append payload to packet
            for (int i = 0; i < p_len; i++) {
                packet[cur_index++] = client->buffer[i];
            }
            packet = (char *) realloc(packet, cur_index);
            // TODO Send packet over the network
            send(socket, (void *) packet, cur_index, 0);
            free(packet);
            return 0;
        }

        int handleInitialConn(int socket) {
            // Complete handshake at the start of a new connection
            std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            std::string request(4096, 0);
            int res = -1;
            while (res == -1) {
                res = read(socket, &request[0], 4095);
            }
            std::string delim = "\r\n";
            int index = 0;
            std::string header = request.substr(request.find("Sec-WebSocket-Key:"), 4096);
            header = header.substr(0, header.find("\r\n"));
            header = header.replace(0, 19, "");

            header += magic;

            unsigned char * sha_str = SHA1(reinterpret_cast<const unsigned char*>(header.c_str()), header.length(), nullptr);
            header = reinterpret_cast<char*>(sha_str);
            header = base64_encode(header);
            std::string response = "HTTP/1.1 101 Switching Protocols\r\n";
            response.append("Upgrade: websocket\r\n");
            response.append("Connection: Upgrade\r\n");
            response.append("Sec-WebSocket-Accept: " + header + "\r\n");
            response.append("\r\n");

            send(socket, (void *) response.c_str(), response.length(), 0);



            return 0;
        }

        int recv(ClientInfo *client) {
            int socket = client->socketfd;
            // Malloc buffer for first two bytes (fin/op/mask/len)
            unsigned char * buffer = (unsigned char *) malloc(2);
            int val = read(socket, buffer, 2);
            if (val < 0) {
                client->hasMSG = false;
                return -1;
            }
            client->hasMSG = true;
            unsigned char fin = buffer[0] >> 7;
            unsigned char op = buffer[0] & 15;
            unsigned char mask_bit = buffer[1] >> 7;
            //unsigned int mask;
            unsigned int p_len = buffer[1] & 127;
            // Check the length in case it is larger than expected
            if (p_len == 127) {
                read(socket, buffer, 2);
                p_len = buffer[0];
                p_len = p_len << 8;
                p_len += buffer[1];
            }
            else if (p_len == 126) {
                buffer = (unsigned char *) realloc(buffer, 8);
                read(socket, buffer, 8);
                p_len = buffer[0];
                p_len << 8;
                for (int i = 1; i < 8; i++) {
                    p_len += buffer[i];
                    p_len = p_len << 8;
                }
            }
            // Read the mask bit if necessary
            int mask[4];
            if (mask_bit) {
                buffer = (unsigned char *) realloc(buffer, 4);
                read(socket, buffer, 4);
                mask[0] = buffer[0];
                mask[1] = buffer[1];
                mask[2] = buffer[2];
                mask[3] = buffer[3];

                // mask = mask << 8;
                // for (int i = 1; i < 3; i++) {
                //     mask += buffer[i];
                //     mask = mask << 8;
                // }
            }
            // Read the paylaod and return (may need to do something with the mask bit?)
            // char * payload = (char *) malloc(p_len);
            read(socket, client->buffer, p_len); // TODO Iterate to make sure everything is read
            for (int i = 0; i < p_len; i++) {
                client->buffer[i] = (char) (((int) client->buffer[i]) ^ mask[i%4]);
            }
            client->buffer[p_len] = '\0';
            client->msgLength = p_len;
            free(buffer);
            return 0;
        }
    
};

int callback(SocketConn *server, ClientInfo *client) {
    // std::string temp = "Client(" + std::to_string(client->socketfd) + ") said: " + client->buffer;
    server->sendMsg(client);
    printf("Client(%d) said: %s\n", client->socketfd, client->buffer);
    client->hasMSG = false;
    return 0;
}

int main() {
    int connected_clients = 0;
    int total_clients = 64;
    ClientInfo *clients = (ClientInfo *) malloc(sizeof(ClientInfo) * total_clients);
    std::string ip_addr = "127.0.0.1";
    SocketConn* server = new SocketConn(ip_addr, 9001);
    server->connect();

    int newConn = -1;
    int i;
    while (1) {
        newConn = server->acceptNewConn();
        if (newConn > -1) {
            // Complete the handshake for incoming connection
            int flags  = fcntl(newConn, F_GETFL, 0);
            flags |= SOCK_NONBLOCK;
            flags = fcntl(newConn, F_SETFL, flags);
            printf("New client connected and was given id %d\n", newConn);
            server->handleInitialConn(newConn);
            clients[connected_clients].socketfd = newConn;
            
            connected_clients++;
        }
        std::string temp = "New client connected and was given id " + std::to_string(newConn) + "\0";
        std::map<int, std::string> msgList;
        
        for (i = 0; i < connected_clients; i++) {
            server->recv(&clients[i]);
            if (clients[i].hasMSG) {
                std::string tempstr = "Client(" + std::to_string(clients[i].socketfd) + ") said: " + clients[i].buffer + '\0';
                printf("%s\n", tempstr.c_str());
                msgList.insert({clients[i].socketfd, tempstr});
                clients[i].hasMSG = false;
                // callback(server, &clients[i]);
            }

            
        }

        for (i = 0; i < connected_clients; i++) {
            
            std::map<int, std::string>::iterator it;
            for (it = msgList.begin(); it != msgList.end(); ++it) {
                if (it->first != clients[i].socketfd) {
                    strcpy(clients[i].buffer, it->second.c_str());
                    clients[i].msgLength = strlen(clients[i].buffer);
                    callback(server, &clients[i]);
                }
            }

            if (i != connected_clients - 1 && newConn != -1) {
                strcpy(clients[i].buffer, temp.c_str());
                clients[i].msgLength = strlen(clients[i].buffer);
                server->sendMsg(&clients[i]);
            }
        }
    }
}

