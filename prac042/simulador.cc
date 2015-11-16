/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/data-rate.h>
#include <ns3/error-model.h>
#include <ns3/random-variable-stream.h>
#include <ns3/point-to-point-net-device.h>
#include <ns3/command-line.h>
#include <ns3/gnuplot.h>
#include <sstream>
#include <ns3/average.h>
#include "Enlace.h"
#include "Observador.h"

using namespace ns3;

#define TSTUDENT10 1.8331
#define NUM_SIMULACIONES 10

NS_LOG_COMPONENT_DEFINE ("Practica04");


double
simulacion (Time wait, uint32_t pktSize, Time delay, DataRate rate, uint32_t window, double prob_error_bit)
{

  // Parámetros de la simulación
  Time     trtx       = wait;
  uint32_t tamPaquete = pktSize;
  Time     rprop      = delay;
  DataRate vtx        = rate;
  uint32_t  tamVentana = window;
  double   perror     = prob_error_bit;
  double   cadenciaEficaz = 0.0;
  double   rendimiento = 0.0;

  // Creamos el modelo de error y le asociamos los parametros
  Ptr<UniformRandomVariable> uniforme = CreateObject<UniformRandomVariable>();
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();


  modelo_error->SetRandomVariable(uniforme);
  modelo_error->SetRate(perror);
  modelo_error->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Configuramos el escenario:
  PointToPointHelper escenario;
  escenario.SetChannelAttribute ("Delay", TimeValue (rprop));
  escenario.SetDeviceAttribute ("DataRate", DataRateValue (vtx));
  escenario.SetQueue ("ns3::DropTailQueue");
  // Creamos los nodos
  NodeContainer      nodos;
  nodos.Create(2);
  // Creamos el escenario
  NetDeviceContainer dispositivos = escenario.Install (nodos);

  // Una aplicación transmisora (que tambien recibira)
  Enlace transmisor (dispositivos.Get (1), trtx, tamPaquete, tamVentana);
  // Y una receptora (que tambien enviara)
  Enlace receptor (dispositivos.Get (0), trtx, tamPaquete, (uint8_t)tamVentana);

  dispositivos.Get(0)->GetObject<PointToPointNetDevice>()->SetReceiveErrorModel(modelo_error);
  dispositivos.Get(1)->GetObject<PointToPointNetDevice>()->SetReceiveErrorModel(modelo_error);

  Observador observador;
  // Suscribimos la traza de paquetes correctamente asentidos.
  dispositivos.Get (0)->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::PaqueteAsentido, &observador));
  // Suscribimos la traza de paquetes erroneos.
  dispositivos.Get (1)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback(&Observador::PaqueteErroneo, &observador));

  // Añadimos cada aplicación a su nodo
  nodos.Get (0)->AddApplication(&transmisor);
  nodos.Get (1)->AddApplication(&receptor);

  //Añadimos una salida a pcap
  escenario.EnablePcap("ppractica04", dispositivos.Get(0));

  // Activamos el transmisor
  transmisor.SetStartTime (Seconds (1.0));
  transmisor.SetStopTime (Seconds (6.0));
  // Activamos el transmisor
  receptor.SetStartTime (Seconds (1.0));
  receptor.SetStopTime (Seconds (6.0));

  Simulator::Run ();
  Simulator::Destroy ();

  //Calculamos la cadencia eficaz y el rendimiento.
  cadenciaEficaz = observador.GETCef(perror, tamPaquete, rprop.GetDouble()*1e-6, vtx.GetBitRate());
  rendimiento = observador.GETRend(cadenciaEficaz, vtx.GetBitRate());

  NS_LOG_DEBUG ("TamPaquete: " << tamPaquete + 8);
  NS_LOG_DEBUG ("Vtx: " << vtx);
  NS_LOG_DEBUG ("Rprop: " << rprop);
  NS_LOG_DEBUG ("RTT: " << Seconds(vtx.CalculateTxTime (tamPaquete + 8)) + 2 * rprop);
  NS_LOG_DEBUG ("Temporizador: " << trtx);
  NS_LOG_DEBUG ("Probabilidad de error de bit: " << perror);
  NS_LOG_INFO  ("Total paquetes: " << observador.TotalPaquetes ());
  NS_LOG_INFO  ("Total paquetes erroneos: " << observador.TotalPaquetesErroneos ());
  NS_LOG_INFO  ("Cadencia eficaz: " << cadenciaEficaz << "b/s");
  NS_LOG_INFO  ("Rendimiento: " << rendimiento << "%");

  return rendimiento;
}

int
main (int argc, char *argv [])
{
  Time::SetResolution (Time::US);

  // Parámetros de la simulación
  Time     trtx       = Time("6ms");
  uint32_t tamPaquete = 121;
  Time     rprop      = Time("0.2ms");
  DataRate vtx        = DataRate("1Mbps");
  uint32_t  tamVentana = 6;
  double   perror     = 0;
  double z =0.0;

  Average<double> ac_rendimientos;

  CommandLine cmd;
  
  //Preparamos la captura de valores por línea de comandos.
  cmd.AddValue("window","Tamaño de la ventana de transmisión.",tamVentana);
  cmd.AddValue("delay","Retardo de propagación del enlace.",rprop);
  cmd.AddValue("rate","Capacidad de transmisión en el canal.",vtx);
  cmd.AddValue("pktSize","Tamaño de la SDU del nivel de enlace.",tamPaquete);
  cmd.AddValue("wait","Tiempo de espera para la retransmisión.",trtx);

  cmd.Parse(argc,argv);

  NS_LOG_INFO ("Se han configurado los siguientes argumentos de entrada:" << std::endl <<
                "     -window:    " << (unsigned int)tamVentana  << std::endl <<
                "     -delay:     " << rprop.GetDouble()/1e3 << "ms" << std::endl <<
                "     -vtx:       " << vtx.GetBitRate()/1e6 << "Mbps" << std::endl <<
                "     -pktSize:   " << tamPaquete << "B" << std::endl <<
                "     -wait:      " << trtx.GetDouble()/1e3 << "ms" << std::endl);

  //Preparamos la gráfica 
  Gnuplot plot;
  plot.SetTitle("Práctica 04 - Rendimiento frente a prob. error de paquete");
  plot.SetLegend("prob. error de paquete", "rendimiento en %");

  //Para imprimir el rotulo de las curvas indicando el retardo
  std::stringstream sstm;
  sstm << "Curva";
  std::string result = sstm.str();

  Gnuplot2dDataset datos (result);
  datos.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  datos.SetErrorBars (Gnuplot2dDataset::Y);

  for (perror = 0.0; perror <= 0.5; perror += 0.05)
  {
    for (int i = 0; i < 10 ; i++)
    {
      ac_rendimientos.Update(simulacion(trtx, tamPaquete, rprop, vtx, tamVentana, perror));

    }

    z = TSTUDENT10*sqrt(ac_rendimientos.Var()/(NUM_SIMULACIONES));

    //Añadimos datos a la gráfica
    datos.Add(perror, ac_rendimientos.Mean(), z);

    //Reseteamos variables average
    ac_rendimientos.Reset();
  }
  //Añadimos los datos al objeto gráfica
  plot.AddDataset(datos);

  //Abrimos el fichero de salida para la grafica
  std::ofstream fich("practica04.plt");
  //Añadimos los datos de la gráfica al fichero
  plot.GenerateOutput(fich);
  //Añadimos esta linea al fichero para que la gráfica no se cierre nada más ejecutarse
  fich << "pause -1" << std::endl;
  //Cerramos el fichero
  fich.close();
}
