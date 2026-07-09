#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define TABLE_SIZE 19
#define MAX_STR 100
#define MAX_BITS 10000

// --- Structure Definitions ---

typedef struct {
    char url[MAX_STR];
    char ip[MAX_STR];
    char mac[MAX_STR];
    int is_occupied;
} HashEntry;

typedef struct {
    int packet_num;
    char src_ip_bin[33];
    char dest_ip_bin[33];
    char payload[11]; // 10 bits + null terminator
} Packet;

typedef struct {
    int frame_num;
    char src_mac_bin[49];
    char dest_mac_bin[49];
    int encap_packet_num;
    char payload[8];  // 7 bits + null terminator
    char trailer[5];  // Simulating 4-bit trailer
} Frame;

// --- Function Prototypes ---

// Part 1: Hash Table and Resolution
int calculate_hash(const char *url);
void init_table(HashEntry table[]);
void insert_table(HashEntry table[], const char *url, const char *ip, const char *mac);
int search_table(HashEntry table[], const char *url);
void generate_random_ip(char *ip);
void generate_random_mac(char *mac);
void resolve_url(HashEntry table[], const char *url, char *resolved_ip, char *resolved_mac);

// Part 2: Application Layer
void read_message_file(char *message);
void char_to_binary(char ch, char *bin_str);
void generate_app_bitstream(const char *message, char *bitstream);

// Part 3: Transport Layer
int generate_port();
void port_to_binary(int port, char *bin_port);
void create_transport_layer(int src_port, int dest_port, const char *app_bits, char *transport_unit);

// Part 4: Network Layer
void ip_to_binary(const char *ip, char *bin_ip);
int create_packets(const char *transport_unit, const char *src_ip_bin, const char *dest_ip_bin, Packet packets[]);

// Part 5: Data Link Layer
void mac_to_binary(const char *mac, char *bin_mac);
int create_frames(Packet packets[], int packet_count, const char *src_mac_bin, const char *dest_mac_bin, Frame frames[]);

// Visualizations and Summaries
void display_encapsulation(const char *app_data, const char *trans_unit, const char *src_ip_bin, const char *dest_ip_bin, int packet_count, int frame_count);
void display_summary(const char *src_url, const char *dest_url, const char *src_ip, const char *dest_ip,
                     const char *src_mac, const char *dest_mac, int src_port, int dest_port,
                     int msg_len, int app_bits, int total_packets, int total_frames);
void run_simulator();

// --- Main Function ---
int main() {
    srand((unsigned int)time(NULL));
    run_simulator();
    return 0;
}

// --- Function Implementations ---

// Part 1 Functions
int calculate_hash(const char *url) {
    int sum = 0;
    for (int i = 0; url[i] != '\0'; i++) {
        sum += url[i];
    }
    return sum % TABLE_SIZE;
}

void init_table(HashEntry table[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i].is_occupied = 0;
    }
}

void insert_table(HashEntry table[], const char *url, const char *ip, const char *mac) {
    int index = calculate_hash(url);
    int start_index = index;
    
    while (table[index].is_occupied) {
        index = (index + 1) % TABLE_SIZE;
        if (index == start_index) {
            printf("Error: Hash Table is full!\n");
            return;
        }
    }
    strcpy(table[index].url, url);
    strcpy(table[index].ip, ip);
    strcpy(table[index].mac, mac);
    table[index].is_occupied = 1;
}

int search_table(HashEntry table[], const char *url) {
    int index = calculate_hash(url);
    int start_index = index;
    
    while (table[index].is_occupied) {
        if (strcmp(table[index].url, url) == 0) {
            return index;
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == start_index) break;
    }
    return -1;
}

void generate_random_ip(char *ip) {
    sprintf(ip, "%d.%d.%d.%d", rand() % 256, rand() % 256, rand() % 256, rand() % 256);
}

void generate_random_mac(char *mac) {
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
            rand() % 256, rand() % 256, rand() % 256, rand() % 256, rand() % 256, rand() % 256);
}

