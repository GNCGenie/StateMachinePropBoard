#include<tinyfsm.hpp>
#include<iostream>

struct Clock : tinyfsm::Event{};
struct Telecommand : tinyfsm::Event
{
    int TC;
};

struct board : tinyfsm::Fsm<board>
{
    virtual void react(Clock const &) {};
    virtual void react(Telecommand const &) {};

    virtual void entry(void) {};
    virtual void exit(void) {};
};

struct STANDBY : board
{
    void entry() override {
        // Initialise RT telemetry
        // Initialise telecommand handling
    }

    void react(Clock const &) override {
        // Generate Telemetry
    }

    void react(Telecommand const & a) override {
        if (a.TC == 1)
            transit<ACTIVE>();
    }
};

struct ACTIVE : board
{
    void entry() override {
        // Initialise RT telemetry
        // Initialise FDIR
        // Initialise PID / Maintainance functions
    }

    void react(Clock const &) override {
        // Generate Telemetry
        // Run Fuel Tank PID
        // Run FDIR & Catch any errors
    }

    void react(Telecommand const & a) override {
        if (a.TC == 2)
            transit<FIRING>();
        if (a.TC == 3)
            transit<STR_TM>();
    }
};

struct STR_TM : board
{
    void entry() override {
        // Initialise FDIR
        // Initialise PID / Maintainance functions
        // Initialise Operational Memory Read
    }

    void react(Clock const &) override {
        // Transfer Stored Telemetry
        // Run Fuel Tank PID
        // Run FDIR & Catch any errors
    }

    void exit() override {
        // Do Nothing
    }

    void react(Telecommand const & a) override {
        if (a.TC == 1)
            transit<ACTIVE>();
    }
};


struct FIRING : board
{
    void entry() override {
        // Initialise RT telemetry
        // Initialise Hi-Freq Data Read/Write
        // Initialise firing sequence
    }

    void react(Clock const &) override {
        // Generate Telemetry
        // Copy propulsion unit data to Volatile memory
    }

    void exit() override {
        // Stop Hi-Freq Data Read/Write
        // Copy Hi-Freq Data from Volatile to Non-volatile
    }

    void react(Telecommand const & a) override {
        if (a.TC == 1)
            transit<ACTIVE>();
    }
};