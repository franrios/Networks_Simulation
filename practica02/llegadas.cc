#include <ns3/core-module.h>

#include "llegadas.h"



using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("Llegadas");


Llegadas::Llegadas (double tasa)
{ 
  // Creamos la variable aleatoria ...
  tEntreLlegadas = CreateObject<ExponentialRandomVariable> ();
  
  // ... y ajustamos su valor medio
  tEntreLlegadas->SetAttribute ("Mean",DoubleValue(1/tasa));
  
  // Programamos la primera llegada.
  // .............
  Simulator::Schedule(Seconds(tEntreLlegadas->GetValue()), &Llegadas::NuevaLlegada,this,Simulator::Now().GetSeconds());
}

void
Llegadas::NuevaLlegada (double tiempo)
{
 double tmp=Simulator::Now().GetSeconds() - tiempo;
  // Calculamos el tiempo transcurrido desde la última llegada
  acumulador.Update(tmp);

  // Presentamos el tiempo transcurrido entre llegadas.
  NS_LOG_INFO("Tiempo transcurrido entre llegadas: " << tmp);

  // Programamos la siguiente llegada
  Simulator::Schedule(Seconds(tEntreLlegadas->GetValue()), &Llegadas::NuevaLlegada,this,Simulator::Now().GetSeconds());

  // Acumulamos el valor del intervalo
  acumulador.Update(tmp);
}


double
Llegadas::Media ()
{
  // Devolvemos el valor medio de las muestras recogidas.
  // .............
  return acumulador.Mean ();
}
