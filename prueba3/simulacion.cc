/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/node.h>
#include <ns3/point-to-point-net-device.h>
#include <ns3/point-to-point-channel.h>
#include <ns3/drop-tail-queue.h>
#include "BitAlternante.h"
#include "ns3/gnuplot.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>




using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Practica03");

int paquetesVsPropagacion (Time retardoPropDesde,
                            Time retardoPropHasta,
                            Time tRetransmisionDesde,
                            Time tRetransmisionHasta,
                            DataRate velocidadTxDesde,
                            DataRate velocidadTxHasta
                            ){
Gnuplot plot1;

  plot1.SetTitle ("Práctica 03-1"); 
  plot1.SetLegend("Valor del temporizador (ms)", "Nº de paquetes");

double temporizador_actual;
double retardo_actual;
double velocidad_actual=((velocidadTxHasta.GetBitRate()+velocidadTxDesde.GetBitRate())/2);
int datos=994;

//El primer bucle for nos permite tener cinco gráficas: una para cada retardo del canal
for(retardo_actual=retardoPropDesde.GetDouble(); 
    retardo_actual<=retardoPropHasta.GetDouble(); 
    retardo_actual=retardo_actual+(retardoPropHasta.GetDouble()-retardoPropDesde.GetDouble())/4)
{
 std::stringstream sstm;
 sstm << "Delay de " << retardo_actual << "ms";
 std::string title = sstm.str();

  Gnuplot2dDataset grafica1;
  grafica1.SetTitle(title);
  grafica1.SetStyle(Gnuplot2dDataset::LINES_POINTS);


  //El segundo bucle for varía el temporizador en diez valores comprendidos entre los valores extremos proporciondos.
  for(temporizador_actual=tRetransmisionDesde.GetDouble();
      temporizador_actual<=tRetransmisionHasta.GetDouble();
      temporizador_actual=(temporizador_actual+(tRetransmisionHasta.GetDouble()-tRetransmisionDesde.GetDouble())/9))
  {  
  NS_LOG_INFO("------------------------------------------"); 
  NS_LOG_INFO("Retardo actual: " << retardo_actual << "ms");
  NS_LOG_INFO("Incrementos del retardo " << ((retardoPropHasta-retardoPropDesde)/4).GetMilliSeconds() << "ms");
  NS_LOG_INFO("temporizador actual: " << temporizador_actual << "ms");
  NS_LOG_INFO("Tasa de transmisión: " << velocidadTxDesde.GetBitRate());
 // NS_LOG_INFO("Incremento de temporizador: " << ((tRetransmisionHasta-tRetransmisionDesde + MilliSeconds(1.1)).GetMilliSeconds())/5);
  // Componentes del escenario:
  // Dos nodos
  Ptr<Node> nodoTx = CreateObject<Node> ();
  Ptr<Node> nodoRx = CreateObject<Node> ();
  // Dos dispositivos de red
  Ptr<PointToPointNetDevice> dispTx = CreateObject<PointToPointNetDevice> ();
  Ptr<PointToPointNetDevice> dispRx = CreateObject<PointToPointNetDevice> ();
  // Un canal punto a punto
  Ptr<PointToPointChannel> canal = CreateObject<PointToPointChannel> ();

  // Una aplicación transmisora
  BitAlternanteTx transmisor(dispRx, Time(temporizador_actual), datos);
  // Y una receptora
  BitAlternanteRx receptor(dispTx);

  // Montamos el escenario:
  // Añadimos una cola a cada dispositivo
  dispTx->SetQueue (CreateObject<DropTailQueue> ());
  dispRx->SetQueue (CreateObject<DropTailQueue> ());
  // Añadimos cada dispositivo a su nodo
  nodoTx->AddDevice (dispTx);
  nodoRx->AddDevice (dispRx);
  // Añadimos cada aplicación a su nodo
  nodoTx->AddApplication(&transmisor);
  nodoRx->AddApplication(&receptor);
  // Asociamos los dos dispositivos al canal
  dispTx->Attach (canal);
  dispRx->Attach (canal);

  dispTx->SetAttribute ("DataRate", DataRateValue(DataRate(velocidad_actual)));

  canal->SetAttribute ("Delay", TimeValue(Time(retardo_actual)));

  // Activamos el transmisor
  transmisor.SetStartTime (Seconds (1.0));
  transmisor.SetStopTime (Seconds (10.0));

 // NS_LOG_UNCOND ("Voy a simular");
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_UNCOND ("Total paquetes: " << transmisor.TotalDatos());


   grafica1.Add(temporizador_actual, transmisor.TotalDatos());
 }
   plot1.AddDataset(grafica1);

}
  std::ofstream fichero("practica03-1.plt");
  plot1.GenerateOutput (fichero);
  fichero << "pause -1" << std::endl;
  fichero.close();

  return 0; 
}

