#pragma once

#include <libhal/i2c.hpp>
#include <libhal-util/i2c.hpp>
#include <libhal/units.hpp>
//i2cc address is 0x62
//need a buffer
//3 things to get from the i2c bus 
//use std::span<hal::byte> for buffer to get data
//use i2c bus 2 for the dev 2 board while testing 
class Co2Sensor {
    
    //constructor
public:
    // const hal::byte sensor_address 0x62;
    // const hal::byte start_add 0x21b1;
    // const hal::byte read_add 0xec05;
    // const hal::byte stop_add 0x3f86;
    const int numBytes = 1;

    enum Addresses : hal::byte 
    {
        address = 0x62,
        start = 0x21b1,
        read = 0xec05,
        stop = 0x3f86
    };

    Co2_sensor(hal::i2c& p_i2c): m_i2c(p_i2c){

        //start periodic measurement //write to hex 0x21b1
        hal::write(m_i2c, Addresses::address, std::array{Addresses::start},hal::never_timeout());
        // hal::transaction(sensor_address, start_address, nullptr, )

    }
    
    hal::result<hal::byte> read_co2(){
        // std::array<hal::byte,9> buffer; //3 bytes for co2, temperature and humidity
        // hal::write_then_read(m_i2c, sensor_address, std::array{read_add}, buffer);
        std::array<hal::byte,numBytes> buffer = hal::write_then_read(m_i2c, sensor_address, std::array{Addresses::read})

        return buffer[0];
    }




    //function that converts data into measurements that we need
    hal::status stop(){
        hal::write(m_i2c, sensor_address, std::array{Addresses::stop},hal::never_timeout());

    }

    //method to read 
    //reads 3 pieces of data: CO2 amount, temperature and humidity
    
    hal::i2c* m_i2c;//initializing the i2c driver

}