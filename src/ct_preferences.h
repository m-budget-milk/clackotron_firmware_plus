#ifndef _CLACKOTRON_PREFERENCES_H_
#define _CLACKOTRON_PREFERENCES_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <Timezone.h>

/// @brief A class for managing preferences that are stored in flash or file system
class CTPreferences {
  public:
    /// @brief Set up preferences provider
    /// @return True if setup was successful, false otherwise
    bool setup();

    /// @brief Check if this is the first boot of the device
    /// @return True if this is the first boot, false otherwise
    bool isFirstBoot();

    /// @brief  Set the first boot flag
    /// @param firstBoot - The boolean value to set the first boot flag to
    void setFirstBoot(bool firstBoot);

    /// @brief Load the list of module addresses from the config
    /// @param moduleAddresses - The array to load the module addresses into
    void loadModuleAddresses(uint8_t *moduleAddresses);

    /// @brief Load the timezone data from the config
    /// @param timeDefault - Pointer to the TimeChangeRule for the default time
    /// @param timeDst - Pointer to the TimeChangeRule for the DST time
    /// @param ntpServer - Pointer to the NTP server address
    /// @param dayNamesShort - Pointer to the array of short day names 
    /// @param dayNamesLong - Pointer to the array of long day names
    void loadTimeZoneData(
      TimeChangeRule *timeDefault, 
      TimeChangeRule *timeDst, 
      char* ntpServer,
      char dayNamesShort[8][3],
      char dayNamesLong[8][4]
    );

    /// @brief Get the current config JSON object
    /// @return The current config JSON object as string
    String getConfig();

    /// @brief Get the the current config template string
    /// @return The config template as string
    String getConfigTemplate();

    /// @brief Get a text parameter from the current config
    /// @param key - The key of the parameter to get
    /// @return The text parameter as string
    String getConfigTextParam(String key);

    /// @brief Get a number parameter from the current config
    /// @param key - The key of the parameter to get
    /// @return The number parameter as uint32_t
    uint32_t getConfigNumberParam(String key);

    /// @brief Set the current config JSON object
    /// @param config - The config JSON object as string
    void setConfig(String config);

    /// @brief Load board module definitions from board_modules.json into memory
    /// @return True if loaded successfully
    bool loadBoardModules();

    /// @brief Get the number of board modules defined in board_modules.json
    /// @return Number of modules
    int getBoardModuleCount();

    /// @brief Get the RS485 address of board module at index i
    /// @param i - Index into the modules array
    /// @return RS485 address byte
    uint8_t getBoardModuleAddress(int i);

    /// @brief Check whether an address exists in board_modules.json
    /// @param addr - RS485 address to check
    /// @return True if address is configured
    bool isValidBoardAddress(uint8_t addr);

    /// @brief Check whether a position index is valid for the given address
    /// @param addr - RS485 address
    /// @param pos - Position index to validate
    /// @return True if position is within the positions array bounds
    bool isValidBoardPosition(uint8_t addr, int pos);

    /// @brief Get the default blade position for a board module
    /// @param addr - RS485 address
    /// @return defaultPosition from board_modules.json, or 0 if not set
    int getBoardDefaultPosition(uint8_t addr);

    /// @brief Get the currently saved blade position for a board module
    /// @param addr - RS485 address
    /// @return Saved position, or getBoardDefaultPosition if not saved
    int getBoardSavedPosition(uint8_t addr);

    /// @brief Persist a new set of board positions
    /// @param json - JSON object string keyed by address, e.g. {"7":14}
    void setBoardPositions(String json);

    /// @brief Build the full board positions response with fallback resolution
    /// @return JSON string e.g. {"modulePositions":{"7":14}}
    String getBoardPositionsJson();

    /// @brief Clear all preferences
    void clear();

  private:
    Preferences preferences;
    DynamicJsonDocument* boardModulesDoc = nullptr;

    void overrideTimeChangeRule(const char* tag, TimeChangeRule *rule, JsonArray data);
    void copyLittleFsFile(const char* src, const char* dst);
};

#endif // _CLACKOTRON_PREFERENCES_H_
