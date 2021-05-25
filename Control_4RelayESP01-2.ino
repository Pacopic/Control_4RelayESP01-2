
/*Este programa envia la activación y desactivación de los relés mediante una web por el puerto serie a la placa de control.
Tiene una IP Fija que se proporciona como parámetro al grabar el ESP
ÉSTE PROGRAMA FUNCIONA CON LA PLACA DE 4 RELÉS:

  IMPORTANTE: Para que funcione con la placa de 4 reles, hay que ver que este encendido el led rojo en ésta. si no lo está, 
  desconectar la alimentacion, mantener pulsado S1 y conectar la alimentación.
  (If you want to use your own ESP8266 firmware, 
  you can select "Mode 1", wait for the green LED slow blink(Red LED on all the time and green LED blink), then send relay 
  commands from UART of ESP8266 to UART of MCU(N76E003) to controll the relay.)

El canal uno se prevee un pestillo de puerta, por lo que se temporiza su actuación

SEGURIDAD: este sistema no es un sistema domótico testado, es una práctica con la que aprender el control de diferentes dispositivos. 
Se responsable con su uso. 
*/

#include <ESP8266WiFi.h>


String Firmware = "Software Control_4Relay_IPFija_ed2";//nombre que aparece abajo de la web para saber que versión de firmware se esta usando.

//Configuración TEXTO para la WEB: Nombre de los dispositivos instalados en cada canal:
 String CANAL1 = "Puerta "; //se inicia cada texto con <h1> para indicar que es un texto con tamaño encabezado 1 y se acaba con </h1> para indicar que termina el tamaño de letra.
 String CANAL2 = "Aux ";
 String CANAL3 = "Enchufe ";
 String CANAL4 = "Bomba Agua ";
 String ZONA = "Zona Exterior1";
 String value = "Seleccione acción"; //declaramos la variable que nos indicará el estado del relé.

//Configura tus datos de red
const char* ssid = "Nombredetured"; // nombre de SSID entre ""
const char* password = "Contraseñadetured"; // contraseña de red entre "

//Usamos una IP fija para facilitar la conexión e identificación del dispositivo.
//Cambia los 0 por el valor de tu red. IMPORTANTE: separados por coma y no por punto.
IPAddress ip(0,0,0,0);
IPAddress gateway(0,0,0,0);
IPAddress subnet(255,255,255,0);

//========Inicio programa=================================================================================
WiFiServer server(80);
unsigned long TIME=0;

//variabgles para almacenar el estado de los reles
  boolean RL1 = 0;
  boolean RL2 = 0;
  boolean RL3 = 0;
  boolean RL4 = 0;

//subrutinas para activación/desactivación de relés
void ACT_RL1()  //secuencia para activación del relé 1
    {
    value = "RELAY1=ON";
    Serial.write (0xA0);  
    Serial.write (0x01);  
    Serial.write (0x01);  
    Serial.write (0xA2); 
    RL1 = 1;
    return; 
    }
void DESACT_RL1() //secuencia para desactivación del relé 1
    {
    value = "RELAY1=OFF";
    Serial.write (0xA0);  
    Serial.write (0x01);  
    Serial.write (0x00);  
    Serial.write (0xA1); 
    RL1 = 0;
    return; 
    }

void ACT_RL2()  //secuencia para activación del relé 2
    {
    value = "RELAY2=ON";
    Serial.write (0xA0);  
    Serial.write (0x02);  
    Serial.write (0x01);  
    Serial.write (0xA3);  
    RL2 = 1;
    return; 
    }
 
void DESACT_RL2() //secuencia para desactivación del relé 2
    {
    value = "RELAY2=OFF";
    Serial.write (0xA0);  
    Serial.write (0x02);  
    Serial.write (0x00);  
    Serial.write (0xA2); 
    RL2 = 0;
    return; 
    }
  
void ACT_RL3 () //secuencia para activación del relé 3
    {
    value = "RELAY3=ON";
    Serial.write(0xA0);  
    Serial.write(0x03);  
    Serial.write(0x01);  
    Serial.write(0xA4);  
    RL3 = 1;
    return; 
    }

