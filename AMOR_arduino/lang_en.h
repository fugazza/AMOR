#ifndef lang_en_h
#define lang_en_h

/*
 * english
 * file with localized strings for text constants
 * constant starting with "S_" appear in standard AMOR output - these are the important ones, because they are displayed always
 * constant starting with "D_" are displayed only if DEBUG mode is enabled (see file "Config.h")
 */
 
/*
 * string constants for module
 * "SerialCommunicator"
 */
#define S_MSG_0_THERMOSTAT_STATUS           "Thermostat status: "
#define S_MSG_0_CH_ENABLED                  "CH enabled"
#define S_MSG_0_DHW_ENABLED                 "DHW enabled"
#define S_MSG_0_COOLING_ENABLED             "cooling enabled"
#define S_MSG_0_OTC_ENABLED                 "OTC active"
#define S_MSG_0_CH2_ENABLED                 "CH enabled"
#define S_MSG_0_BOILER_STATUS               "      Boiler status: "
#define S_MSG_0_FAULTS                      "fault indication"
#define S_MSG_0_CH_ON                       "CH mode"
#define S_MSG_0_DHW_ON                      "DHW mode"
#define S_MSG_0_FLAME_ON                    "flame"
#define S_MSG_0_COOLING_ON                  "cooling"             
#define S_MSG_0_CH2_ON                      "CH2 mode"
#define S_MSG_0_DIAG_EVENT                  "diagnostic indication"
#define S_MSG_1_CH_SETPOINT                 "Setpoint for CH water temperature"
#define S_MSG_2_MASTER_CONFIG               "Master config: "
#define S_MSG_2_MASTER_MEMBER_ID            "; master member ID: "
#define S_MSG_3_SLAVE_CONFIG                "Slave config: "
#define S_MSG_3_SLAVE_MEMBER_ID             "; slave member ID: "
#define S_MSG_3_HAS_DHW                     "DHW present"
#define S_MSG_3_REGULATION_TYPE             "control type"
#define S_MSG_3_HAS_COOLING                 "cooling config"
#define S_MSG_3_DHW_TYPE                    "DHW config"
#define S_MSG_3_PUMP_CONTROL                "pum control"        
#define S_MSG_3_HAS_CH2                     "CH2 present"
#define S_MSG_5_FAULT_FLAGS                 "Fault flags: "
#define S_MSG_5_ERROR_CODE                  "; OEM fault code: "
#define S_MSG_5_SERVICE_REQUEST             "service request"
#define S_MSG_5_LOCKOUT_RESET               "lockout reset"
#define S_MSG_5_WATER_LOW_PRESSURE          "low water pressure"
#define S_MSG_5_FLAME_ERROR                 "gas/flame fault"
#define S_MSG_5_AIR_PRESSSURE_ERROR         "air pressure fault"        
#define S_MSG_5_WATER_TEMPERATURE_TOO_HIGH  "water over-temp"
#define S_MSG_9_REMOTE_OVERRIDE_ROOM_SETPOINT "Remote override room setpoint"
#define S_MSG_10_TSP_COUNT                  "Number of Transparent Slave Parameters"
#define S_MSG_14_MAX_MODULATION             "Maximum modulation level"
#define S_MSG_16_ROOM_SETPOINT              "Room setpoint"
#define S_MSG_17_MODULATION_LEVEL           "Modulation level"
#define S_MSG_18_WATER_PRESSURE             "CH water pressure"
#define S_MSG_19_DHW_FLOW_RATE              "DHW flow rate"
#define S_MSG_24_ROOM_TEMPERATURE           "Room temperature"
#define S_MSG_25_FEED_TEMP                  "Boiler water temperature"
#define S_MSG_26_DHW_TEMP                   "DHW temperature"
#define S_MSG_27_OUTSIDE_TEMPERATURE        "Outside temperature"
#define S_MSG_28_RETURN_WATER_TEMPERATURE   "Return water temperature"
#define S_MSG_33_EXHAUST_TEMPERATURE        "Exhaust temperature"
#define S_MSG_48_DHW_BOUNDS                 "DHW min..max allowable temperature: "
#define S_MSG_49_CH_BOUNDS                  "CH min..max allowable temperature: "
#define S_MSG_56_DHW_SETPOINT               "DHW setpoint"
#define S_MSG_57_MAX_CH_SETPOINT            "Maximum CH water setpoint"
#define S_MSG_99                            "Undocumented command #99"
#define S_MSG_100_MANUAL_CHANGE_PRIORITY    "Remote override function: Manual Change Priority = "
#define S_MSG_100_PROGRAM_CHANGE_PRIORITY   "; Program Change Priority = "
#define S_MSG_115_OEM_DIAGNOSTIC_CODE       "OEM diagnostic code"

