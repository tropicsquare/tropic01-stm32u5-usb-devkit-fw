#ifndef GPREG_H
#define	GPREG_H

// RAM-retention registers used for reboot infos

#define GPREG_BOOT_POR           0x00000000UL // default
#define	GPREG_BOOT_REBOOT        0x00000100UL // 
#define GPREG_BOOT_STAY_IN_BOOT  0x0000AABBUL // 
#define GPREG_WDID_REBOOT_RQ     0x0000b098UL

#define GPREG_WR_ENABLE

#define GPREG_BOOT  TAMP->BKP0R // information for bootloader (if present)
#define GPREG_WDID  TAMP->BKP1R // watchdog-reset reason

#define	GPREG_WRITE(reg, value) {GPREG_WR_ENABLE; reg=value;}


#endif // ! GPREG_H

