#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int byte_destuff(const char *in, char *out)
{
    int i = 0, j = 0;
    int len = strlen(in);
    char current_byte[9];
    char next_byte[9];

    while (i < len)
    {
        strncpy(current_byte, in + i, 8);
        current_byte[8] = '\0';

        if (strcmp(current_byte, "01111101") == 0)
        {
            if (i + 8 >= len)
            {
                printf("ERROR IN DATA\n");
                return 0;
            }
            strncpy(next_byte, in + i + 8, 8);
            next_byte[8] = '\0';

            if (strcmp(next_byte, "01111110") == 0 || strcmp(next_byte, "01111101") == 0)
            {
                strcpy(out + j, next_byte);
                j += 8;
                i += 16;
            }
            else
            {
                printf("ERROR IN DATA\n");
                return 0;
            }
        }
        else
        {
            strcpy(out + j, current_byte);
            j += 8;
            i += 8;
        }
    }
    out[j] = '\0';
    return 1;
}

unsigned short bin16_to_int(const char *bin)
{
    unsigned short val = 0;
    int i;
    for (i = 0; i < 16; i++)
        val = (val << 1) | (bin[i] - '0');
    return val;
}

int verify_checksum(const char *addr, const char *ctrl, const char *proto, const char *payload, const char *received_fcs)
{
    char concat[200] = {0};
    strcpy(concat, addr);
    strcat(concat, ctrl);
    strcat(concat, proto);
    strcat(concat, payload);
    strcat(concat, received_fcs);

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

    return ((unsigned short)sum == 0xFFFF);
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

    printf("--- RECEIVER PROCESSING ---\n\n");
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

        if (line_len < 48) continue;

        printf("Received Frame %d:\n%s\n", frame_counter, raw_line);

        char start_flag[9], end_flag[9];
        strncpy(start_flag, raw_line, 8); start_flag[8] = '\0';
        strncpy(end_flag, raw_line + line_len - 8, 8); end_flag[8] = '\0';

        if (strcmp(start_flag, "01111110") != 0 || strcmp(end_flag, "01111110") != 0)
        {
            printf("Frame corrupted (Invalid Flags)\n\n");
            frame_counter++;
            continue;
        }

        char addr[9], ctrl[9], proto[17], fcs[17];
        strncpy(addr, raw_line + 8, 8); addr[8] = '\0';
        strncpy(ctrl, raw_line + 16, 8); ctrl[8] = '\0';
        strncpy(proto, raw_line + 24, 16); proto[16] = '\0';
        strncpy(fcs, raw_line + line_len - 24, 16); fcs[16] = '\0';

        int stuffed_payload_len = line_len - 8 - 8 - 8 - 16 - 16 - 8;
        char stuffed_payload[300] = {0};
        strncpy(stuffed_payload, raw_line + 40, stuffed_payload_len);
        stuffed_payload[stuffed_payload_len] = '\0';

        printf("Extracted Address: %s\n", addr);
        printf("Extracted Control: %s\n", ctrl);
        printf("Extracted Protocol: %s\n", proto);
        printf("Stuffed Payload: %s\n", stuffed_payload);

        char destuffed_payload[200] = {0};
        if (!byte_destuff(stuffed_payload, destuffed_payload))
        {
            printf("Checksum Status: Frame corrupted (Discarded)\n\n");
            frame_counter++;
            continue;
        }

        printf("Destuffed Payload: %s\n", destuffed_payload);

        if (verify_checksum(addr, ctrl, proto, destuffed_payload, fcs))
        {
            printf("Checksum Status: Valid (Accepted)\n\n");
            strcat(network_stream, destuffed_payload);
        }
        else
        {
            printf("Checksum Status: Frame corrupted (Discarded)\n\n");
        }

        frame_counter++;
    }
    fclose(file);

    printf("--------------------------------------------------------------------\n\n");
    printf("--- LAYER PROTOCOL DE-ENCAPSULATION ---\n\n");
    printf("Recovered Network Stream:\n%s\n\n", network_stream);

    char src_ip_bin[33], dest_ip_bin[33];
    strncpy(src_ip_bin, network_stream, 32); src_ip_bin[32] = '\0';
    strncpy(dest_ip_bin, network_stream + 32, 32); dest_ip_bin[32] = '\0';

    char src_ip[50], dest_ip[50];
    bin_to_ip(src_ip_bin, src_ip);
    bin_to_ip(dest_ip_bin, dest_ip);

    char transport_stream[16384];
    strcpy(transport_stream, network_stream + 64);
    printf("Recovered Transport Stream:\n%s\n\n", transport_stream);

    char src_port_bin[17], dest_port_bin[17];
    strncpy(src_port_bin, transport_stream, 16); src_port_bin[16] = '\0';
    strncpy(dest_port_bin, transport_stream + 16, 16); dest_port_bin[16] = '\0';

    char application_stream[16384];
    strcpy(application_stream, transport_stream + 32);
    printf("Recovered Application Stream:\n%s\n\n", application_stream);

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
        if (ascii_char == '\0') break;
        final_message[char_idx++] = ascii_char;
    }
    final_message[char_idx] = '\0';

    printf("Recovered Message: %s\n\n", final_message);

    FILE *out_file = fopen("output.txt", "w");
    if (out_file)
    {
        fprintf(out_file, "%s", final_message);
        fclose(out_file);
        printf("Recovered message successfully.\n");
    }

    return 0;
}
