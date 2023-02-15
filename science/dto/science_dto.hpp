#pragma once

namespace science{

    // commands are parsed from mission control
    struct science_commands{
        int mc_button = 0;
        int automatic = 1;
        int e_stop = 0;
    };

    struct science_status{
        int move_revolver_status = Status::NotStarted;
        int seal_status = Status::NotStarted;
        int depressurize_status = Status::NotStarted;
        int inject_status = Status::NotStarted;
        int sensor_status = Status::NotStarted;
        int clear_status = Status::NotStarted;
        int unseal_status = Status::NotStarted;
    };

    enum Status {
        NotStarted = 0,
        InProgress = 1,
        Complete = 2
    };

    // science data is what we send mission control
    struct science_data{
        science_status status;
        float methane_level = 0.0;
        float co2_level = 0.0;
        float pressure_level = 0.0;
    };

}