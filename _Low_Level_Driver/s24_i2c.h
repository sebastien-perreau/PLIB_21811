#ifndef __DEF_I2C
#define	__DEF_I2C

typedef enum
{
    I2C1 = 0,
    I2C2,
    I2C3,
    I2C4,
    I2C5,
    I2C_NUMBER_OF_MODULES
} I2C_MODULE;

typedef enum
{
    I2C_FREQUENCY_100KHZ        = 100,
    I2C_FREQUENCY_400KHZ        = 400,
    I2C_FREQUENCY_1MHZ          = 1000
} I2C_FREQUENCY;

typedef enum
{
    I2C_STOP_ON_IDLE            = 0x00002000,
    I2C_CONTINUE_ON_IDLE        = 0x00000000,
            
    I2C_SLEW_RATE_NORMAL_SPEED  = 0x00000200,   // 100 KHz & 1 MHz  (Standard mode & Fast mode Plus))
    I2C_SLEW_RATE_HIGH_SPEED    = 0x00000000,   // 400 KHz          (Fast mode)
            
    I2C_ENABLE_SMBUS            = 0x00000100,
    I2C_DISABLE_SMBUS           = 0x00000000
} I2C_CONFIGURATION;

typedef enum
{
    I2C_USE_7BIT_ADDRESS                = 0x00000000,
    I2C_USE_10BIT_ADDRESS               = 0x00000400,
    I2C_ENABLE_GENERAL_CALL_ADDRESS     = 0x00000080,
    I2C_USE_RESERVED_ADDRESSES          = 0x00000800
} I2C_ADDRESS_MODE;

typedef enum
{
    _HOME = 0,
    _START,
    _RESTART,
    _ADDRESS_COMPONENT_LSB,
    _ADDRESS_COMPONENT_MSB,
    _ADDRESS_REGISTER_LSB,
    _ADDRESS_REGISTER_MSB,
    _SEND_BYTE,
    _READ_BYTE,
    _WAIT_AND_VERIFY,
    _STOP,
    _FAIL
} I2C_STATE_MACHIN;

typedef enum
{
    I2C_SEND_ADDRESS_COMPONENT_LSB  = (1 << 0),
    I2C_SEND_ADDRESS_COMPONENT_MSB  = (1 << 1),
    I2C_SEND_ADDRESS_REGISTERS_LSB  = (1 << 2),
    I2C_SEND_ADDRESS_REGISTERS_MSB  = (1 << 3),
} I2C_FLAGS;

typedef struct
{
    I2C_MODULE              module;
    uint16_t                address_component;
    uint16_t                address_registers;
    BUS_MANAGEMENT_PARAMS   bus_management_params;
    uint32_t                flags;
    state_machine_t         state_machine;
} I2C_PARAMS;

#define I2C_PARAMS_INSTANCE(_module, _address, _periodic_time, _flags)             \
{                                                   \
    .module = _module,                              \
    .address_component = _address,                  \
    .address_registers = 0,                         \
    .bus_management_params =                        \
    {                                               \
        .is_running = false,                        \
        .waiting_period = _periodic_time,           \
        .tick = -1                                  \
    },                                              \
    .flags = _flags,                                \
    .state_machine = {0}                            \
}

typedef union 
{
  struct 
  {
    unsigned SEN:1;
    unsigned RSEN:1;
    unsigned PEN:1;
    unsigned RCEN:1;
    unsigned ACKEN:1;
    unsigned ACKDT:1;
    unsigned STREN:1;
    unsigned GCEN:1;
    unsigned SMEN:1;
    unsigned DISSLW:1;
    unsigned A10M:1;
    unsigned STRICT:1;
    unsigned SCLREL:1;
    unsigned SIDL:1;
    unsigned :1;
    unsigned BUS_ON:1;
    unsigned :16;
  };
  struct 
  {
    unsigned w:32;
  };
} __I2CxCONbits_t;

typedef union 
{
  struct 
  {
    unsigned TBF:1;
    unsigned RBF:1;
    unsigned R_W:1;
    unsigned S:1;
    unsigned P:1;
    unsigned D_A:1;
    unsigned I2COV:1;
    unsigned IWCOL:1;
    unsigned ADD10:1;
    unsigned GCSTAT:1;
    unsigned BCL:1;
    unsigned :3;
    unsigned TRSTAT:1;
    unsigned ACKSTAT:1;
    unsigned :16;
  };
  struct 
  {
    unsigned w:32;
  };
} __I2CxSTATbits_t;

typedef struct
{
    union 
    {
        volatile uint32_t	I2CCON;
        volatile __I2CxCONbits_t I2CCONbits;
    };
	volatile uint32_t	I2CCONCLR;
	volatile uint32_t	I2CCONSET;
	volatile uint32_t	I2CCONINV;
    
    union 
    {
        volatile uint32_t	I2CSTAT;
        volatile __I2CxSTATbits_t I2CSTATbits;
    };
	volatile uint32_t	I2CSTATCLR;
	volatile uint32_t	I2CSTATSET;
	volatile uint32_t	I2CSTATINV;
    
	volatile uint32_t	I2CADD;
	volatile uint32_t	I2CADDCLR;
	volatile uint32_t	I2CADDSET;
	volatile uint32_t	I2CADDINV;
    
    volatile uint32_t	I2CMSK;
	volatile uint32_t	I2CMSKCLR;
	volatile uint32_t	I2CMSKSET;
	volatile uint32_t	I2CMSKINV;
    
    volatile uint32_t	I2CBRG;
	volatile uint32_t	I2CBRGCLR;
	volatile uint32_t	I2CBRGSET;
	volatile uint32_t	I2CBRGINV;
    
    volatile uint32_t	I2CTX;
	volatile uint32_t	I2CTXCLR;
	volatile uint32_t	I2CTXSET;
	volatile uint32_t	I2CTXINV;
    
    volatile uint32_t	I2CRX;
	volatile uint32_t	I2CRXCLR;
	volatile uint32_t	I2CRXSET;
	volatile uint32_t	I2CRXINV;
} I2C_REGISTERS;

void i2c_init_as_master(    I2C_MODULE id, 
                            serial_event_handler_t evt_handler,
                            I2C_FREQUENCY frequency,
                            I2C_CONFIGURATION configuration);
void i2c_enable(I2C_MODULE id, bool enable);
void i2c_configuration(I2C_MODULE id, I2C_CONFIGURATION configuration);
void i2c_set_frequency(I2C_MODULE id, I2C_FREQUENCY frequency);

void i2c_start(I2C_MODULE id);
void i2c_restart(I2C_MODULE id);
void i2c_stop(I2C_MODULE id);
void i2c_receiver_active_sequence(I2C_MODULE id);
bool i2c_get_byte(I2C_MODULE id, uint8_t *data);
void i2c_send_ack(I2C_MODULE id, bool v_ack);
bool i2c_is_ack_send(I2C_MODULE id);
bool i2c_is_ack_received(I2C_MODULE id);
bool i2c_send_byte(I2C_MODULE id, uint8_t data);
bool i2c_is_byte_transmitted(I2C_MODULE id);
bool i2c_is_busy(I2C_MODULE id);

void i2c_set_slave_address(I2C_MODULE id, uint32_t address, uint32_t mask, I2C_ADDRESS_MODE mode);
void i2c_interrupt_handler(I2C_MODULE id, IRQ_EVENT_TYPE evt_type, uint32_t data);

#endif
