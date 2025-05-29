#ifndef RESET_H
#define	RESET_H

typedef enum {

	RESET_POWER_ON = 0,
	RESET_USER_RQ,
	RESET_WDT,
	RESET_BOD,

} reset_type_e;

reset_type_e reset_get_type(void);
void reset_clear(void);

#endif // ! RESET_H


