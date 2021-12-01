#include <SoftwareSerial.h>

SoftwareSerial gprs(2, 3); // RX, TX

String dataMasuk = "";

String Server = "AT+HTTPPARA=\"URL\",\"http://tytomulyono.com/test.php";

void setup() {
  Serial.begin(9600);
  gprs.begin(9600);

  dataMasuk.reserve(200);
}

void loop() {
  request();
  
  delay(10000);
}


void request(){
  gprs.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  ShowResponse(3000);

  gprs.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
  ShowResponse(3000);

  gprs.println("AT+SAPBR=1,1");
  ShowResponse(5000);

  gprs.println("AT+HTTPINIT");
  ShowResponse(5000);

  Serial.println(Server);
  gprs.print(Server);
  gprs.println("\"");
  ShowResponse(3000);
  
  // set http action type 0 = GET, 1 = POST, 2 = HEAD
  gprs.println("AT+HTTPACTION=0");
  GetResponse(10000);

  gprs.println("AT+HTTPREAD");
  GetResponse(5000);               // respon di simpan kedalam variabel dataMasuk

  Serial.print("Full data :");
  Serial.println(dataMasuk);

  String data0 = parse(dataMasuk, '#', 0);
  String data1 = parse(dataMasuk, '#', 1);
  String data2 = parse(dataMasuk, '#', 2);

  Serial.println("--------------------------------------------------------------------");
  
  Serial.print("data 0 :");
  Serial.println(data0);
  
  Serial.print("data 1 :");
  Serial.println(data1);
  
  Serial.print("data 2 :");
  Serial.println(data2);

  Serial.println("--------------------------------------------------------------------");
    
  gprs.println("AT+HTTPTERM");
  ShowResponse(3000);

  gprs.println("AT+SAPBR=0,1");
  ShowResponse(5000);
}



void ShowResponse(int wait){
  Serial.print("response : ");
  long timeNOW = millis();
  while(millis()-timeNOW < wait){
    if(gprs.find("OK")){
      Serial.println("GOOD");
      return;
    }
    delay(10);
  }
  Serial.println("error time out");
}


void GetResponse(int wait){
  dataMasuk = "";
  Serial.print("response : ");
  long timeNOW2 = millis();
  while(millis()-timeNOW2 < wait){
    while(gprs.available()>0){
      dataMasuk += (char)gprs.read();
    }
    delay(1);
  }
  Serial.println(dataMasuk);
  Serial.println();
}



String parse(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  } 

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