void resolve_url(HashEntry table[], const char *url, char *resolved_ip, char *resolved_mac) {
    int idx = search_table(table, url);
    if (idx != -1) {
        printf("  [Hit] URL '%s' found in cache.\n", url);
        strcpy(resolved_ip, table[idx].ip);
        strcpy(resolved_mac, table[idx].mac);
    } else {
        printf("  [Miss] URL '%s' not found. Performing dynamic generation...\n", url);
        generate_random_ip(resolved_ip);
        generate_random_mac(resolved_mac);
        insert_table(table, url, resolved_ip, resolved_mac);
        printf("  [Saved] Generated & cached Address Info -> IP: %s | MAC: %s\n", resolved_ip, resolved_mac);
    }
}

// Part 2 Functions
void read_message_file(char *message) {
    FILE *file = fopen("message.txt", "r");
    if (!file) {
        printf("Error: message.txt not found! Using a fallback message.\n");
        strcpy(message, "NetSim");
        return;
    }
    if (fgets(message, MAX_STR, file) != NULL) {
        // Strip trailing newline character if present
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }
    } else {
        strcpy(message, "NetSim");
    }
    fclose(file);
}

void char_to_binary(char ch, char *bin_str) {
    for (int i = 7; i >= 0; i--) {
        bin_str[7 - i] = (ch & (1 << i)) ? '1' : '0';
    }
    bin_str[8] = '\0';
}

void generate_app_bitstream(const char *message, char *bitstream) {
    bitstream[0] = '\0';
    char temp[9];
    printf("\n--- PART 2 — Application Layer ---\n");
    printf("Original Message: \"%s\"\n", message);
    printf("Character-wise Binary Conversion:\n");
    
    for (int i = 0; message[i] != '\0'; i++) {
        char_to_binary(message[i], temp);
        printf("  '%c' -> %s\n", message[i], temp);
        strcat(bitstream, temp);
    }
    printf("Complete Application Bitstream: %s\n", bitstream);
    printf("Total Application Layer Bits: %lu bits\n", strlen(bitstream));
}

// Part 3 Functions
int generate_port() {
    return (rand() % (65535 - 1024 + 1)) + 1024;
}

void port_to_binary(int port, char *bin_port) {
    for (int i = 15; i >= 0; i--) {
        bin_port[15 - i] = (port & (1 << i)) ? '1' : '0';
    }
    bin_port[16] = '\0';
}

void create_transport_layer(int src_port, int dest_port, const char *app_bits, char *transport_unit) {
    char src_bin[17], dest_bin[17];
    port_to_binary(src_port, src_bin);
    port_to_binary(dest_port, dest_bin);
    
    printf("\n--- PART 3 — Transport Layer ---\n");
    printf("Source Port: %d (Bin: %s)\n", src_port, src_bin);
    printf("Destination Port: %d (Bin: %s)\n", dest_port, dest_bin);
    
    // Header creation = Source Port Bin + Dest Port Bin
    sprintf(transport_unit, "%s%s%s", src_bin, dest_bin, app_bits);
    printf("Transport Header (Ports): %s%s\n", src_bin, dest_bin);
    printf("Transport Layer Segment (Segment Payload + Header):\n  %s\n", transport_unit);
}

// Part 4 Functions
void ip_to_binary(const char *ip, char *bin_ip) {
    int octets[4];
    sscanf(ip, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]);
    bin_ip[0] = '\0';
    
    for (int i = 0; i < 4; i++) {
        char temp[9];
        for (int j = 7; j >= 0; j--) {
            temp[7 - j] = (octets[i] & (1 << j)) ? '1' : '0';
        }
        temp[8] = '\0';
        strcat(bin_ip, temp);
    }
}

int create_packets(const char *transport_unit, const char *src_ip_bin, const char *dest_ip_bin, Packet packets[]) {
    int total_bits = strlen(transport_unit);
    int p_count = 0;
    int offset = 0;
    
    printf("\n--- PART 4 — Network Layer ---\n");
    printf("Source IP Binary      : %s\n", src_ip_bin);
    printf("Destination IP Binary : %s\n", dest_ip_bin);
    printf("\nSplitting Payload into 10-bit Packet chunks:\n");
    
    while (offset < total_bits) {
        packets[p_count].packet_num = p_count + 1;
        strcpy(packets[p_count].src_ip_bin, src_ip_bin);
        strcpy(packets[p_count].dest_ip_bin, dest_ip_bin);
        
        int k = 0;
        for (k = 0; k < 10 && offset < total_bits; k++, offset++) {
            packets[p_count].payload[k] = transport_unit[offset];
        }
        // Zero-padding rule if remaining bits are short of 10 bits
        while (k < 10) {
            packets[p_count].payload[k++] = '0';
        }
        packets[p_count].payload[10] = '\0';
        
        printf("  [Packet %02d] Src IP: %.8s... | Dest IP: %.8s... | Payload Bits: %s\n", 
               packets[p_count].packet_num, packets[p_count].src_ip_bin, packets[p_count].dest_ip_bin, packets[p_count].payload);
        
        p_count++;
    }
    printf("\nTotal Number of Packets Generated: %d\n", p_count);
    return p_count;
}

