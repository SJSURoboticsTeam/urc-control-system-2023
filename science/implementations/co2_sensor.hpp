#pragma once

#include <libhal-util/i2c.hpp>
#include <libhal/units.hpp>
#include <span>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

//i2cc address is 0x62
//need a buffer
//3 things to get from the i2c bus 
//use std::span<hal::byte> for buffer to get data
//use i2c bus 2 for the dev 2 board while testing 

using namespace hal::literals;
using namespace std::chrono_literals;
namespace science {
class Co2Sensor {
private:
Co2Sensor(hal::i2c& p_i2c, hal::steady_clock* c): m_i2c(&p_i2c), clock{c} {

        //start periodic measurement //write to hex 0x21b1
        // hal::write(m_i2c, Addresses::address, std::span{a},hal::never_timeout());
        // hal::write(m_i2c, Addresses::address, std::span{a});
        // hal::transaction(sensor_address, start_address, nullptr, )
    }  
    //constructor
public:
    enum Addresses : hal::byte
    {
        address = 0x62,
        // start_first_half = 0xb1,
        // start_second_half = 0x21,
        // read_first_half = 0x05,
        // read_second_half = 0xec,
        // stop_first_half = 0x86,
        // stop_second_half = 0x3f 
        start_first_half = 0x21,
        start_second_half = 0xb1,
        read_first_half = 0xec,
        read_second_half = 0x05,
        stop_first_half = 0x3f,
        stop_second_half = 0x86 
    };

    
    static hal::result<Co2Sensor> create(hal::i2c& p_i2c, hal::steady_clock& c){

        Co2Sensor co2sensor(p_i2c, c);
        HAL_CHECK(co2sensor.start());
        return co2sensor;
    }

    hal::status start(){
        std::array<hal::byte, 2> a{ {start_first_half, start_second_half} };
        HAL_CHECK(hal::write(*m_i2c, Addresses::address, a, hal::never_timeout()));
        return hal::success();

    }
    
    hal::result<int16_t> read_co2(){
        // std::array<hal::byte,9> buffer; //3 bytes for co2, temperature and humidity
        std::array<const hal::byte, 2> burrito {read_first_half, read_second_half} ;
        
        std::array<hal::byte,9> buffer;
        
        // hal::write_then_read(m_i2c, Addresses::address, burrito, buffer, hal::never_timeout());
        HAL_CHECK(hal::write(*m_i2c, Addresses::address, burrito));
        hal::delay(*clock,1ms);

        HAL_CHECK(hal::read(*m_i2c, Addresses::address, buffer, hal::never_timeout()));
        int16_t result = buffer[0] << 8 | buffer[1] << 0;
        
        return result;
        // return buffer;
        // return buffer[0];
    }




    //function that converts data into measurements that we need
    hal::status stop(){
        std::array<hal::byte, 2> c{ {stop_first_half, stop_second_half} };

        HAL_CHECK(hal::write(*m_i2c, Addresses::address, std::span{c},hal::never_timeout()));
    }

    //method to read 
    //reads 3 pieces of data: CO2 amount, temperature and humidity
    
    hal::i2c* m_i2c;//initializing the i2c driver
    // hal::serial& serial;
    hal::steady_clock* clock;

};
}
