#ifndef E1000_DEF_H_
#define E1000_DEF_H_

//#define E1000_NUM_RX_DESC 2048 //2048
//#define E1000_NUM_TX_DESC 2048
//#define RX_PACKET_SIZE    2048 //2048

//#define E1000_NUM_RX_DESC 16384 //2048
//#define E1000_NUM_TX_DESC 16384

#define E1000_NUM_RX_DESC 8192 //2048
#define E1000_NUM_TX_DESC 8192

//#define E1000_NUM_RX_DESC 32768 //2048
//#define E1000_NUM_TX_DESC 32768


#define RX_PACKET_SIZE    1600 //2048

#define INTEL_VEND  0x8086
#define E1000_DEV   0x100E
#define E1000_DEV1  0x153A
#define E1000_DEV2  0x10EA
#define E1000_DEV3  0x100F
#define E1000_DEV4  0x1004

#define E1000_CTRL_PHY_RST    0x80000000

#define REG_CTRL        0x0000
#define REG_STATUS      0x0008
#define REG_EEPROM      0x0014
#define REG_CTRL_EXT    0x0018
#define REG_ICR         0x00C0
#define REG_IMASK       0x00D0
#define REG_RCTRL       0x0100
#define REG_RXDESCLO    0x2800
#define REG_RXDESCHI    0x2804
#define REG_RXDESCLEN   0x2808
#define REG_RXDESCHEAD  0x2810
#define REG_RXDESCTAIL  0x2818

#define REG_TCTRL       0x0400
#define REG_TXDESCLO    0x3800
#define REG_TXDESCHI    0x3804
#define REG_TXDESCLEN   0x3808
#define REG_TXDESCHEAD  0x3810
#define REG_TXDESCTAIL  0x3818


#define REG_RDTR         0x2820 // RX Delay Timer Register
#define REG_RXDCTL       0x3828 // RX Descriptor Control
#define REG_RADV         0x282C // RX Int. Absolute Delay Timer
#define REG_RSRPD        0x2C00 // RX Small Packet Detect Interrupt



#define REG_TIPG         0x0410 // Transmit Inter Packet Gap



/*#define RCTRL_EN        0x00000002
#define RCTRL_SBP       0x00000004
#define RCTRL_UPE       0x00000008
#define RCTRL_MPE       0x00000010
#define RCTRL_8192      0x00030000*/

#define ECTRL_ASDE      0x20//auto speed enable
#define ECTRL_FD        0x01//FULL DUPLEX
#define ECTRL_SLU       0x40//set link up
#define ECTRL_100M      0x100//set link up
#define ECTRL_1000M     0x200//set link up
#define ECTRL_FRCSPD    0x800//set link up


#define RCTL_EN                         (1 << 1)    // Receiver Enable
#define RCTL_SBP                        (1 << 2)    // Store Bad Packets
#define RCTL_UPE                        (1 << 3)    // Unicast Promiscuous Enabled
#define RCTL_MPE                        (1 << 4)    // Multicast Promiscuous Enabled
#define RCTL_LPE                        (1 << 5)    // Long Packet Reception Enable
#define RCTL_LBM_NONE                   (0 << 6)    // No Loopback
#define RCTL_LBM_PHY                    (3 << 6)    // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF                 (0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER              (1 << 8)    // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH               (2 << 8)    // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36                      (0 << 12)   // Multicast Offset - bits 47:36
#define RCTL_MO_35                      (1 << 12)   // Multicast Offset - bits 46:35
#define RCTL_MO_34                      (2 << 12)   // Multicast Offset - bits 45:34
#define RCTL_MO_32                      (3 << 12)   // Multicast Offset - bits 43:32
#define RCTL_BAM                        (1 << 15)   // Broadcast Accept Mode
#define RCTL_VFE                        (1 << 18)   // VLAN Filter Enable
#define RCTL_CFIEN                      (1 << 19)   // Canonical Form Indicator Enable
#define RCTL_CFI                        (1 << 20)   // Canonical Form Indicator Bit Value
#define RCTL_DPF                        (1 << 22)   // Discard Pause Frames
#define RCTL_PMCF                       (1 << 23)   // Pass MAC Control Frames
#define RCTL_SECRC                      (1 << 26)   // Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256                  (3 << 16)
#define RCTL_BSIZE_512                  (2 << 16)
#define RCTL_BSIZE_1024                 (1 << 16)
#define RCTL_BSIZE_2048                 (0 << 16)
#define RCTL_BSIZE_4096                 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192                 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384                ((1 << 16) | (1 << 25))



// Transmit Command

#define CMD_EOP                         (1 << 0)    // End of Packet
#define CMD_IFCS                        (1 << 1)    // Insert FCS
#define CMD_IC                          (1 << 2)    // Insert Checksum
#define CMD_RS                          (1 << 3)    // Report Status
#define CMD_RPS                         (1 << 4)    // Report Packet Sent
#define CMD_VLE                         (1 << 6)    // VLAN Packet Enable
#define CMD_IDE                         (1 << 7)    // Interrupt Delay Enable


// TCTL Register

#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */

#define TCTL_EN                         0x00000002  // Transmit Enable
#define TCTL_PSP                        0x00000008  // Pad Short Packets
#define TCTL_CT                         0x00000ff0  // Collision Threshold
#define TCTL_COLD_SHIFT                 0x003ff000  // Collision Distance
#define TCTL_SWXOFF                     (1 << 22)   // Software XOFF Transmission
#define TCTL_RTLC                       0x10000000  // Re-transmit on Late Collision
#define TCTL_COLLISION_THRESHOLD        15
#define TCTL_CT_SHIFT                   4
#define TSTA_DD                         (1 << 0)    // Descriptor Done
#define TSTA_EC                         (1 << 1)    // Excess Collisions
#define TSTA_LC                         (1 << 2)    // Late Collision
#define LSTA_TU                         (1 << 3)    // Transmit Underrun

#define E1000_MODELS_COUNT              0x5
#endif