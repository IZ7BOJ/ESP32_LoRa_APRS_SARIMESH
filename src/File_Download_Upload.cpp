// File_Download_Upload.cpp
// from  https://github.com/G6EJD/ESP32-8266-File-Upload

#include "GUI/CSS.h"
#include "File_Download_Upload.h"
char NewConfFile[128]; 

// All supporting functions from here...

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Header(){
  web_server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  web_server.sendHeader("Pragma", "no-cache"); 
  web_server.sendHeader("Expires", "-1"); 
  web_server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  web_server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();
  web_server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Content(){
  web_server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Stop(){
  web_server.sendContent("");
  web_server.client().stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SelectInput(String heading1, String command, String arg_calling_name){
  SendHTML_Header();
  webpage += F("<h3>"); webpage += heading1 + "</h3>"; 
  webpage += F("<FORM action='/"); webpage += command + "' method='post'>"; // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
  webpage += F("<input type='text' name='"); webpage += arg_calling_name; webpage += F("' value=''><br>");
  webpage += F("<type='submit' name='"); webpage += arg_calling_name; webpage += F("' value=''><br>");
  webpage += F("<a href='/files.html'>[Back]</a>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportLittleFSNotPresent(){
  SendHTML_Header();
  webpage += F("<h3>No LittleFS Card present</h3>"); 
  webpage += F("<a href='/files.html'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportFileNotPresent(String target){
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/files.html");  webpage += "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportFSNotPresent(void){
  SendHTML_Header();
  webpage += F("<h3>LittleFS not available</h3>"); 
  webpage += F("<a href='/files.html"); webpage +=  "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportCouldNotCreateFile(String target){
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String file_size(int bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void LittleFS_file_download(String filename){
  if (has_LittleFS) { 
    File download = LittleFS.open("/"+filename);    
    if (download) {
      web_server.sendHeader("Content-Type", "text/text");
      web_server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      web_server.sendHeader("Connection", "close");
      web_server.streamFile(download, "application/octet-stream");
      download.close();
      } 
      else ReportFileNotPresent("download"); 
    }
  else ReportFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Upload(){
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>"); 
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:40%' type='file' name='fupload' id = 'fupload' value=''><br>");
  webpage += F("<br><button class='buttons' style='width:10%' type='submit'>Upload File</button><br>");
  webpage += F("<a href='/files.html'>[Back]</a><br><br>");
  append_page_footer();
  web_server.send(200, "text/html",webpage);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Clear(){
  dropAllFiles();
  append_page_header();
  webpage += F("<h3>Removed all unneeded files....</h3>"); 
  webpage += F("<a href='/files.html'>[Back]</a><br><br>");
  append_page_footer();
  web_server.send(200, "text/html",webpage);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
File UploadFile; 
void handleFileUpload(){ // upload a new file to the Filing system
  HTTPUpload& uploadfile = web_server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    strcpy((char *)NewConfFile,filename.c_str());
//    debugA("======> NewConfFile= [%s] \n", NewConfFile);
    Serial.print("Upload File Name: "); Serial.println(filename);
    LittleFS.remove(filename);                         // Remove a previous version, otherwise data is appended the file again
    UploadFile = LittleFS.open(filename, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          // If the file was successfully created
    {                                    
      UploadFile.close();   // Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      debugA("Loading [%s] to running configuration\n", NewConfFile );
      loadConfigurationFromFlash(NewConfFile);         // load config from LittleFS to running config
      UpdateLocalConfigData();             // Update running configuration dependent data
      debugA("Saving running configuration to FRAM / LittleFS\n" );      
      WriteConfig();                       // write running configuration to FRAM or LittleFS 
      if(has_FM24W256){
         saveConfigurationToFlash("/WebConf.conf");  // save running configuration to standard config file on LittleFS FS
         };
      
      webpage = "";
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>"); 
      webpage += F("<h2>Uploaded File Name: "); webpage += uploadfile.filename+"</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br>"; 
      webpage += F("<center><a href='/files.html'>[Back]</a></center>");
      append_page_footer();
      web_server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FilesHomePage(){
  Serial.println("LittleFS Content");
  listDir(LittleFS, "/", 3);
  Serial.println("--------------------\r\n");
  
  SendHTML_Header();

 if(Admin_Mode){
  webpage += F("<a href='/download'><button>Save</button></a>");
  webpage += F("<a href='/upload'><button>Restore</button></a>");
  webpage += F("<a href='/admin.html'><button>Home</button></a>");
  webpage += F("<a href='/clear'><button>Clear</button></a>");  
  webpage += F("<center><H1> ESP32 File List</H1></center>");
  webpage += F("<center>Use <A href='https://jsonformatter.org/json-pretty-print'  target='JsonBrowser'>JSON Browser</A> to display configuration files</H1></center>");
  webpage += ("<center><table width=80%>"); 

     File root = LittleFS.open("/");
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
 //           webpage += ("<tr><td>DIR</td><td>"); webpage += file.name(); webpage +=("</td><td></td></tr>");
            Serial.print("  DIR : ");
            Serial.println(file.name());
   //         if(levels){
   //             listDir(LittleFS, file.name(), levels -1);
   //         }
        } else {
            webpage += ("<tr><td>FILE</td><td>"); webpage += file.name(); webpage +=("</td><td>SIZE: "); webpage += file.size(); webpage +=("</td></tr>");
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
  webpage += ("</table></center>"); 
  }
else{
  webpage += F("<center><H1> Please enter Admin Mode to use these functions</H1></center>");
  webpage += F("<a href='/admin.html'>[Back]</a>");
};
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Download(){ // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
/*
  if (web_server.args() > 0 ) { // Arguments were received
    if (web_server.hasArg("download")) LittleFS_file_download(web_server.arg(0));
  }
  else SelectInput("Enter filename to download","download","download");
*/

//  String date_tag = String(UTC.dateTime(now(), "YmdHis"));
//  String date_tag = String(UTC.dateTime(now(), "His"));
//  String SaveFileName = "/WebConf_" + DeviceId + "-"+ (String) date_tag +".conf";
//  String SaveFileName = "" + DeviceId + "-"+ (String) date_tag +".conf";
  String SaveFileName = "" + DeviceId + ".conf";
  String ConfFileName = "/" + DeviceId + ".conf";
 
  debugA("Saving current configuration to file %s\r\n", SaveFileName.c_str()); 
  saveConfigurationToFlash(ConfFileName.c_str());  // to LittleFS FS
  LittleFS_file_download(SaveFileName.c_str());

}




void listAllFiles(){
  File root = LittleFS.open("/");
  File file = root.openNextFile();
 
  while(file){
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
    };
}

void dropAllFiles(){
  File root = LittleFS.open("/");
  File file = root.openNextFile();

  while(file){
    if( strcmp(file.name(), "/WebConf.conf")){
      strcpy(NewConfFile,file.name());    
      Serial.print("Removing FILE: ");
      Serial.println(file.name());
      file.close();
      LittleFS.remove(NewConfFile);     
      };
     file = root.openNextFile();
    };
}
