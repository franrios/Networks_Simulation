/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/callback.h>
#include <ns3/packet.h>
#include "Enlace.h"
#include "CabEnlace.h"



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Enlace");

Enlace::Enlace(Ptr<NetDevice> disp,
                                 Time           espera,
                                 uint32_t       tamPqt,
                                 uint8_t        tamTx):m_ventanaTx(tamTx,(uint32_t)RANGO)
{
  NS_LOG_FUNCTION (disp << espera << tamPqt << tamTx);
  //
  //
  // QUITAR LAS VARIABLES INUTILES DEBIDO A VENTANA.H
  //
  //
  // Inicializamos las variables privadas
  m_disp      = disp;
  m_esperaACK = espera;
  m_tamPqt    = tamPqt;
  m_tamTx     = tamTx;
  m_tx        = 0;
  m_rx        = 0;
  m_ventIni   = 0;
  m_totalPqt  = 0;
  m_totalPqtACK = 0;
}

void
Enlace::ACKRecibido(uint32_t numSecuencia)
{
  NS_LOG_FUNCTION ((unsigned int)numSecuencia);

  //ESTE LOG HAY QUE MODIFICARLO
  NS_LOG_DEBUG ("Recibido ACK en nodo " << m_node->GetId() << " con "
                << (unsigned int) numSecuencia); // << ". La ventana es [" 
  //              << (unsigned int) (m_ventIni)%RANGO << "," << (unsigned int) (m_ventIni + m_tamTx - 1)%RANGO << "].");

  // Comprobamos si el número de secuencia del ACK se corresponde con
  // el de secuencia del siguiente paquete a transmitir
  if(m_ventanaTx.EnVentana(numSecuencia))
  {
      // Si es correcto desactivo el temporizador
      m_temporizador.Cancel();
      // Desplazamos la ventana
      m_ventanaTx.Asentida(numSecuencia);
      //ESTE LOG HAY QUE CAMBIARLO
      //NS_LOG_DEBUG("La ventana se desliza a [" << (unsigned int) (m_ventIni)%RANGO 
      //              << "," << (unsigned int) (m_ventIni + m_tamTx - 1)%RANGO << "].");

      // Se transmite un nuevo paquete
      EnviaPaqueteDatos();
  } 
}

void
Enlace::DatoRecibido(uint32_t numSecuencia)
{
  NS_LOG_FUNCTION ((unsigned int)numSecuencia);

  NS_LOG_DEBUG ("Recibido paquete en nodo " << m_node->GetId() << " con "
                << (unsigned int) numSecuencia);

  if (numSecuencia == m_rx) 
    // Actualizo ventana de recepcion.
    m_rx = (m_rx+1)%RANGO;
  // Transmito en cualquier caso un ACK
  EnviaACK();
}

void 
Enlace::PaqueteRecibido(Ptr<NetDevice>        receptor,
                             Ptr<const Packet>     recibido,
                             uint16_t              protocolo,
                             const Address &       desde,
                             const Address &       hacia,
                             NetDevice::PacketType tipoPaquete)
{
  NS_LOG_FUNCTION (receptor << recibido->GetSize () <<
                   std::hex << protocolo <<
                   desde << hacia << tipoPaquete);

  // Obtengo una copia del paquete
  Ptr<Packet> copia = recibido->Copy();

  CabEnlace header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);
  // Obtenemos el tipo y el numero de secuencia.
  uint32_t tipo = (uint32_t)header.GetTipo();
  uint32_t numSecuencia = (uint32_t)header.GetSecuencia();

  if (tipo == 0)
  {
    // Paquete de datos recibido.
    DatoRecibido(numSecuencia);
  } 
  else 
  {
    // Paquete de ACK recibido
    ACKRecibido(numSecuencia);
  }
}

void
Enlace::VenceTemporizador()
{
  NS_LOG_FUNCTION_NOARGS ();
  //ESTE LOG HAY QUE MODIFICARLO
  NS_LOG_DEBUG ("NODO " << m_node->GetId() <<": VENCE TEMPORIZADOR" );
  //  << "Se reenvian los paquetes con numero de secuencia perteneciente al intervalo: ["
  //  << (unsigned int) (m_ventIni)%RANGO << "," << (m_ventIni + m_tamTx - 1)%RANGO << "].");

  //Iniciamos la ventana de transmision.
  m_ventanaTx.Vacia();
  // Se reenvia un paquete
  EnviaPaqueteDatos();

  NS_LOG_DEBUG("NODO " << m_node->GetId() << ": SE RECUPERA DE ERROR");
}


void
Enlace::EnviaPaqueteDatos()
{
  NS_LOG_FUNCTION_NOARGS ();

  while (m_ventanaTx.Credito() > 0)
  {
    Ptr<Packet> paquete = Create<Packet> (m_tamPqt);

    //Obtenemos el numero de secuencia a enviar y
    //actualizamos el puntero que apunta al siguiente numero de secuencia a enviar.
    m_tx = m_ventanaTx.Pendiente();
    //Formamos la cabecera
    CabEnlace header;
    header.SetData (0, m_tx);

    //Añadimos la cabecera
    paquete->AddHeader (header);

    // Envío el paquete
    m_node->GetDevice(0)->Send(paquete, m_disp->GetAddress(), 0x0800);


    NS_LOG_INFO ("   Solicitado envio de paquete de " << paquete->GetSize () <<
                 " octetos en nodo " << m_node->GetId() <<
                 " con " << (unsigned int) m_tx <<
                 " en " << Simulator::Now());

    //ESTO ES INNECESARIO
    //Aumentamos el total de paquetes transmitidos
    m_totalPqt++;

    // Programo el temporizador si no esta ya en marcha
    if (m_esperaACK != 0 && m_temporizador.IsRunning() == false)
        m_temporizador=Simulator::Schedule(m_esperaACK,&Enlace::VenceTemporizador,this);
  }
  NS_LOG_DEBUG("NODO " << m_node->GetId() << ": LLENA LA VENTANA");
}

void
Enlace::EnviaACK()
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Packet> p = Create<Packet> (1);

  //Formamos la cabecera
  CabEnlace header;
  header.SetData (1, m_rx);
  //Añadimos la cabecera
  p->AddHeader (header);

  m_node->GetDevice(0)->Send(p, m_disp->GetAddress(), 0x0800);

  NS_LOG_DEBUG ("Transmitido ACK de " << p->GetSize () <<
                " octetos en nodo " << m_node->GetId() <<
                " con " << (unsigned int) m_rx <<
                " en " << Simulator::Now());

  //Aumentamos el numero de ACKs transmitidos.
  m_totalPqtACK++;
}
