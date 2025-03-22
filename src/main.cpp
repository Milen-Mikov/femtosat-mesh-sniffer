#include <stdio.h>
#include "pico/stdlib.h"

#include "config.h"
#include "packet_defs.h"

#include "hardware/spi.h"
#include "RadioLib.h"
#include "hal/RPiPico/PicoHal.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// HAL for radiolib
static PicoHal myHal(SPI_PORT, PIN_MISO, PIN_MOSI, PIN_SCK);
// LoRa radio object
static SX1262 LoRaModule = SX1262(new Module( &myHal, PIN_CS, PIN_DIO1, PIN_LORA_RST, PIN_BUSY));


int main()
{
    stdio_init_all();

    // waiting for serial monitor
    sleep_ms(5000);

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
    }

    // starting LoRa module
    printf("[SX1262] Initializing ...\n");

    int16_t LoRaState = LoRaModule.begin(
        LORA_FREQ, 
        LORA_BANDWIDTH, 
        LORA_SPREADINGFACTOR, 
        LORA_CODINGRATE, 
        LORA_SYNCWORD, 
        LORA_TX_POWER, 
        LORA_PREAMBLE_LENGTH, 
        LORA_TXCO_VOLTAGE, 
        LORA_USE_LDO
    );

    // if radio fails to init, keep retrying
    if (LoRaState != RADIOLIB_ERR_NONE) {

        printf("[SX1262] Radio init failed, code %d\n", LoRaState);

        while (LoRaState != RADIOLIB_ERR_NONE) {

            LoRaState = LoRaModule.begin(
                LORA_FREQ, 
                LORA_BANDWIDTH, 
                LORA_SPREADINGFACTOR, 
                LORA_CODINGRATE, 
                LORA_SYNCWORD, 
                LORA_TX_POWER, 
                LORA_PREAMBLE_LENGTH, 
                LORA_TXCO_VOLTAGE, 
                LORA_USE_LDO
            );

            printf("[SX1262] Radio init failed, code %d\n", LoRaState);
            sleep_ms(1000);
        }
    }

    // Once radio init works, continue
    printf("[SX1262] Radio init success\n");

    // setting up RF switch. LR62XE's RF switch setup is identical to the one found
    // in the SX1262DVK1CAS reference design schematic. DIO2 can be used to toggle
    // RF switch if ANTSW is held high and DIO2 is toggled. 
    // DIO2 should be high to TX, low to RX.
    // RadioLib already has function to handle this automatically.

    while (LoRaModule.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) { // retry until dio2 is set as rf switch
        printf("[SX1262] Failed to set DIO2 as RF switch! \n");
        sleep_ms(1000);
    }

    // turning on rxboostedgain. better rx gain at the cost of power
    if (LoRaModule.setRxBoostedGainMode(true, true) != RADIOLIB_ERR_NONE) { 
        // this isnt as important so retrying is unneeded
        printf("[SX1262] Failed to enable RX boosted gain mode! \n"); 
    }

    uint8_t lora_rxbuffer[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    lora_t* rx_packet_no_gps; // pointer to a lora_t packet
    lora_t_with_gps* rx_packet_gps; // pointer to gps packet

    printf("[SX1262] Starting Receive loop. \n");

    // receive loop
    while (true) {

        // blocking receive
        LoRaState = LoRaModule.receive(lora_rxbuffer, 0);

        // if lora packet was received
        if (LoRaState == RADIOLIB_ERR_NONE) {
            switch (lora_rxbuffer[0]) { // check lora packet id

            case PTYPE_LORA_NO_GPS:     // no gps packet

            // casting rx_buffer as a lora_t packet
            rx_packet_no_gps = (lora_t*)lora_rxbuffer;

            printf("Non GPS packet received. Capsule ID: %d. Packet time: %d. \n", rx_packet_no_gps->id, rx_packet_no_gps->t);
            printf("IMU Data: Accel X: %d, Accel Y: %d, Accel Z: %d, Ang Vel X: %d, Ang Vel Y: %d, Ang Vel Z: %d",
            rx_packet_no_gps->imu_data.accelx,
            rx_packet_no_gps->imu_data.accely,
            rx_packet_no_gps->imu_data.accelz,
            rx_packet_no_gps->imu_data.angvelx,
            rx_packet_no_gps->imu_data.angvely,
            rx_packet_no_gps->imu_data.angvelz);

            printf("BME280 Data: Temperature: %d, Humidity: %d, Pressure: %d",
            rx_packet_no_gps->accel_data.accelx,
            rx_packet_no_gps->accel_data.accely,
            rx_packet_no_gps->accel_data.accelz);

            break;

            case PTYPE_LORA_HAS_GPS:     // gps packet
            // casting rx_buffer as a gps packet
            rx_packet_gps = (lora_t_with_gps*)lora_rxbuffer;

            printf("Non GPS packet received. Capsule ID: %d. Packet time: %d. \n", rx_packet_gps->sensor_data.id, rx_packet_gps->sensor_data.t);
            printf("IMU Data: Accel X: %d, Accel Y: %d, Accel Z: %d, Ang Vel X: %d, Ang Vel Y: %d, Ang Vel Z: %d",
            rx_packet_gps->sensor_data.imu_data.accelx,
            rx_packet_gps->sensor_data.imu_data.accely,
            rx_packet_gps->sensor_data.imu_data.accelz,
            rx_packet_gps->sensor_data.imu_data.angvelx,
            rx_packet_gps->sensor_data.imu_data.angvely,
            rx_packet_gps->sensor_data.imu_data.angvelz);

            printf("BME280 Data: Temperature: %d, Humidity: %d, Pressure: %d",
            rx_packet_gps->sensor_data.accel_data.accelx,
            rx_packet_gps->sensor_data.accel_data.accely,
            rx_packet_gps->sensor_data.accel_data.accelz);

            break;

            default: // not either type of packet
            printf("Non mesh packet received. Discarding. \n");
            break;

            }
        }
        
        // if receive timed out
        else if (LoRaState == RADIOLIB_ERR_RX_TIMEOUT) {
            // nothing
        }

        // if some other error
        else {
            printf("[SX1262] Receive failure. Status code: %d \n", LoRaState);
        }
    }
}