int paquetesVsVelocidad (Time retardoPropDesde,
                         Time retardoPropHasta,
                         Time tRetransmisionDesde,
                         Time tRetransmisionHasta,
                         DataRate velocidadTxDesde,
                         DataRate velocidadTxHasta
                        ){
Gnuplot plot2;

plot2.SetTitle ("Práctica 03-2"); 
plot2.SetLegend("Valor del temporizador (ms)", "Nº de paquetes");

double temporizador_actual;        //Variable usada para ir incrementando el valor del temporizador
double retardo_actual=(retardoPropHasta.GetDouble()+retardoPropDesde.GetDouble())/2; //variable para incrementar retardo
double velocidad_actual;          //variable para incrementar velocidad
float npaquetes_teorico=0;        //usado para el cálculo del número de paquetes que deben enviarse correctamente
int datos=994;


//El primer bucle for nos permite tener cinco curvas: una para cada tasa de transmisión.
for(velocidad_actual=velocidadTxDesde.GetBitRate(); 
    velocidad_actual<=velocidadTxHasta.GetBitRate(); 
    velocidad_actual=velocidad_actual+(velocidadTxHasta.GetBitRate()-velocidadTxDesde.GetBitRate())/4)
{
 std::stringstream sstm;
 sstm << "Tasa de txon de " << velocidad_actual/1e+06 << "Mbps";
 std::string title = sstm.str();

  Gnuplot2dDataset grafica2;
  grafica2.SetTitle(title);
  grafica2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
  
  //El segundo bucle for varía el temporizador en diez valores comprendidos entre los valores extremos proporciondos.
  for(temporizador_actual=tRetransmisionDesde.GetDouble();
      temporizador_actual<=tRetransmisionHasta.GetDouble();
      temporizador_actual=(temporizador_actual+(tRetransmisionHasta.GetDouble()-tRetransmisionDesde.GetDouble())/9))
  {
  //Los siguientes logs se usan para obtener información en tiempo de ejecución de cómo varían las variables  
  NS_LOG_INFO("------------------------------------------"); 
  NS_LOG_INFO("Retardo actual: " << retardo_actual << "ms");
  NS_LOG_INFO("Velocidad actual: " << velocidad_actual << "bps");
  //NS_LOG_INFO("Incrementos del velocidad " << ((velocidadTxHasta-velocidadTxDesde)/4).GetBitRate() << "Mbps");
  NS_LOG_INFO("temporizador actual: " << temporizador_actual << "ms");
  // NS_LOG_INFO("Tasa de transmisión: " << velocidadTxDesde.GetBitRate());
  // NS_LOG_INFO("Incremento de temporizador: " << ((tRetransmisionHasta-tRetransmisionDesde + MilliSeconds(1.1)).GetMilliSeconds())/5);
  
  // Componentes del escenario:
  // Dos nodos
  Ptr<Node> nodoTx = CreateObject<Node> ();
  Ptr<Node> nodoRx = CreateObject<Node> ();
  // Dos dispositivos de red
  Ptr<PointToPointNetDevice> dispTx = CreateObject<PointToPointNetDevice> ();
  Ptr<PointToPointNetDevice> dispRx = CreateObject<PointToPointNetDevice> ();
  // Un canal punto a punto
  Ptr<PointToPointChannel> canal = CreateObject<PointToPointChannel> ();

  // Una aplicación transmisora
  BitAlternanteTx transmisor(dispRx, Time(temporizador_actual), datos);
  // Y una receptora
  BitAlternanteRx receptor(dispTx);

  // Montamos el escenario:
  // Añadimos una cola a cada dispositivo
  dispTx->SetQueue (CreateObject<DropTailQueue> ());
  dispRx->SetQueue (CreateObject<DropTailQueue> ());
  // Añadimos cada dispositivo a su nodo
  nodoTx->AddDevice (dispTx);
  nodoRx->AddDevice (dispRx);
  // Añadimos cada aplicación a su nodo
  nodoTx->AddApplication(&transmisor);
  nodoRx->AddApplication(&receptor);
  // Asociamos los dos dispositivos al canal
  dispTx->Attach (canal);
  dispRx->Attach (canal);
    // Modificamos los parámetos configurables
 /* canal->SetAttribute ("Delay", StringValue ("2ms"));*/
  dispTx->SetAttribute ("DataRate", DataRateValue(DataRate(velocidad_actual)));

  canal->SetAttribute ("Delay", TimeValue(Time(retardo_actual)));
  //dispTx->SetAttribute ("DataRate", StringValue(tasa_string));


  // Activamos el transmisor
  transmisor.SetStartTime (Seconds (1.0));
  transmisor.SetStopTime (Seconds (10.0));

 // NS_LOG_UNCOND ("Voy a simular");
  Simulator::Run ();
  Simulator::Destroy ();

  /*El cálculo del número de paquetes teóricos que se envían correctamente se realiza de manera acorde a la siguiente fórmula:
  suponemos que el temporizador de retransmisiones es mayor que el tiempo que tarda en enviarse un paquete y en recibir su ACK
  Este tiempo es igual a:

                Datos
                |       Añadidos a los datos 
                |       |     NetDevice
                |       |     |     ACK  
                |       |     |     |    NetDevice
                |       |     |     |    |
          (994bytes+1byte+2bytes+1byte+2byte)*8bits/byte
          _______________________________________________   + 2*Retardo_de_propagación(en s)

                        Tasa de transmisión(en bps)
  
      La operación anterior nos proporciona como resultado el tiempo transcurrido entre que se envía un paquete y se
      recibe su ACK sin haber retransmisiones.

      Para saber cuántos paquetes se envían correctamente durante la simulación con las condiciones anteriores se divide
      los 9s de simulación entre el tiempo anterior.
  */


  npaquetes_teorico=9/(8000/velocidad_actual+2*retardo_actual/1000);

  NS_LOG_UNCOND ("Total paquetes: " << transmisor.TotalDatos());
  NS_LOG_UNCOND ("Teórico: " << npaquetes_teorico);


   grafica2.Add(temporizador_actual, transmisor.TotalDatos());
 }
   plot2.AddDataset(grafica2);

}
  std::ofstream fichero2("practica03-2.plt");
  plot2.GenerateOutput (fichero2);
  fichero2 << "pause -1" << std::endl;
  fichero2.close();

  return 0; 
}

