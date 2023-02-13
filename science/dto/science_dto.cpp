// commands are parsed from mission control
struct science_commands{
    int mc_button = 0;
    int automatic = 1;
}

// science data is what we send mission control
struct science_data{
    int move_revolver_status = 0;
    int seal_status = 0;
    int depressurize_status = 0;
    int inject_status = 0;
    int sensor_status = 0;
    int clear_status = 0;
    float methane_level = 0.0;
    float CO2_level = 0.0;
    float pressure_level = 0.0;
}
// for status data
    // 0 = not started
    // 1 = in progress
    // 2 = complete