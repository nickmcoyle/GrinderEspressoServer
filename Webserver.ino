void handleWebsite() {
  buildWebsite();
  server.send(200, "text/html", WebSite);
}

void handleXML() {
  buildXML();
  server.send(200, "text/xml", XML);
}

void initWebServer() {  
  //server response for any URL
  server.on("/", [](){
    /*
    //auth
    if(!server.authenticate(WWW_USERNAME, WWW_PASSWORD)) {
      return server.requestAuthentication();
    } else {
      handleWebsite();      
      server.send(200, "text/plain", "Login OK");
    }
    */
    handleWebsite();
    server.send(200, "text/plain", "Okay");
  });
  server.on("/xml", handleXML);
  server.on("/togglePower", togglePower);
  server.begin();
}
