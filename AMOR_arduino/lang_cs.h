#ifndef lang_cs_h
#define lang_cs_h

/*
 * czech
 * file with localized strings for text constants
 * constant starting with "S_" appear in standard AMOR output - these are the important ones, because they are displayed always
 * constant starting with "D_" are displayed only if DEBUG mode is enabled (see file "Config.h")
 */


/*
 * string constants for module
 * "SerialCommunicator"
 */
#define S_MSG_0_THERMOSTAT_STATUS           "Status Termostat: "
#define S_MSG_0_CH_ENABLED                  "topeni zapnuto"
#define S_MSG_0_DHW_ENABLED                 "ohrev TUV zapnut"
#define S_MSG_0_COOLING_ENABLED             "chlazeni zapnuto"
#define S_MSG_0_OTC_ENABLED                 "kompenzace podle vnejsi teploty"
#define S_MSG_0_CH2_ENABLED                 "topeni 2 zapnuto"
#define S_MSG_0_BOILER_STATUS               "      Status kotel: "
#define S_MSG_0_FAULTS                      "indikace chyby"
#define S_MSG_0_CH_ON                       "topeni"
#define S_MSG_0_DHW_ON                      "tepla voda"
#define S_MSG_0_FLAME_ON                    "plamen"
#define S_MSG_0_COOLING_ON                  "chlazeni"             
#define S_MSG_0_CH2_ON                      "topeni 2"
#define S_MSG_0_DIAG_EVENT                  "diagnosticka udalost"
#define S_MSG_1_CH_SETPOINT                 "Cilova teplota topne vody"
#define S_MSG_2_MASTER_CONFIG               "Master config: "
#define S_MSG_2_MASTER_MEMBER_ID            "; master member ID: "
#define S_MSG_3_SLAVE_CONFIG                "Slave config: "
#define S_MSG_3_SLAVE_MEMBER_ID             "; slave member ID: "
#define S_MSG_3_HAS_DHW                     "ma ohrev TUV"
#define S_MSG_3_REGULATION_TYPE             "typ regulace"
#define S_MSG_3_HAS_COOLING                 "ma chlazeni"
#define S_MSG_3_DHW_TYPE                    "typ ohrevu TUV"
#define S_MSG_3_PUMP_CONTROL                "ovladani cerpadla" 
#define S_MSG_3_HAS_CH2                     "ma topeni 2"
#define S_MSG_5_FAULT_FLAGS                 "Chyby: "
#define S_MSG_5_ERROR_CODE                  "; error code: "
#define S_MSG_5_SERVICE_REQUEST             "service request"
#define S_MSG_5_LOCKOUT_RESET               "lockout reset"
#define S_MSG_5_WATER_LOW_PRESSURE          "nizky tlak vody"
#define S_MSG_5_FLAME_ERROR                 "chyba plynu/plamene"
#define S_MSG_5_AIR_PRESSSURE_ERROR         "tlak vzduchu"  
#define S_MSG_5_WATER_TEMPERATURE_TOO_HIGH  "prilis vysoka teplota vody"
#define S_MSG_9_REMOTE_OVERRIDE_ROOM_SETPOINT "Remote override room setpoint"
#define S_MSG_10_TSP_COUNT                  "Pocet \"Transparent Slave\" parametru"
#define S_MSG_14_MAX_MODULATION             "Maximalni dovolena modulace"
#define S_MSG_16_ROOM_SETPOINT              "Cilova pokojova teplota"
#define S_MSG_17_MODULATION_LEVEL           "Modulace"
#define S_MSG_18_WATER_PRESSURE             "Tlak vody"
#define S_MSG_19_DHW_FLOW_RATE              "Prutok uzitkove vody"
#define S_MSG_24_ROOM_TEMPERATURE           "Pokojova teplota"
#define S_MSG_25_FEED_TEMP                  "Teplota topne vody"
#define S_MSG_26_DHW_TEMP                   "Teplota uzitkove vody"
#define S_MSG_27_OUTSIDE_TEMPERATURE        "Venkovni teplota"
#define S_MSG_28_RETURN_WATER_TEMPERATURE   "Teplota zpatecky"
#define S_MSG_33_EXHAUST_TEMPERATURE        "Teplota spalin"
#define S_MSG_48_DHW_BOUNDS                 "Povoleny rozsah nastaveni teploty uzitkove vody: "
#define S_MSG_49_CH_BOUNDS                  "Povoleny rozsah nastaveni teploty topeni: "
#define S_MSG_56_DHW_SETPOINT               "Cilova teplota uzitkove vody"
#define S_MSG_57_MAX_CH_SETPOINT            "Maximalni povolena teplota topne vody"
#define S_MSG_99                            "Nezdokumentovana zprava #99"
#define S_MSG_100_MANUAL_CHANGE_PRIORITY    "Remote override function: Manual Change Priority = "
#define S_MSG_100_PROGRAM_CHANGE_PRIORITY   "; Program Change Priority = "
#define S_MSG_115_OEM_DIAGNOSTIC_CODE       "OEM diagnostic code"