// Part 5 Functions
void mac_to_binary(const char *mac, char *bin_mac) {
    unsigned int bytes[6];
    sscanf(mac, "%X:%X:%X:%X:%X:%X", &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);
    bin_mac[0] = '\0';
    
    for (int i = 0; i < 6; i++) {
        char temp[9];
        for (int j = 7; j >= 0; j--) {
            temp[7 - j] = (bytes[i] & (1 << j)) ? '1' : '0';
        }
        temp[8] = '\0';
        strcat(bin_mac, temp);
    }
}

int create_frames(Packet packets[], int packet_count, const char *src_mac_bin, const char *dest_mac_bin, Frame frames[]) {
    int f_count = 0;
    
    printf("\n--- PART 5 — Data Link Layer ---\n");
    printf("Source MAC Binary      : %s\n", src_mac_bin);
    printf("Destination MAC Binary : %s\n", dest_mac_bin);
    printf("\nSplitting Packet Payloads into 7-bit Frame chunks:\n");
    
    for (int p = 0; p < packet_count; p++) {
        char *payload = packets[p].payload;
        int total_bits = strlen(payload);
        int offset = 0;
        
        while (offset < total_bits) {
            frames[f_count].frame_num = f_count + 1;
            strcpy(frames[f_count].src_mac_bin, src_mac_bin);
            strcpy(frames[f_count].dest_mac_bin, dest_mac_bin);
            frames[f_count].encap_packet_num = packets[p].packet_num;
            strcpy(frames[f_count].trailer, "0000"); // Standard simulation Trailer zeros
            
            int k = 0;
            for (k = 0; k < 7 && offset < total_bits; k++, offset++) {
                frames[f_count].payload[k] = payload[offset];
            }
            // Zero padding rule if remaining packet chunk is short of 7 bits
            while (k < 7) {
                frames[f_count].payload[k++] = '0';
            }
            frames[f_count].payload[7] = '\0';
            
            printf("  [Frame %02d] Src MAC: %.8s... | Dest MAC: %.8s... | Encap Packet: %02d | Payload: %s | Trailer: %s\n",
                   frames[f_count].frame_num, frames[f_count].src_mac_bin, frames[f_count].dest_mac_bin, 
                   frames[f_count].encap_packet_num, frames[f_count].payload, frames[f_count].trailer);
            
            f_count++;
        }
    }
    printf("\nTotal Number of Frames Generated: %d\n", f_count);
    return f_count;
}

// Part 6 Function
void display_encapsulation(const char *app_data, const char *trans_unit, const char *src_ip_bin, const char *dest_ip_bin, int packet_count, int frame_count) {
    printf("\n--- PART 6 — Encapsulation Visualization ---\n");
    printf(" [Application Layer]\n");
    printf("    Data: %s\n", app_data);
    printf("      ↓\n");
    printf(" [Transport Layer]\n");
    printf("    Segment: [ TH (SrcPort + DestPort) ] + %s\n", app_data);
    printf("      ↓\n");
    printf(" [Network Layer]\n");
    printf("    Combined Packet Stream Payload Length: %lu bits\n", strlen(trans_unit));
    printf("    Total Packets Structured into 10-bit units: %d packets\n", packet_count);
    printf("      ↓\n");
    printf(" [Data Link Layer]\n");
    printf("    Frames Structured into [ MH (SrcMAC + DestMAC) ] + 7-bit Payload + [ Trailer (0000) ]\n");
    printf("    Total Frames Processed for Physical Layer Delivery: %d frames\n", frame_count);
}

