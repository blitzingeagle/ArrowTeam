/*
 * File:   px_ascii.c
 * Author: morris
 *
 * Created on January 29, 2018, 10:52 PM
 */

#include <stdbool.h>

#include "px_ascii.h"

px_char new_px_char(uint32_t a, uint32_t b) {
    px_char p;
    p.part[1] = a;
    p.part[0] = b;
    return p;
}

inline px_char px_map(char c) {
    // <editor-fold defaultstate="collapsed" desc="PX Mapping">
    switch(c) {
        case 32: return PX_32;
        case 33: return PX_33;
        case 34: return PX_34;
        case 35: return PX_35;
        case 36: return PX_36;
        case 37: return PX_37;
        case 38: return PX_38;
        case 39: return PX_39;
        case 40: return PX_40;
        case 41: return PX_41;
        case 42: return PX_42;
        case 43: return PX_43;
        case 44: return PX_44;
        case 45: return PX_45;
        case 46: return PX_46;
        case 47: return PX_47;
        case 48: return PX_48;
        case 49: return PX_49;
        case 50: return PX_50;
        case 51: return PX_51;
        case 52: return PX_52;
        case 53: return PX_53;
        case 54: return PX_54;
        case 55: return PX_55;
        case 56: return PX_56;
        case 57: return PX_57;
        case 58: return PX_58;
        case 59: return PX_59;
        case 60: return PX_60;
        case 61: return PX_61;
        case 62: return PX_62;
        case 63: return PX_63;
        case 64: return PX_64;
        case 65: return PX_65;
        case 66: return PX_66;
        case 67: return PX_67;
        case 68: return PX_68;
        case 69: return PX_69;
        case 70: return PX_70;
        case 71: return PX_71;
        case 72: return PX_72;
        case 73: return PX_73;
        case 74: return PX_74;
        case 75: return PX_75;
        case 76: return PX_76;
        case 77: return PX_77;
        case 78: return PX_78;
        case 79: return PX_79;
        case 80: return PX_80;
        case 81: return PX_81;
        case 82: return PX_82;
        case 83: return PX_83;
        case 84: return PX_84;
        case 85: return PX_85;
        case 86: return PX_86;
        case 87: return PX_87;
        case 88: return PX_88;
        case 89: return PX_89;
        case 90: return PX_90;
        case 91: return PX_91;
        case 92: return PX_92;
        case 93: return PX_93;
        case 94: return PX_94;
        case 95: return PX_95;
        case 96: return PX_96;
        case 97: return PX_97;
        case 98: return PX_98;
        case 99: return PX_99;
        case 100: return PX_100;
        case 101: return PX_101;
        case 102: return PX_102;
        case 103: return PX_103;
        case 104: return PX_104;
        case 105: return PX_105;
        case 106: return PX_106;
        case 107: return PX_107;
        case 108: return PX_108;
        case 109: return PX_109;
        case 110: return PX_110;
        case 111: return PX_111;
        case 112: return PX_112;
        case 113: return PX_113;
        case 114: return PX_114;
        case 115: return PX_115;
        case 116: return PX_116;
        case 117: return PX_117;
        case 118: return PX_118;
        case 119: return PX_119;
        case 120: return PX_120;
        case 121: return PX_121;
        case 122: return PX_122;
        case 123: return PX_123;
        case 124: return PX_124;
        case 125: return PX_125;
        case 126: return PX_126;
        default: return PX_32;
    }
    // </editor-fold>
}

void print_px_char(int x, int y, char c) {
    draw_px_char(x, y, px_map(c));
}

void print_px_string(int x, int y, char *s) {
    int idx = 0;
    while(s[idx] != '\0') {
        print_px_char(x+idx*(PX_CHAR_WIDTH+1), y, s[idx]);
        
        idx++;
    }
}

