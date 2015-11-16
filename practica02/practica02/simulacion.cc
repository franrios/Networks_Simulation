#include <ns3/core-module.h>

#include "llegadas.h"
#include "ns3/gnuplot.h"

#define MEDIA 10
#define MAX 10;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Simulacion");

int main (int argc, char *argv[])
{

  int media=10;
  int paso=10;
  int valores=5;

  /*Preparamos los parámetros solicitados para poder ser introducidos
    opcionalmente por línea de comandos*/
  CommandLine cmd;

  cmd.AddValue("tasaMedia","Media inicial",media);
  cmd.AddValue("paso","Paso entre medias",paso);
  cmd.AddValue("valores","Número de valores que se obtienen por línea de comandos",valores);
  cmd.Parse(argc,argv);

  Gnuplot plot;

  int i=0;
  int numSimulaciones=MAX;
  //Modificamos la precisión de la clase time para trabajar en segundos
  //Time::SetResolution(Time::S);
  Average<double>acumulador_ic;
  double s2n=0.0;
  double z=0.0;
  double media_aux=0.0;
  double media_variable=MAX

  double tstudent=1.8331; //Constante proveniente de la tabla;

  plot.SetTitle ("Práctica 02"); 
  Gnuplot2dDataset datos;
  datos.SetTitle("Muestras ");
  datos.SetStyle(Gnuplot2dDataset::POINTS);
  datos.SetErrorBars(Gnuplot2dDataset::Y);

for(media_variable=media; media_variable<(media + valores*paso); media_variable=media_variable+paso)
{
 for ( i = 0; i < numSimulaciones; i++ )
 {
    // Definimos el modelo a simular
    Llegadas modelo (media_variable);

  // Programo la finalización de la simulación.
  Simulator::Stop(Seconds(10000));

  // Lanzamos la simulación.
  Simulator::Run();

  NS_LOG_INFO ("Número de simulación: " << i+1);
    // Presentamos la media muestral
  media_aux=modelo.Media();
  NS_LOG_INFO ("Media: " << modelo.Media ());
   
  // Finalizamos la simulación
  Simulator::Destroy ();

  acumulador_ic.Update(media_aux);

  //Calculamos la varianza
 s2n=acumulador_ic.Var()/(numSimulaciones-1);
 z=tstudent*sqrt(s2n);
 }

 NS_LOG_INFO("Valor de media_variable: " << media_variable);
 NS_LOG_INFO ("Intervalo de confianza: [" << acumulador_ic.Mean() - z << "," << acumulador_ic.Mean() + z << "]");
 datos.Add(media_variable, acumulador_ic.Mean(), z);
 //Reseteamos el acumulador para poder obtener otro valor para el cálculo del intervalo de confianza
 acumulador_ic.Reset();
}

//Para la gráfica
  plot.AddDataset(datos);
  std::ofstream fichero("practica02.plt");
  plot.GenerateOutput (fichero);
  fichero << "pause -1" << std::endl;
  fichero.close();
  
}
