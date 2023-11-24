#ifndef _file_transfer_func_h
#define _file_transfer_func_h

#include "esp_system.h"      // Built-in
#include "esp_spi_flash.h"   // Built-in
#include "esp_wifi_types.h"  // Built-in
#include "esp_bt.h"          // Built-in
#include <SPIFFS.h>          // Built-in
#include <WiFi.h>            // Built-in
#include <ESPmDNS.h>         // Built-in
#include <AsyncTCP.h>           // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  // https://github.com/me-no-dev/ESPAsyncWebServer

void Dir(AsyncWebServerRequest *);
void Directory(void);
void UploadFileSelect(void);
void Format(void);
void handleFileUpload(AsyncWebServerRequest, const String, size_t, uint8_t *, size_t, bool);
void File_Stream(void);
void File_Delete(void);
void Handle_File_Delete(String filename);
void File_Rename(void);
void Handle_File_Rename(AsyncWebServerRequest *, String, int);
String processor(const String *);
void notFound(AsyncWebServerRequest *);
void Handle_File_Download(void);
String getContentType(String);
void Select_File_For_Function(String, String);
void SelectInput(String, String, String);
int GetFileSize(String);
void Home(void);
void Page_Not_Found(void);
void Display_About(void);
String ConvBinUnits(int, int);
String EncryptionType(wifi_auth_mode_t);
bool StartMDNSservice(const char *);
String HTML_Header(void);
String HTML_Footer(void);

void init_file_transfer(void);
void wifi_task(void *);
void file_transfer_update(void);
bool file_transfer_server_started(void);
IPAddress file_transfer_get_ip(void);
int file_transfer_get_connections(void);
void file_transfer_write_csv_entry(void);
uint32_t get_last_record_unix(void);
#endif
