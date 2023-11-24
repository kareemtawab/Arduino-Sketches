#include "_file_transfer_func.h"
#include "_eeprom_func.h"
#include "_rtc_func.h"
#include "_geiger_func.h"
#include "_gps_func.h"
#include "esp_system.h"         // Built-in
#include "esp_spi_flash.h"      // Built-in
#include "esp_wifi_types.h"     // Built-in
#include "esp_bt.h"             // Built-in
#include <SPIFFS.h>             // Built-in
#include <WiFi.h>               // Built-in
#include <ESPmDNS.h>            // Built-in
#include <AsyncTCP.h>           // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  // https://github.com/me-no-dev/ESPAsyncWebServer
#define FS SPIFFS               // In preparation for the introduction of LITTLFS see https://github.com/lorol/LITTLEFS replace SPIFFS with LITTLEFS

const char *ft_server_ssid = "M5Core2 GM Counter";
bool ft_server_started = false;
extern int hours, minutes, seconds, days, months, years;
extern uint32_t unix;
uint32_t last_record_unix;
IPAddress ft_server_ip;
AsyncWebServer server(80);

typedef struct
{
  String filename;
  String ftype;
  String fsize;
} fileinfo;

String webpage, MessageLine;
fileinfo Filenames[200];  // Enough for most purposes!
int start, downloadtime = 1, uploadtime = 1, downloadsize, uploadsize, downloadrate, uploadrate, numfiles;