// Part 7 Function
void display_summary(const char *src_url, const char *dest_url, const char *src_ip, const char *dest_ip,
                     const char *src_mac, const char *dest_mac, int src_port, int dest_port,
                     int msg_len, int app_bits, int total_packets, int total_frames) {
    printf("\n=================================================================\n");
    printf("                    PART 7 — FINAL SUMMARY                      \n");
    printf("=================================================================\n");
    printf(" DIRECTION:  %s  ----->  %s\n", src_url, dest_url);
    printf("-----------------------------------------------------------------\n");
    printf(" METRIC                     SOURCE                        DESTINATION\n");
    printf("-----------------------------------------------------------------\n");
    printf(" IP Address                 %-29s %-30s\n", src_ip, dest_ip);
    printf(" MAC Address                %-29s %-30s\n", src_mac, dest_mac);
    printf(" Port Number                %-29d %-30d\n", src_port, dest_port);
    printf("-----------------------------------------------------------------\n");
    printf(" TRANSMISSION STATISTICS:\n");
    printf("  * Original Text Message Length : %d Characters\n", msg_len);
    printf("  * Application Stream Size      : %d Bits\n", app_bits);
    printf("  * Total Network Packets        : %d Packets\n", total_packets);
    printf("  * Total Data Link Frames       : %d Frames\n", total_frames);
    printf("=================================================================\n");
}

// Orchestrator Function
void run_simulator() {
    HashEntry table[TABLE_SIZE];
    init_table(table);
    
    // Preload system DNS records
    insert_table(table, "google.com", "142.250.190.46", "00:1A:2B:3C:4D:5E");
    insert_table(table, "youtube.com", "208.65.153.238", "11:22:33:44:55:66");
    insert_table(table, "facebook.com", "31.13.65.36", "AA:BB:CC:DD:EE:FF");
    insert_table(table, "amazon.com", "54.239.28.85", "A1:B2:C3:D4:E5:F6");
    insert_table(table, "wikipedia.org", "198.35.26.96", "FE:DC:BA:98:76:54");

    char src_url[MAX_STR], dest_url[MAX_STR];
    printf("============================================================\n");
    printf("         NETWORK LAYERED TRANSMISSION SIMULATOR            \n");
    printf("============================================================\n");
    printf("Enter Source URL (e.g., google.com): ");
    scanf("%99s", src_url);
    printf("Enter Destination URL (e.g., customsite.org): ");
    scanf("%99s", dest_url);

    // Part 1: Address Resolution
    char src_ip[MAX_STR], src_mac[MAX_STR];
    char dest_ip[MAX_STR], dest_mac[MAX_STR];
    
    printf("\n--- PART 1 — URL Resolution ---\n");
    resolve_url(table, src_url, src_ip, src_mac);
    resolve_url(table, dest_url, dest_ip, dest_mac);

    // Part 2: Application Layer Execution
    char message[MAX_STR];
    char app_bitstream[MAX_BITS];
    read_message_file(message);
    generate_app_bitstream(message, app_bitstream);

    // Part 3: Transport Layer Execution
    int src_port = generate_port();
    int dest_port = generate_port();
    char transport_unit[MAX_BITS];
    create_transport_layer(src_port, dest_port, app_bitstream, transport_unit);

    // Part 4: Network Layer Execution
    char src_ip_bin[33], dest_ip_bin[33];
    ip_to_binary(src_ip, src_ip_bin);
    ip_to_binary(dest_ip, dest_ip_bin);
    
    Packet packets[500];
    int total_packets = create_packets(transport_unit, src_ip_bin, dest_ip_bin, packets);

    // Part 5: Data Link Layer Execution
    char src_mac_bin[49], dest_mac_bin[49];
    mac_to_binary(src_mac, src_mac_bin);
    mac_to_binary(dest_mac, dest_mac_bin);
    
    Frame frames[1000];
    int total_frames = create_frames(packets, total_packets, src_mac_bin, dest_mac_bin, frames);

    // Part 6: Encapsulation Visualization
    display_encapsulation(app_bitstream, transport_unit, src_ip_bin, dest_ip_bin, total_packets, total_frames);

    // Part 7: Summary Evaluation Display
    display_summary(src_url, dest_url, src_ip, dest_ip, src_mac, dest_mac, 
                    src_port, dest_port, (int)strlen(message), (int)strlen(app_bitstream), 
                    total_packets, total_frames);
}