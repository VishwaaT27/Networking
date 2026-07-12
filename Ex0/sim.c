#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char url[100];
    char ip[100];
    char mac[100];
    int is_occ;
} HashT;

typedef struct
{
    int packet_no;
    char src_ip[100];
    char dest_ip[100];
    char data[33];
    int size;
} Packet;

typedef struct
{
    int frame_no;
    char src_mac[100];
    char dest_mac[100];
    int packet_no;
    char data[17];
    char trailer[9];
} Frame;



int hash(const char *url)
{
    int sum = 0, i;
    for (i = 0; url[i] != '\0'; i++)
        sum += url[i];
    return sum % 20;
}

void initialise_hash(HashT table[])
{
    int i;
    for (i = 0; i < 20; i++)
        table[i].is_occ = 0;
}

void insert(HashT table[], const char *url, const char *ip, const char *mac)
{
    int index = hash(url);
    int st = index;

    while (table[index].is_occ)
    {
        index = (index + 1) % 20;
        if (index == st) return;
    }
    strcpy(table[index].url, url);
    strcpy(table[index].ip, ip);
    strcpy(table[index].mac, mac);
    table[index].is_occ = 1;
}

int search(HashT table[], const char *url, HashT *result)
{
    int index = hash(url);
    int st = index;

    while (table[index].is_occ)
    {
        if (strcmp(table[index].url, url) == 0)
        {
            *result = table[index];
            return 1;
        }
        index = (index + 1) % 20;
        if (index == st) break;
    }
    return 0;
}

void random_ip(char *ip)
{
    sprintf(ip, "%d.%d.%d.%d", rand() % 223 + 1, rand() % 255, rand() % 255, rand() % 254 + 1);
}

void random_mac(char *mac)
{
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", rand() % 256, rand() % 256, rand() % 256, rand() % 256, rand() % 256, rand() % 256);
}

void map(HashT table[], const char *url, HashT *entry)
{
    if (search(table, url, entry))
        printf("Search Hash Table for '%s': Found matching entry.\n", url);
    else
    {
        printf(" -> Search Hash Table for '%s': Match Not Found! Generating Dynamic Values...\n", url);
        char new_ip[100], new_mac[100];
        random_ip(new_ip);
        random_mac(new_mac);
        insert(table, url, new_ip, new_mac);
        search(table, url, entry);
        printf("[Allocated] IP: %s | MAC: %s\n", entry->ip, entry->mac);
    }
}

void preload(HashT table[])
{
    insert(table, "google.com", "142.250.190.46", "00:1A:2B:3C:4D:5E");
    insert(table, "youtube.com", "208.65.153.238", "1A:2B:3C:4D:5E:6F");
    insert(table, "facebook.com", "157.240.22.35", "3C:4D:5E:6F:7A:8B");
    insert(table, "amazon.com", "54.239.28.85", "5E:6F:7A:8B:9C:0D");
    insert(table, "wikipedia.org", "198.35.26.96", "7A:8B:9C:0D:1E:2F");
}

