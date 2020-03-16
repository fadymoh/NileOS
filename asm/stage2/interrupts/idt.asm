[BITS 64]
extern idt_common_stub

global idtInit
extern interruptDescriptorTablePointer  ; there is a problem here somehow when interrupt files are included!
;bydrb m3 el globalDescriptorTablePointer


idtInit:
   lidt [interruptDescriptorTablePointer]
   ret

%macro ISR_NOERRCODE 1
  [GLOBAL isr%1]
  isr%1:
    cli
    push qword 0
    push qword %1
    jmp idt_common_stub
%endmacro

%macro ISR_ERRCODE 1
  [GLOBAL isr%1]
  isr%1:
    cli
    push qword %1
    jmp idt_common_stub
%endmacro

%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push qword 0
    push qword %2
    jmp idt_common_stub
%endmacro

%macro APIRQ 2
  global apirq%1
  apirq%1:
    cli
    push qword 0
    push qword %2
    jmp idt_common_stub
%endmacro


%macro SWIRQ 2
  global swirq%1
  swirq%1:
    cli
    push qword 0
    push qword %2
    jmp idt_common_stub
%endmacro


ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31


IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47
IRQ   16,    48
IRQ   17,    49
IRQ   18,    50
IRQ   19,    51
IRQ   20,    52
IRQ   21,    53
IRQ   22,    54
IRQ   23,    55
IRQ   24,    56
IRQ   25,    57
IRQ   26,    58
IRQ   27,    59
IRQ   28,    60
IRQ   29,    61
IRQ   30,    62
IRQ   31,    63

IRQ   32,    64
IRQ   33,    65
IRQ   34,    66
IRQ   35,    67
IRQ   36,    68
IRQ   37,    69
IRQ   38,    70
IRQ   39,    71
IRQ   40,    72
IRQ   41,    73
IRQ   42,    74
IRQ   43,    75
IRQ   44,    76
IRQ   45,    77
IRQ   46,    78
IRQ   47,    79
IRQ   48,    80
IRQ   49,    81
IRQ   50,    82
IRQ   51,    83
IRQ   52,    84
IRQ   53,    85
IRQ   54,    86
IRQ   55,    87
IRQ   56,    88
IRQ   57,    89
IRQ   58,    90
IRQ   59,    91
IRQ   60,    92
IRQ   61,    93
IRQ   62,    94
IRQ   63,    95


APIRQ   0,    96
APIRQ   1,    97
APIRQ   2,    98
APIRQ   3,    99
APIRQ   4,    100
APIRQ   5,    101
APIRQ   6,    102
APIRQ   7,    103
APIRQ   8,    104
APIRQ   9,    105
APIRQ   10,    106
APIRQ   11,    107
APIRQ   12,    108
APIRQ   13,    109
APIRQ   14,    110
APIRQ   15,    111
APIRQ   16,    112
APIRQ   17,    113
APIRQ   18,    114
APIRQ   19,    115
APIRQ   20,    116
APIRQ   21,    117
APIRQ   22,    118
APIRQ   23,    119
APIRQ   24,    120
APIRQ   25,    121
APIRQ   26,    122
APIRQ   27,    123
APIRQ   28,    124
APIRQ   29,    125
APIRQ   30,    126
APIRQ   31,    127

SWIRQ   0,    128
SWIRQ   1,    129
SWIRQ   2,    130
SWIRQ   3,    131
SWIRQ   4,    132
SWIRQ   5,    133
SWIRQ   6,    134
SWIRQ   7,    135
SWIRQ   8,    136
SWIRQ   9,    137
SWIRQ   10,    138
SWIRQ   11,    139
SWIRQ   12,    140
SWIRQ   13,    141
SWIRQ   14,    142
SWIRQ   15,    143
SWIRQ   16,    144
SWIRQ   17,    145
SWIRQ   18,    146
SWIRQ   19,    147
SWIRQ   20,    148
SWIRQ   21,    149
SWIRQ   22,    150
SWIRQ   23,    151
SWIRQ   24,    152
SWIRQ   25,    153
SWIRQ   26,    154
SWIRQ   27,    155
SWIRQ   28,    156
SWIRQ   29,    157
SWIRQ   30,    158
SWIRQ   31,    159
SWIRQ   32,    160
SWIRQ   33,    161
SWIRQ   34,    162
SWIRQ   35,    163
SWIRQ   36,    164
SWIRQ   37,    165
SWIRQ   38,    166
SWIRQ   39,    167
SWIRQ   40,    168
SWIRQ   41,    169
SWIRQ   42,    170
SWIRQ   43,    171
SWIRQ   44,    172
SWIRQ   45,    173
SWIRQ   46,    174
SWIRQ   47,    175
SWIRQ   48,    176
SWIRQ   49,    177
SWIRQ   50,    178
SWIRQ   51,    179
SWIRQ   52,    180
SWIRQ   53,    181
SWIRQ   54,    182
SWIRQ   55,    183
SWIRQ   56,    184
SWIRQ   57,    185
SWIRQ   58,    186
SWIRQ   59,    187
SWIRQ   60,    188
SWIRQ   61,    189
SWIRQ   62,    190
SWIRQ   63,    191
SWIRQ   64,    192
SWIRQ   65,    193
SWIRQ   66,    194
SWIRQ   67,    195
SWIRQ   68,    196
SWIRQ   69,    197
SWIRQ   70,    198
SWIRQ   71,    199
SWIRQ   72,    200
SWIRQ   73,    201
SWIRQ   74,    202
SWIRQ   75,    203
SWIRQ   76,    204
SWIRQ   77,    205
SWIRQ   78,    206
SWIRQ   79,    207
SWIRQ   80,    208
SWIRQ   81,    209
SWIRQ   82,    210
SWIRQ   83,    211
SWIRQ   84,    212
SWIRQ   85,    213
SWIRQ   86,    214
SWIRQ   87,    215
SWIRQ   88,    216
SWIRQ   89,    217
SWIRQ   90,    218
SWIRQ   91,    219
SWIRQ   92,    220
SWIRQ   93,    221
SWIRQ   94,    222
SWIRQ   95,    223
SWIRQ   96,    224
SWIRQ   97,    225
SWIRQ   98,    226
SWIRQ   99,    227
SWIRQ   100,    228
SWIRQ   101,    229
SWIRQ   102,    230
SWIRQ   103,    231
SWIRQ   104,    232
SWIRQ   105,    233
SWIRQ   106,    234
SWIRQ   107,    235
SWIRQ   108,    236
SWIRQ   109,    237
SWIRQ   110,    238
SWIRQ   111,    239
SWIRQ   112,    240
SWIRQ   113,    241
SWIRQ   114,    242
SWIRQ   115,    243
SWIRQ   116,    244
SWIRQ   117,    245
SWIRQ   118,    246
SWIRQ   119,    247
SWIRQ   120,    248
SWIRQ   121,    249
SWIRQ   122,    250
SWIRQ   123,    251
SWIRQ   124,    252
SWIRQ   125,    253
SWIRQ   126,    254


[GLOBAL isr255]
isr255:
        iretq
