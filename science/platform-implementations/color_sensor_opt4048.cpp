#include "color_sensor_opt4048.hpp"
#include <libhal/units.hpp>
#include <stdlib.h>


using namespace hal::literals;
using namespace std::chrono_literals;
namespace science {
opt4048::opt4048(hal::i2c& p_i2c, hal::steady_clock& p_clock, hal::serial& p_terminal) 
    : m_i2c(p_i2c)
    , m_clock(p_clock)
    , m_terminal(p_terminal)
{
}

hal::result<opt4048> opt4048::create(hal::i2c& p_i2c, hal::steady_clock& p_clock, hal::serial& p_terminal){
    opt4048 color_sensor(p_i2c, p_clock, p_terminal);
    HAL_CHECK(color_sensor.adjust_settings()); //
    //set basic settings
    return color_sensor;
}

hal::status opt4048::adjust_settings(){
    //Datasheet: 
    //write BYTE 1: [QWAKE, 0, RANGE (0xC), conversion time part 1]
    //write BYTE 2: [converstion time part 2, operating mode, 1, 0, 11]
    // 1ms conversion time for now: 0010
    // 0xC=1100
    //settings are predetermined
    hal::byte write1 = 0b00110000;
    hal::byte write2 = 0b10110011; // guessing almost all of these. read about the latch, and the interrupts

    std::array<hal::byte, 2> to_send_array= {write1,write2};
    HAL_CHECK(hal::write(m_i2c, opt4048::device_address, to_send_array, hal::never_timeout()));

    return hal::success();
}

hal::status opt4048::read_channel(hal::byte address, std::span<hal::byte> output){
    std::array<hal::byte, 1> address_out = {address};

    HAL_CHECK(hal::write_then_read(m_i2c, opt4048::device_address, address_out, output));

    return hal::success();
}

hal::result<opt4048::adc_codes> opt4048::get_adc_codes(){ //returns x,y,
    //read x channel, y channel and z channel do mantisssa math
    //mantissa channel_x = result_msb_chx << 8 + RESULT_LSB_CHx
    //ADC_CODES_CHx = (MANTISSA_CHx<<EXPONENT_CHx)
    /*
        uint16_t byte1 = 0b11001100; //204 -> 0b110000000000
        uint16_t byte2 = 0b10011001; //153 -> 0b000010011001
        //110010011001 = 3225
        uint16_t exponent = byte1 >> 4; //1100 -> 12
        uint16_t msb = byte1 << 12; //1100000000000000
        msb = msb >> 4; //110000000000 -> 3072
        msb = msb | byte2; //110010011001 -> 3225
        cout << exponent << endl;
        cout << msb << endl;
    */ 
    std::array<uint16_t, 2> channel_0_msb;  // 0 has exponent (4 bits), and 1 has the msb (12bits)
    HAL_CHECK(get_msb_exp(register_addresses::channel0_msb, channel_0_msb));

    //do this for channel 1, 2, and 3
    std::array<uint16_t, 2> channel_1_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel1_msb, channel_1_msb));
    
    std::array<uint16_t, 2> channel_2_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel2_msb, channel_2_msb));

    std::array<uint16_t, 2> channel_3_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel3_msb, channel_3_msb));

    std::array<hal::byte, 2> channel_0_lsb;
    HAL_CHECK(read_channel(register_addresses::channel0_lsb, channel_0_lsb));
    uint8_t lsb0 = channel_0_lsb[0];

    std::array<hal::byte, 2> channel_1_lsb;
    HAL_CHECK(read_channel(register_addresses::channel1_lsb, channel_1_lsb));
    uint8_t lsb1 = channel_1_lsb[0];

    std::array<hal::byte, 2> channel_2_lsb;
    HAL_CHECK(read_channel(register_addresses::channel2_lsb, channel_2_lsb));
    uint8_t lsb2 = channel_2_lsb[0];

    std::array<hal::byte, 2> channel_3_lsb;
    HAL_CHECK(read_channel(register_addresses::channel3_lsb, channel_3_lsb));
    uint8_t lsb3 = channel_3_lsb[0];

    int32_t mantissa0 = (channel_0_msb[1] << 8) + lsb0;
    int32_t adc_codes0 = mantissa0 << channel_0_msb[0];

    int32_t mantissa1 = (channel_1_msb[1] << 8) + lsb1;
    int32_t adc_codes1 = mantissa1 << channel_1_msb[0];

    int32_t mantissa2 = (channel_2_msb[1] << 8) + lsb2;
    int32_t adc_codes2 = mantissa2 << channel_2_msb[0];

    int32_t mantissa3 = (channel_3_msb[1] << 8) + lsb3;
    int32_t adc_codes3 = mantissa3 << channel_3_msb[0];
    

    adc_codes codes;
    codes.ch0 = adc_codes0;
    codes.ch1 = adc_codes1;
    codes.ch2 = adc_codes2;
    codes.ch3 = adc_codes3;
    return codes;

}