int main (int argc, char *argv[])
{

  int error=0;

  DataRate velocidadTxDesde=DataRate("0.5Mbps");
  DataRate velocidadTxHasta=DataRate("3Mbps");
  Time retardoPropDesde=Time("1ms");
  Time retardoPropHasta=Time("10ms");
  Time tRetransmisionDesde=Time("1ms");
  Time tRetransmisionHasta=Time("20ms");


  /*Preparamos los parámetros solicitados para poder ser introducidos
    opcionalmente por línea de comandos*/
  CommandLine cmd;

  cmd.AddValue("velocidadTxDesde","Time Out del temporizador",velocidadTxDesde);
  cmd.AddValue("velocidadTxHasta","datos",velocidadTxHasta);
  cmd.AddValue("retardoPropDesde","Retraso del canal",retardoPropDesde);
  cmd.AddValue("retardoPropHasta","Time Out del temporizador",retardoPropHasta);
  cmd.AddValue("tRetransmisionDesde","Retraso del canal",tRetransmisionDesde);
  cmd.AddValue("tRetransmisionHasta","Time Out del temporizador",tRetransmisionHasta);

  cmd.Parse(argc,argv);

  Time::SetResolution (Time::MS);

  //Función encargada de generar la primera grafica requerida
  error=paquetesVsPropagacion(retardoPropDesde,retardoPropHasta,tRetransmisionDesde,tRetransmisionHasta, velocidadTxDesde, velocidadTxHasta);
  error=paquetesVsVelocidad(retardoPropDesde,retardoPropHasta,tRetransmisionDesde,tRetransmisionHasta, velocidadTxDesde, velocidadTxHasta);

/*
En el enunciado de la práctica se solicita el cálculo del intervalo de confianza. 

Para poder realizar cálculos de ese tipo y que proporcionen información útil
en la simulación, es necesario que las variables de entrada sean de carácter aleatorio.

En esta simulación carecemos de variables de entradas aleatorias y, por consiguiente,
los resultados siempre son los mismos (a iguales valores de entrada e independientemente del número de simulaciones).
Esto nos lleva a concluir que no es útil calcular un intervalo de confianza.
*/

  return error;
}