void DESACT_RL3 ()  //secuencia para desactivación del relé 
    {
    value = "RELAY3=OFF";
    Serial.write (0xA0);  
    Serial.write (0x03);  
    Serial.write (0x00);  
    Serial.write (0xA3); 
    RL3 = 0;
    return; 
    }
    
void ACT_RL4()  //secuencia para activación del relé 4
    {
    value = "RELAY4=ON";
    Serial.write (0xA0);  
    Serial.write (0x04);  
    Serial.write (0x01);  
    Serial.write (0xA5); 
    RL4 = 1;
    return; 
    }

void DESACT_RL4() //secuencia para desactivación del relé 4
    {
    value = "RELAY4=OFF";
    Serial.write (0xA0);  
    Serial.write (0x04);  
    Serial.write (0x00);  
    Serial.write (0xA4); 
    RL4 = 0;
    return; 
    }
    
void setup() 
{
  //===Comunicación Serie
  Serial.begin(115200); //  
                     
  // Inicio Conexión a Red Wifi
  //añadido para la iP fija
  WiFi.mode(WIFI_STA);  //Wifi en modo estación. Se utiliza para conectar el módulo ESP a una red wifi existente.
  WiFi.config(ip,gateway,subnet); //Configuración de la conexión para IP fija. Se indica la IP,La puerta de enlace y la máscara de red que previamente se han indicado.
  WiFi.begin(ssid, password);   //Inica el intento de conexión a la red SSID con la pasword que se le proporcionó.

  while (WiFi.status() != WL_CONNECTED) //Bucle. Mientras el estado de WiFi.status no sea el de "WL_CONECTED" se ejecutará.
  {
    delay(1000);
  }
  server.begin();   // Inicia el servidor Web. 
  
  // Al reiniciar el dispositivo, arrancamos con todas las salidas desactivadas.
  DESACT_RL1 ();
  DESACT_RL2 ();
  DESACT_RL3 ();
  DESACT_RL4 ();
}

