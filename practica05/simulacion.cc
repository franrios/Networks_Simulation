/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("practica05");




int
main (int argc, char *argv[])
{
	GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
	Time::SetResolution (Time::US);


	uint32_t nCsma      = 10;                  //nodos csma (10 por defecto)
    uint32_t tamPaquete = 1024;                //tamaño de las SDU de aplicación (1024 octetos por defecto)
    Time intervalo      = Time("1s");          //tiempo entre dos paquetes consecutivos enviados por el mismo cliente (1s por defecto)
    Time retardoProp    = Time("6.56us");      //retardo de propagación en el bus (6,56µs por defecto)
    DataRate capacidad  = DataRate("100Mbps"); //capacidad del bus (100Mb/s por defecto)
    
	CommandLine cmd;
	cmd.AddValue ("nCsma", "Número de nodos de la red local", nCsma);
    cmd.AddValue ("retardoProp", "Retardo de propagación en el bus", retardoProp);
    cmd.AddValue ("capacidad", "Capacidad del bus", capacidad);
    cmd.AddValue ("tamPaquete", "Tamaño de la SDU de aplicación", tamPaquete);
    cmd.AddValue ("intervalo", "Tiempo entre dos paquetes consecutivos enviados por el mismo cliente", intervalo );

	cmd.Parse (argc,argv);

    NS_LOG_INFO("Parámetros: " << (unsigned int)nCsma << " nodos,");
       // NS_LOG_INFO(retardoProp.GetSeconds());
       // NS_LOG_INFO(TimeValue(retardoProp));
       // NS_LOG_INFO( (unsigned int) tamPaquete);
       // NS_LOG_INFO( capacidad.GetBitRate());


	NodeContainer csmaNodes;
	csmaNodes.Create (nCsma);  //creamos el número de nodos según la variable nCsma

    CsmaHelper csma;           //creamos un objeto para el canal CSMA
    csma.SetChannelAttribute ("DataRate", DataRateValue(capacidad));         //La capacidad del canal
    csma.SetChannelAttribute ("Delay", TimeValue (retardoProp));     //retardo de propagación

    NetDeviceContainer csmaDevices;                                         //creamos el bojeto para manejar el contenedor
    csmaDevices = csma.Install (csmaNodes);
	// Instalamos la pila TCP/IP en todos los nodos
    InternetStackHelper stack;                //creamos el objeto para manejar la pila
    stack.Install (csmaNodes);                //instalamos la pila
    // Y les asignamos direcciones
    Ipv4AddressHelper address;                 //creamos el objeto para manejar la dirección de red
    address.SetBase ("10.1.2.0", "255.255.255.0");  //definimos la dirección de red
    Ipv4InterfaceContainer csmaInterfaces = address.Assign (csmaDevices); //asignamos la dirección de red a los dispositivos

    /////////// Instalación de las aplicaciones
    // Servidor
    UdpEchoServerHelper echoServer (9);     //creamos la aplicación del servidor echo y le asignamis el puerto 9
    ApplicationContainer serverApp = echoServer.Install (csmaNodes.Get (nCsma - 1));   //instalamos la app en el nodo nCsma-1 
    serverApp.Start (Seconds (1.0));
    serverApp.Stop (Seconds (10.0));
    // Clientes
    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma - 1), 9); //creamos un cliente echo con la dir y puerto
                                                                               // del servidor

    //definimos algunos parámetros sobre el cliente echo
    echoClient.SetAttribute ("MaxPackets", UintegerValue (10000));
    echoClient.SetAttribute ("Interval", TimeValue (intervalo));
    echoClient.SetAttribute ("PacketSize", UintegerValue (tamPaquete));
	NodeContainer clientes;
    for (uint32_t i = 0; i < nCsma - 1; i++)
    {
    	clientes.Add (csmaNodes.Get (i));
    }
    ApplicationContainer clientApps = echoClient.Install (clientes);
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    // Cálculo de rutas
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    csma.EnablePcap ("practica05", csmaDevices.Get (nCsma - 1), true); 

    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
