/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include "Observador.h"


NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_paquetes = 0;
  m_paquetesError = 0;
}


void
Observador::PaqueteAsentido (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  m_paquetes ++;
}

void
Observador::PaqueteErroneo (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);

  m_paquetesError ++;
}


uint64_t
Observador::TotalPaquetes ()
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_paquetes;
}

uint64_t
Observador::TotalPaquetesErroneos ()
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_paquetesError;
}

double 
Observador::GETCef (double prob_error_bit, uint32_t tamPkt, double ret_prop, double velTx)
{
  double Ce = (1 - prob_error_bit) * ((tamPkt * 8) / ((tamPkt + 8) * 8 + prob_error_bit * 2 * ret_prop * velTx)) * velTx;
  return Ce;
}

double
Observador::GETRend (double Ce, double velTx)
{
  double Rendimiento = (Ce/velTx)*100;
  return Rendimiento;
}