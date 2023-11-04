#pragma once

#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>
#include <span>

using namespace hal::literals;
using namespace std::chrono_literals;
           
namespace science {
class pressure_sensor_bme680
{
private:
  pressure_sensor_bme680(hal::i2c& p_i2c, hal::steady_clock& c, hal::serial& t)
    : m_i2c(p_i2c)
    , steady_clock{ c }
    , terminal{ t }
  {

  }

public:
  enum addresses : hal::byte
  {
    address = 0x77,
    write_add = 0b01110110, //7 bit device address to be 0b111011X0 (X is determined by state of SDO pin)
    forced_mode = 0x74,
    filter = 0x75,
    status_register = 0x1D,
    calib_start_address = 0x89,
    calib_start_address_2 = 0xE1,
    press_msb = 0x1F //start measuring adc values of pressure and temperatuer from here

  };


  enum index_values //calibration values 
  {
    t1_lsb = 8,
    t1_msb = 9,
    t2_lsb = 1, //8A
    t2_msb = 2, //8B
    t3 = 3, //8C
    p1_lsb = 5, //8E
    p1_msb = 6, //8F
    p2_lsb = 7, //90
    p2_msb = 8, //91
    p3 = 9,  //92
    p4_lsb = 11, //94
    p4_msb = 12, //95
    p5_lsb = 13, //96
    p5_msb = 14, //97
    p7 = 15, //98
    p6 = 16,  //99
    p8_lsb = 19, //9A 9B skipped 9C
    p8_msb = 20, //9D
    p9_lsb = 21, //9E
    p9_msb = 22, //9F
    p10 = 23 //A0
  };

  //RW is 0 when write mode and 1 when read mode
  static hal::result<pressure_sensor_bme680> create(hal::i2c& p_i2c,
                                        hal::steady_clock& clock, hal::serial& terminal)
  {
    pressure_sensor_bme680 pressure_sensor_bme680(p_i2c, clock, terminal);
    HAL_CHECK(pressure_sensor_bme680.start());
    return pressure_sensor_bme680;
  }
  hal::status start()
  {

    //set the sensor to forced mode in order to start calculations
    

    HAL_CHECK(get_callibration());
    //need to write 01 to address 0x74
    std::array<hal::byte,2> forced_mode_add = {addresses::forced_mode, 0b10001100}; //first 2 bits must be set to 01 for forced mode
    std::array<hal::byte,2> filter_add = {addresses::filter,0b00001000}; //first 2 bits must be set to 01 for forced mode


    hal::print<64>(terminal, "\ninitializing addresses\n");

    //need to test if this works
    //first 3 bits of this write address is to select temperature oversampling
    //4-6th bit to select pressure oversampling
    //7-8th bit for forced mode, which is what we need
    HAL_CHECK(hal::write(m_i2c, addresses::address, forced_mode_add,hal::never_timeout())); //change from sleep mode to forced mode
    HAL_CHECK(hal::delay(steady_clock, 1ms));

  
    HAL_CHECK(hal::write(m_i2c, addresses::address, filter_add,hal::never_timeout()));
    HAL_CHECK(hal::delay(steady_clock, 1ms));

    HAL_CHECK(set_focus_mode(0));

    hal::print<64>(terminal, "Forced mode written to\n");

    
    return hal::success();
    
  }
  hal::status set_focus_mode(int bit){
    // std::array<hal::byte,1> write1 = {0b00000001}; //T,P,H oversampling and force mode
    std::array<hal::byte,1> forced_mode_add = {addresses::forced_mode}; //first 2 bits must be set to 01 for forced mode
    std::array<hal::byte, 1> current_address;

    HAL_CHECK(hal::write_then_read(m_i2c,addresses::address,forced_mode_add, current_address, hal::never_timeout()));

    //current_address now has the current values (should be 0x54)

    hal::byte toWrite = current_address[0] | bit;

    std::array<hal::byte,2> write_address = {addresses::forced_mode,toWrite};

    HAL_CHECK(hal::write(m_i2c, addresses::address, write_address, hal::never_timeout())); 


    return hal::success();
  }
  