//#############################################################################################
void Dir(AsyncWebServerRequest *request) {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    String Fname1, Fname2;
    int index = 0;
    Directory();  // Get a list of the current files on the FS
    webpage = HTML_Header();
    webpage += "<h3>Filing System Content</h3><br>";
    if (numfiles > 0) {
      webpage += "<table class='center'>";
      webpage += "<tr><th>Type</th><th>File Name</th><th>File Size</th><th class='sp'></th><th>Type</th><th>File Name</th><th>File Size</th></tr>";
      while (index < numfiles) {
        Fname1 = Filenames[index].filename;
        Fname2 = Filenames[index + 1].filename;
        webpage += "<tr style = 'background-color:#606060;'>";
        webpage += "<td style = 'width:5%'>" + Filenames[index].ftype + "</td><td style = 'width:25%'>" + Fname1 + "</td><td style = 'width:10%'>" + Filenames[index].fsize + "</td>";
        webpage += "<td class='sp'></td>";
        if (index < numfiles - 1) {
          webpage += "<td style = 'width:5%'>" + Filenames[index + 1].ftype + "</td><td style = 'width:25%'>" + Fname2 + "</td><td style = 'width:10%'>" + Filenames[index + 1].fsize + "</td>";
        }
        webpage += "</tr>";
        index = index + 2;
      }
      webpage += "</table>";
      webpage += "<p style='background-color:yellow;'><b>" + MessageLine + "</b></p>";
      MessageLine = "";
    } else {
      webpage += "<h2>No Files Found</h2>";
    }
    webpage += HTML_Footer();
    request->send(200, "text/html", webpage);
  }
}
//#############################################################################################
void Directory() {
  numfiles = 0;  // Reset number of FS files counter
  File root = FS.open("/");
  if (root) {
    root.rewindDirectory();
    File file = root.openNextFile();
    while (file) {  // Now get all the filenames, file types and sizes
      Filenames[numfiles].filename = (String(file.name()).startsWith("/") ? String(file.name()).substring(1) : file.name());
      Filenames[numfiles].ftype = (file.isDirectory() ? "Dir" : "File");
      Filenames[numfiles].fsize = ConvBinUnits(file.size(), 1);
      file = root.openNextFile();
      numfiles++;
    }
    root.close();
  }
}
//#############################################################################################
void UploadFileSelect() {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    webpage = HTML_Header();
    webpage += "<h3>Select a File to [UPLOAD] to this device</h3>";
    webpage += "<form method = 'POST' action = '/handleupload' enctype='multipart/form-data'>";
    webpage += "<input type='file' name='filename'><br><br>";
    webpage += "<input type='submit' value='Upload'>";
    webpage += "</form>";
    webpage += HTML_Footer();
  }
}
//#############################################################################################
void Format() {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    webpage = HTML_Header();
    webpage += "<h3>*** Format Filing System on this device? ***</h3>";
    webpage += "<form action='/handleformat'>";
    webpage += "<input type='radio' id='YES' name='format' value = 'YES'><label for='YES'>YES</label><br><br>";
    webpage += "<input type='radio' id='NO'  name='format' value = 'NO' checked><label for='NO'>NO</label><br><br>";
    webpage += "<input type='submit' value='Format?'>";
    webpage += "</form>";
    webpage += HTML_Footer();
  }
}
//#############################################################################################
void handleFileUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    String file = filename;
    if (!filename.startsWith("/")) file = "/" + filename;
    request->_tempFile = FS.open(file, "w");
    if (!request->_tempFile)
      start = millis();
  }
  if (request->_tempFile) {
    if (len) {
      request->_tempFile.write(data, len);  // Chunked data
    }
    if (final) {
      uploadsize = request->_tempFile.size();
      request->_tempFile.close();
      uploadtime = millis() - start;
      request->redirect("/dir");
    }
  }
}
//#############################################################################################
void File_Stream() {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    SelectInput("Select a File to Stream", "handlestream", "filename");
  }
}
//#############################################################################################
void File_Delete() {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    SelectInput("Select a File to Delete", "handledelete", "filename");
  }
}
//#############################################################################################
void Handle_File_Delete(String filename) {  // Delete the file
  webpage = HTML_Header();
  if (!filename.startsWith("/")) filename = "/" + filename;
  File dataFile = FS.open(filename, "r");  // Now read FS to see if file exists
  if (dataFile) {                          // It does so delete it
    FS.remove(filename);
    webpage += "<h3>File '" + filename.substring(1) + "' has been deleted</h3>";
    webpage += "<a href='/dir'>[Enter]</a><br><br>";
  } else {
    webpage += "<h3>File [ " + filename + " ] does not exist</h3>";
    webpage += "<a href='/dir'>[Enter]</a><br><br>";
  }
  webpage += HTML_Footer();
}
//#############################################################################################
void File_Rename() {  // Rename the file
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    Directory();
    webpage = HTML_Header();
    webpage += "<h3>Select a File to [RENAME] on this device</h3>";
    webpage += "<FORM action='/renamehandler'>";
    webpage += "<table class='center'>";
    webpage += "<tr><th>File name</th><th>New Filename</th><th>Select</th></tr>";
    int index = 0;
    while (index < numfiles) {
      webpage += "<tr style = 'background-color:#606060;'><td><input type='text' name='oldfile' style='color:black;' value = '" + Filenames[index].filename + "' readonly></td>";
      webpage += "<td><input type='text' name='newfile'></td><td><input type='radio' name='choice'></tr>";
      index++;
    }
    webpage += "</table><br>";
    webpage += "<input type='submit' value='Enter'>";
    webpage += "</form>";
    webpage += HTML_Footer();
  }
}
//#############################################################################################
void Handle_File_Rename(AsyncWebServerRequest *request, String filename, int Args) {  // Rename the file
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    String newfilename;
    //int Args = request->args();
    webpage = HTML_Header();
    for (int i = 0; i < Args; i++) {
      if (request->arg(i) != "" && request->arg(i + 1) == "on") {
        filename = request->arg(i - 1);
        newfilename = request->arg(i);
      }
    }
    if (!filename.startsWith("/")) filename = "/" + filename;
    if (!newfilename.startsWith("/")) newfilename = "/" + newfilename;
    File CurrentFile = FS.open(filename, "r");                                                // Now read FS to see if file exists
    if (CurrentFile && filename != "/" && newfilename != "/" && (filename != newfilename)) {  // It does so rename it, ignore if no entry made, or Newfile name exists already
      if (FS.rename(filename, newfilename)) {
        filename = filename.substring(1);
        newfilename = newfilename.substring(1);
        webpage += "<h3>File '" + filename + "' has been renamed to '" + newfilename + "'</h3>";
        webpage += "<a href='/dir'>[Enter]</a><br><br>";
      }
    } else {
      if (filename == "/" && newfilename == "/") webpage += "<h3>File was not renamed</h3>";
      else webpage += "<h3>New filename exists, cannot rename</h3>";
      webpage += "<a href='/rename'>[Enter]</a><br><br>";
    }
    CurrentFile.close();
    webpage += HTML_Footer();
  }
}

