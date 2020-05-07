# coinesAPI - PC

## Supported boards

1. Application Board 2.0
2. Application Board 3.0
  - APP2.0 compatibility mode - Support based on pin mappings in APP3.0 EEPROM 
    - COINES code written for APP2.0 can work with APP3.0 shuttle board without any changes. 
    - Development Desktop 2.0 relies on this mode for supporting old APP2.0 shuttle boards with APP3.0
  - APP3.0 native pin support
     - Used when there equivalent APP2.0 shuttle for a given APP3.0 mini shuttle
     - Will come to full use when APP2.0 board is deprecated.  
3. Zeús board
   - To enable Zeús board support, set `ZEUS_QUIRK` to `1`  in `pc.mk` and do a clean build.