  hal::status get_callibration(){
    hal::print<64>(terminal, "Inside get callibration\n");
    std::array<hal::byte, 25> coeff_arr1;
    std::array<hal::byte, 15> coeff_arr2;  
    

    std::array<hal::byte,1> start_add_1 = {{addresses::calib_start_address}}; //first 2 bits must be set to 01 for forced mode
    std::array<hal::byte,1> start_add_2 = {{addresses::calib_start_address_2}}; //first 2 bits must be set to 01 for forced mode


    HAL_CHECK(hal::write_then_read(m_i2c,addresses::address, start_add_1, coeff_arr1, hal::never_timeout()));
    
    HAL_CHECK(hal::delay(steady_clock, 1ms));

    HAL_CHECK(hal::write_then_read(m_i2c,addresses::address, start_add_2, coeff_arr2, hal::never_timeout()));

    // //test print coeff array2:
    // for(int i = 0; i < 2; i ++){
    //   hal::print<64>(terminal, "Byte %x = %x\n", addresses::calib_start_address_2 + i, coeff_arr2[i]);
    // }


    _T1 = (uint16_t)HAL_CHECK(concat_byte(coeff_arr2[index_values::t1_msb], coeff_arr2[index_values::t1_lsb]));
    _T2 = (int16_t)HAL_CHECK((concat_byte(coeff_arr1[index_values::t2_msb], coeff_arr1[index_values::t2_lsb])));
    _T3 = (int8_t)(coeff_arr1[index_values::t3]);

   

    /*************************************
    ** Pressure related coefficients    **  
    *************************************/
    _P1  = (uint16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p1_msb], coeff_arr1[index_values::p1_lsb]));
    _P2  = (int16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p2_msb], coeff_arr1[index_values::p2_lsb]));
    _P3  = (int8_t)coeff_arr1[index_values::p3];
    _P4  = (int16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p4_msb], coeff_arr1[index_values::p4_lsb]));
    _P5  = (int16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p5_msb], coeff_arr1[index_values::p5_lsb]));
    _P6  = (int8_t)(coeff_arr1[index_values::p6]);
    _P7  = (int8_t)(coeff_arr1[index_values::p7]);
    _P8  = (int16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p8_msb], coeff_arr1[index_values::p8_lsb]));
    _P9  = (int16_t)HAL_CHECK(concat_byte(coeff_arr1[index_values::p9_msb], coeff_arr1[index_values::p9_lsb]));
    _P10 = (uint8_t)(coeff_arr1[index_values::p10]);

    hal::print<64>(terminal, "End callibration\n");
    return hal::success();  

  } 

  hal::status get_parsed_data(){ //not sure if this needs to happen just once or every time the pressure sensor is called to get data
    

    // hal::print<64>(terminal, "beginning of get temperature parameter\n"); 

    HAL_CHECK(set_focus_mode(1));
    std::array<hal::byte, 15> buff;                        // declare array for registers
    uint32_t adc_temp, adc_pres;                               // Raw ADC temperature and pressure
    std::array<hal::byte,1> start_adc_address = {addresses::status_register}; //first 2 bits must be set to 01 for forced mode

    HAL_CHECK(hal::write_then_read(m_i2c,addresses::address, start_adc_address, buff, hal::never_timeout()));

    // HAL_CHECK(hal::delay(steady_clock, 1ms));

    adc_pres = (uint32_t)(((uint32_t)buff[2] << 12) | ((uint32_t)buff[3] << 4) | ((uint32_t)buff[4] >> 4));  // put the 3 bytes of Pressure
    adc_temp = (uint32_t)(((uint32_t)buff[5] << 12) | ((uint32_t)buff[6] << 4) | ((uint32_t)buff[7] >> 4));  // put the 3 bytes of Temperature

    // hal::print<64>(terminal, "raw adc values: %d\n",adc_temp);

    

    // _Temperature = _Temperature / 100;

    HAL_CHECK(calculate_temperature(adc_temp));
    
    HAL_CHECK(hal::delay(steady_clock, 1ms));

    HAL_CHECK(calculate_pressure(adc_pres));

    return hal::success();

     
  }
  hal::result<int16_t> get_temperature(){
    return _Temperature;
  }
  hal::result<int32_t> get_pressure() {
    return _Pressure;
  }
  
  hal::status calculate_temperature(uint32_t adc_temp){
    int64_t var1;
    int64_t var2;
    int64_t var3;

    var1         = ((int32_t)adc_temp >> 3) - ((int32_t)_T1 << 1);  // Perform calibration/adjustment
    var2         = (var1 * (int32_t)_T2) >> 11;                     // of Temperature values according
    var3         = ((var1 >> 1) * (var1 >> 1)) >> 12;               // to formula defined by Bosch
    var3         = ((var3) * ((int32_t)_T3 << 4)) >> 14;
    _tfine       = (int32_t)(var2 + var3);
    _Temperature = (int16_t)(((_tfine * 5) + 128) >> 8);
    return hal::success();
  }

  hal::status calculate_pressure(uint32_t adc_pres){
    int32_t var1;
    int32_t var2;
    int32_t var3;
    // int32_t _Pressure;

    /* This value is used to check precedence to multiplication or division
     * in the pressure compensation equation to achieve least loss of precision and
     * avoiding overflows.
     * i.e Comparing value, pres_ovf_check = (1 << 31) >> 1
     */
    const int32_t pres_ovf_check = INT32_C(0x40000000);

    /*lint -save -e701 -e702 -e713 */
    var1 = (((int32_t)_tfine) >> 1) - 64000;
    var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)_P6) >> 2;
    var2 = var2 + ((var1 * (int32_t)_P5) << 1);
    var2 = (var2 >> 2) + ((int32_t)_P4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)_P3 << 5)) >> 3) +
           (((int32_t)_P2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)_P1) >> 15;
    _Pressure = 1048576 - adc_pres;
    _Pressure = (int32_t)((_Pressure - (var2 >> 12)) * ((uint32_t)3125));
    if (_Pressure >= pres_ovf_check)
    {
        _Pressure = ((_Pressure / var1) << 1);
    }
    else
    {
        _Pressure = ((_Pressure << 1) / var1);
    }

    var1 = ((int32_t)_P9 * (int32_t)(((_Pressure >> 3) * (_Pressure >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(_Pressure >> 2) * (int32_t)_P8) >> 13;
    var3 =
        ((int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) *
         (int32_t)_P10) >> 17;
    _Pressure = (int32_t)(_Pressure) + ((var1 + var2 + var3 + ((int32_t)_P7 << 7)) >> 4);

    /*lint -restore */
    // hal::print<64>(terminal, "%d\n", _Pressure);
    _Pressure = (uint32_t)_Pressure;
    return hal::success();
  }


  hal::result<uint16_t> concat_byte(hal::byte msb, hal::byte lsb){ //

    uint16_t result = (uint16_t)(msb << 8 | lsb);
    return result;
  }

  hal::i2c& m_i2c;
  hal::steady_clock& steady_clock;
  hal::serial& terminal;
  uint8_t  _H6, _P10;                  ///< unsigned configuration vars
  int8_t   _T3, _P3, _P6, _P7;                                        ///< signed configuration vars
  uint16_t _T1, _P1;                                ///< unsigned 16bit configuration vars
  int16_t  _T2, _P2, _P4, _P5, _P8, _P9,_Temperature;                ///< signed 16bit configuration vars
  int32_t  _tfine, _Pressure, _Humidity, _Gas;  ///< signed 32bit configuration vars

};
}  // namespace science