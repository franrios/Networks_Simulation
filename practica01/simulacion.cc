#include <ns3/simulator.h>
#include <ns3/random-variable-stream.h>
#include <ns3/core-module.h>
#include <ns3/config.h>
#include <ns3/average.h>

#define MAX 20

/*
En la ejecución, para mostrar los logs de RandomVariableStream es necesario
modificar la variable de entonrno de NS_LOG de la siguiente manera:
export 'NS_LOG=Salida_info=level_info|prefix_time:RandomVariableStream=level_all|prefix_time'
*/

//Definición de registros para componentes
NS_LOG_COMPONENT_DEFINE ("Salida_info");


using namespace ns3;

int main (int argc, char *argv[])
{
  // Definición de variables
  int numRepeticiones;
  int i;
  double  min=10.0;
  double max=4.0;
  double actual=0.0;
  double actual2=0.0;
  double actual3=0.0;
  double varianza1=0.0;
  double varianza2=0.0;
  double ic_ini=0.0, ic_fin=0.0;
  Ptr<UniformRandomVariable> var;
  Ptr<UniformRandomVariable> var2;
  Average<double> acumulador;
  Average<double> acumulador2;

 //modificación de los valores por defecto
  Config::SetDefault("ns3::UniformRandomVariable::Max",DoubleValue(max));

  numRepeticiones = MAX;
  var = CreateObject<UniformRandomVariable> ();
  
  for ( i = 0; i < numRepeticiones; i++ )
  { 
	actual2=var->GetValue ();
 //       std::cout << actual2 << std::endl;
        acumulador.Update(actual2);	
  }
 
 //modificación de los valores por defecto
 var->SetAttribute ("Min",DoubleValue(min));

  for ( i = 0; i < numRepeticiones; i++ )
  {  
        actual=var->GetValue ();
   //     std::cout << actual << std::endl;
        acumulador.Update(actual);  
 }
  var2 = CreateObject<UniformRandomVariable> ();


  for ( i = 0; i < numRepeticiones; i++ )
  {  
        actual3=var2->GetValue ();
       // std::cout << actual3 << std::endl;
        acumulador2.Update(actual3); 
  }
     varianza1=acumulador.Var ();
     varianza2=acumulador2.Var ();



     /*std::cout << "La media de var es:    " << acumulador.Mean () << std::endl;
     std::cout << "El minimo  de var es:  " << acumulador.Min () << std::endl;
     std::cout << "El máximo   de var es: " << acumulador.Max () << std::endl;
     std::cout << "La varianza de var es: " << varianza1 << std::endl;
*/
     //Salidas con los registros
     NS_LOG_INFO ("La media de var es:    " << acumulador.Mean ());
     NS_LOG_INFO ("El minimo  de var es:  " << acumulador.Min ());
     NS_LOG_INFO ("El máximo   de var es: " << acumulador.Max ());
     NS_LOG_INFO ("La varianza de var es: " << varianza1);


     NS_LOG_INFO ("La media de var2 es:    " << acumulador2.Mean ());
     NS_LOG_INFO ("El minimo  de var2 es:  " << acumulador2.Min ());
     NS_LOG_INFO ("El máximo   de var2 es: " << acumulador2.Max ());
     NS_LOG_INFO ("La varianza de var2 es: " << varianza2);

    //Cálculo del intervalo de ocnfianza a partir de la función Error90
     ic_ini= acumulador2.Mean() - acumulador2.Error90();
     ic_fin= acumulador2.Mean() + acumulador2.Error90();
     NS_LOG_INFO ("Intervalo de confianza con una prob. de 0.9: [" << ic_ini << "," << ic_fin << "]");

}
