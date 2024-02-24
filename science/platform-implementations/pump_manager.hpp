#include <libhal/output_pin.hpp>

namespace sjsu::science {

    class pump_manager {
        private:
            
            std::array<hal::output_pin&,5> m_pumps;
            pump_manager(
                hal::output_pin& p_deionized_water_pump, 
                hal::output_pin& p_sample_pump, 
                hal::output_pin& p_molisch_reagent_pump, 
                hal::output_pin& p_sulfuric_acid_pump, 
                hal::output_pin& p_biuret_reagent
            );

        public:
            enum class pumps{
                DEIONIZED_WATER=0,
                SAMPLE,
                MOLISCH_REAGENT,
                SULFURIC_ACID,
                BIURET_REAGENT
            };
            static hal::result<pump_manager> create(
                hal::output_pin& p_deionized_water_pump, 
                hal::output_pin& p_sample_pump, 
                hal::output_pin& p_molisch_reagent_pump, 
                hal::output_pin& p_sulfuric_acid_pump, 
                hal::output_pin& p_biuret_reagent
            );
            hal::status pump(pumps pump, hal::time_duration duration);
    };

}