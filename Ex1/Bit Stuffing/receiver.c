#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bit_destuff(const char *in_stuffed, char *out_original)
{
    int i, j = 0, count = 0;
    for (i = 0; in_stuffed[i] != '\0'; i++)
    {
        out_original[j++] = in_stuffed[i];
        if (in_stuffed[i] == '1')
        {
            count++;
        }
        else
        {
            count = 0;
        }

        if (count == 5)
        {
            if (in_stuffed[i + 1] == '0')
            {
                i++; 
                count = 0;
            }
            else if (in_stuffed[i + 1] == '1')
            {
                printf("ERROR IN DATA\n");
                return 0;
            }
        }
    }
    out_original[j] = '\0';
    return 1;
}

int bin_to_int(const char *bin, int len)
{
    int val = 0, i;
    for (i = 0; i < len; i++)
    {
        val = (val << 1) | (bin[i] - '0');
    }
    return val;
}

char bin_to_char(const char *bin)
{
    return (char)bin_to_int(bin, 8);
}

void bin_to_ip(const char *bin, char *ip)
{
    int o1 = bin_to_int(bin, 8);
    int o2 = bin_to_int(bin + 8, 8);
    int o3 = bin_to_int(bin + 16, 8);
    int o4 = bin_to_int(bin + 24, 8);
    sprintf(ip, "%d.%d.%d.%d", o1, o2, o3, o4);
}

int main()
{
    FILE *file = fopen("transmission.txt", "r");
    if (!file)
    {
        printf("No transmitted frames available.\n");
        return 0;
    }

    char raw_line[500];
    if (fgets(raw_line, sizeof(raw_line), file) == NULL)
    {
        printf("No transmitted frames available.\n");
        fclose(file);
        return 0;
    }
    rewind(file);

    printf("--- PART 1: RECEIVER PROCESSING LAYER-2 FRAMES ---\n");
    char network_stream[16384] = {0};
    int frame_counter = 1;

    while (fgets(raw_line, sizeof(raw_line), file) != NULL)
    {
        int line_len = strlen(raw_line);
        while (line_len > 0 && (raw_line[line_len - 1] == '\n' || raw_line[line_len - 1] == '\r'))
        {
            raw_line[line_len - 1] = '\0';
            line_len--;
        }

        printf("Received Frame %d: %s\n", frame_counter, raw_line);

        // Frame offsets setup based on structural lengths
        // Beginning flag(8) + MAC Hdr(48+48=96) + Data Start flag(8) = 112
        char mac_header[97];
        strncpy(mac_header, raw_line + 8, 96);
        mac_header[96] = '\0';
        printf("  Extracted Header : %s\n", mac_header);

        // Find positions of inner flags for dynamic payload isolation
        // Look after the first 112 characters for the Data End Flag
        char stuffed_data[100] = {0};
        int idx = 112;
        int s_idx = 0;
        while (strncmp(raw_line + idx, "01111110", 8) != 0)
        {
            stuffed_data[s_idx++] = raw_line[idx++];
        }
        stuffed_data[s_idx] = '\0';
        printf("  Stuffed Data     : %s\n", stuffed_data);

        char destuffed_data[100] = {0};
        if (!bit_destuff(stuffed_data, destuffed_data))
        {
            printf("  Frame %d dropped due to processing error.\n\n", frame_counter);
            frame_counter++;
            continue; 
        }
        printf("  Destuffed Data   : %s\n", destuffed_data);

        // Drop the 16-bit payload padding down to the actual payload bits received
        // Since frames stack right-aligned bits from build_frames, skip leading padded zeros
        int destuffed_len = strlen(destuffed_data);
        if (destuffed_len > 16) {
            strcat(network_stream, destuffed_data + (destuffed_len - 16));
        } else {
            strcat(network_stream, destuffed_data);
        }
        
        frame_counter++;
        printf("\n");
    }
    fclose(file);

    printf("--------------------------------------------------------------------\n\n");
    printf("--- PART 2: LAYER PROTOCOL REVERSAL DEENCAPSULATION ---\n");
    printf("Recovered Network Stream:\n%s\n\n", network_stream);

    // Network Layer Header Reversal (64 bits total: 32 bit Src IP + 32 bit Dest IP)
    char src_ip_bin[33], dest_ip_bin[33];
    strncpy(src_ip_bin, network_stream, 32); src_ip_bin[32] = '\0';
    strncpy(dest_ip_bin, network_stream + 32, 32); dest_ip_bin[32] = '\0';

    char src_ip[50], dest_ip[50];
    bin_to_ip(src_ip_bin, src_ip);
    bin_to_ip(dest_ip_bin, dest_ip);
    printf("Recovered Packet info -> Src IP: %s | Dest IP: %s\n", src_ip, dest_ip);

    // Transport Layer Segment Isolation
    char transport_stream[16384];
    strcpy(transport_stream, network_stream + 64);
    printf("Recovered Transport Stream:\n%s\n\n", transport_stream);

    // Transport Header Reversal (32 bits total: 16 bit Src Port + 16 bit Dest Port)
    char src_port_bin[17], dest_port_bin[17];
    strncpy(src_port_bin, transport_stream, 16); src_port_bin[16] = '\0';
    strncpy(dest_port_bin, transport_stream + 16, 16); dest_port_bin[16] = '\0';
    printf("Recovered Transport Header -> Src Port: %d | Dest Port: %d\n", 
           bin_to_int(src_port_bin, 16), bin_to_int(dest_port_bin, 16));

    // Application stream extraction
    char application_stream[16384];
    strcpy(application_stream, transport_stream + 32);
    printf("Recovered Application Stream:\n%s\n\n", application_stream);

    // Reconstruct ASCII Text Characters
    char final_message[2000] = {0};
    int app_len = strlen(application_stream);
    int char_idx = 0;
    int i;
    for (i = 0; i + 8 <= app_len; i += 8)
    {
        char temp_byte[9];
        strncpy(temp_byte, application_stream + i, 8);
        temp_byte[8] = '\0';
        
        char ascii_char = bin_to_char(temp_byte);
        if (ascii_char == '\0') break; // Handle end of string marker
        final_message[char_idx++] = ascii_char;
    }
    final_message[char_idx] = '\0';

    printf("Recovered Message: %s\n", final_message);
    printf("--------------------------------------------------------------------\n\n");

    FILE *out_file = fopen("output.txt", "w");
    if (out_file)
    {
        fprintf(out_file, "%s\n", final_message);
        fclose(out_file);
        printf("Recovered message successfully.\n");
    }
    return 0;
}