#define D_SERCOM_NEW_DATETIME               "nove datum a cas = "

/*
 * string constants for module
 * "SingleRegulator"
 */
#define D_CH_REGULATOR_EVALUATING           " - regulator topeni vyhodnocuje: "
#define D_SR_FIXED_SETPOINT_REGULATION_ENABLED  "zapnuto - "
#define D_SR_PID_REGULATION_ENABLED         " -> zapnuto"
#define D_SR_PID_REGULATION_DISABLED        " -> disabled"
#define D_SR_DISABLED_INVALID_TEMP          "vypnuto (teplota neni aktualni)"
#define D_SR_DISABLED_PROGRAM_OFF           "vypnuto (program OFF)"
#define D_SR_TEMPERATURE_VALIDATOR          "validator teploty: pokojova teplota = "
#define D_SR_TEMP_VALIDATOR_NOW             "; cas ted"
#define D_SR_TEMP_VALIDATOR_LAST_TIME       "; cas posledni aktualizace "
#define D_SR_TEMP_VALIDATOR_RESULT          "; vysledek validace = "
#define D_SR_ROOM_TEMPERATURE_UPDATED       "regulator: pokojova teplota aktualizovana, nova hodnota = "

/*
 * string constants for module
 * "DHWregulator"
 */
#define D_DHW_REGULATOR_EVALUATING          " - TUV regulator vyhodnocuje: "
#define D_DHW_PROGRAM_TYPE                  " - typ programu = "
#define D_DHW_ENABLED                       "zapnuto - cilova teplota"
#define D_DHW_DISABLED_INVALID_TEMPERATURE  "vypnuto (neplatna teplota TUV)."
#define D_DHW_DISABLED_NO_PROGRAM           "vypnuto (zadny program neni aktivni)."

/*
 * string constants for module
 * "EspMqttCommunicator"
 */
#define D_ESPMQTT_OLD_VALUE_FOUND           "Stara hodnota pro zpravu ID "
#define D_ESPMQTT_ON_INDEX                  " byla nalezena pod indexem "
#define D_ESPMQTT_OLD_VALUE_NOT_FOUND       "Zadna stara hodnota nebyla nalezena pro zpravu ID "

/*
 * string constants shared by following modules
 * "EspMqttCommunicator" and "SerialCommunicator"
 */
#define D_COMMUNICATOR_NL_WITHOUT_CR        "ERROR: prijat znak \\n, kteremu nepredchazel znak \\r - zahazuji zpravu."
#define D_COMMUNICATOR_VALUE_RECEIVED       "Prijata data: obsah zpravy = '"
#define D_COMMUNICATOR_TOPIC                "' - topic = '"
#define D_COMMUNICATOR_VALUE                "', hodnota = '"
#define D_COMMUNICATOR_PROVIDER_FOUND       "Provider pro topic '"
#define D_COMMUNICATOR_SENDING_VALUE        "' byl nalezen -> posilam hodnotu "

/*
 * string constants for module
 * "OTlooper"
 */
#define D_OT_LOOPER_SENDING_MESSAGE         "  Looper - posila zpravu "
#define D_OTLOOPER_THERMOSTAT_MESSAGE       "    Zpráva z termostatu "
#define D_OTLOOPER_ANSWERING                "    Odpovidam: "

/*
 * string constants for module
 * "Programmer"
 */
#define D_PROG_PROGRAMMER                   "Programator - vyhodnoceni v case %u:"
#define D_PROG_PROGRAM_DESCRIPTION          "#%u = deb: %u, start: %u, stop: %u, typ: %u"
#define D_PROG_SELECTED_PROGRAM             "Programator - vybrany program: "
#define D_PROG_DEFAULT_PROGRAM              "Programator - vychozi program"
#define D_PROG_DATA_UPDATED_BUT_NOT_PROGRAM "Programator - data prijata, ale nejsou typu 'program'"
#define D_PROG_NEW_PROGRAM_WRONG_ID         "Programator - aktualizace - spatne cislo programu: %u zadano, ale povoleno [0..%u]"
#define D_PROG_NEW_PROGRAM_UPDATING         "Programator - program c.%u je aktualizovan."
#define D_PROG_NEW_PROGRAM_DESCRIPTION      "   typ %u, , teplota %.2f, den %u, start %u:%u, stop %u:%u"
#define D_PROG_PROGRAM_DEACTIVATION         "Programator - program c.%u deaktivovan."

#endif
