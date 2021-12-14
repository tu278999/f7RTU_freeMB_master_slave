#ifndef USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START                    0
#define S_DISCRETE_INPUT_NDISCRETES               8//16

#define S_COIL_START                              0
#define S_COIL_NCOILS                             8//64

#define S_REG_INPUT_START                         0
#define S_REG_INPUT_NREGS                         16//100

#define S_REG_HOLDING_START                       0		//sever has to read in this field if client write in to it
#define S_REG_HOLDING_NREGS                       16//100

/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0

/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0


/* -----------------------Master Defines -------------------------------------*/

#define M_DISCRETE_INPUT_START                    0
#define M_DISCRETE_INPUT_NDISCRETES               16

#define M_COIL_START                              0
#define M_COIL_NCOILS                             16

#define M_REG_INPUT_START                         0
#define M_REG_INPUT_NREGS                         8

#define M_REG_HOLDING_START                       0
#define M_REG_HOLDING_NREGS                       8

/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

#endif
