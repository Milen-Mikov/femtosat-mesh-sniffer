// structures for various data packets,
// used for data logging, lora transmission.

// this is taken from iss-software/src/packet.h, then modified a good amount
// see page 82 of matt's diseration for some background
// subject to change if necessary

// packet types
#define PTYPE_GGA           1    // gps, nmea::GgaData
#define PTYPE_RMC           2    // gps, nmea::RmcData
#define PTYPE_ACC           3    // high g accel
#define PTYPE_IMU           4    // 9 axis imu
#define PTYPE_TC            5    // thermocouple
#define PTYPE_PRS           6    // pressure sensor
#define PTYPE_SPEC          7    // spectrometer
#define PTYPE_LORA_NO_GPS   20   // lora broadcast, while there's no GPS fix
#define PTYPE_LORA_HAS_GPS  21   // lora broadcast, while there's a GPS fix

// high g accel
// type PTYPE_ACC
struct acc_t {
  uint16_t t;       // time since computer start, in tenths of a second. 
                    // so t = 3 means that you're 0.3 seconds into flight
  int16_t accelx;   // x acceleration, hold data * 10
  int16_t accely;   // y acceleration, hold data * 10
  int16_t accelz;   // z acceleration, hold data * 10
};                  // 8 bytes

// 9 axis imu
// type PTYPE_IMU
struct imu_t {
  uint16_t t;       // time since computer start, in seconds * 10
  int16_t angvelx;  // x acceleration in 10 * m/s/s
  int16_t angvely;  // y acceleration in 10 * m/s/s
  int16_t angvelz;  // z acceleration in 10 * m/s/s,
  int16_t accelx;   // x angular velocity in 10 * deg/s
  int16_t accely;   // y angular velocity in 10 * deg/s
  int16_t accelz;   // z angular velocity in 10 * deg/s
  uint16_t packing; // memory alignment packing
};                  // 16 bytes

// thermocouple
// type PTYPE_TC
struct tc_t {
  uint16_t t;       // time since computer start, in seconds * 10
  int16_t internal; // internal temp, in 10 * deg C
  int16_t data[6];  // 6 thermocouple channels, 2 bytes each. in 10 * deg C
};                  // 16 bytes for 6 channels

// pressure sensor
// type PTYPE_PRS
struct prs_t {
  uint16_t t;       // time since computer start, in seconds * 10
  uint16_t data[5]; // 5 pressure channels, 2 bytes each
};                  // 12 bytes for 5 channels

// GPS, NMEA RMC sentence
// type PTYPE_RMC
struct rmc_t {
  uint16_t t;       // time since computer start, in seconds * 10
  uint16_t packing; // memory alignment packing
  uint16_t time[4]; // hh:mm:ss:us UTC GPS time
  float lat;
  float lon;
  float speed;
  float heading;
};                  // 26 bytes

// GPS, NMEA GGA sentence
// type PTYPE_GGA
struct gga_t {
  uint16_t t;       // time since computer start, in seconds * 10
  uint16_t packing; // memory alignment packing
  uint16_t time[4]; // utc time hh:mm:ss:us 
  float lat;
  float lon;        
  float hdop;       // Horizontal dilution of precision.
  float alt;        // Height above mean sea level.
};                  // 26 bytes

// spectrometer
// type PTYPE_SPEC
// see matt's diseration 
struct spec_t {
  uint16_t t;       // time since computer start, in seconds * 10
  uint8_t data[6];  // bin averages. 6 bins, 1 byte each
  uint8_t peaks[6]; // bin peaks. 6 bins, 1 byte each
  uint16_t packing; // memory alignment packing
  uint32_t itime;   // integration time
};                  // 18 bytes

// lora packet, no GPS data
// type PTYPE_LORA_NO_GPS
// sends a snapshot of the capsule's current sensor data when there's no GPS fix
struct lora_t {
  uint8_t id_type;        // id for the type of lora packet
  uint8_t id;             // unique id of capsule
  uint16_t t;             // time since computer start, in seconds * 10
  acc_t accel_data;       // 8 bytes
  imu_t imu_data;         // 16 bytes
  tc_t  tc_data;          // 16 bytes
  prs_t pressure_data;    // 12 bytes
  spec_t spec_data;       // 20 bytes
};                        // 76 bytes

// lora packet, with GPS data
// type PTYPE_LORA_HAS_GPS
// sends a snapshot of the capsule's current sensor data, w/ gps data stapled on the end
struct lora_t_with_gps {
  lora_t sensor_data;     // 76 bytes
  rmc_t rmc_data;         // 28 bytes
  gga_t gga_data;         // 28 bytes
};                        // 132 bytes