// PAGE_Root.cpp

#include "PAGE_Root.h"



void sendRootPage() {        
    if (web_server.args() > 0 ) { // Are there any POST/GET Fields ? 
       for ( uint8_t i = 0; i < web_server.args(); i++ ) {  // Iterate through the fields
            
        } ;
    } ;
    web_server.send ( 200, "text/html", PAGE_Root ); 
}
