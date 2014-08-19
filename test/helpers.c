#include <stdio.h>

#include "helpers.h"

void print_bytes(char *buf, int len, int line, int padding) {
    for(int k = 0; k < padding; k++) {
        printf(" ");
    }
    for(int i = 1; i <= len; i++) {
        printf("%02x ", (uint8_t)buf[i-1]);
        if(i % line == 0) {
            printf("\n");
            for(int k = 0; k < padding; k++) {
                printf(" ");
            }
        }
    }
}

void print_pilot_info(sd_pilot *pilot) {
    if(pilot != NULL) {
        printf("### Pilot header for %s:\n", pilot->name);
        printf("  - Wins:        %d\n", pilot->wins);
        printf("  - Losses:      %d\n", pilot->losses);
        printf("  - Robot ID:    %d\n", pilot->robot_id);
        printf("  - Offense:     %d\n", pilot->offense);
        printf("  - Defense:     %d\n", pilot->defense);
        printf("  - Money:       %d\n", pilot->money);
        printf("  - Color:       %d,%d,%d\n", 
            pilot->color_1,
            pilot->color_2,
            pilot->color_3);

        printf("  - Stats:       ");
        print_bytes(pilot->stats, 8, 10, 0);
        printf("\n");
        printf("  - unk_block_a:\n");
        print_bytes(pilot->unk_block_a, 107, 16, 5);
        printf("\n");
        printf("  - Force arena: %d\n", pilot->force_arena);
        printf("  - unk_block_b: ");
        print_bytes(pilot->unk_block_a, 3, 16, 0);
        printf("\n");
        printf("  - Movement:    %d\n", pilot->movement);
        printf("  - unk_block_c: ");
        print_bytes(pilot->unk_block_c, 6, 16, 0);
        printf("\n");
        printf("  - Enhancement: ");
        print_bytes(pilot->enhancements, 11, 16, 0);
        printf("\n");
        printf("  - Flags:       %d\n", pilot->flags);

        printf("  - Reqs:        ");
        print_bytes((char*)pilot->reqs, 10, 16, 0);
        printf("\n");
        printf("  - Attitude:    ");
        print_bytes((char*)pilot->attitude, 6, 16, 0);
        printf("\n");
        printf("  - unk_block_d: ");
        print_bytes(pilot->unk_block_d, 6, 16, 0);
        printf("\n");

        printf("  - AP Throw:    %d\n", pilot->ap_throw);
        printf("  - AP Special:  %d\n", pilot->ap_special);
        printf("  - AP Jump:     %d\n", pilot->ap_jump);
        printf("  - AP High:     %d\n", pilot->ap_high);
        printf("  - AP Low:      %d\n", pilot->ap_low);
        printf("  - AP Middle:   %d\n", pilot->ap_middle);

        printf("  - Pref jump    %d\n", pilot->pref_jump);
        printf("  - Pref fwd     %d\n", pilot->pref_fwd);
        printf("  - Pref back    %d\n", pilot->pref_back);

        printf("  - unk_block_e: ");
        print_bytes(pilot->unk_block_d, 4, 16, 0);
        printf("\n");

        printf("  - Learning     %f\n", pilot->learning);
        printf("  - Forget       %f\n", pilot->forget);
        printf("  - Winnings     %d\n", pilot->winnings);
        printf("  - Photo ID     %d\n", pilot->photo_id);

        printf("  - unk_block_f: ");
        print_bytes(pilot->unk_block_f, 24, 26, 0);
        printf("\n");
        printf("  - unk_block_g:\n");
        print_bytes(pilot->unk_block_g, 166, 16, 5);
        printf("\n\n");
    }
}