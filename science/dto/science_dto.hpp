#pragma once

namespace science{

    // commands are parsed from mission control
    struct science_commands{
        int state_step = 0;
        int automatic = 1;
        int e_stop = 0;
    };
    
    enum Status {
        NotStarted = 0,
        InProgress = 1,
        Complete = 2
    };

    struct science_status{
        int move_revolver_status = Status::NotStarted;
        int seal_status = Status::NotStarted;
        int depressurize_status = Status::NotStarted;
        int inject_status = Status::NotStarted;
        int clear_status = Status::NotStarted;
        int unseal_status = Status::NotStarted;
    };

    // science data is what we send mission control
    struct science_data{
        science_status status;
        float methane_level = 0.0;
        float co2_level = 0.0;
        float pressure_level = 0.0;
    };

    const char kResponseBodyFormat[] =
  "{\"state_step\":%d,\"automatic\":%d,\"e_stop\":%d}\n";

    const char kGETRequestFormat[] =
  "science?rev_status=%d&seal_status=%d&depressure_status=%d&inj_status=%d&clear_status=%d&"
  "unseal_status=%d&methane_level=%d&co2_level=%d&pressure_level=%d";

}