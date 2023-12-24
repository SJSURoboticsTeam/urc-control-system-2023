#include "color_sensor_opt4048.hpp"
#include <libhal/units.hpp>
#include <stdlib.h>


using namespace hal::literals;
using namespace std::chrono_literals;
namespace sjsu::science {
opt4048::opt4048(hal::i2c& p_i2c, hal::steady_clock& p_clock, hal::serial& p_terminal) 
    : m_i2c(p_i2c)
    , m_clock(p_clock)
    , m_terminal(p_terminal)
{
}
static void print_byte_array(hal::serial& console, std::span<hal::byte> arr) {
  for(auto& i : arr) {
    hal::print<16>(console, "0x%02x ", i);
  }
  hal::print(console, "\n");
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
    std::array<hal::byte, 3> to_send_array= {register_addresses::settings,write1,write2};
    HAL_CHECK(hal::write(m_i2c, opt4048::device_address, to_send_array, hal::never_timeout()));
    print_byte_array(m_terminal, to_send_array); //should output: 0x0A, 0x30, 0xB3
    return hal::success();
}

hal::status opt4048::read_channel(hal::byte address, std::span<hal::byte> output){
    std::array<hal::byte, 1> address_out = {address};

    HAL_CHECK(hal::write_then_read(m_i2c, opt4048::device_address, address_out, output));
    // print<30>(m_terminal, "0x%02x: \t", address);
    // print_byte_array(m_terminal, output);
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
    
    hal::delay(m_clock, 50ms);
    //do this for channel 1, 2, and 3
    std::array<uint16_t, 2> channel_1_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel1_msb, channel_1_msb));
    hal::delay(m_clock, 50ms);

    std::array<uint16_t, 2> channel_2_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel2_msb, channel_2_msb));
    hal::delay(m_clock, 50ms);

    std::array<uint16_t, 2> channel_3_msb;
    HAL_CHECK(get_msb_exp(register_addresses::channel3_msb, channel_3_msb));
    hal::delay(m_clock, 50ms);

    std::array<hal::byte, 2> channel_0_lsb;
    HAL_CHECK(read_channel(register_addresses::channel0_lsb, channel_0_lsb));
    uint8_t lsb0 = channel_0_lsb[0];
    hal::delay(m_clock, 50ms);

    std::array<hal::byte, 2> channel_1_lsb;
    HAL_CHECK(read_channel(register_addresses::channel1_lsb, channel_1_lsb));
    uint8_t lsb1 = channel_1_lsb[0];
    hal::delay(m_clock, 50ms);

    std::array<hal::byte, 2> channel_2_lsb;
    HAL_CHECK(read_channel(register_addresses::channel2_lsb, channel_2_lsb));
    uint8_t lsb2 = channel_2_lsb[0];
    hal::delay(m_clock, 50ms);

    std::array<hal::byte, 2> channel_3_lsb;
    HAL_CHECK(read_channel(register_addresses::channel3_lsb, channel_3_lsb));
    uint8_t lsb3 = channel_3_lsb[0];
    hal::delay(m_clock, 50ms);

    uint32_t mantissa0 = (channel_0_msb[1] << 8) + lsb0;
    uint32_t adc_codes0 = mantissa0 << channel_0_msb[0];

    uint32_t mantissa1 = (channel_1_msb[1] << 8) + lsb1;
    uint32_t adc_codes1 = mantissa1 << channel_1_msb[0];

    uint32_t mantissa2 = (channel_2_msb[1] << 8) + lsb2;
    uint32_t adc_codes2 = mantissa2 << channel_2_msb[0];

    uint32_t mantissa3 = (channel_3_msb[1] << 8) + lsb3;
    uint32_t adc_codes3 = mantissa3 << channel_3_msb[0];
    
  


    
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
    // print<30>(m_terminal, "0x%02x: \t", register_A);
    // print<30>(m_terminal, "OUTPUT 0: 0x%02x\t", output[0]);
    // print<30>(m_terminal, "OUTPUT 1: 0x%02x\n", output[1]);
    uint16_t exponent = 0;
    uint16_t msb = 0;
    exponent = output[0] >> 4;
    // print<30>(m_terminal, "OUTPUT 0: 0x%02x\n", output[0]);

    msb = output[0] << 12;
    msb = (msb >> 4) | output[1];
    // print<30>(m_terminal, "MSB: %u\n", msb);
    // print<30>(m_terminal, "Exponent: %u\n", exponent);
    data[0] = exponent;
    data[1] = msb;
    return hal::success();
}
hal::result<opt4048::xyz_values> opt4048::adc_codes_to_xyz(adc_codes adc) {
    //from datasheet
    xyz_values values;
    double ADC_to_XYZ[4][4] = { { 0.000234892992, -0.0000189652390,  0.0000120811684,       0 },
							   { 0.0000407467441,  0.000198958202, -0.0000158848115,  0.00215 },
							   { 0.0000928619404, -0.0000169739553,  0.000674021520,       0 },
							   {             0,              0,              0,       0 }
	};

    //matrix multipliations from the datasheet, got the actual calculations from @retrospy's Github and www.brucelindbloom.com
    values.x = adc.ch0 * ADC_to_XYZ[0][0] + adc.ch1 * ADC_to_XYZ[1][0] + adc.ch2 * ADC_to_XYZ[2][0] + adc.ch3 * ADC_to_XYZ[3][0];
	values.y = adc.ch0 * ADC_to_XYZ[0][1] + adc.ch1 * ADC_to_XYZ[1][1] + adc.ch2 * ADC_to_XYZ[2][1] + adc.ch3 * ADC_to_XYZ[3][1];
	values.z = adc.ch0 * ADC_to_XYZ[0][2] + adc.ch1 * ADC_to_XYZ[1][2] + adc.ch2 * ADC_to_XYZ[2][2] + adc.ch3 * ADC_to_XYZ[3][2];
	values.lux = adc.ch0 * ADC_to_XYZ[0][3] + adc.ch1 * ADC_to_XYZ[1][3] + adc.ch2 * ADC_to_XYZ[2][3] + adc.ch3 * ADC_to_XYZ[3][3];
    print<30>(m_terminal, "ADC CODES: %d \t", adc.ch0);
    print<30>(m_terminal, "%d \t", adc.ch1);
    print<30>(m_terminal, "%d \t", adc.ch2);
    print<30>(m_terminal, "%d \n", adc.ch3);
    print<30>(m_terminal, "XYZ VALUES: %f \t", values.x);
    print<30>(m_terminal, "%f \t", values.y);
    print<30>(m_terminal, "%f \t", values.z);
    print<30>(m_terminal, "%f \n", values.lux);
    return values;
}
hal::result<double> opt4048::sRGBCompandingFunction(double val)  // helper function retrospy github and www.brucelindbloom.com
  {
    if (val <= 0.0031308)
      return val *= 12.92;
    else
      return pow(1.055 * val, 1 / 2.4) - 0.055;
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
    double rgbR = HAL_CHECK(sRGBCompandingFunction(xyz.x * XYZ_to_RGB[0][0] + xyz.y * XYZ_to_RGB[0][1] + xyz.z * XYZ_to_RGB[0][2]));
    double rgbG = HAL_CHECK(sRGBCompandingFunction(xyz.x * XYZ_to_RGB[1][0] + xyz.y * XYZ_to_RGB[1][1] + xyz.z * XYZ_to_RGB[1][2]));
    double rgbB = HAL_CHECK(sRGBCompandingFunction(xyz.x * XYZ_to_RGB[2][0] + xyz.y * XYZ_to_RGB[2][1] + xyz.z * XYZ_to_RGB[2][2]));
	rgb.r = std::min(1.0, std::max(0.0, rgbR)) * 255;
	rgb.g = std::min(1.0, std::max(0.0, rgbG)) * 255;
	rgb.b = std::min(1.0, std::max(0.0, rgbB)) * 255;

	return rgb;
}

hal::result<opt4048::rgb_values> opt4048::get_data(){// will do all the conversions and return required data
    auto adc = HAL_CHECK(get_adc_codes());
    hal::print(m_terminal, "got adc codes\n");
    auto xyz = HAL_CHECK(adc_codes_to_xyz(adc));
    hal::print(m_terminal, "got xyz codes\n");

    auto rgb = HAL_CHECK(xyz_to_rgb(xyz));
    hal::print(m_terminal, "got rgb codes\n");

    return rgb;
}  

}