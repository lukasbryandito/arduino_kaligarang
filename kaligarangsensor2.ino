    #include <SPI.h>
    #include <Ethernet.h>
    #include <TinyGPS++.h>
    #include <SoftwareSerial.h>
    #include "DHT.h"
    #define DHTPIN 9
    #define DHTTYPE DHT22
    DHT dht(DHTPIN, DHTTYPE);
    
    SoftwareSerial serial_gps(4,3);
    TinyGPSPlus gps;
    
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//mac addrs
    byte ip[] = {192, 168, 1, 64 }; //ip ethernet
    byte serv[] = {192, 168, 1, 9} ; //ip pc
    EthernetClient cliente;
    
    const int analogInPin = A0; 
    int sensorValue = 0; 
    int buf[10],temp;
    unsigned long int avgValue;
    const int trigger=7;
    const int echo=8;
    float jrk; 
    float b;
    float latitude, longitude;
    static float ntu;
    static float teg;
  
    //SETUP-------------------------------------
    void setup() {
    Serial.begin(9600); //setting the baud rate: 9600
    serial_gps.begin(9600);
    Ethernet.begin(mac, ip);
    dht.begin();
    pinMode(trigger,OUTPUT);
    pinMode(echo,INPUT);
    }


   //LOOP---------------------------------------
    void loop() {
       for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 //jarak
      digitalWrite(trigger,LOW);
      delayMicroseconds(5);
      digitalWrite(trigger,HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger,LOW);
      jrk = pulseIn(echo,HIGH);

      if(jrk>1.0){
        jrk = 1.0;
      }

      while(serial_gps.available()) 
          {
          gps.encode(serial_gps.read());
          }
        if(gps.location.isUpdated()) 
          {
            latitude = gps.location.lat();
            longitude = gps.location.lng();
          }
        String lokasi = String(latitude) + "," + String(longitude);
 
    float hum = dht.readHumidity(); //Pembacan humidity dan akan memunculakan di hum
    float temp = dht.readTemperature(); //Pembacaan temperatur dalam celsius dan akan memunculkannya di temp
    //float fah = dht.readTemperature(true); //reading the temperature in Fahrenheit pembacaan temperatur dalam satuan fahrenheit
    //float heat_index = dht.computeHeatIndex(fah, hum); //Pembacaan heat index dalam Fahrenheit
    //float heat_indexC = dht.convertFtoC(heat_index); //Convert heat index dalam Celsius
     
    float pHVol = (float)avgValue*5.0/1024/6;
    float phValue = -5.70 * pHVol + 21.34;

    float distance = (float)jrk*0.0001657;

    int val = analogRead(A1);
    teg = (float)val*(5.0/1024);
    float ntu = 10000-((float)teg/4.52)*10000;

    
    if (cliente.connect(serv, 80)) { //Connecting IP address and port 
      
    Serial.println("connected");
    cliente.print("GET /data/data.php?"); //Menghubungkan dan mengirim values ke db Kaligarang ikom
    cliente.print("temperatur=");
    cliente.print(temp);
    cliente.print("&kelembaban=");
    cliente.print(hum);
    cliente.print("&ph=");
    cliente.print(phValue);
    cliente.print("&volt=");
    cliente.print(pHVol);
    cliente.print("&tinggi=");
    cliente.print(distance);
    cliente.print("&lokasi=");
    cliente.print(lokasi);
    cliente.print("&teg= ");
    cliente.print(teg);
    cliente.print("&ntu=");
    cliente.println(ntu);

    
    //Cetak values dalam serial monitor
    Serial.print("Temperature= ");
    Serial.println(temp);
    Serial.print("Kelembaban= ");
    Serial.println(hum);
    Serial.print("ph Vol = ");
    Serial.println(pHVol);
    Serial.print("ph Value = ");
    Serial.println(phValue);
    //Serial.print("sensor avgValue = ");
    //Serial.println(avgValue);
    Serial.print("tinggi=" );
    Serial.println(distance);
    Serial.println(lokasi);
    Serial.print("teg=");
    Serial.println(teg);
    Serial.print("ntu =");
    Serial.print(ntu);
    
    cliente.stop(); //koneksi berhenti
    }
    else {
    
    Serial.println("connection failed");// connection failed akan muncul ketika tidak mendapatkan koneksi
    }
    delay(5000);
    }