String processor(const String &var) {
  if (var == "HELLO_FROM_TEMPLATE") return F("Hello world!");
  return String();
}
//#############################################################################################
// Not found handler is also the handler for 'delete', 'download' and 'stream' functions
void notFound(AsyncWebServerRequest *request) {  // Process selected file types
  String filename;
  if (request->url().startsWith("/downloadhandler") || request->url().startsWith("/streamhandler") || request->url().startsWith("/deletehandler") || request->url().startsWith("/renamehandler")) {
    // Now get the filename and handle the request for 'delete' or 'download' or 'stream' functions
    if (!request->url().startsWith("/renamehandler")) filename = request->url().substring(request->url().indexOf("~/") + 1);
    start = millis();
    if (request->url().startsWith("/downloadhandler")) {
      MessageLine = "";
      File file = FS.open(filename, "r");
      String contentType = getContentType("download");
      AsyncWebServerResponse *response = request->beginResponse(contentType, file.size(), [file](uint8_t *buffer, long maxLen, long total) mutable -> long {
        return file.read(buffer, maxLen);
      });
      response->addHeader("Server", "ESP Async Web Server");
      request->send(response);
      downloadtime = millis() - start;
      downloadsize = GetFileSize(filename);
      // request->redirect("/dir");
    }
    if (request->url().startsWith("/streamhandler")) {
      String ContentType = getContentType(filename);
      AsyncWebServerResponse *response = request->beginResponse(FS, filename, ContentType);
      request->send(response);
      downloadsize = GetFileSize(filename);
      downloadtime = millis() - start;
      // request->redirect("/dir");
    }
    if (request->url().startsWith("/deletehandler")) {
      Handle_File_Delete(filename);  // Build webpage ready for display
      request->send(200, "text/html", webpage);
    }
    if (request->url().startsWith("/renamehandler")) {
      Handle_File_Rename(request, filename, request->args());  // Build webpage ready for display
      request->send(200, "text/html", webpage);
    }
  } else {
    Page_Not_Found();
    request->send(200, "text/html", webpage);
  }
}
//#############################################################################################
void Handle_File_Download() {
  String filename = "";
  int index = 0;
  Directory();  // Get a list of files on the FS
  webpage = HTML_Header();
  webpage += "<h3>Select a File to Download</h3>";
  webpage += "<table>";
  webpage += "<tr><th>File Name</th><th>File Size</th></tr>";
  while (index < numfiles) {
    webpage += "<tr><td style = 'background-color:#606060;'><a href='" + Filenames[index].filename + "'></a><td>" + Filenames[index].fsize + "</td></tr>";
    index++;
  }
  webpage += "</table>";
  webpage += "<p>" + MessageLine + "</p>";
  webpage += HTML_Footer();
}
//#############################################################################################
String getContentType(String filenametype) {  // Tell the browser what file type is being sent
  if (filenametype == "download") {
    return "application/octet-stream";
  } else if (filenametype.endsWith(".txt")) {
    return "text/plain";
  } else if (filenametype.endsWith(".htm")) {
    return "text/html";
  } else if (filenametype.endsWith(".html")) {
    return "text/html";
  } else if (filenametype.endsWith(".css")) {
    return "text/css";
  } else if (filenametype.endsWith(".js")) {
    return "application/javascript";
  } else if (filenametype.endsWith(".png")) {
    return "image/png";
  } else if (filenametype.endsWith(".gif")) {
    return "image/gif";
  } else if (filenametype.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filenametype.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filenametype.endsWith(".xml")) {
    return "text/xml";
  } else if (filenametype.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filenametype.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filenametype.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}
//#############################################################################################
void Select_File_For_Function(String title, String function) {
  if (eeprom_get_log()) {
    webpage = HTML_Header();
    webpage += "<h3>Filesystem is in use! Please disable the logging feature from the device's settings.</h3>";
    webpage += HTML_Footer();
  } else {
    String Fname1, Fname2;
    int index = 0;
    Directory();  // Get a list of files on the FS
    webpage = HTML_Header();
    webpage += "<h3>Select a File to " + title + " from this device</h3>";
    webpage += "<table class='center'>";
    webpage += "<tr><th>File Name</th><th>File Size</th><th class='sp'></th><th>File Name</th><th>File Size</th></tr>";
    while (index < numfiles) {
      Fname1 = Filenames[index].filename;
      Fname2 = Filenames[index + 1].filename;
      if (Fname1.startsWith("/")) Fname1 = Fname1.substring(1);
      if (Fname2.startsWith("/")) Fname1 = Fname2.substring(1);
      webpage += "<tr style = 'background-color:#606060;'>";
      webpage += "<td style='width:25%'><button><a href='" + function + "~/" + Fname1 + "'>" + Fname1 + "</a></button></td><td style = 'width:10%'>" + Filenames[index].fsize + "</td>";
      webpage += "<td class='sp'></td>";
      if (index < numfiles - 1) {
        webpage += "<td style='width:25%'><button><a href='" + function + "~/" + Fname2 + "'>" + Fname2 + "</a></button></td><td style = 'width:10%'>" + Filenames[index + 1].fsize + "</td>";
      }
      webpage += "</tr>";
      index = index + 2;
    }
    webpage += "</table>";
    webpage += HTML_Footer();
  }
}
//#############################################################################################
void SelectInput(String Heading, String Command, String Arg_name) {
  webpage = HTML_Header();
  webpage += "<h3>" + Heading + "</h3>";
  webpage += "<form  action='/" + Command + "'>";
  webpage += "Filename: <input type='text' name='" + Arg_name + "'><br><br>";
  webpage += "<input type='submit' value='Enter'>";
  webpage += "</form>";
  webpage += HTML_Footer();
}
//#############################################################################################
int GetFileSize(String filename) {
  int filesize;
  File CheckFile = FS.open(filename, "r");
  filesize = CheckFile.size();
  CheckFile.close();
  return filesize;
}
//#############################################################################################
void Home() {
  webpage = HTML_Header();
  webpage += "<h1>Geiger-Müller Counter</h1>";
  webpage += "<h4>with GPS Tracker and Data Logger</h4>";
  webpage += "<img src = 'icon' alt='icon'>";
  webpage += "<h3>Radiation Readings</h3>";
  webpage += "<table class='center'>";
  webpage += "<tr style = 'background-color:#606060;'><th>CPM (Counts)</th><th>Radiation 60s SMA (µSv/h)</th><th>Radiation (µSv/h)</th><th>Radiation (mRem/h)</th><th>Radiation (mR/h)</th></tr>";
  webpage += "<tr><td>" + String(geiger_getCPM()) + "</td>";
  webpage += "<td>" + String(geiger_getAvgSieverts()) + "</td>";
  webpage += "<td>" + String(geiger_getSieverts()) + "</td>";
  webpage += "<td>" + String(geiger_getRems()) + "</td>";
  webpage += "<td>" + String(geiger_getRoentgens()) + "</td></tr>";
  webpage += "</table> ";
  webpage += HTML_Footer();
}
//#############################################################################################
void Page_Not_Found() {
  webpage = HTML_Header();
  webpage += "<div class='notfound'>";
  webpage += "<h1>Sorry</h1>";
  webpage += "<p>Error 404 - Page Not Found</p>";
  webpage += "</div><div class='left'>";
  webpage += "<p>The page you were looking for was not found, it may have been moved or is currently unavailable.</p>";
  webpage += "<p>Please check the address is spelt correctly and try again.</p>";
  webpage += "<p>Or click <b><a href='/'>[Here]</a></b> for the home page.</p></div>";
  webpage += HTML_Footer();
}
//#############################################################################################
void Display_About() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  if (WiFi.scanComplete() == -2) WiFi.scanNetworks(true, false);  // Scan parameters are (async, show_hidden) if async = true, don't wait for the result
  webpage = HTML_Header();
  webpage += "<h3>System Information</h3>";
  webpage += "<h4>Transfer Statistics</h4>";
  webpage += "<table class='center'>";
  webpage += "<tr style = 'background-color:#606060;'><th>Last Upload</th><th>Last Download/Stream</th><th>Units</th></tr>";
  webpage += "<tr><td>" + ConvBinUnits(uploadsize, 1) + "</td><td>" + ConvBinUnits(downloadsize, 1) + "</td><td>File Size</td></tr> ";
  webpage += "<tr><td>" + ConvBinUnits((float)uploadsize / uploadtime * 1024.0, 1) + "/Sec</td>";
  webpage += "<td>" + ConvBinUnits((float)downloadsize / downloadtime * 1024.0, 1) + "/Sec</td><td>Transfer Rate</td></tr>";
  webpage += "</table>";
  webpage += "<h4>Filing System</h4>";
  webpage += "<table class='center'>";
  webpage += "<tr style = 'background-color:#606060;'><th>Total Space</th><th>Used Space</th><th>Free Space</th><th>Number of Files</th></tr>";
  webpage += "<tr><td>" + ConvBinUnits(FS.totalBytes(), 1) + "</td>";
  webpage += "<td>" + ConvBinUnits(FS.usedBytes(), 1) + "</td>";
  webpage += "<td>" + ConvBinUnits(FS.totalBytes() - FS.usedBytes(), 1) + "</td>";
  webpage += "<td>" + (numfiles == 0 ? "Pending Dir or Empty" : String(numfiles)) + "</td></tr>";
  webpage += "</table>";
  webpage += "<h4>CPU Information</h4>";
  webpage += "<table class='center'>";
  webpage += "<tr style = 'background-color:#606060;'><th>Parameter</th><th>Value</th></tr>";
  webpage += "<tr><td>Number of Cores</td><td>" + String(chip_info.cores) + "</td></tr>";
  webpage += "<tr><td>Chip revision</td><td>" + String(chip_info.revision) + "</td></tr>";
  webpage += "<tr><td>Internal or External Flash Memory</td><td>" + String(((chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "Embedded" : "External")) + "</td></tr>";
  webpage += "<tr><td>Flash Memory Size</td><td>" + String((spi_flash_get_chip_size() / (1024 * 1024))) + " MB</td></tr>";
  webpage += "<tr><td>Current Free RAM</td><td>" + ConvBinUnits(ESP.getFreeHeap(), 1) + "</td></tr>";
  webpage += "</table>";
  webpage += "<h4>Network Information</h4>";
  webpage += "<table class='center'>";
  webpage += "<tr style = 'background-color:#606060;'><th>Parameter</th><th>Value</th></tr>";
  webpage += "<tr><td>LAN IP Address</td><td>" + String(ft_server_ip[0]) + "." + String(ft_server_ip[1]) + "." + String(ft_server_ip[2]) + "." + String(ft_server_ip[3]) + "</td></tr>";
  webpage += "<tr><td>Network Adapter MAC Address</td><td>" + String(WiFi.softAPmacAddress()) + "</td></tr>";
  webpage += "<tr><td>WiFi SSID</td><td>" + String(ft_server_ssid) + "</td></tr>";
  webpage += "<tr><td>WiFi Channel</td><td>" + String(WiFi.channel()) + "</td></tr>";
  webpage += "<tr><td>WiFi Encryption Type</td><td>" + String(EncryptionType(WiFi.encryptionType(0))) + "</td></tr>";
  webpage += "</table> ";
  webpage += HTML_Footer();
}
//#############################################################################################
String ConvBinUnits(int bytes, int resolution) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < 1024 * 1024) {
    return String((bytes / 1024.0), resolution) + " KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String((bytes / 1024.0 / 1024.0), resolution) + " MB";
  } else return "";
}
//#############################################################################################
String EncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "OPEN";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2 PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA WPA2 PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2 ENTERPRISE";
    case (WIFI_AUTH_MAX):
      return "WPA2 MAX";
    default:
      return "";
  }
}
//#############################################################################################
bool StartMDNSservice(const char *Name) {
  esp_err_t err = mdns_init();  // Initialise mDNS service
  if (err) {
    printf("MDNS Init failed: %d\n", err);  // Return if error detected
    return false;
  }
  mdns_hostname_set(Name);  // Set hostname
  return true;
}
//#############################################################################################
String HTML_Header() {
  String page;
  page = "<!DOCTYPE html>";
  page += "<html lang = 'en'>";
  page += "<head>";
  page += "<title>M5Core2 GM Counter</title>";
  page += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\" charset='UTF-8'>";  // Needed if you want to display special characters like the ° symbol
  page += "<style>";
  page += "body {width:75em;margin-left:auto;margin-right:auto;font-family:Arial,Helvetica,sans-serif;font-size:16px;color:yellow;background-color:black;text-align:center;}";
  page += "footer {padding:0.08em;background-color:yellow;font-size:1.1em;color:black;}";
  page += "table {font-family:arial,sans-serif;border-collapse:collapse;width:70%;}";  // 70% of 75em!
  page += "table.center {margin-left:auto;margin-right:auto;}";
  page += "td, th {border:1px solid #dddddd;text-align:left;padding:8px;}";
  page += "tr:nth-child(even) {background-color:black;}";
  page += "h4 {color:yellow;font:0.8em;text-align:left;font-style:oblique;text-align:center;}";
  page += ".center {margin-left:auto;margin-right:auto;}";
  page += ".topnav {overflow: hidden;background-color:yellow;}";
  page += ".topnav a {float:left;color:black;text-align:center;padding:0.6em 0.6em;text-decoration:none;font-size:1.3em;}";
  page += ".topnav a:hover {background-color:#606060;color:yellow;}";
  page += ".topnav a.active {background-color:yellow;color:black;}";
  page += ".notfound {padding:0.8em;text-align:center;font-size:1.5em;}";
  page += ".left {text-align:left;}";
  page += ".medium {font-size:1.4em;padding:0;margin:0}";
  page += ".ps {font-size:0.7em;padding:0;margin:0}";
  page += ".sp {background-color:#606060;white-space:nowrap;width:2%;}";
  page += "</style>";
  page += "</head>";
  page += "<body>";
  page += "<div class = 'topnav'>";
  page += "<a href='/'>Home</a>";
  page += "<a href='/dir'>Directory</a>";
  page += "<a href='/upload'>Upload</a> ";
  page += "<a href='/download'>Download</a>";
  page += "<a href='/stream'>Stream</a>";
  page += "<a href='/delete'>Delete</a>";
  page += "<a href='/rename'>Rename</a>";
  page += "<a href='/system'>About</a>";
  page += "<a href='/format'>Format FS</a>";
  // page += "<a href='/newpage'>New Page</a>";
  // page += "<a href='/logout'>[Log-out]</a>";
  page += "</div>";
  return page;
}
//#############################################################################################
String HTML_Footer() {
  String page;
  page += "<br><br><footer>";
  page += "<p class='ps'><i>This asynchronous file webserver source code is provided by G6EJD - David on Github. Copyright &copy;&nbsp;David L Bird 2021</i></p>";
  page += "</footer>";
  page += "</body>";
  page += "</html>";
  return page;
}

