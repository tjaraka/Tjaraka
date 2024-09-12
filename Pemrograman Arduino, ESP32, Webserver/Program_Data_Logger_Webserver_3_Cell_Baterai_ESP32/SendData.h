//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String Web = webpage; //Read HTML contents
 server.send(200, "text/html", Web); //Send web page
}

float Cell1;
float Cell2;
float Cell3;

void handleCELL() {
 int ukur1 = analogRead(Sensor1);
 int ukur2 = analogRead(Sensor2);
 int ukur3 = analogRead(Sensor3);
 String dataCELL = "{\"Cell1\":\""+String(Cell1)+"\", \"Cell2\":\""+ String(Cell2) +"\", \"Cell3\":\""+ String(Cell3) +"\"}";
 
 digitalWrite(LED,!digitalRead(LED)); //Toggle LED on data request ajax
 server.send(200, "text/plane", dataCELL); //Send ADC value, temperature and humidity JSON to client ajax request
 
 float hitungan1 = ((ukur1 *3.3)/4095);
 float akhir1 = hitungan1 / (3300.0/(900.0+3300.0));
 Cell1 = akhir1;
 float hitungan2 = ((ukur2 *3.3)/4095);
 float akhir2 = hitungan2 / (3300.0/(900.0+3300.0));
 Cell2 = akhir2;
 float hitungan3 = ((ukur3 *3.3)/4095);
 float akhir3 = hitungan3 / (3300.0/(900.0+3300.0));
 Cell3 = akhir3;
 
 Serial.print("Cell 1 : ");
 Serial.print(Cell1);
 Serial.println(" Volt");
 Serial.print("Cell 2 : ");
 Serial.print(Cell2);
 Serial.println(" Volt");
 Serial.print("Cell 3 : ");
 Serial.print(Cell3);
 Serial.println(" Volt");
}