void loop()         //Bucle para conectarse a un cliente.
{
// Check if a client has connected
  WiFiClient client = server.available();
  if (!client) //espera a que haya un cliente. mientras no tenga cliente, volverá atrás y a esperar a que se conecte uno.
  {
    return;
  }
TIME = millis()+2000;  //guardar el valor de millis            
  while(!client.available())   // Wait until the client sends some data
  {
    delay(3);
    if (TIME < millis())//comprobar el valor de millis  si es mayor que millis mas un valor, salir
      {                 //esto es para evitar que se quede en un bucle infinito. hay un tiempo de 2segundos para que se conecte.
        break;
      }
  }
// Lectura de la línea de respuesta del navegador.
  String request = client.readStringUntil('\r'); //Guarda en el String request la petición del cliente hasta que se termine la línea, es decir, cuando encuentra \rla primera línea de petición del cliente.
  int numclient = WiFi.softAPgetStationNum();
  client.flush(); //Borran todos los datos que quedaron en el buffer del cliente. 
 //Comprobación de la petición del cliente: (se puede hacer por switch y case)
  if (request.indexOf("/REL1=ON") != -1)  //Si el requerimiento del cliente tiene el texto "/REL1=ON" ejecuta el if.
    {
    ACT_RL1 (); //Funciona como pulsaror, por lo que le damos un tiempo y desactivamos.
    delay(3000);
    DESACT_RL1 ();
    }
  if (request.indexOf("/REL1=OFF") != -1)  //busca en la variable request, la cadena /REL1=OFF. Si la encuentra, devolverá el valor de la posición donde esta. Si no la encuentra, devolverá -1.
  {                                        //se ejecutará la condición si el valor devuelto es distinto a -1, es decir, si ha encontrado el valor en cualquier posición. 
    DESACT_RL1 ();
  }
//Activacion/Desactivacion Relé 2
      if (request.indexOf("/REL2=ON") != -1)  //Aquí busca el valor final de la URL. 
          {
          ACT_RL2 (); 
          }
      if (request.indexOf("/REL2=OFF") != -1)  
        {
          DESACT_RL2 ();
        }
  //Activacion/Desactivacion Relé 3
      if (request.indexOf("/REL3=ON") != -1)  //Aquí busca el valor final de la URL. 
          {
          ACT_RL3 ();
          }
      if (request.indexOf("/REL3=OFF") != -1)  
        {
          DESACT_RL3 ();
        }
//Activacion/Desactivacion Relé 4
      if (request.indexOf("/REL4=ON") != -1)  //Aquí busca el valor final de la URL. 
          {
          ACT_RL4 ();
          }
      if (request.indexOf("/REL4=OFF") != -1)  
        {
          DESACT_RL4 ();
        }
//Activacion/Desactivacion TODO. Hay una opción para desactivarlo todo con un solo botón. 
      
      if (request.indexOf("RELTODO=OFF") != -1)  
      {
        DESACT_RL1 ();
        delay (25);
        DESACT_RL2 ();
        delay (25);
        DESACT_RL3 ();
        delay (25);
        DESACT_RL4 ();
      }
// Inicio programación WEB. Aquí empieza a crearse la web que vemos.
//La web se forma enviando por cliente.print cada una de las líneas de programación que componen la pagina web, separando solo las instrucciones para cambiar o dar valor a algún dato.
Serial.println("Inicio WEB");
    client.println("HTTP/1.1 200 OK");  //
    client.println("Content-Type: text/html");  //
    client.println("<meta charset='UTF-8'>");
    client.println(""); //  this is a must
    client.println("<!DOCTYPE HTML>");      //Creamos un tipo de documento HTML
//Empieza la pagina web
    client.print("<html>");     //Inicio del documento HTML
  //ENCABEZADO
      client.print("<head>");
      client. print("<title>HOME-");  //Encabezado del documento. Aquí podemos poner como queremos que se llame la pagina web.
      client.print(ZONA); 
      client.print("</title>"); 
       if (request.indexOf("/REL") == -1)  //Si encuentra que no se ejecutó algún comando, resetea la web cada 10segundos. No encuentra coincidencia "REL" Indexof devuelve -1.
       {
        client.print("<meta http-equiv=\"refresh\"content=\"7; URL=http://192.168.1.11\">"); //le dice a la pagina web que vay a ala pagina original cada 5seg. Si refrescamos, puede por ejemplo, volver a abrir la puerta.
       }
       else //si se ejecutó algún comando, resetea la web tras un segundo.
       {
        client.print("<meta http-equiv=\"refresh\"content=\"0.1; URL=http://192.168.1.11\">"); //le dice a la pagina web que vay a ala pagina original cada 5seg. Si refrescamos, puede por ejemplo, volver a abrir la puerta.
       }
        client.print("</head>"); 
      
  //CUERPO
      client.print("<body bgcolor=\"\#ffb3ba\"> "); //indicacion que inicia el cuerpo de la web. delante de cada " para el código HTML hay que poner una barra\ para que el compilador no la tome como lenguage arduino
      client.print("<center><p> <a href=\"http://192.168.1.10\"><h1>MyHome</h1></a></p></center>"); //Indicamos que el texto será tipo "h1" encabezado 1, que es el mas grande.
      
      client.print("<center><Table border=2 width=\"90%\" bgcolor=\"#ffffba\"><tr><td colspan=\"2\"> <center><a href=\"http://");
      
      client.print(ip[0]);
      client.print(".");
      client.print(ip[1]);
      client.print(".");
      client.print(ip[2]);
      client.print(".");
      client.print(ip[3]);
      client.print("\"><h2><font color= \"\#0a86d6\" >");
      client.print(ZONA);
      client.print("</font></h2></a></center></td></td></tr>"); // Creamos una tabla con una celda con el texto dispositivos. luego haremos las columnas y las filas
      //Pulsador de puerta
      client.print("<tr>");  //creamos la primera fila de la tabla
      client.print("<td width=\"70%\"> <center><h2><font color= \"\#0a86d6\" >");
      client.print(CANAL1);
      client.print("</font></h2></center></td>");//primera celda de la tabla, columna del dispositivo.
        if (RL1 == 0) //creamos la columna de seleccion de la acción
        {
         client.print("<td width=\"30%\"> <center><a href=\"/REL1=ON\"><h2><font color= \"#0a86d6\">ABRIR</font></h2></a></center></td>");
         client.print("</tr>");//cerrramos la fila
        }
        else
        {
         client.print("<td width=\"30%\"> <center><h2><font color= \"#0a86d6\">ABRIENDO</font></h2></center></td>");
         client.print("</tr>");//cerrramos la fila
        }

//Bomba de agua  
  client.print("<tr>");  //creamos la segunda fila de la tabla
  client.print("<td width=\"70%\"> <center><h2><font color= \"\#0a86d6\" >");
  client.print(CANAL4);
  client.print("</font></h2></center></td>"); //creamos la primera celda de la columna
  if (RL4 == 0) //elegimos la segunda celda de la fila en la otra columna
      {
       client.print("<td width=\"30%\"> <center><a href=\"/REL4=ON\"><h2><font color= \"#0a86d6\">ACTIVAR</font></h2></a></center></td>");
       client.print("</tr>");//cerrramos la linea
      }
     else
      {
      client.print("<td width=\"30%\"> <center><a href=\"/REL4=OFF\"><h2><font color= \"\#0a86d6\">DESACTIVAR</font></h2></a></center></td>");
      client.print("</tr>");//cerrramos la linea
      }
 
// otros canales 
  //CANAL 2
  client.print("<tr>");  //creamos la tercera fila de la tabla
  client.print("<td width=\"70%\"> <center><h2><font color= \"\#0a86d6\" >");
  client.print(CANAL2);
  client.print("</font></h2></center></td>"); 
  if (RL2 == 0)
      {
       client.print("<td width=\"30%\"> <center><a href=\"/REL2=ON\"><h2><font color= \"\#0a86d6\">ACTIVAR</font></h2></a></center></td>");
       client.print("</tr>");//cerrramos la columna
      }
     else
      {
      client.print("<td width=\"30%\"> <center><a href=\"/REL2=OFF\"><h2><font color= \"\#0a86d6\">DESACTIVAR</font></h2></a></center></td>");
      client.print("</tr>");//cerrramos la columna
      }
  //CANAL 3
    client.print("<tr>");  //creamos la tercera fila de la tabla
  client.print("<td width=\"70%\"> <center><h2><font color= \"\#0a86d6\" >");
  client.print(CANAL3);
  client.print("</font></h2></center></td>"); 
  if (RL3 == 0)
      {
       client.print("<td width=\"30%\"> <center><a href=\"/REL3=ON\"><h2><font color= \"\#0a86d6\">ACTIVAR</font></h2></a></center></td>");
       client.print("</tr>");//cerrramos la columna
      }
     else
      {
      client.print("<td width=\"30%\"> <center><a href=\"/REL3=OFF\"><h2><font color= \"\#0a86d6\">DESACTIVAR</font></h2></a></center></td>");
      client.print("</tr>");//cerrramos la columna
      } 

//Opción de desactivar todo:
    client.print("<tr>");  //creamos la sexta fila
    client.print("<td colspan=\"2\">  <center><a href=\"/RELTODO=OFF\"><h1><font color= \"\#0a86d6\" >DESACTIVAR TODO</font></h1></a></center></td>");
    client.print("</tr>"); 
    client.print("</table></center>");  //ceramos la tabla y el centrado
//imprime el número de clientes conectados. 
    client.print("<p>"); 
    client.print("<br><br><br><br><br><br>");
//Imprime versión software que estamos ejecutando. 
    client.print("<p>"); 
    client.print("<br>");
    client.print("Firmware: ");
    client.print(Firmware);
    client.print("</p>");   

//cerramos el cuerpo de  la pagina web y la web.
    client.print("</body>");  //Final del cuerpo de la web
    client.print("</html>");
 
  delay(1); //aumentamos delay de 1 a 3 para ver si ese es el problema con el puerto serie.
  
  if (request.indexOf("/favicon") ==-1)
  {
      //este codigo se ejecutaría una vez que se ha terminado la conexión con el server.
  }
}




  
