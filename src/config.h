// pin defines, radio settings, etc

// SPI defines. 
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_MOSI 19
#define PIN_SCK 18
#define PIN_CS 17
#define PIN_DIO1 22
#define PIN_LORA_RST 21
#define PIN_BUSY 26

// LoRa defines
// see https://unsigned.io/understanding-lora-parameters/
#define LORA_FREQ               915.0
#define LORA_BANDWIDTH          500.0
#define LORA_SPREADINGFACTOR    9
#define LORA_CODINGRATE         7
#define LORA_SYNCWORD           RADIOLIB_SX126X_SYNC_WORD_PRIVATE // 0x12
#define LORA_TX_POWER           -9      // in dBm. DO NOT EXCEED 9 dBm! 
                                        // set to lowest option (-9dBm) to prevent overcurrent on USB power
#define LORA_PREAMBLE_LENGTH    8
#define LORA_TXCO_VOLTAGE       0
#define LORA_USE_LDO            true