void init_file_transfer(void) {

  if (!FS.begin(true)) {
    Serial.println("Failed to mount filesystem!");
  } else {
    Serial.println("Filesystem mounted!");
  }

  // ##################### HOMEPAGE HANDLER ###########################
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Home();  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### DOWNLOAD HANDLER ##########################
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request) {
    Select_File_For_Function("[DOWNLOAD]", "downloadhandler");  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### UPLOAD HANDLERS ###########################
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request) {
    UploadFileSelect();  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // Set handler for '/handleupload'
  server.on(
    "/handleupload", HTTP_POST, [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
       size_t len, bool final) {
      handleFileUpload(request, filename, index, data, len, final);
    });

  // Set handler for '/handleformat'
  server.on("/handleformat", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->getParam("format")->value() == "YES") {
      FS.end();
      bool formatted = FS.format();
    }
    request->redirect("/dir");
  });

  // ##################### STREAM HANDLER ############################
  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request) {
    Select_File_For_Function("[STREAM]", "streamhandler");  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### RENAME HANDLER ############################
  server.on("/rename", HTTP_GET, [](AsyncWebServerRequest *request) {
    File_Rename();  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### DIR HANDLER ###############################
  server.on("/dir", HTTP_GET, [](AsyncWebServerRequest *request) {
    Dir(request);  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### DELETE HANDLER ############################
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    Select_File_For_Function("[DELETE]", "deletehandler");  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### FORMAT HANDLER ############################
  server.on("/format", HTTP_GET, [](AsyncWebServerRequest *request) {
    Format();  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### SYSTEM HANDLER ############################
  server.on("/system", HTTP_GET, [](AsyncWebServerRequest *request) {
    Display_About();  // Build webpage ready for display
    request->send(200, "text/html", webpage);
  });

  // ##################### IMAGE HANDLER ############################
  server.on("/icon", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(FS, "/icon.gif", "image/gif");
  });

  // ##################### NOT FOUND HANDLER #########################
  server.onNotFound(notFound);
}

void wifi_task(void *pvParameters) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ft_server_ssid);
  ft_server_ip = WiFi.softAPIP();
  vTaskDelete(NULL);
}

void file_transfer_update(void) {
  if (eeprom_get_ft() && !ft_server_started) {
    // xTaskCreatePinnedToCore(wifi_task, "wifi_task", 8192, NULL, 95, NULL, 0);
    server.begin();  // Start the server
    Directory();     // Update the file list
    ft_server_started = true;
  }
  if (!eeprom_get_ft() && ft_server_started) {
    server.end();
    // WiFi.softAPdisconnect(true);
    ft_server_ip[0] = 0;
    ft_server_ip[1] = 0;
    ft_server_ip[2] = 0;
    ft_server_ip[3] = 0;
    ft_server_started = false;
  }
  if (eeprom_get_log() && geiger_getDangerLevel() > 0) {
    file_transfer_write_csv_entry();
  }
}

bool file_transfer_server_started(void) {
  return ft_server_started;
}

IPAddress file_transfer_get_ip(void) {
  return ft_server_ip;
}

int file_transfer_get_connections(void) {
  return WiFi.softAPgetStationNum();
}

void file_transfer_write_csv_entry(void) {
  char fileNameToLogTo[35];
  sprintf(fileNameToLogTo, "/%04d.%02d.%02d.csv", years, months, days);
  bool fe = FS.exists(fileNameToLogTo);
  if (fe) {
    File dataFileToLogTo = FS.open(fileNameToLogTo, FILE_APPEND);
    char entry[200];
    sprintf(entry, "%ld,%04d-%02d-%02d,%02d:%02d:%02d,", unix, years, months, days, hours, minutes, seconds);
    dataFileToLogTo.print(entry);
    sprintf(entry, "%04d,%05.2f,%05.2f,%04.2f,%06.2f,%d,", geiger_getCPM(), geiger_getAvgSieverts(), geiger_getSieverts(), geiger_getRems(), geiger_getRoentgens(), geiger_getDangerLevel());
    dataFileToLogTo.print(entry);
    sprintf(entry, "%.6f,%.6f,%.1f,", gps_get_lat(), gps_get_long(), gps_get_alt());
    dataFileToLogTo.print(entry);
    dataFileToLogTo.println(gps_get_sat());
    dataFileToLogTo.close();
  } else {
    File dataFileToLogTo = FS.open(fileNameToLogTo, FILE_WRITE);
    char entry[200];
    dataFileToLogTo.println(F("unix,date,time,cpm,avg_usv/h,usv/h,mrem/h,mr/h,danger_level,longitude,latitude,altitude,satellites"));
    sprintf(entry, "%ld,%04d-%02d-%02d,%02d:%02d:%02d,", unix, years, months, days, hours, minutes, seconds);
    dataFileToLogTo.print(entry);
    sprintf(entry, "%04d,%05.2f,%05.2f,%04.2f,%06.2f,%d,", geiger_getCPM(), geiger_getAvgSieverts(), geiger_getSieverts(), geiger_getRems(), geiger_getRoentgens(), geiger_getDangerLevel());
    dataFileToLogTo.print(entry);
    sprintf(entry, "%.6f,%.6f,%.1f,", gps_get_lat(), gps_get_long(), gps_get_alt());
    dataFileToLogTo.print(entry);
    dataFileToLogTo.println(gps_get_sat());
    dataFileToLogTo.close();
  }
  last_record_unix = unix;
}

uint32_t get_last_record_unix(void) {
  return last_record_unix;
}