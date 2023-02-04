#pragma once

#include "generic_adc_sensor.hpp"

class PressureSensor : public GenericAdcSensor {
public:
    PressureSensor(hal::adc* adc) : GenericAdcSensor(adc) {};

    virtual hal::result<float> read_convert_data() override {
        // TODO: implement proper conversion formula.
        return read_raw_adc();
    }

    hal::result<bool> is_pressurized() {
        return hal::new_error("implement a way to detect a pressureized or depressureized chamber, will depend on sensor specs and how its powered.");
        return false;
    }
};
