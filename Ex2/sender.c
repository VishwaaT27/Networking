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
    int packet_no;
    char data[17];
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

int build_frames(Packet packets[], int total_packets, Frame frames[])
{
    int f_idx = 0;
    int p;
    for (p = 0; p < total_packets; p++)
    {
        frames[f_idx].frame_no = f_idx + 1;
        frames[f_idx].packet_no = packets[p].packet_no;
        strncpy(frames[f_idx].data, packets[p].data, 16);
        frames[f_idx].data[16] = '\0';
        f_idx++;

        frames[f_idx].frame_no = f_idx + 1;
        frames[f_idx].packet_no = packets[p].packet_no;
        strncpy(frames[f_idx].data, packets[p].data + 16, 16);
        frames[f_idx].data[16] = '\0';
        f_idx++;
    }
    return f_idx;
}

unsigned short bin16_to_int(const char *bin)
{
    unsigned short val = 0;
    int i;
    for (i = 0; i < 16; i++)
        val = (val << 1) | (bin[i] - '0');
    return val;
}

void calculate_fcs(const char *addr, const char *ctrl, const char *proto, const char *payload16, char *fcs_bin)
{
    char concat[100] = {0};
    strcpy(concat, addr);
    strcat(concat, ctrl);
    strcat(concat, proto);
    strcat(concat, payload16);

    unsigned int sum = 0;
    int len = strlen(concat);
    int i;

    for (i = 0; i < len; i += 16)
    {
        char word_bin[17];
        strncpy(word_bin, concat + i, 16);
        word_bin[16] = '\0';
        unsigned short word_val = bin16_to_int(word_bin);
        sum += word_val;
        if (sum > 0xFFFF)
            sum = (sum & 0xFFFF) + 1;
    }
    unsigned short checksum = ~((unsigned short)sum);
    int_to_bin(checksum, fcs_bin);
}

void byte_stuff(const char *in, char *out)
{
    int i, j = 0;
    int len = strlen(in);
    char current_byte[9];
    for (i = 0; i < len; i += 8)
    {
        strncpy(current_byte, in + i, 8);
        current_byte[8] = '\0';
        if (strcmp(current_byte, "01111110") == 0 || strcmp(current_byte, "01111101") == 0)
        {
            strcpy(out + j, "01111101");
            j += 8;
        }
        strcpy(out + j, current_byte);
        j += 8;
    }
    out[j] = '\0';
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
        printf("ERROR! message.txt could not be read.\n");
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

    printf("--- PART 5: DATA LINK LAYER (PPP FRAMING & BYTE STUFFING) ---\n");

    Frame frames[1000];
    int total_frames = build_frames(packets, total_packets, frames);

    int proto;
    char proto_bin[17] = {0};

    printf("Select Protocol\n");
    printf("1. LCP   (0xC021)\n");
    printf("2. PAP   (0xC023)\n");
    printf("3. CHAP  (0xC223)\n");
    printf("4. NCP   (0x8021)\n");
    printf("Enter choice: ");
    scanf("%d", &proto);

    switch (proto)
    {
    case 1: strcpy(proto_bin, "1100000000100001"); break;
    case 2: strcpy(proto_bin, "1100000000100011"); break;
    case 3: strcpy(proto_bin, "1100001000100011"); break;
    case 4: strcpy(proto_bin, "1000000000100001"); break;
    default: strcpy(proto_bin, "0000000000000000"); break;
    }
    printf("\n");

    FILE *t_file = fopen("transmission.txt", "w");
    if (!t_file) return 1;

    char flag[] = "01111110";
    char address[] = "11111111";
    char control[] = "00000011";

    for (i = 0; i < total_frames; i++)
    {
        char fcs_bin[17] = {0};
        char stuffed_payload[200] = {0};
        char final_frame[500] = {0};

        calculate_fcs(address, control, proto_bin, frames[i].data, fcs_bin);
        byte_stuff(frames[i].data, stuffed_payload);

        strcpy(final_frame, flag);
        strcat(final_frame, address);
        strcat(final_frame, control);
        strcat(final_frame, proto_bin);
        strcat(final_frame, stuffed_payload);
        strcat(final_frame, fcs_bin);
        strcat(final_frame, flag);

        printf("Frame %d (Encap Packet #%d)\n", frames[i].frame_no, frames[i].packet_no);
        printf("Original Payload:\n%s\n", frames[i].data);
        printf("Checksum Generated:\n%s\n", fcs_bin);
        printf("Stuffed Payload:\n%s\n", stuffed_payload);
        printf("Final PPP Frame:\n%s\n\n", final_frame);

        fprintf(t_file, "%s\n", final_frame);
    }

    fclose(t_file);
    printf("Transmitted frames written to transmission.txt successfully.\n");
    printf("Total Frames Transmitted: %d\n", total_frames);
    printf("====================================================================\n");

    return 0;
}