int read_file(const char *filename, char *msg, int max_len)
{
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    if (fgets(msg, max_len, file) != NULL)
    {
        int len = strlen(msg);
        if (len > 0 && (msg[len - 1] == '\n' || msg[len - 1] == '\r'))
            msg[len - 1] = '\0';
    }
    else
    {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

void char_to_bin(char ch, char *bin)
{
    int i;
    for (i = 7; i >= 0; i--)
        bin[7 - i] = ((int)ch) & (1 << i) ? '1' : '0';
    bin[8] = '\0';
}

void app_bitstream(const char *msg, char *stream)
{
    stream[0] = '\0';
    char temp[9];
    int i;
    for (i = 0; msg[i] != '\0'; i++)
    {
        char_to_bin(msg[i], temp);
        strcat(stream, temp);
    }
}

int random_port()
{
    return (rand() % (65535 - 1024 + 1)) + 1024;
}

void int_to_bin(int val, char *bin)
{
    int i;
    for (i = 15; i >= 0; i--)
        bin[15 - i] = (val & (1 << i)) ? '1' : '0';
    bin[16] = '\0';
}

void transport_stream(const char *src_port_bin, const char *dest_port_bin, const char *app_stream, char *trans_stream)
{
    strcpy(trans_stream, src_port_bin);
    strcat(trans_stream, dest_port_bin);
    strcat(trans_stream, app_stream);
}

void ip_to_bin(const char *ip, char *bin)
{
    int octets[4];
    sscanf(ip, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]);

    bin[0] = '\0';
    char temp[9];
    int i;
    for (i = 0; i < 4; i++)
    {
        char_to_bin((char)octets[i], temp);
        strcat(bin, temp);
    }
}

void create_network_layer_unit(const char *src_ip_bin, const char *dest_ip_bin, const char *trans_stream, char *net_stream)
{
    strcpy(net_stream, src_ip_bin);
    strcat(net_stream, dest_ip_bin);
    strcat(net_stream, trans_stream);
}

int build_packets(const char *net_stream, const char *src_ip, const char *dest_ip, Packet packets[])
{
    int net_len = strlen(net_stream);
    int p_idx = 0;
    int curr_bit = 0;

    while (curr_bit < net_len)
    {
        packets[p_idx].packet_no = p_idx + 1;
        strcpy(packets[p_idx].src_ip, src_ip);
        strcpy(packets[p_idx].dest_ip, dest_ip);

        int packet_chars = 0;
        while (packet_chars < 32 && curr_bit < net_len)
        {
            packets[p_idx].data[packet_chars] = net_stream[curr_bit];
            packet_chars++;
            curr_bit++;
        }

        int actual_size = packet_chars;
        while (packet_chars < 32)
        {
            packets[p_idx].data[packet_chars] = '0';
            packet_chars++;
        }
        packets[p_idx].data[32] = '\0';
        packets[p_idx].size = actual_size;

        p_idx++;
    }
    return p_idx;
}

void mac_to_binary_48bit(const char *mac_str, char *binary_str)
{
    unsigned int bytes[6];
    sscanf(mac_str, "%x:%x:%x:%x:%x:%x", &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);

    binary_str[0] = '\0';
    char temp[9];
    int i;
    for (i = 0; i < 6; i++)
    {
        char_to_bin((char)bytes[i], temp);
        strcat(binary_str, temp);
    }
}

int build_frames(Packet packets[], int total_packets, const char *src_mac, const char *dest_mac, Frame frames[])
{
    int f_idx = 0;
    int p;
    for (p = 0; p < total_packets; p++)
    {
        int curr_bit = 0;
        int total_curr_bits = packets[p].size;

        while (curr_bit < 32)
        {
            frames[f_idx].frame_no = f_idx + 1;
            strcpy(frames[f_idx].src_mac, src_mac);
            strcpy(frames[f_idx].dest_mac, dest_mac);
            frames[f_idx].packet_no = packets[p].packet_no;

            int actual_bits_in_frame = 0;
            if (total_curr_bits > curr_bit)
            {
                actual_bits_in_frame = total_curr_bits - curr_bit;
                if (actual_bits_in_frame > 16) actual_bits_in_frame = 16;
            }

            int padding = 16 - actual_bits_in_frame;
            int b;
            for (b = 0; b < padding; b++)
                frames[f_idx].data[b] = '0';
            for (b = 0; b < actual_bits_in_frame; b++)
                frames[f_idx].data[padding + b] = packets[p].data[curr_bit + b];

            frames[f_idx].data[16] = '\0';
            strcpy(frames[f_idx].trailer, "00000000");

            curr_bit += 16;
            f_idx++;
        }
    }
    return f_idx;
}

void display_encapsulation_flow(int app_len, int trans_len, int net_len, int total_packets, int total_frames)
{
    printf("Application Layer Data    = %3d bits\n", app_len);
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  (Encap Transport Header: +32 bits)\n");
    printf("       v  \n");
    printf("       v  \n");
    printf("Transport Layer Data Unit = %3d bits\n", trans_len);
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  (Encap Network Header:   +64 bits)\n");
    printf("       v  \n");
    printf("       v  \n");
    printf("Network Layer Data Unit   = %3d bits\n", net_len);
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("Split into 32-bit Payload Chunks...\n");
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("       v  \n");
    printf("Split into 32-bit Packets with IP Headers...\n");
    printf("(Packets Generated            = %3d packets)\n", total_packets);
    printf("Split into 16-bit Frames with MAC Headers and Trailers...\n");
    printf("Total Layer-2 Frames Built    = %3d frames\n\n", total_frames);
}


int main()
{
    srand(72);
    HashT table[20];
    initialise_hash(table);
    preload(table);
    char src_url[100];
    char dest_url[100];
    printf("Enter Source URL (e.g., google.com): ");
    scanf("%s", src_url);
    printf("Enter Destination URL (e.g., myserver.net): ");
    scanf("%s", dest_url);
    printf("\n");

    HashT src, dest;
    printf("--- PART 1: URL MAPPING ---\n");
    map(table, src_url, &src);
    map(table, dest_url, &dest);
    printf("\nResolved Source: %s -> IP: %s -> MAC: %s\n", src.url, src.ip, src.mac);
    printf("Resolved Dest:   %s -> IP: %s -> MAC: %s\n", dest.url, dest.ip, dest.mac);
    printf("--------------------------------------------------------------------\n\n");

    char msg[1000] = {0};
    if (read_file("message.txt", msg, sizeof(msg)) == 0)
    {
        printf("ERROR! message.txt could not be read. Please verify file matches requirements.\n");
        return 1;
    }

    char app_stream[16384] = {0};
    app_bitstream(msg, app_stream);
    int app_stream_size = strlen(app_stream);

    printf("--- PART 2: APPLICATION LAYER ---\n");
    printf("Original Message: %s\n\n", msg);
    printf("Character-wise Binary Conversion:\n");
    int i;
    for (i = 0; msg[i] != '\0'; i++)
    {
        char bin[9];
        char_to_bin(msg[i], bin);
        printf(" '%c' -> %s\n", msg[i], bin);
    }
    printf("\nComplete Application Bitstream:\n%s\n", app_stream);
    printf("Total Application Layer Bits: %d bits\n", app_stream_size);
    printf("--------------------------------------------------------------------\n\n");

    int src_port = random_port();
    int dest_port = random_port();
    char src_port_bin[17], dest_port_bin[17];
    int_to_bin(src_port, src_port_bin);
    int_to_bin(dest_port, dest_port_bin);

    char trans_stream[16384] = {0};
    transport_stream(src_port_bin, dest_port_bin, app_stream, trans_stream);

    printf("--- PART 3: TRANSPORT LAYER ---\n");
    printf("Source Port: %d (Binary: %s)\n", src_port, src_port_bin);
    printf("Dest Port:   %d (Binary: %s)\n", dest_port, dest_port_bin);
    printf("Transport Header Added: %s%s\n", src_port_bin, dest_port_bin);
    printf("Transport Layer Data Unit Bitstream:\n%s\n", trans_stream);
    printf("Total Transport Layer Size: %d bits\n", (int)strlen(trans_stream));
    printf("--------------------------------------------------------------------\n\n");

    char src_ip_bin[33], dest_ip_bin[33];
    ip_to_bin(src.ip, src_ip_bin);
    ip_to_bin(dest.ip, dest_ip_bin);

    char net_stream[16384] = {0};
    create_network_layer_unit(src_ip_bin, dest_ip_bin, trans_stream, net_stream);

    printf("--- PART 4: NETWORK LAYER ---\n");
    printf("Source IP Address: %s (Binary: %s)\n", src.ip, src_ip_bin);
    printf("Dest IP Address:   %s (Binary: %s)\n", dest.ip, dest_ip_bin);
    printf("Network Header Formed: %s%s\n", src_ip_bin, dest_ip_bin);
    printf("Network Layer Data Unit Bitstream:\n%s\n", net_stream);
    printf("Total Network Layer Size: %d bits\n\n", (int)strlen(net_stream));

    printf(">> Initializing Packet Formation <<\n");
    Packet packets[500];
    int total_packets = build_packets(net_stream, src.ip, dest.ip, packets);
    for (i = 0; i < total_packets; i++)
    {
        printf("    [Packet %d] -> Src IP: %s | Dest IP: %s | Size: %d bits | Data: %s\n",
               packets[i].packet_no, packets[i].src_ip, packets[i].dest_ip,
               packets[i].size, packets[i].data);
    }
    printf("Total Number of Packets Generated: %d\n", total_packets);
    printf("--------------------------------------------------------------------\n\n");

    char src_mac_bin[49], dest_mac_bin[49];
    mac_to_binary_48bit(src.mac, src_mac_bin);
    mac_to_binary_48bit(dest.mac, dest_mac_bin);

    printf("--- PART 5: DATA LINK LAYER ---\n");
    printf("Source MAC Address: %s (Binary: %s)\n", src.mac, src_mac_bin);
    printf("Dest MAC Address:   %s (Binary: %s)\n", dest.mac, dest_mac_bin);
    printf("MAC Header Formed:   %s%s\n\n", src_mac_bin, dest_mac_bin);

    printf(">> Initializing Framing <<\n");
    Frame frames[1000];
    int total_frames = build_frames(packets, total_packets, src.mac, dest.mac, frames);
    for (i = 0; i < total_frames; i++)
    {
        printf("    [Frame %d] -> Encap Pkt: #%d | Src MAC: %s | Dest MAC: %s | Data: %s | Trailer: %s\n",
               frames[i].frame_no, frames[i].packet_no, frames[i].src_mac,
               frames[i].dest_mac, frames[i].data, frames[i].trailer);
    }
    printf("Total Number of Frames Generated: %d\n", total_frames);
    printf("--------------------------------------------------------------------\n\n");

    printf("--- PART 6: ENCAPSULATION VISUALIZATION & WRAP-UP ---\n");
    display_encapsulation_flow(app_stream_size, (int)strlen(trans_stream), (int)strlen(net_stream), total_packets, total_frames);
    printf("====================================================================\n");

    return 0;
}