hal::status opt4048::get_msb_exp(hal::byte register_A, std::span<uint16_t> data) {
    std::array<hal::byte, 2> output;
    HAL_CHECK(read_channel(register_A, output ));    
    uint16_t exponent = output[0] >> 4;
    uint16_t msb = output[0] << 12;
    msb = (msb >> 4) | output[1];

    data[0] = exponent;
    data[1] = msb;
    return hal::success();
}
hal::result<opt4048::xyz_values> opt4048::adc_codes_to_xyz(adc_codes adc) {
    //from datasheet
    xyz_values values;
    float ADC_to_XYZ[4][4] = { { 2.34892992e-4, -1.89652390e-5,  1.20811684e-5,       0 },
							   { 4.07467441e-5,  1.98958202e-4, -1.58848115e-5, 2.15e-3 },
							   { 9.28619404e-5, -1.69739553e-5,  6.74021520e-4,       0 },
							   {             0,              0,              0,       0 }
	};
    //matrix multipliations from the datasheet, got the actual calculations from @retrospy's Github and www.brucelindbloom.com
    values.x = adc.ch0 * ADC_to_XYZ[0][0] + adc.ch1 * ADC_to_XYZ[1][0] + adc.ch2 * ADC_to_XYZ[2][0] + adc.ch3 * ADC_to_XYZ[3][0];
	values.y = adc.ch0 * ADC_to_XYZ[0][1] + adc.ch1 * ADC_to_XYZ[1][1] + adc.ch2 * ADC_to_XYZ[2][1] + adc.ch3 * ADC_to_XYZ[3][1];
	values.z = adc.ch0 * ADC_to_XYZ[0][2] + adc.ch1 * ADC_to_XYZ[1][2] + adc.ch2 * ADC_to_XYZ[2][2] + adc.ch3 * ADC_to_XYZ[3][2];
	values.lux = adc.ch0 * ADC_to_XYZ[0][3] + adc.ch1 * ADC_to_XYZ[1][3] + adc.ch2 * ADC_to_XYZ[2][3] + adc.ch3 * ADC_to_XYZ[3][3];

    return values;
}
//from retrospy github and www.brucelindbloom.com
hal::result<opt4048::rgb_values> opt4048::xyz_to_rgb(xyz_values xyz) {
    //  XYZ of D65 Illuminant from retrospy github and www.brucelindbloom.com

    xyz_values D65_Illuminant = { 95.0500,100.0000,108.9000,NAN};
    rgb_values rgb;

	// XYZ of D65 Illuminant
	xyz.x /= D65_Illuminant.x;
	xyz.y /= D65_Illuminant.y;
	xyz.z /= D65_Illuminant.y;

	rgb.r = std::min(1.0f, std::max(0.0f, sRGBCompandingFunction(xyz.x * XYZ_to_RGB[0][0] + xyz.y * XYZ_to_RGB[0][1] + xyz.z * XYZ_to_RGB[0][2])));
	rgb.g = std::min(1.0f, std::max(0.0f, sRGBCompandingFunction(xyz.x * XYZ_to_RGB[1][0] + xyz.y * XYZ_to_RGB[1][1] + xyz.z * XYZ_to_RGB[1][2])));
	rgb.b = std::min(1.0f, std::max(0.0f, sRGBCompandingFunction(xyz.x * XYZ_to_RGB[2][0] + xyz.y * XYZ_to_RGB[2][1] + xyz.z * XYZ_to_RGB[2][2])));

	return rgb;
}




float sRGBCompandingFunction(float val)  // helper function retrospy github and www.brucelindbloom.com
  {
    if (val <= 0.0031308)
      return val *= 12.92;
    else
      return pow(1.055 * val, 1 / 2.4) - 0.055;
  }


hal::result<opt4048::rgb_values> opt4048::get_data(){// will do all the conversions and return required data
    auto adc = HAL_CHECK(get_adc_codes());
    auto xyz = HAL_CHECK(adc_codes_to_xyz(adc));
    auto rgb = HAL_CHECK(xyz_to_rgb(xyz));

    return rgb;
}  

}