#define D_SERCOM_NEW_DATETIME               "new datetime = "

/*
 * string constants for module
 * "SingleRegulator"
 */
#define D_CH_REGULATOR_EVALUATING           " - CH Regulator evaluating: "
#define D_SR_FIXED_SETPOINT_REGULATION_ENABLED  "enabled - "
#define D_SR_PID_REGULATION_ENABLED         " -> enabled"
#define D_SR_PID_REGULATION_DISABLED        " -> disabled"
#define D_SR_DISABLED_INVALID_TEMP          "disabled (invalid temperature)"
#define D_SR_DISABLED_PROGRAM_OFF           "disabled (program OFF)"
#define D_SR_TEMPERATURE_VALIDATOR          "temperature validator: roomTemperature = "
#define D_SR_TEMP_VALIDATOR_NOW             "; time now "
#define D_SR_TEMP_VALIDATOR_LAST_TIME       "; last update time "
#define D_SR_TEMP_VALIDATOR_RESULT          "; time condition status = "
#define D_SR_ROOM_TEMPERATURE_UPDATED       "simple regulator: room temperature updated, new value = "

/*
 * string constants for module
 * "DHWregulator"
 */
#define D_DHW_REGULATOR_EVALUATING          " - DHW regulator evaluating: "
#define D_DHW_PROGRAM_TYPE                  " - program type = "
#define D_DHW_ENABLED                       "enabled - target temperature"
#define D_DHW_DISABLED_INVALID_TEMPERATURE  "disabled (invalid DHW temperature)."
#define D_DHW_DISABLED_NO_PROGRAM           "disabled (no program active)."

/*
 * string constants for module
 * "EspMqttCommunicator"
 */
#define D_ESPMQTT_OLD_VALUE_FOUND           "Old value for id "
#define D_ESPMQTT_ON_INDEX                  " found on index "
#define D_ESPMQTT_OLD_VALUE_NOT_FOUND       "Old value not found for id "

/*
 * string constants shared by following modules
 * "EspMqttCommunicator" and "SerialCommunicator"
 */
#define D_COMMUNICATOR_NL_WITHOUT_CR        "ERROR: \\n received without \\r - discarding message."
#define D_COMMUNICATOR_VALUE_RECEIVED       "Value received: command value = '"
#define D_COMMUNICATOR_TOPIC                "' - topic = '"
#define D_COMMUNICATOR_VALUE                "', value = '"
#define D_COMMUNICATOR_PROVIDER_FOUND       "Provider for topic '"
#define D_COMMUNICATOR_SENDING_VALUE        "' found -> sending value "

/*
 * string constants for module
 * "OTlooper"
 */
#define D_OT_LOOPER_SENDING_MESSAGE         "  Looper - sending message "
#define D_OTLOOPER_THERMOSTAT_MESSAGE       "    Thermostat message "
#define D_OTLOOPER_ANSWERING                "    Answering: "

/*
 * string constants for module
 * "Programmer"
 */
#define D_PROG_PROGRAMMER                   "programmer - evaluating at time %u:"
#define D_PROG_PROGRAM_DESCRIPTION          "#%u = day: %u, start: %u, stop: %u, type: %u"
#define D_PROG_SELECTED_PROGRAM             "programmer - selected program: "
#define D_PROG_DEFAULT_PROGRAM              "programmer - default program"
#define D_PROG_DATA_UPDATED_BUT_NOT_PROGRAM "programmer - program update received, but data is not a program"
#define D_PROG_NEW_PROGRAM_WRONG_ID         "programmer - program update - wrong number of program: %u set, but only [0..%u] allowed!"
#define D_PROG_NEW_PROGRAM_UPDATING         "programmer - program no.%u is being updated."
#define D_PROG_NEW_PROGRAM_DESCRIPTION      "   type %u, , temperature %.2f, day %u, start %u:%u, stop %u:%u"
#define D_PROG_PROGRAM_DEACTIVATION         "programmer - program no.%u deactivated."

#endif
